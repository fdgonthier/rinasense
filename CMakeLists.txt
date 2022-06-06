# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(TARGET_TYPE "freertos_idf"
  CACHE STRING "Type of build: 'linux_glibc', 'freertos_idf'")

if(${TARGET_TYPE} STREQUAL "freertos_idf")
  if ($ENV{IDF_PATH} NOT STREQUAL "")
    message("Configuring: ESP-IDF build")
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    add_compile_definitions(configUSE_POSIX_ERRNO=1)
    project(RINA_sensor)
  else()
    message(FATAL_ERROR "The 'freertos_idf' target needs the IDF_PATH environment variable to be defined")
  endif()
elseif (${TARGET_TYPE} STREQUAL "linux_glib")
  project(RINA_sensor)

  message("Configuring: POSIX build")

  include(CTest)

  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(CMAKE_C_COMPILER "gcc")

  #
  # Set all include path variables here so as to avoid circular
  # dependencies problem below.
  #
  set(ARP826_INCLUDES           "${CMAKE_CURRENT_SOURCE_DIR}/components/ARP826/include")
  set(BufferManagement_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/BufferManagement/include")
  set(CdapProto_INCLUDES        "${CMAKE_CURRENT_SOURCE_DIR}/components/CdapProto/include")
  set(Common_INCLUDES           "${CMAKE_CURRENT_SOURCE_DIR}/components/Common/include")
  set(configRINA_INCLUDES       "${CMAKE_CURRENT_SOURCE_DIR}/components/configRINA/include")
  set(configSensor_INCLUDES     "${CMAKE_CURRENT_SOURCE_DIR}/components/configSensor/include")
  set(EFCP_INCLUDES             "${CMAKE_CURRENT_SOURCE_DIR}/components/EFCP/include")
  set(Enrollment_INCLUDES       "${CMAKE_CURRENT_SOURCE_DIR}/components/Enrollment/include")
  set(FlowAllocator_INCLUDES    "${CMAKE_CURRENT_SOURCE_DIR}/components/FlowAllocator/include")
  set(IPCP_INCLUDES             "${CMAKE_CURRENT_SOURCE_DIR}/components/IPCP/include")
  set(NetworkInterface_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/NetworkInterface/include")
  set(RINA_API_INCLUDES         "${CMAKE_CURRENT_SOURCE_DIR}/components/RINA_API/include")
  set(Ribd_INCLUDES             "${CMAKE_CURRENT_SOURCE_DIR}/components/Ribd/include")
  set(Rmt_INCLUDES              "${CMAKE_CURRENT_SOURCE_DIR}/components/Rmt/include")
  set(ShimIPCP_INCLUDES         "${CMAKE_CURRENT_SOURCE_DIR}/components/ShimIPCP/include")

  set(mock_IPCP_INCLUDES        "${CMAKE_CURRENT_SOURCE_DIR}/components/Mocks/IPCP/include")
  set(mock_NetworkInterface_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/Mocks/NetworkInterface/include")

  set(NetworkInterface_MQ_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/NetworkInterface/mq/include")

  #
  # Component: configRINA
  #
  file(GLOB configRINA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/configRINA/*.c"
  )
  add_library(configRINA STATIC ${configRINA_SOURCES})
  target_include_directories(configRINA PUBLIC ${configRINA_INCLUDES})
  set_target_properties(configRINA PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Portability layer, target specific
  #
  include("${CMAKE_CURRENT_SOURCE_DIR}/components/Portability/${TARGET_TYPE}/CMakeLists.txt")

  #
  # Portability layer tests
  #
  include("${CMAKE_CURRENT_SOURCE_DIR}/components/Portability/tests/CMakeLists.txt")

  #
  # Mock objects
  #
  include("${CMAKE_CURRENT_SOURCE_DIR}/components/Mocks/CMakeLists.txt")

  #
  # Component: configSensor
  #
  file(GLOB configSensor_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/configSensor/*.c"
  )
  add_library(configSensor STATIC ${configSensor_SOURCES})
  target_include_directories(configSensor PUBLIC ${configSensor_INCLUDES})
  set_target_properties(configSensor PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: Common
  #
  set(Common_DIR "${CMAKE_CURRENT_SOURCE_DIR}/components/Common")
  set(Common_TEST_DIR "${Common_DIR}/tests")
  file(GLOB Common_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Common/*.c"
  )
  add_library(Common STATIC ${Common_SOURCES})
  target_include_directories(Common PUBLIC
    ${Common_INCLUDES}
    ${Portability_INCLUDES}
    ${configSensor_INCLUDES}
  )
  include("${Common_TEST_DIR}/CMakeLists.txt")

  #
  # Component: BufferManagement
  #
  set(BufferManagement_DIR "${CMAKE_CURRENT_SOURCE_DIR}/components/BufferManagement")
  set(BufferManagement_TEST_DIR "${BufferManagement_DIR}/tests")
  file(GLOB BufferManagement_SOURCES
    "${BufferManagement_DIR}/*.c"
  )
  add_library(BufferManagement STATIC ${BufferManagement_SOURCES})
  target_include_directories(BufferManagement PUBLIC
    ${BufferManagement_INCLUDES}
    ${configSensor_INCLUDES}
    ${ARP826_INCLUDES}
    ${Common_INCLUDES}
    #    ${EFCP_INCLUDES}
    ${IPCP_INCLUDES}
    #    ${NetworkInterface_INCLUDES}
    #    ${Rmt_INCLUDES}
    #    ${ShimIPCP_INCLUDES}
    ${Portability_INCLUDES}
  )
  include("${BufferManagement_TEST_DIR}/CMakeLists.txt")

  #
  # Component: CDAP
  #
  file(GLOB CdapProto_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/CdapProto/*.c"
  )
  add_library(CdapProto STATIC ${CdapProto_SOURCES})
  target_include_directories(CdapProto PUBLIC ${CdapProto_INCLUDES})
  set_target_properties(CdapProto PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: IPCP
  #
  # NOTE: WE ONLY PARTIALLY COMPILE THE PIDM FILES FOR THE MOMENT!
  set(IPCP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/components/IPCP")
  set(IPCP_TEST_DIR "${IPCP_DIR}/tests")
  file(GLOB IPCP_SOURCES "${IPCP_DIR}/pidm.c")
  add_library(IPCP STATIC ${IPCP_SOURCES})
  target_include_directories(IPCP PUBLIC
    ${configSensor_INCLUDES}
    ${configRINA_INCLUDES}
    ${IPCP_INCLUDES}
    ${Portability_INCLUDES}
    ${Common_INCLUDES}
  )
  include("${IPCP_TEST_DIR}/CMakeLists.txt")

  #
  # Component: ARP826
  #
  set(ARP826_DIR "${CMAKE_CURRENT_SOURCE_DIR}/components/ARP826")
  set(ARP826_TEST_DIR "${ARP826_DIR}/tests")
  file(GLOB ARP826_SOURCES "${ARP826_DIR}/*.c")
  add_library(ARP826 STATIC ${ARP826_SOURCES})
  target_include_directories(ARP826 PUBLIC
    ${mock_IPCP_INCLUDES}
    ${mock_NetworkInterface_INCLUDES}
    ${configSensor_INCLUDES}
    ${Common_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${ARP826_INCLUDES}
    ${IPCP_INCLUDES}
    ${Rmt_INCLUDES}
    ${ShimIPCP_INCLUDES}
    ${EFCP_INCLUDES}
    ${Portability_INCLUDES}
  )
  include("${ARP826_TEST_DIR}/CMakeLists.txt")

  #
  # Component: Rmt
  #
  file(GLOB Rmt_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Rmt/*.c"
  )
  add_library(Rmt STATIC ${Rmt_SOURCES})
  target_include_directories(Rmt PUBLIC
    ${Rmt_INCLUDES}
    ${EFCP_INCLUDES}
  )
  set_target_properties(Rmt PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: ShimIPCP
  #
  file(GLOB ShimIPCP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/ShimIPCP/*.c"
  )
  add_library(ShimIPCP STATIC ${ShimIPCP_SOURCES})
  target_include_directories(ShimIPCP PUBLIC
    ${ShimIPCP_INCLUDES}
    ${configSensor_INCLUDES}
    ${configRINA_INCLUDES}
    ${ARP826_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${EFCP_INCLUDES}
    ${IPCP_INCLUDES}
    ${NetworkInterface_INCLUDES}
    ${Rmt_INCLUDES}
  )
  set_target_properties(ShimIPCP PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: EFCP
  #
  file(GLOB EFCP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/EFCP/*.c"
  )
  add_library(EFCP STATIC ${EFCP_SOURCES})
  target_include_directories(EFCP PUBLIC
    ${ARP826_INCLUDES}
    ${IPCP_INCLUDES}
    ${EFCP_INCLUDES}
    ${Rmt_INCLUDES}
    ${configSensor_INCLUDES}
  )
  set_target_properties(EFCP PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: NetworkInterface
  #
  set(NetworkInterface_DIR "${CMAKE_CURRENT_SOURCE_DIR}/components/NetworkInterface")
  file(GLOB NetworkInterface_SOURCES
    "${NetworkInterface_DIR}/mq/*.c"
  )
  add_library(NetworkInterface STATIC ${NetworkInterface_SOURCES})
  target_include_directories(NetworkInterface PUBLIC
    ${Common_INCLUDES}
    ${configRINA_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${NetworkInterface_INCLUDES}
    ${NetworkInterface_MQ_INCLUDES}
    ${ARP826_INCLUDES}
    ${mock_IPCP_INCLUDES}
    ${IPCP_INCLUDES}
    ${configSensor_INCLUDES}
    ${Rmt_INCLUDES}
    ${ShimIPCP_INCLUDES}
    ${EFCP_INCLUDES}
    ${Portability_INCLUDES}
  )
  include("${NetworkInterface_DIR}/tests/CMakeLists.txt")

  #
  # Component: Ribd
  #
  file(GLOB Ribd_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Ribd/*.c"
  )
  add_library(Ribd STATIC ${Ribd_SOURCES})
  target_include_directories(Ribd PUBLIC
    ${configSensor_INCLUDES}
    ${configRINA_INCLUDES}
    ${ARP826_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${ShimIPCP_INCLUDES}
    ${EFCP_INCLUDES}
    ${Enrollment_INCLUDES}
    ${CdapProto_INCLUDES}
    ${Ribd_INCLUDES}
    ${RINA_API_INCLUDES}
    ${Rmt_INCLUDES}
    ${IPCP_INCLUDES}
  )
  set_target_properties(Ribd PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: Enrollment
  #
  file(GLOB Enrollment_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Enrollment/*.c"
  )
  add_library(Enrollment STATIC ${Enrollment_SOURCES})
  target_include_directories(Enrollment PUBLIC
    ${configSensor_INCLUDES}
    ${configRINA_INCLUDES}
    ${ARP826_INCLUDES}
    ${CdapProto_INCLUDES}
    ${Enrollment_INCLUDES}
    ${FlowAllocator_INCLUDES}
    ${IPCP_INCLUDES}
    ${Ribd_INCLUDES}
    ${RINA_API_INCLUDES}
    ${Rmt_INCLUDES}
  )
  set_target_properties(Enrollment PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: FlowAllocator
  #
  file(GLOB FlowAllocator_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/FlowAllocator/*.c"
  )
  add_library(FlowAllocator STATIC ${FlowAllocator_SOURCES})
  target_include_directories(FlowAllocator PUBLIC
    ${FlowAllocator_INCLUDES}
    ${configSensor_INCLUDES}
    ${configRINA_INCLUDES}
    ${ARP826_INCLUDES}
    ${CdapProto_INCLUDES}
    ${Enrollment_INCLUDES}
    ${IPCP_INCLUDES}
    ${Ribd_INCLUDES}
    ${RINA_API_INCLUDES}
    ${Rmt_INCLUDES}
  )
  set_target_properties(FlowAllocator PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: RINA_API
  #
  file(GLOB RINA_API_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/RINA_API/*.c"
  )
  add_library(RINA_API STATIC ${RINA_API_SOURCES})
  target_include_directories(RINA_API PUBLIC
    ${RINA_API_INCLUDES}
    ${configSensor_INCLUDES}
    ${ARP826_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${EFCP_INCLUDES}
    ${IPCP_INCLUDES}
    ${Rmt_INCLUDES}
    ${ShimIPCP_INCLUDES}
  )
  set_target_properties(RINA_API PROPERTIES EXCLUDE_FROM_ALL TRUE)

  # -> CFLAGS := -ggdb3 -O0 -DprojCOVERAGE_TEST=0 -D_WINDOWS_
  add_definitions(-DprojCOVERAGE_TEST=0)
  add_definitions(-D_WINDOWS_)
  #add_definitions(-DESP_PLATFORM)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3 -O0 -include stdbool.h")

  # -> LDFLAGS := -ggdb3 -O0 -pthread
  # pthread included after
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -ggdb3 -O0")

  # Create executable with demo code files
  # add_executable(${PROJECT_NAME}
  #   ${CMAKE_CURRENT_SOURCE_DIR}/main/main.c
  #   ${CMAKE_CURRENT_SOURCE_DIR}/port/rslist.c
  # )
  # include_directories(
  #   ${CMAKE_CURRENT_SOURCE_DIR}/Common/include
  #   ${CMAKE_CURRENT_SOURCE_DIR}/components/Common/include
  #   ${CMAKE_CURRENT_SOURCE_DIR}/port/freertos/include
  # )


  # target_link_libraries(${PROJECT_NAME}
  #   freertos
  #   CdapProto
  #   configRINA
  #   configSensor
  #   IPCP
  # )

  ## Add pthread
  #target_link_libraries(${PROJECT_NAME} pthread)
else()
  message("Unknown target ${TARGET_TYPE}")
endif()
