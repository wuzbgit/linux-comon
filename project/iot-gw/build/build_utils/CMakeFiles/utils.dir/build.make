# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/toolchain/rk3576/aarch64-buildroot-linux-gnu_sdk-buildroot/bin/cmake

# The command to remove a file.
RM = /opt/toolchain/rk3576/aarch64-buildroot-linux-gnu_sdk-buildroot/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /opt/Project/linux-com/project/iot-gw

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /opt/Project/linux-com/project/iot-gw/build

# Include any dependencies generated for this target.
include build_utils/CMakeFiles/utils.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include build_utils/CMakeFiles/utils.dir/compiler_depend.make

# Include the progress variables for this target.
include build_utils/CMakeFiles/utils.dir/progress.make

# Include the compile flags for this target's objects.
include build_utils/CMakeFiles/utils.dir/flags.make

build_utils/CMakeFiles/utils.dir/debug/debug.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/debug/debug.c.o: /opt/Project/linux-com/common/utils/debug/debug.c
build_utils/CMakeFiles/utils.dir/debug/debug.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object build_utils/CMakeFiles/utils.dir/debug/debug.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/debug/debug.c.o -MF CMakeFiles/utils.dir/debug/debug.c.o.d -o CMakeFiles/utils.dir/debug/debug.c.o -c /opt/Project/linux-com/common/utils/debug/debug.c

build_utils/CMakeFiles/utils.dir/debug/debug.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/debug/debug.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/debug/debug.c > CMakeFiles/utils.dir/debug/debug.c.i

build_utils/CMakeFiles/utils.dir/debug/debug.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/debug/debug.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/debug/debug.c -o CMakeFiles/utils.dir/debug/debug.c.s

build_utils/CMakeFiles/utils.dir/gpio/gpio.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/gpio/gpio.c.o: /opt/Project/linux-com/common/utils/gpio/gpio.c
build_utils/CMakeFiles/utils.dir/gpio/gpio.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object build_utils/CMakeFiles/utils.dir/gpio/gpio.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/gpio/gpio.c.o -MF CMakeFiles/utils.dir/gpio/gpio.c.o.d -o CMakeFiles/utils.dir/gpio/gpio.c.o -c /opt/Project/linux-com/common/utils/gpio/gpio.c

build_utils/CMakeFiles/utils.dir/gpio/gpio.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/gpio/gpio.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/gpio/gpio.c > CMakeFiles/utils.dir/gpio/gpio.c.i

build_utils/CMakeFiles/utils.dir/gpio/gpio.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/gpio/gpio.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/gpio/gpio.c -o CMakeFiles/utils.dir/gpio/gpio.c.s

build_utils/CMakeFiles/utils.dir/network/netip.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/network/netip.c.o: /opt/Project/linux-com/common/utils/network/netip.c
build_utils/CMakeFiles/utils.dir/network/netip.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object build_utils/CMakeFiles/utils.dir/network/netip.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/network/netip.c.o -MF CMakeFiles/utils.dir/network/netip.c.o.d -o CMakeFiles/utils.dir/network/netip.c.o -c /opt/Project/linux-com/common/utils/network/netip.c

build_utils/CMakeFiles/utils.dir/network/netip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/network/netip.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/network/netip.c > CMakeFiles/utils.dir/network/netip.c.i

build_utils/CMakeFiles/utils.dir/network/netip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/network/netip.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/network/netip.c -o CMakeFiles/utils.dir/network/netip.c.s

build_utils/CMakeFiles/utils.dir/network/ping.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/network/ping.c.o: /opt/Project/linux-com/common/utils/network/ping.c
build_utils/CMakeFiles/utils.dir/network/ping.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object build_utils/CMakeFiles/utils.dir/network/ping.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/network/ping.c.o -MF CMakeFiles/utils.dir/network/ping.c.o.d -o CMakeFiles/utils.dir/network/ping.c.o -c /opt/Project/linux-com/common/utils/network/ping.c

build_utils/CMakeFiles/utils.dir/network/ping.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/network/ping.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/network/ping.c > CMakeFiles/utils.dir/network/ping.c.i

build_utils/CMakeFiles/utils.dir/network/ping.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/network/ping.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/network/ping.c -o CMakeFiles/utils.dir/network/ping.c.s

build_utils/CMakeFiles/utils.dir/rf/rf_common.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/rf/rf_common.c.o: /opt/Project/linux-com/common/utils/rf/rf_common.c
build_utils/CMakeFiles/utils.dir/rf/rf_common.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object build_utils/CMakeFiles/utils.dir/rf/rf_common.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/rf/rf_common.c.o -MF CMakeFiles/utils.dir/rf/rf_common.c.o.d -o CMakeFiles/utils.dir/rf/rf_common.c.o -c /opt/Project/linux-com/common/utils/rf/rf_common.c

build_utils/CMakeFiles/utils.dir/rf/rf_common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/rf/rf_common.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/rf/rf_common.c > CMakeFiles/utils.dir/rf/rf_common.c.i

build_utils/CMakeFiles/utils.dir/rf/rf_common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/rf/rf_common.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/rf/rf_common.c -o CMakeFiles/utils.dir/rf/rf_common.c.s

build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.o: /opt/Project/linux-com/common/utils/rf/rf_lynq.c
build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.o -MF CMakeFiles/utils.dir/rf/rf_lynq.c.o.d -o CMakeFiles/utils.dir/rf/rf_lynq.c.o -c /opt/Project/linux-com/common/utils/rf/rf_lynq.c

build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/rf/rf_lynq.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/rf/rf_lynq.c > CMakeFiles/utils.dir/rf/rf_lynq.c.i

build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/rf/rf_lynq.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/rf/rf_lynq.c -o CMakeFiles/utils.dir/rf/rf_lynq.c.s

build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.o: /opt/Project/linux-com/common/utils/rf/rf_quectel.c
build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.o -MF CMakeFiles/utils.dir/rf/rf_quectel.c.o.d -o CMakeFiles/utils.dir/rf/rf_quectel.c.o -c /opt/Project/linux-com/common/utils/rf/rf_quectel.c

build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/rf/rf_quectel.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/rf/rf_quectel.c > CMakeFiles/utils.dir/rf/rf_quectel.c.i

build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/rf/rf_quectel.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/rf/rf_quectel.c -o CMakeFiles/utils.dir/rf/rf_quectel.c.s

build_utils/CMakeFiles/utils.dir/threads/thread.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/threads/thread.c.o: /opt/Project/linux-com/common/utils/threads/thread.c
build_utils/CMakeFiles/utils.dir/threads/thread.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object build_utils/CMakeFiles/utils.dir/threads/thread.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/threads/thread.c.o -MF CMakeFiles/utils.dir/threads/thread.c.o.d -o CMakeFiles/utils.dir/threads/thread.c.o -c /opt/Project/linux-com/common/utils/threads/thread.c

build_utils/CMakeFiles/utils.dir/threads/thread.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/threads/thread.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/threads/thread.c > CMakeFiles/utils.dir/threads/thread.c.i

build_utils/CMakeFiles/utils.dir/threads/thread.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/threads/thread.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/threads/thread.c -o CMakeFiles/utils.dir/threads/thread.c.s

build_utils/CMakeFiles/utils.dir/utest/utest.c.o: build_utils/CMakeFiles/utils.dir/flags.make
build_utils/CMakeFiles/utils.dir/utest/utest.c.o: /opt/Project/linux-com/common/utils/utest/utest.c
build_utils/CMakeFiles/utils.dir/utest/utest.c.o: build_utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object build_utils/CMakeFiles/utils.dir/utest/utest.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_utils/CMakeFiles/utils.dir/utest/utest.c.o -MF CMakeFiles/utils.dir/utest/utest.c.o.d -o CMakeFiles/utils.dir/utest/utest.c.o -c /opt/Project/linux-com/common/utils/utest/utest.c

build_utils/CMakeFiles/utils.dir/utest/utest.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/utils.dir/utest/utest.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/common/utils/utest/utest.c > CMakeFiles/utils.dir/utest/utest.c.i

build_utils/CMakeFiles/utils.dir/utest/utest.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/utils.dir/utest/utest.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/common/utils/utest/utest.c -o CMakeFiles/utils.dir/utest/utest.c.s

# Object files for target utils
utils_OBJECTS = \
"CMakeFiles/utils.dir/debug/debug.c.o" \
"CMakeFiles/utils.dir/gpio/gpio.c.o" \
"CMakeFiles/utils.dir/network/netip.c.o" \
"CMakeFiles/utils.dir/network/ping.c.o" \
"CMakeFiles/utils.dir/rf/rf_common.c.o" \
"CMakeFiles/utils.dir/rf/rf_lynq.c.o" \
"CMakeFiles/utils.dir/rf/rf_quectel.c.o" \
"CMakeFiles/utils.dir/threads/thread.c.o" \
"CMakeFiles/utils.dir/utest/utest.c.o"

# External object files for target utils
utils_EXTERNAL_OBJECTS =

/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/debug/debug.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/gpio/gpio.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/network/netip.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/network/ping.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/rf/rf_common.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/rf/rf_lynq.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/rf/rf_quectel.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/threads/thread.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/utest/utest.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/build.make
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so: build_utils/CMakeFiles/utils.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking C shared library /opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so"
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/utils.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
build_utils/CMakeFiles/utils.dir/build: /opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so
.PHONY : build_utils/CMakeFiles/utils.dir/build

build_utils/CMakeFiles/utils.dir/clean:
	cd /opt/Project/linux-com/project/iot-gw/build/build_utils && $(CMAKE_COMMAND) -P CMakeFiles/utils.dir/cmake_clean.cmake
.PHONY : build_utils/CMakeFiles/utils.dir/clean

build_utils/CMakeFiles/utils.dir/depend:
	cd /opt/Project/linux-com/project/iot-gw/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /opt/Project/linux-com/project/iot-gw /opt/Project/linux-com/common/utils /opt/Project/linux-com/project/iot-gw/build /opt/Project/linux-com/project/iot-gw/build/build_utils /opt/Project/linux-com/project/iot-gw/build/build_utils/CMakeFiles/utils.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : build_utils/CMakeFiles/utils.dir/depend

