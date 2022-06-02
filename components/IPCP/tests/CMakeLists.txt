add_executable(test_ipcp_pidm "${CMAKE_CURRENT_LIST_DIR}/test_ipcp_pidm.c")
target_link_libraries(test_ipcp_pidm
  IPCP
  Common
  portability
)

add_test(
  NAME test_ipcp_pidm
  COMMAND "./test_ipcp_pidm"
)
