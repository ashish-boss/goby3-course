#include "pGobyCourseTopsideManager.h"

using goby::glog;
using goby::moos::operator<<;
using goby3_course::moos::protobuf::GobyCourseTopsideManagerConfig;

std::shared_ptr<GobyCourseTopsideManagerConfig> global_config;
goby3_course::moos::GobyCourseTopsideManager* goby3_course::moos::GobyCourseTopsideManager::inst_ =
    0;

int main(int argc, char* argv[])
{
    return goby::moos::run<goby3_course::moos::GobyCourseTopsideManager>(argc, argv);
}

goby3_course::moos::GobyCourseTopsideManager*
goby3_course::moos::GobyCourseTopsideManager::get_instance()
{
    if (!inst_)
    {
        global_config.reset(new GobyCourseTopsideManagerConfig);
        inst_ = new goby3_course::moos::GobyCourseTopsideManager(*global_config);
    }
    return inst_;
}

void goby3_course::moos::GobyCourseTopsideManager::delete_instance() { delete inst_; }

goby3_course::moos::GobyCourseTopsideManager::GobyCourseTopsideManager(
    GobyCourseTopsideManagerConfig& cfg)
    : GobyMOOSApp(&cfg),
      cfg_(cfg),
      geodesy_({cfg_.common().lat_origin() * boost::units::degree::degrees,
                cfg_.common().lon_origin() * boost::units::degree::degrees})
{
    subscribe_nav_from_usv();
}

goby3_course::moos::GobyCourseTopsideManager::~GobyCourseTopsideManager() {}

void goby3_course::moos::GobyCourseTopsideManager::loop() {}

void goby3_course::moos::GobyCourseTopsideManager::subscribe_nav_from_usv()
{
    subscribe_pb("NAVIGATION_REPORT_IN", &GobyCourseTopsideManager::handle_incoming_nav, this);
}

void goby3_course::moos::GobyCourseTopsideManager::handle_incoming_nav(
    const goby3_course::dccl::NavigationReport& dccl_nav)
{
    glog.is_verbose() && glog << "Received DCCL nav: " << dccl_nav.ShortDebugString() << std::endl;

    goby::middleware::frontseat::protobuf::NodeStatus frontseat_nav =
        nav_convert(dccl_nav, geodesy_);

    if (cfg_.has_vehicle_name_prefix())
        frontseat_nav.set_name(cfg_.vehicle_name_prefix() + frontseat_nav.name());

    glog.is_verbose() && glog << "^^ Converts to frontseat NodeStatus: "
                              << frontseat_nav.ShortDebugString() << std::endl;

    std::string node_report = node_report_from_nav(frontseat_nav);

    glog.is_verbose() && glog << "^^ Converts to NODE_REPORT: " << node_report << std::endl;

    publish("NODE_REPORT", node_report);
}
