#include "pGobyCoursePattern.h"

using goby::glog;
using goby::moos::operator<<;
using goby3_course::moos::protobuf::GobyCoursePatternConfig;

std::shared_ptr<GobyCoursePatternConfig> global_config;
goby3_course::moos::GobyCoursePattern* goby3_course::moos::GobyCoursePattern::inst_ = 0;

int main(int argc, char* argv[])
{
    return goby::moos::run<goby3_course::moos::GobyCoursePattern>(argc, argv);
}

goby3_course::moos::GobyCoursePattern* goby3_course::moos::GobyCoursePattern::get_instance()
{
    if (!inst_)
    {
        global_config.reset(new GobyCoursePatternConfig);
        inst_ = new goby3_course::moos::GobyCoursePattern(*global_config);
    }
    return inst_;
}

void goby3_course::moos::GobyCoursePattern::delete_instance() { delete inst_; }

goby3_course::moos::GobyCoursePattern::GobyCoursePattern(GobyCoursePatternConfig& cfg)
    : GobyMOOSApp(&cfg), cfg_(cfg)
{
    // example subscription -
    //    handle_db_time called each time mail from DB_TIME is received
    subscribe("DB_TIME", &GobyCoursePattern::handle_db_time, this);
}

goby3_course::moos::GobyCoursePattern::~GobyCoursePattern() {}

void goby3_course::moos::GobyCoursePattern::loop()
{
    // example publication
    publish("TEST", MOOSTime());
    publish("CONFIG_A", cfg_.config_a());
}

void goby3_course::moos::GobyCoursePattern::handle_db_time(const CMOOSMsg& msg)
{
    glog.is_verbose() && glog << "Time is: " << std::setprecision(15) << msg.GetDouble()
                              << std::endl;
}
