add_library(goby3_course_nav SHARED convert.cpp)
target_link_libraries(goby3_course_nav
  contacts geometry mbutil 
  goby
  goby3_course_messages)
