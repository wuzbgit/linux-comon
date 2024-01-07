set(TARGET_NAME "elec_test")
set(PROCJET_VERSION 1.0.0)

set(UNITTEST_LIB
  ${COMMON_LINK_LIB}
  ${UTILS_LIB}
  ${MODULE_LIB}
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} APP_SRC_FILE)
add_executable(${TARGET_NAME} ${APP_SRC_FILE})

target_link_libraries(${TARGET_NAME}
  -Wl,--start-group
  ${UNITTEST_LIB}
  -Wl,--end-group
)
