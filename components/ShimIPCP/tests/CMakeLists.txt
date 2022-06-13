add_executable(test_shim_ipcp "${CMAKE_CURRENT_LIST_DIR}/test_shim_ipcp.c")
target_link_libraries(test_shim_ipcp
  ShimIPCP
  portability
  ARP826
  Rmt
  mock_IPCP
  rt
  pthread
  NetworkInterface
  BufferManagement
  IPCP
  EFCP
  mock_Ribd
  CdapProto
  Common
)

add_test(
  NAME test_shim_ipcp
  COMMAND "./test_shim_ipcp"
)
