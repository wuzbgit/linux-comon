set(PROCJET_VERSION 1.0.0)
set(APP_RS485_TEST "rs485_test")
set(APP_RS422_TEST "rs422_test")
set(APP_AI_TEST "ai_test")
set(APP_DI_TEST "di_test")
set(APP_DO_TEST "do_test")
set(APP_RTC_TEST "rtc_test")
set(APP_RF_TEST "rf_test")
set(APP_ETH_TEST "eth_test")
set(APP_UNIT_TEST "unit_test")
set(APP_LED_TEST "led_blink_test")
set(APP_MB_TEST "mb_test")

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

add_executable(${APP_RS422_TEST} rs422_test.c)
target_link_libraries(${APP_RS422_TEST}
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

add_executable(${APP_DI_TEST} di_test.c)
target_link_libraries(${APP_DI_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)

add_executable(${APP_AI_TEST} ai_test.c)
target_link_libraries(${APP_AI_TEST}
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

add_executable(${APP_RF_TEST} rf_test.c)
target_link_libraries(${APP_RF_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)

add_executable(${APP_ETH_TEST} eth_test.c)
target_link_libraries(${APP_ETH_TEST}
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

add_executable(${APP_UNIT_TEST} unit_test.c)
target_link_libraries(${APP_UNIT_TEST}
  -Wl,--start-group
  ${HARDWARE_LIB}
  -Wl,--end-group
)


# add_executable(${APP_MB_TEST} mb_test.cpp)
# target_link_libraries(${APP_MB_TEST}
#   -Wl,--start-group
#   ${HARDWARE_LIB}
#   -Wl,--end-group
# )