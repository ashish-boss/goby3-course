#include <goby/middleware/marshalling/protobuf.h>
// this space intentionally left blank
#include <boost/lexical_cast.hpp>
#include <boost/units/io.hpp>
#include <boost/units/systems/temperature/celsius.hpp>
#include <goby/middleware/frontseat/groups.h>
#include <goby/middleware/io/line_based/pty.h>
#include <goby/middleware/protobuf/frontseat_data.pb.h>
#include <goby/util/linebasedcomms/nmea_sentence.h>
#include <goby/util/sci.h>
#include <goby/zeromq/application/multi_thread.h>
#include <random>

#include "config.pb.h"
#include "goby3-course/groups.h"
#include "goby3-course/messages/example.pb.h"

using goby::glog;
namespace si = boost::units::si;
namespace celsius = boost::units::celsius;

namespace config = goby3_course::config;
namespace groups = goby3_course::groups;
namespace zeromq = goby::zeromq;
namespace middleware = goby::middleware;
using boost::units::absolute;
using boost::units::quantity;

constexpr goby::middleware::Group pty_in{"pty_in"};
constexpr goby::middleware::Group pty_out{"pty_out"};

namespace goby3_course
{
namespace apps
{
class CTDSimulator : public zeromq::MultiThreadApplication<config::CTDSimulator>
{
  public:
    CTDSimulator();

  private:
    void handle_incoming_serial(const goby::util::NMEASentence& nmea);
    void loop() override;

  private:
    // depth -> temperature
    std::map<quantity<si::length>, quantity<absolute<celsius::temperature>>> temperatures_;
    // depth -> salinity
    std::map<quantity<si::length>, double> salinities_;

    quantity<si::length> latest_depth_{0 * si::meters};

    bool streaming_{false};

    std::random_device rd_{};
    std::mt19937 gen_{rd_()};
    std::normal_distribution<> temp_distribution_{0, 0.02};
    std::normal_distribution<> sal_distribution_{0, 0.01};
};

} // namespace apps
} // namespace goby3_course

int main(int argc, char* argv[])
{
    return goby::run<goby3_course::apps::CTDSimulator>(
        goby::middleware::ProtobufConfigurator<config::CTDSimulator>(argc, argv));
}

// Main thread

goby3_course::apps::CTDSimulator::CTDSimulator()
    : zeromq::MultiThreadApplication<config::CTDSimulator>(1 * si::hertz)
{
    glog.add_group("main", goby::util::Colors::yellow);
    glog.add_group("in", goby::util::Colors::lt_cyan);

    std::ifstream csv_ifs(cfg().csv_file().c_str());
    if (!csv_ifs.is_open())
        glog.is_die() && glog << "Could not open CSV file: " << cfg().csv_file() << std::endl;

    std::string line;
    while (std::getline(csv_ifs, line))
    {
        std::vector<std::string> fields;
        boost::split(fields, line, boost::is_any_of(","));

        if (fields.size() != 3)
        {
            glog.is_warn() && glog << "Invalid CSV line (should be 3 fields): " << line
                                   << std::endl;
            continue;
        }

        try
        {
            enum
            {
                PRESSURE = 0,
                TEMPERATURE = 1,
                SALINITY = 2
            };
            // treat pressure (in dBars) as depth (in meters)
            auto depth = boost::lexical_cast<double>(fields[PRESSURE]) * si::meters;
            auto salinity = boost::lexical_cast<double>(fields[SALINITY]);
            auto temperature =
                boost::lexical_cast<double>(fields[TEMPERATURE]) * absolute<celsius::temperature>();

            temperatures_[depth] = temperature;
            salinities_[depth] = salinity;
            glog.is_debug1() && glog << "Added depth: " << depth << ", temperature: " << temperature
                                     << ", salinity: " << salinity << std::endl;
        }
        catch (boost::bad_lexical_cast&)
        {
            glog.is_warn() && glog << "Invalid CSV line (maybe header?): " << line << std::endl;
        }
    }

    using PTYThread = goby::middleware::io::PTYThreadLineBased<pty_in, pty_out>;
    launch_thread<PTYThread>(cfg().serial());

    interthread().subscribe<pty_in>([this](const goby::middleware::protobuf::IOData& io_msg) {
        try
        {
            goby::util::NMEASentence nmea(io_msg.data(), goby::util::NMEASentence::VALIDATE);
            handle_incoming_serial(nmea);
        }
        catch (const goby::util::bad_nmea_sentence& e)
        {
            glog.is_warn() && glog << group("in") << "Invalid NMEA sentence: " << e.what()
                                   << std::endl;
        }
    });

    interprocess().subscribe<goby::middleware::frontseat::groups::node_status>(
        [this](const goby::middleware::frontseat::protobuf::NodeStatus& frontseat_nav) {
            latest_depth_ = frontseat_nav.global_fix().depth_with_units();
            glog.is_debug1() && glog << "Updated depth: " << latest_depth_ << std::endl;
        });
}

void goby3_course::apps::CTDSimulator::handle_incoming_serial(const goby::util::NMEASentence& nmea)
{
    glog.is_verbose() && glog << group("in") << nmea.message() << std::endl;
    if (nmea.sentence_id() == "CMD")
    {
        if (nmea.size() >= 2)
        {
            if (nmea[1] == "START")
            {
                goby::middleware::protobuf::IOData io_msg;
                io_msg.set_data("$ZCACK,START\r\n");
                interthread().publish<pty_out>(io_msg);
                streaming_ = true;
            }
            else if (nmea[1] == "STOP")
            {
                goby::middleware::protobuf::IOData io_msg;
                io_msg.set_data("$ZCACK,STOP\r\n");
                interthread().publish<pty_out>(io_msg);
                streaming_ = false;
            }
        }
    }
}

void goby3_course::apps::CTDSimulator::loop()
{
    if (streaming_)
    {
        using goby::util::linear_interpolate;
        goby::util::NMEASentence nmea;
        nmea.push_back("$ZCDAT");
        nmea.push_back(linear_interpolate(latest_depth_, salinities_) + sal_distribution_(gen_));
        nmea.push_back(linear_interpolate(latest_depth_, temperatures_).value() +
                       temp_distribution_(gen_));
        nmea.push_back(latest_depth_.value());
        goby::middleware::protobuf::IOData io_msg;
        io_msg.set_data(nmea.message_cr_nl());
        interthread().publish<pty_out>(io_msg);
    }
}
