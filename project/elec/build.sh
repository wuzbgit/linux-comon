#! /bin/bash
set -e

CUR_DIR=$(cd `dirname $0`; pwd)
TOP_DIR=${CUR_DIR}/../../
PROJECT_DIR=${CUR_DIR}
OUT_DIR=${PROJECT_DIR}/out
OUT_BUILD_DIR=${PROJECT_DIR}/build
OUT_ROOTFS_DIR=${OUT_DIR}/rootfs

TARGET_BOARD=t507
BUILD_TYPE=release
ROOTFS=""
TOOLCHAIN_FILE=""
BUILD_MODULE=all

BUILD_TYPE_LIST=("release" "debug")
BUILD_MODULE_LIST=("all" "app" "test" "module" "utils" "service")
BOARD_LIST=("imx8m" "t507")

declare -A ROOTFS_LIST
ROOTFS_LIST=(
    [imx8m]="/opt/toolchain/imx8m/aarch64-buildroot-linux-gnu_sdk-buildroot/aarch64-buildroot-linux-gnu/sysroot"
    [t507]="/opt/toolchain/t507/aarch64-buildroot-linux-gnu_sdk-buildroot/aarch64-buildroot-linux-gnu/sysroot"
)

declare -A CROSSCOMPILE_TOOLCHAIN_LIST
CROSSCOMPILE_TOOLCHAIN_LIST=(
    [imx8m]="${PROJECT_DIR}/toolchain/CrossCompile_imx8m.cmake"
    [t507]="${PROJECT_DIR}/toolchain/CrossCompile_t507.cmake"
)

function do_select() {
    select selection in $@
    do
        echo ${selection}
        break;
    done
}

function select_argument() {
    PS3="Please select the BUILD_MODULE: "
    BUILD_MODULE=$(do_select ${BUILD_MODULE_LIST[@]})
    echo -e "setup BUILD_MODULE=${BUILD_MODULE}"

    PS3="Please select the TARGET_BOARD: "
    TARGET_BOARD=$(do_select ${TARGET_BOARD_LIST[@]})
    echo -e "setup TARGET_BOARD=${TARGET_BOARD}"

    PS3="Please select the BUILD_TYPE: "
    BUILD_TYPE=$(do_select ${BUILD_TYPE_LIST[@]})
    echo -e "setup BUILD_TYPE=${BUILD_TYPE}"

    ROOTFS=${ROOTFS_LIST[${TARGET_BOARD}]}
    TOOLCHAIN_FILE=${CROSSCOMPILE_TOOLCHAIN_LIST[${TARGET_BOARD}]}
    echo -e "setup ROOTFS=${ROOTFS}"
    echo -e "setup TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"    
}

function setup_argument() {
    if [ "x$1" != "x" ]; then
        BUILD_MODULE=$1
    fi

    if [ "x$2" != "x" ]; then
        TARGET_BOARD=$2
    fi

    if [ "x$3" != "x" ]; then
        BUILD_TYPE=$3
    fi

    ROOTFS=${ROOTFS_LIST[${TARGET_BOARD}]}
    TOOLCHAIN_FILE=${CROSSCOMPILE_TOOLCHAIN_LIST[${TARGET_BOARD}]}

    echo -e "setup BUILD_MODULE=${BUILD_MODULE}"
    echo -e "setup TARGET_BOARD=${TARGET_BOARD}"
    echo -e "setup BUILD_TYPE=${BUILD_TYPE}"
    echo -e "setup ROOTFS=${ROOTFS}"
    echo -e "setup TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"           
}

function do_build() {
    echo "do_build"
    CMAKE_FLAGS="\
        -DTARGET_BOARD=${TARGET_BOARD}  \
        -DBUILD_TYPE=${BUILD_TYPE}  \
        -DBUILD_MODULE=${BUILD_MODULE}  \
        -DROOTFS=${ROOTFS}  \
        -DTOP_DIR=${TOP_DIR}  \
        -DOUT_DIR=${OUT_DIR}  \
        -DPROJECT_DIR=${PROJECT_DIR}  \
        -DOUT_ROOTFS_DIR=${OUT_ROOTFS_DIR}  \
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}  \
        "
    rm -rf ${OUT_DIR}
    rm -rf ${OUT_BUILD_DIR}
    mkdir -p ${OUT_BUILD_DIR}
    cd ${OUT_BUILD_DIR} && cmake ${CMAKE_FLAGS} $@ -Wno-dev --debug-output ${PROJECT_DIR}
    make -C ${OUT_BUILD_DIR} VERBOSE=on
}

#Start Build
if [ "$1"x == "-i"x ]; then
    select_argument
else
    setup_argument $*
fi

do_build
