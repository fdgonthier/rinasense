# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(FREERTOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS/FreeRTOS"
  CACHE PATH "Directory of FreeRTOS sources")
set(FREERTOS_PLUS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS/FreeRTOS-Plus"
  CACHE PATH "Directory of FreeRTOS-Plus sources")
set(FREERTOS_POSIX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS-Plus-POSIX"
  CACHE PATH "Directory of FreeRTOS-Plus/POSIX sources")
set(TARGET_TYPE "freertos_idf"
  CACHE STRING "Type of build: 'linux_glibc', 'freertos_idf'")

if(${TARGET_TYPE} STREQUAL "freertos_idf")
  message("Configuring: ESP-IDF build")

  include($ENV{IDF_PATH}/tools/cmake/project.cmake)

  add_compile_definitions(configUSE_POSIX_ERRNO=1)

  project(RINA_sensor)

elseif (${TARGET_TYPE} STREQUAL "linux_glib")
  project(RINA_sensor)

  message("Configuring: POSIX build")

  include(CTest)

  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(CMAKE_C_COMPILER "gcc")

  ## FIXME: This is a mess.
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/components/Common/include
  )

  ##
  ## FreeRTOS
  ##
  set(FREERTOS_INCLUDES
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/freertos
    ${FREERTOS_DIR}/Source/include
    ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix
    ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/utils
    #${FREERTOS_DIR}/Demo/Common/include
    #${FREERTOS_DIR}/Demo/Posix_GCC
  )
  set(FREERTOS_POSIX_INCLUDES
    ${FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include
    ${FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include/portable
    ${FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include/portable/pc/windows
    ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix
    ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/utils
    ${FREERTOS_POSIX_DIR}/include
    ${FREERTOS_POSIX_DIR}/include/private
  )
  set(FREERTOS_TRACE_INCLUDES
    ${FREERTOS_PLUS_DIR}/Source/FreeRTOS-Plus-Trace/Include
  )

  # FreeRTOS + Trace
  file(GLOB FREERTOS_TRACE_SOURCES
    ${FREERTOS_PLUS_DIR}/Source/FreeRTOS-Plus-Trace/*.c
  )
  # FreeRTOS + POSIX
  file(GLOB FREERTOS_POSIX_SOURCES
    ${FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/*.c
  )
  # FreeRTOS
  file(GLOB FREERTOS_SOURCES
    ${FREERTOS_DIR}/Source/*.c
    ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/*.c
    ${FREERTOS_PLUS_DIR}/Source/*.c
  )

  # Link together
  add_library(freertos STATIC
    ${FREERTOS_SOURCES}
    ${FREERTOS_POSIX_SOURCES}
    ${FREERTOS_TRACE_SOURCES}
  )
  target_include_directories(freertos PUBLIC
    ${FREERTOS_INCLUDES}
    ${FREERTOS_POSIX_INCLUDES}
    ${FREERTOS_TRACE_INCLUDES}
  )
  set_target_properties(freertos PROPERTIES EXCLUDE_FROM_ALL TRUE)

  # Create a link to the 'freertos' include directory so that #include
  # statement such as #include "freertos/FreeRTOS.h" work.
  file(CREATE_LINK ${FREERTOS_DIR}/Source/include ${CMAKE_CURRENT_BINARY_DIR}/freertos SYMBOLIC)

  #
  # Component: configRINA
  #
  file(GLOB configRINA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/configRINA/*.c"
  )
  set(configRINA_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/configRINA/include")
  add_library(configRINA STATIC ${configRINA_SOURCES})
  target_include_directories(configRINA PUBLIC ${configRINA_INCLUDES})
  set_target_properties(configRINA PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Portability layer
  #
  include("${CMAKE_CURRENT_SOURCE_DIR}/components/Portability/${TARGET_TYPE}/CMakeLists.txt")

  #
  # Component: configSensor
  #
  file(GLOB configSensor_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/configSensor/*.c"
  )
  set(configSensor_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/configSensor/include")
  add_library(configSensor STATIC ${configSensor_SOURCES})
  target_include_directories(configSensor PUBLIC ${configSensor_INCLUDES})
  set_target_properties(configSensor PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: Common
  #
  set(Common_DIR "${CMAKE_CURRENT_SOURCE_DIR}/components/Common")
  set(Common_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/Common/include")
  set(Common_TEST_DIR "${Common_DIR}/tests")

  file(GLOB Common_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Common/*.c"
  )

  add_library(Common STATIC ${Common_SOURCES})
  target_include_directories(Common PUBLIC
    ${Common_INCLUDES}
    ${portability_INCLUDES}
    ${configSensor_INCLUDES}
  )
  #set_target_properties(Common PROPERTIES EXCLUDE_FROM_ALL TRUE)
  include("${Common_TEST_DIR}/CMakeLists.txt")

  #
  # Component: CDAP
  #
  file(GLOB CdapProto_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/CdapProto/*.c"
  )
  set(CdapProto_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/CdapProto/include")
  add_library(CdapProto STATIC ${CdapProto_SOURCES})
  target_include_directories(CdapProto PUBLIC ${CdapProto_INCLUDES})
  set_target_properties(CdapProto PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: IPCP
  #
  file(GLOB IPCP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/IPCP/*.c"
  )
  set(IPCP_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/IPCP/include")
  add_library(IPCP STATIC ${IPCP_SOURCES})
  target_include_directories(IPCP PUBLIC
    ${configSensor_INCLUDES}
    ${configRINA_INCLUDES}
    ${IPCP_INCLUDES}
    ${FREERTOS_INCLUDES}
    ${ARP826_INCLUDES}
    ${Rmt_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${ShimIPCP_INCLUDES}
    ${EFCP_INCLUDES}
    ${Ribd_INCLUDES}
    ${RINA_API_INCLUDES}
    ${NetworkInterface_INCLUDES}
    ${Enrollment_INCLUDES}
  )
  set_target_properties(IPCP PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: ARP826
  #
  file(GLOB ARP826_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/ARP826/*.c"
  )
  set(ARP826_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/ARP826/include")
  add_library(ARP826 STATIC ${ARP826_SOURCES})
  target_include_directories(ARP826 PUBLIC
    ${configSensor_INCLUDES}
    ${NetworkInterface_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${ARP826_INCLUDES}
    ${IPCP_INCLUDES}
    ${FREERTOS_INCLUDES}
    ${Rmt_INCLUDES}
    ${ShimIPCP_INCLUDES}
    ${EFCP_INCLUDES}
  )
  set_target_properties(ARP826 PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: Rmt
  #
  file(GLOB Rmt_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Rmt/*.c"
  )
  set(Rmt_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/Rmt/include")
  add_library(Rmt STATIC ${Rmt_SOURCES})
  target_include_directories(Rmt PUBLIC
    ${Rmt_INCLUDES}
    ${EFCP_INCLUDES}
    ${FREERTOS_INCLUDES}
  )
  set_target_properties(Rmt PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: BufferManagement
  #
  set(BufferManagement_DIR "${CMAKE_CURRENT_SOURCE_DIR}/components/BufferManagement")
  set(BufferManagement_TEST_DIR "${BufferManagement_DIR}/tests")
  file(GLOB BufferManagement_SOURCES
    "${BufferManagement_DIR}/*.c"
  )
  set(BufferManagement_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/BufferManagement/include")
  add_library(BufferManagement STATIC ${BufferManagement_SOURCES})
  target_include_directories(BufferManagement PUBLIC
    ${BufferManagement_INCLUDES}
    ${configSensor_INCLUDES}
    ${ARP826_INCLUDES}
    #    ${EFCP_INCLUDES}
    ${IPCP_INCLUDES}
    #    ${NetworkInterface_INCLUDES}
    #    ${Rmt_INCLUDES}
    #    ${ShimIPCP_INCLUDES}
    ${FREERTOS_INCLUDES}
    ${FREERTOS_POSIX_INCLUDES}
    ${FREERTOS_TRACE_INCLUDES}
    ${portability_INCLUDES}
  )
  include("${BufferManagement_TEST_DIR}/CMakeLists.txt")

  #
  # Component: ShimIPCP
  #
  file(GLOB ShimIPCP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/ShimIPCP/*.c"
  )
  set(ShimIPCP_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/ShimIPCP/include")
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
    ${FREERTOS_INCLUDES}
  )
  set_target_properties(ShimIPCP PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: EFCP
  #
  file(GLOB EFCP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/EFCP/*.c"
  )
  set(EFCP_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/EFCP/include")
  add_library(EFCP STATIC ${EFCP_SOURCES})
  target_include_directories(EFCP PUBLIC
    ${ARP826_INCLUDES}
    ${IPCP_INCLUDES}
    ${EFCP_INCLUDES}
    ${Rmt_INCLUDES}
    ${configSensor_INCLUDES}
    ${FREERTOS_INCLUDES}
  )
  set_target_properties(EFCP PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: NetworkInterface
  #
  file(GLOB NetworkInterface_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/NetworkInterface/*.c"
  )
  set(NetworkInterface_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/NetworkInterface/include")
  add_library(NetworkInterface STATIC ${NetworkInterface_SOURCES})
  target_include_directories(NetworkInterface PUBLIC
    ${configRINA_INCLUDES}
    ${BufferManagement_INCLUDES}
    ${NetworkInterface_INCLUDES}
    ${FREERTOS_INCLUDES}
    ${ARP826_INCLUDES}
    ${IPCP_INCLUDES}
    ${configSensor_INCLUDES}
    ${Rmt_INCLUDES}
    ${ShimIPCP_INCLUDES}
    ${EFCP_INCLUDES}
  )
  set_target_properties(NetworkInterface PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: Ribd
  #
  file(GLOB Ribd_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Ribd/*.c"
  )
  set(Ribd_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/Ribd/include")
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
    ${FREERTOS_INCLUDES}
  )
  set_target_properties(Ribd PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: Enrollment
  #
  file(GLOB Enrollment_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/Enrollment/*.c"
  )
  set(Enrollment_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/Enrollment/include")
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
    ${FREERTOS_INCLUDES}
  )
  set_target_properties(Enrollment PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: FlowAllocator
  #
  file(GLOB FlowAllocator_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/FlowAllocator/*.c"
  )
  set(FlowAllocator_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/FlowAllocator/include")
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
    ${FREERTOS_INCLUDES}
  )
  set_target_properties(FlowAllocator PROPERTIES EXCLUDE_FROM_ALL TRUE)

  #
  # Component: RINA_API
  #
  file(GLOB RINA_API_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/components/RINA_API/*.c"
  )
  set(RINA_API_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/components/RINA_API/include")
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
    ${FREERTOS_INCLUDES}
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
