#ifndef GOBY3_COURSE_MOOS_PATTERN_H
#define GOBY3_COURSE_MOOS_PATTERN_H

#include "goby/moos/goby_moos_app.h"

#include "config.pb.h"

namespace goby3_course
{
namespace moos
{
class GobyCoursePattern : public goby::moos::GobyMOOSApp
{
  public:
    static GobyCoursePattern* get_instance();
    static void delete_instance();

  private:
    GobyCoursePattern(protobuf::GobyCoursePatternConfig& cfg);
    ~GobyCoursePattern();

    void loop(); // from GobyMOOSApp

    void handle_db_time(const CMOOSMsg& msg);

  private:
    protobuf::GobyCoursePatternConfig& cfg_;
    static GobyCoursePattern* inst_;
};
} // namespace moos
} // namespace goby3_course

#endif
