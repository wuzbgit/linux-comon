cmake_minimum_required (VERSION 2.6)
project (elec)
  
set(ROOTFS_PATH                  ${OUT_ROOTFS_DIR})
set(UTILS_PATH                   ${TOP_DIR}/common/utils)
set(SERVICE_PATH                 ${TOP_DIR}/common/service)
set(ARCHIVE_PATH                 ${OUT_DIR}/archive)
set(PROJECT_MODULE_PATH          ${PROJECT_DIR}/module)
set(PROJECT_TEST_PATH            ${PROJECT_DIR}/test)


message(" CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
message(" ROOTFS = ${ROOTFS}")
message(" ROOTFS_PATH = ${ROOTFS_PATH}")
message(" UTILS_PATH = ${UTILS_PATH}")
message(" SERVICE_PATH = ${ARCHIVE_PATH}")
message(" PROJECT_MODULE_PATH = ${PROJECT_MODULE_PATH}")
message(" PROJECT_TEST_PATH = ${PROJECT_TEST_PATH}")
message(" CMAKE_SYSROOT = ${CMAKE_SYSROOT}")
message(" CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")

include_directories(${ROOTFS}/usr/include ${ROOTFS}/usr/include/base)
include_directories(${UTILS_PATH})
include_directories(${SERVICE_PATH}/include)
include_directories(${PROJECT_MODULE_PATH}/include)

set(MAKE_COMPILE_CFLAGS
  " \
  -Wall \
  -Werror \
  -pipe \
  -D_GNU_SOURCE \
  "
)

set(COMMON_LINK_LIB
  m
  pthread
  modbus
)

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_SHARED_LINKER_FLASG "")

# set(CMAKE_C_FLAGS ${MAKE_COMPILE_CFLAGS})
# set(CMAKE_CXX_FLAGS ${MAKE_COMPILE_CFLAGS})
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -s")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -s")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOTFS_PATH}/opt/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOTFS_PATH}/usr/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_PATH}) 

if(EXISTS ${UTILS_PATH})
  add_subdirectory(${UTILS_PATH} build_utils)
  set(UTILS_LIB "utils")
endif()

if(("${BUILD_MODULE}" MATCHES "all") OR ("${BUILD_MODULE}" MATCHES "app") OR ("${BUILD_MODULE}" MATCHES "test") OR ("${BUILD_MODULE}" MATCHES "module"))
  if(EXISTS ${PROJECT_MODULE_PATH})
    add_subdirectory(${PROJECT_MODULE_PATH} build_modules)
    set(MODULE_LIB "elecmod")
  endif()
endif()

if(("${BUILD_MODULE}" MATCHES "all") OR ("${BUILD_MODULE}" MATCHES "test"))
  if(EXISTS ${PROJECT_TEST_PATH})
    add_subdirectory(${PROJECT_TEST_PATH} build_test)
  endif()
endif()
