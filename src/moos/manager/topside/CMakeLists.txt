set(APP pGobyCourseTopsideManager)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR} config.proto)

add_executable(${APP}
  ${APP}.cpp
  ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(${APP}
  goby_moos
  goby3_course_nav
  goby3_course_messages)
