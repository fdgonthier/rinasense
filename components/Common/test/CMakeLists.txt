if("$ENV{TARGET_TYPE}" STREQUAL "freertos_idf")
  idf_component_register(SRC_DIRS "."
    INCLUDE_DIRS "."
    REQUIRES unity Common)
else()
  # Linked lists
  add_executable(test_lists "${CMAKE_CURRENT_LIST_DIR}/test_lists.c")
  target_link_libraries(test_lists
    Common
    Portability
    unity
  )

  add_test(
    NAME test_lists
    COMMAND "./test_lists"
  )

  # RINA names
  add_executable(test_rina_name "${CMAKE_CURRENT_LIST_DIR}/test_rina_name.c")
  target_link_libraries(test_rina_name
    RINA
    NetworkInterface_MQ
    Portability
    unity
  )

  add_test(
    NAME test_rina_name
    COMMAND "./test_rina_name"
  )

  # GHA and GPA objects
  add_executable(test_rina_gpha "${CMAKE_CURRENT_LIST_DIR}/test_rina_gpha.c")
  target_link_libraries(test_rina_gpha
    RINA
    NetworkInterface_MQ
    Portability
    unity
  )

  add_test(
    NAME test_rina_gpha
    COMMAND "./test_rina_gpha"
  )

  # Bit array
  add_executable(test_bit_array "${CMAKE_CURRENT_LIST_DIR}/test_bit_array.c")
  target_link_libraries(test_bit_array
    RINA
    NetworkInterface_MQ
    Portability
    unity
  )

  add_test(
    NAME test_bit_array
    COMMAND "./test_bit_array"
  )

  # Number manager
  add_executable(test_num_mgr "${CMAKE_CURRENT_LIST_DIR}/test_num_mgr.c")
  target_link_libraries(test_num_mgr
    RINA
    NetworkInterface_MQ
    Portability
    unity
  )

  add_test(
    NAME test_num_mgr
    COMMAND "./test_num_mgr"
  )

  # IDs
  add_executable(test_ids "${CMAKE_CURRENT_LIST_DIR}/test_ids.c")
  target_link_libraries(test_ids
    RINA
    NetworkInterface_MQ
    Portability
    unity
  )

  add_test(
    NAME "test_ids"
    COMMAND "./test_ids"
  )

  # Queues
  add_executable(test_queue "${CMAKE_CURRENT_LIST_DIR}/test_queue.c")
  target_link_libraries(test_queue
    RINA
    NetworkInterface_MQ
    Portability
    unity
  )

  add_test(
    NAME "test_queue"
    COMMAND "./test_queue"
  )

  # Netbufs
  add_executable(test_netbuf "${CMAKE_CURRENT_LIST_DIR}/test_netbuf.c")
  target_link_libraries(test_netbuf
    Common
    Portability
    unity
  )

  add_test(
    NAME test_netbuf
    COMMAND "./test_netbuf"
  )

  # Data packer
  add_executable(test_datapacker "${CMAKE_CURRENT_LIST_DIR}/test_datapacker.c")
  target_link_libraries(test_datapacker
    Common
    Portability
    unity
  )

  add_test(
    NAME test_datapacker
    COMMAND "./test_datapacker"
  )

  # Array list
  add_executable(test_arraylist "${CMAKE_CURRENT_LIST_DIR}/test_arraylist.c")
  target_link_libraries(test_arraylist
    Common
    Portability
    unity
  )

  add_test(
    NAME test_arraylist
    COMMAND "./test_arraylist"
  )
endif()
