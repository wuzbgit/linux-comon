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
include build_test/CMakeFiles/ai_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include build_test/CMakeFiles/ai_test.dir/compiler_depend.make

# Include the progress variables for this target.
include build_test/CMakeFiles/ai_test.dir/progress.make

# Include the compile flags for this target's objects.
include build_test/CMakeFiles/ai_test.dir/flags.make

build_test/CMakeFiles/ai_test.dir/ai_test.c.o: build_test/CMakeFiles/ai_test.dir/flags.make
build_test/CMakeFiles/ai_test.dir/ai_test.c.o: /opt/Project/linux-com/project/iot-gw/test/ai_test.c
build_test/CMakeFiles/ai_test.dir/ai_test.c.o: build_test/CMakeFiles/ai_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object build_test/CMakeFiles/ai_test.dir/ai_test.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT build_test/CMakeFiles/ai_test.dir/ai_test.c.o -MF CMakeFiles/ai_test.dir/ai_test.c.o.d -o CMakeFiles/ai_test.dir/ai_test.c.o -c /opt/Project/linux-com/project/iot-gw/test/ai_test.c

build_test/CMakeFiles/ai_test.dir/ai_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/ai_test.dir/ai_test.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/project/iot-gw/test/ai_test.c > CMakeFiles/ai_test.dir/ai_test.c.i

build_test/CMakeFiles/ai_test.dir/ai_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/ai_test.dir/ai_test.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/project/iot-gw/test/ai_test.c -o CMakeFiles/ai_test.dir/ai_test.c.s

# Object files for target ai_test
ai_test_OBJECTS = \
"CMakeFiles/ai_test.dir/ai_test.c.o"

# External object files for target ai_test
ai_test_EXTERNAL_OBJECTS =

/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/bin/ai_test: build_test/CMakeFiles/ai_test.dir/ai_test.c.o
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/bin/ai_test: build_test/CMakeFiles/ai_test.dir/build.make
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/bin/ai_test: /opt/Project/linux-com/project/iot-gw/out/rootfs/usr/lib/libutils.so
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/bin/ai_test: /opt/Project/linux-com/project/iot-gw/out/archive/libiotmod.a
/opt/Project/linux-com/project/iot-gw/out/rootfs/usr/bin/ai_test: build_test/CMakeFiles/ai_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable /opt/Project/linux-com/project/iot-gw/out/rootfs/usr/bin/ai_test"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ai_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
build_test/CMakeFiles/ai_test.dir/build: /opt/Project/linux-com/project/iot-gw/out/rootfs/usr/bin/ai_test
.PHONY : build_test/CMakeFiles/ai_test.dir/build

build_test/CMakeFiles/ai_test.dir/clean:
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && $(CMAKE_COMMAND) -P CMakeFiles/ai_test.dir/cmake_clean.cmake
.PHONY : build_test/CMakeFiles/ai_test.dir/clean

build_test/CMakeFiles/ai_test.dir/depend:
	cd /opt/Project/linux-com/project/iot-gw/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /opt/Project/linux-com/project/iot-gw /opt/Project/linux-com/project/iot-gw/test /opt/Project/linux-com/project/iot-gw/build /opt/Project/linux-com/project/iot-gw/build/build_test /opt/Project/linux-com/project/iot-gw/build/build_test/CMakeFiles/ai_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : build_test/CMakeFiles/ai_test.dir/depend

