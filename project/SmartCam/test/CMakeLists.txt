set(PROCJET_VERSION 1.0.0)
set(APP_RS485_TEST "rs485_test")
set(APP_VIDEO_TEST "video_server")
set(APP_DO_TEST "do_test")
set(APP_RTC_TEST "rtc_test")
set(APP_LED_TEST "led_test")
set(APP_SENSOR_TEST "sensor_test")

set(HARDWARE_LIB
  ${COMMON_LINK_LIB}
  ${UTILS_LIB}
  ${MODULE_LIB}
)

add_executable(${APP_RS485_TEST} rs485_test.c)
target_link_libraries(${APP_RS485_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)

add_executable(${APP_RTC_TEST} rtc_test.c)
target_link_libraries(${APP_RTC_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)

add_executable(${APP_DO_TEST} do_test.c)
target_link_libraries(${APP_DO_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)

add_executable(${APP_LED_TEST} led_test.c)
target_link_libraries(${APP_LED_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)

add_executable(${APP_VIDEO_TEST} video_server.c)

add_executable(${APP_SENSOR_TEST} sensor_test.c)
target_link_libraries(${APP_SENSOR_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)
