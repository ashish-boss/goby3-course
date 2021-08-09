#ifndef GOBY3_COURSE_MOOS_AUV_MANAGER_H
#define GOBY3_COURSE_MOOS_AUV_MANAGER_H

#include "goby/moos/goby_moos_app.h"
#include "goby/util/geodesy.h"

#include "goby3-course/messages/nav_dccl.pb.h"
#include "goby3-course/nav/convert.h"

#include "config.pb.h"

namespace goby3_course
{
namespace moos
{
class GobyCourseAUVManager : public goby::moos::GobyMOOSApp
{
  public:
    static GobyCourseAUVManager* get_instance();
    static void delete_instance();

  private:
    GobyCourseAUVManager(protobuf::GobyCourseAUVManagerConfig& cfg);
    ~GobyCourseAUVManager();

    void loop() override;

    void subscribe_our_nav();
    void handle_our_nav(const goby::middleware::frontseat::protobuf::NodeStatus& frontseat_nav);

  private:
    protobuf::GobyCourseAUVManagerConfig& cfg_;
    static GobyCourseAUVManager* inst_;

    goby::util::UTMGeodesy geodesy_;
};
} // namespace moos
} // namespace goby3_course

#endif
