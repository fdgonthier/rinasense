add_executable(bftest1 "${CMAKE_CURRENT_LIST_DIR}/bftest1.c")
target_link_libraries(bftest1
  BufferManagement
  portability
  pthread
)

add_test(
  NAME bftest1
  COMMAND "./bftest1"
)
