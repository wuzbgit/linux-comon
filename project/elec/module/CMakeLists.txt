set(TARGET_NAME "elecmod")

file(GLOB_RECURSE LIB_SRC_C_FILE "src/*.c")
file(GLOB_RECURSE LIB_SRC_CPP_FILE "src/*.cpp")
set(LIB_SRC_FILE ${LIB_SRC_C_FILE} ${LIB_SRC_CPP_FILE})
message("elec module LIB_SRC_FILE=${LIB_SRC_FILE}")
add_library(${TARGET_NAME} STATIC ${LIB_SRC_FILE})
