# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /opt/Project/linux-com/project/iot-gw

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /opt/Project/linux-com/project/iot-gw/build

# Include any dependencies generated for this target.
include build_test/CMakeFiles/rf_test.dir/depend.make

# Include the progress variables for this target.
include build_test/CMakeFiles/rf_test.dir/progress.make

# Include the compile flags for this target's objects.
include build_test/CMakeFiles/rf_test.dir/flags.make

build_test/CMakeFiles/rf_test.dir/rf_test.c.o: build_test/CMakeFiles/rf_test.dir/flags.make
build_test/CMakeFiles/rf_test.dir/rf_test.c.o: ../test/rf_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object build_test/CMakeFiles/rf_test.dir/rf_test.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/rf_test.dir/rf_test.c.o   -c /opt/Project/linux-com/project/iot-gw/test/rf_test.c

build_test/CMakeFiles/rf_test.dir/rf_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/rf_test.dir/rf_test.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/project/iot-gw/test/rf_test.c > CMakeFiles/rf_test.dir/rf_test.c.i

build_test/CMakeFiles/rf_test.dir/rf_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/rf_test.dir/rf_test.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/project/iot-gw/test/rf_test.c -o CMakeFiles/rf_test.dir/rf_test.c.s

# Object files for target rf_test
rf_test_OBJECTS = \
"CMakeFiles/rf_test.dir/rf_test.c.o"

# External object files for target rf_test
rf_test_EXTERNAL_OBJECTS =

../out/rootfs/usr/bin/rf_test: build_test/CMakeFiles/rf_test.dir/rf_test.c.o
../out/rootfs/usr/bin/rf_test: build_test/CMakeFiles/rf_test.dir/build.make
../out/rootfs/usr/bin/rf_test: ../out/rootfs/usr/lib/libutils.so
../out/rootfs/usr/bin/rf_test: ../out/archive/libiotmod.a
../out/rootfs/usr/bin/rf_test: build_test/CMakeFiles/rf_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../../out/rootfs/usr/bin/rf_test"
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rf_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
build_test/CMakeFiles/rf_test.dir/build: ../out/rootfs/usr/bin/rf_test

.PHONY : build_test/CMakeFiles/rf_test.dir/build

build_test/CMakeFiles/rf_test.dir/clean:
	cd /opt/Project/linux-com/project/iot-gw/build/build_test && $(CMAKE_COMMAND) -P CMakeFiles/rf_test.dir/cmake_clean.cmake
.PHONY : build_test/CMakeFiles/rf_test.dir/clean

build_test/CMakeFiles/rf_test.dir/depend:
	cd /opt/Project/linux-com/project/iot-gw/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /opt/Project/linux-com/project/iot-gw /opt/Project/linux-com/project/iot-gw/test /opt/Project/linux-com/project/iot-gw/build /opt/Project/linux-com/project/iot-gw/build/build_test /opt/Project/linux-com/project/iot-gw/build/build_test/CMakeFiles/rf_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : build_test/CMakeFiles/rf_test.dir/depend

