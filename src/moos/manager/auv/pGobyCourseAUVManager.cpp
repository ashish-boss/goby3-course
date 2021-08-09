#include "pGobyCourseAUVManager.h"

using goby::glog;
using goby::moos::operator<<;
using goby3_course::moos::protobuf::GobyCourseAUVManagerConfig;

std::shared_ptr<GobyCourseAUVManagerConfig> global_config;
goby3_course::moos::GobyCourseAUVManager* goby3_course::moos::GobyCourseAUVManager::inst_ = 0;

int main(int argc, char* argv[])
{
    return goby::moos::run<goby3_course::moos::GobyCourseAUVManager>(argc, argv);
}

goby3_course::moos::GobyCourseAUVManager* goby3_course::moos::GobyCourseAUVManager::get_instance()
{
    if (!inst_)
    {
        global_config.reset(new GobyCourseAUVManagerConfig);
        inst_ = new goby3_course::moos::GobyCourseAUVManager(*global_config);
    }
    return inst_;
}

void goby3_course::moos::GobyCourseAUVManager::delete_instance() { delete inst_; }

goby3_course::moos::GobyCourseAUVManager::GobyCourseAUVManager(GobyCourseAUVManagerConfig& cfg)
    : GobyMOOSApp(&cfg),
      cfg_(cfg),
      geodesy_({cfg_.common().lat_origin() * boost::units::degree::degrees,
                cfg_.common().lon_origin() * boost::units::degree::degrees})
{
    glog.add_group("nav", goby::util::Colors::lt_blue);

    subscribe_our_nav();
}

goby3_course::moos::GobyCourseAUVManager::~GobyCourseAUVManager() {}

void goby3_course::moos::GobyCourseAUVManager::loop() {}

void goby3_course::moos::GobyCourseAUVManager::subscribe_our_nav()
{
    subscribe_pb("IFS_NODE_STATUS", &GobyCourseAUVManager::handle_our_nav, this);
}

void goby3_course::moos::GobyCourseAUVManager::handle_our_nav(
    const goby::middleware::frontseat::protobuf::NodeStatus& frontseat_nav)
{
    glog.is_verbose() && glog << group("nav") << "Received frontseat NodeStatus: "
                              << frontseat_nav.ShortDebugString() << std::endl;

    goby3_course::dccl::NavigationReport dccl_nav =
        nav_convert(frontseat_nav, cfg_.vehicle_id(), geodesy_);
    glog.is_verbose() && glog << group("nav")
                              << "^^ Converts to DCCL nav: " << dccl_nav.ShortDebugString()
                              << std::endl;

    publish_pb("NAVIGATION_REPORT_OUT", dccl_nav);
}
