set(PROCJET_VERSION 1.0.0)
set(APP_BOARD_SRV "board_iot_srv")
set(APP_IOT_DAEMON "iot_daemon")
set(APP_IOT_SD     "sd_test")
set(APP_IOT_MOUNT_SD     "sdcard_monitor")

set(APP_BOARD_LIB
  ${COMMON_LINK_LIB}
  ${UTILS_LIB}
  ${MODULE_LIB}
)

add_executable(${APP_BOARD_SRV} board_srv.c)
target_link_libraries(${APP_BOARD_SRV}
  -Wl,--start-group
  ${APP_BOARD_LIB}
  -Wl,--end-group
)

add_executable(${APP_IOT_DAEMON} iot_daemon.c)
target_link_libraries(${APP_IOT_DAEMON}
  -Wl,--start-group
  ${APP_BOARD_LIB}
  -Wl,--end-group
)

add_executable(${APP_IOT_SD} sd.c)
target_link_libraries(${APP_IOT_SD}
  -Wl,--start-group
  udev
  -Wl,--end-group
 )

 add_executable(${APP_IOT_MOUNT_SD} mount_sd.c sd_log.c)
 target_link_libraries(${APP_IOT_MOUNT_SD}
 -Wl,--start-group
 pthread
 -Wl,--end-group
)