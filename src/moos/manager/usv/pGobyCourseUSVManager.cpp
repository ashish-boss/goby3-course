#include "pGobyCourseUSVManager.h"

using goby::glog;
using goby::moos::operator<<;
using goby3_course::moos::protobuf::GobyCourseUSVManagerConfig;

std::shared_ptr<GobyCourseUSVManagerConfig> global_config;
goby3_course::moos::GobyCourseUSVManager* goby3_course::moos::GobyCourseUSVManager::inst_ = 0;

int main(int argc, char* argv[])
{
    return goby::moos::run<goby3_course::moos::GobyCourseUSVManager>(argc, argv);
}

goby3_course::moos::GobyCourseUSVManager* goby3_course::moos::GobyCourseUSVManager::get_instance()
{
    if (!inst_)
    {
        global_config.reset(new GobyCourseUSVManagerConfig);
        inst_ = new goby3_course::moos::GobyCourseUSVManager(*global_config);
    }
    return inst_;
}

void goby3_course::moos::GobyCourseUSVManager::delete_instance() { delete inst_; }

goby3_course::moos::GobyCourseUSVManager::GobyCourseUSVManager(GobyCourseUSVManagerConfig& cfg)
    : GobyMOOSApp(&cfg),
      cfg_(cfg),
      geodesy_({cfg_.common().lat_origin() * boost::units::degree::degrees,
                cfg_.common().lon_origin() * boost::units::degree::degrees})
{
    glog.add_group("auv_nav", goby::util::Colors::lt_green);
    glog.add_group("usv_nav", goby::util::Colors::lt_blue);

    subscribe_our_nav();
    subscribe_auv_nav();
}

goby3_course::moos::GobyCourseUSVManager::~GobyCourseUSVManager() {}

void goby3_course::moos::GobyCourseUSVManager::loop() {}

void goby3_course::moos::GobyCourseUSVManager::subscribe_our_nav()
{
    subscribe_pb("IFS_NODE_STATUS", &GobyCourseUSVManager::handle_our_nav, this);
}

void goby3_course::moos::GobyCourseUSVManager::handle_our_nav(
    const goby::middleware::frontseat::protobuf::NodeStatus& frontseat_nav)
{
    glog.is_verbose() && glog << group("usv_nav") << "Received frontseat NodeStatus: "
                              << frontseat_nav.ShortDebugString() << std::endl;

    goby3_course::dccl::NavigationReport dccl_nav =
        nav_convert(frontseat_nav, cfg_.vehicle_id(), geodesy_);
    glog.is_verbose() && glog << group("usv_nav")
                              << "^^ Converts to DCCL nav: " << dccl_nav.ShortDebugString()
                              << std::endl;

    publish_pb("NAVIGATION_REPORT_OUT", dccl_nav);
}

void goby3_course::moos::GobyCourseUSVManager::subscribe_auv_nav()
{
    subscribe_pb("AUV_NAVIGATION_REPORT_IN", &GobyCourseUSVManager::handle_auv_nav, this);
}

void goby3_course::moos::GobyCourseUSVManager::handle_auv_nav(
    const goby3_course::dccl::NavigationReport& dccl_nav)
{
    glog.is_verbose() && glog << group("auv_nav")
                              << "Received DCCL nav: " << dccl_nav.ShortDebugString() << std::endl;

    publish_pb("NAVIGATION_REPORT_OUT", dccl_nav);
}
