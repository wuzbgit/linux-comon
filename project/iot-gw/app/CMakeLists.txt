set(PROCJET_VERSION 1.0.0)
set(APP_BOARD_SRV "board_iot_srv")
set(APP_IOT_DAEMON "iot_daemon")

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
