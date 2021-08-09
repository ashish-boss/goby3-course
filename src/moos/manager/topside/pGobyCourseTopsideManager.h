#ifndef GOBY3_COURSE_MOOS_TOPSIDE_MANAGER_H
#define GOBY3_COURSE_MOOS_TOPSIDE_MANAGER_H

#include "goby/moos/goby_moos_app.h"
#include "goby/util/geodesy.h"

#include "goby3-course/messages/nav_dccl.pb.h"
#include "goby3-course/nav/convert.h"

#include "config.pb.h"

namespace goby3_course
{
namespace moos
{
class GobyCourseTopsideManager : public goby::moos::GobyMOOSApp
{
  public:
    static GobyCourseTopsideManager* get_instance();
    static void delete_instance();

  private:
    GobyCourseTopsideManager(protobuf::GobyCourseTopsideManagerConfig& cfg);
    ~GobyCourseTopsideManager();

    void loop() override;
    void subscribe_nav_from_usv();
    void handle_incoming_nav(const goby3_course::dccl::NavigationReport& dccl_nav);

  private:
    protobuf::GobyCourseTopsideManagerConfig& cfg_;
    static GobyCourseTopsideManager* inst_;

    goby::util::UTMGeodesy geodesy_;
};
} // namespace moos
} // namespace goby3_course

#endif
