set(PROCJET_VERSION 1.0.0)
set(APP_VIDEO_DAEMON "video_daemon")

message("ROOTFS=${ROOTFS}")

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
set(CMAKE_PREFIX_PATH ${CMAKE_SYSROOT}/usr)

find_package(PkgConfig REQUIRED)

set(PKG_CONFIG_SYSROOT_DIR "${CMAKE_SYSROOT}")
set(PKG_CONFIG_LIBDIR "${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/pkgconfig")
set(PKG_CONFIG_PATH "${CMAKE_SYSROOT}/usr/share/pkgconfig")

pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)

include_directories(${GSTREAMER_INCLUDE_DIRS} ${GSTREAMER_APP_INCLUDE_DIRS})
link_directories(${GSTREAMER_LIBRARY_DIRS} ${GSTREAMER_APP_LIBRARY_DIRS})

set(APP_BOARD_LIB
  ${COMMON_LINK_LIB}
  ${UTILS_LIB}
  ${MODULE_LIB}
)

add_executable(${APP_VIDEO_DAEMON} video_daemon.c tcp_server.c video.c alg.c gstreamer_pipe.c ringbuffer.c)
target_link_libraries(${APP_VIDEO_DAEMON}
  -Wl,--start-group
  ${APP_BOARD_LIB}
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_APP_LIBRARIES}
  pthread
  -Wl,--end-group
)

