#ifndef GOBY3_COURSE_MOOS_USV_MANAGER_H
#define GOBY3_COURSE_MOOS_USV_MANAGER_H

#include "goby/moos/goby_moos_app.h"
#include "goby/util/geodesy.h"

#include "goby3-course/messages/nav_dccl.pb.h"
#include "goby3-course/nav/convert.h"

#include "config.pb.h"

namespace goby3_course
{
namespace moos
{
class GobyCourseUSVManager : public goby::moos::GobyMOOSApp
{
  public:
    static GobyCourseUSVManager* get_instance();
    static void delete_instance();

  private:
    GobyCourseUSVManager(protobuf::GobyCourseUSVManagerConfig& cfg);
    ~GobyCourseUSVManager();

    void loop() override;

    void subscribe_our_nav();
    void subscribe_auv_nav();

    void handle_our_nav(const goby::middleware::frontseat::protobuf::NodeStatus& frontseat_nav);
    void handle_auv_nav(const goby3_course::dccl::NavigationReport& dccl_nav);

  private:
    protobuf::GobyCourseUSVManagerConfig& cfg_;
    static GobyCourseUSVManager* inst_;

    goby::util::UTMGeodesy geodesy_;
};
} // namespace moos
} // namespace goby3_course

#endif
