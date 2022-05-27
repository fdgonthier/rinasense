add_executable(test_rina_name "${CMAKE_CURRENT_LIST_DIR}/test_rina_name.c")
target_link_libraries(test_rina_name
  Common
  portability
)

add_executable(test_rina_gpha "${CMAKE_CURRENT_LIST_DIR}/test_rina_gpha.c")
target_link_libraries(test_rina_gpha
  Common
  portability
)

add_test(
  NAME test_rina_name
  COMMAND "./test_rina_name"
)

add_test(
  NAME test_rina_gpha
  COMMAND "./test_rina_gpha"
)
