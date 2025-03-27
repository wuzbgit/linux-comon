set(CMAKE_SYSTEM_NAME Linux)
set(TOOLCHAIN_PATH /opt/toolchain/rk3576/rk3576_ubuntu)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/aarch64-buildroot-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/aarch64-buildroot-linux-gnu-g++)
set(CMAKE_SYSROOT ${TOOLCHAIN_PATH}/aarch64-buildroot-linux-gnu/sysroot/)

set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu/pkgconfig")

# set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
