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
include build_app/CMakeFiles/board_iot_srv.dir/depend.make

# Include the progress variables for this target.
include build_app/CMakeFiles/board_iot_srv.dir/progress.make

# Include the compile flags for this target's objects.
include build_app/CMakeFiles/board_iot_srv.dir/flags.make

build_app/CMakeFiles/board_iot_srv.dir/board_srv.c.o: build_app/CMakeFiles/board_iot_srv.dir/flags.make
build_app/CMakeFiles/board_iot_srv.dir/board_srv.c.o: ../app/board_srv.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object build_app/CMakeFiles/board_iot_srv.dir/board_srv.c.o"
	cd /opt/Project/linux-com/project/iot-gw/build/build_app && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/board_iot_srv.dir/board_srv.c.o   -c /opt/Project/linux-com/project/iot-gw/app/board_srv.c

build_app/CMakeFiles/board_iot_srv.dir/board_srv.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/board_iot_srv.dir/board_srv.c.i"
	cd /opt/Project/linux-com/project/iot-gw/build/build_app && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /opt/Project/linux-com/project/iot-gw/app/board_srv.c > CMakeFiles/board_iot_srv.dir/board_srv.c.i

build_app/CMakeFiles/board_iot_srv.dir/board_srv.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/board_iot_srv.dir/board_srv.c.s"
	cd /opt/Project/linux-com/project/iot-gw/build/build_app && /opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin//arm-buildroot-linux-gnueabihf-gcc --sysroot=/opt/toolchain/t113-i/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /opt/Project/linux-com/project/iot-gw/app/board_srv.c -o CMakeFiles/board_iot_srv.dir/board_srv.c.s

# Object files for target board_iot_srv
board_iot_srv_OBJECTS = \
"CMakeFiles/board_iot_srv.dir/board_srv.c.o"

# External object files for target board_iot_srv
board_iot_srv_EXTERNAL_OBJECTS =

../out/rootfs/usr/bin/board_iot_srv: build_app/CMakeFiles/board_iot_srv.dir/board_srv.c.o
../out/rootfs/usr/bin/board_iot_srv: build_app/CMakeFiles/board_iot_srv.dir/build.make
../out/rootfs/usr/bin/board_iot_srv: ../out/rootfs/usr/lib/libutils.so
../out/rootfs/usr/bin/board_iot_srv: ../out/archive/libiotmod.a
../out/rootfs/usr/bin/board_iot_srv: build_app/CMakeFiles/board_iot_srv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/opt/Project/linux-com/project/iot-gw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../../out/rootfs/usr/bin/board_iot_srv"
	cd /opt/Project/linux-com/project/iot-gw/build/build_app && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/board_iot_srv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
build_app/CMakeFiles/board_iot_srv.dir/build: ../out/rootfs/usr/bin/board_iot_srv

.PHONY : build_app/CMakeFiles/board_iot_srv.dir/build

build_app/CMakeFiles/board_iot_srv.dir/clean:
	cd /opt/Project/linux-com/project/iot-gw/build/build_app && $(CMAKE_COMMAND) -P CMakeFiles/board_iot_srv.dir/cmake_clean.cmake
.PHONY : build_app/CMakeFiles/board_iot_srv.dir/clean

build_app/CMakeFiles/board_iot_srv.dir/depend:
	cd /opt/Project/linux-com/project/iot-gw/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /opt/Project/linux-com/project/iot-gw /opt/Project/linux-com/project/iot-gw/app /opt/Project/linux-com/project/iot-gw/build /opt/Project/linux-com/project/iot-gw/build/build_app /opt/Project/linux-com/project/iot-gw/build/build_app/CMakeFiles/board_iot_srv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : build_app/CMakeFiles/board_iot_srv.dir/depend

