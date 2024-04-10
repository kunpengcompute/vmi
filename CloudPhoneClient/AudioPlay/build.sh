#!/bin/bash
# build AudioEngineClient
# Copyright © Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
set -e
cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=AudioEngineClient
source ${cur_file_path}/../../scripts/compile_log.sh

clean_build_env()
{
    info "Begin Clean"
    rm -rf build
    rm -rf .externalNativeBuild
    info "Clean success"
}

check_env()
{
    [ -z "${AN_NDKDIR}" ] && error "please set AN_NDKDIR is the path of NDK" && return -1
    return 0
}

ndk_compile()
{
    target=${1}
    folder=${2}
    echo "AN_NDKDIR:${AN_NDKDIR}"
    cmake -DCMAKE_TOOLCHAIN_FILE=${AN_NDKDIR}/build/cmake/android.toolchain.cmake -DANDROID_ABI="${target}" -DANDROID_NDK=${AN_NDKDIR} -DANDROID_STL=c++_shared -DANDROID_PLATFORM=android-22 ${folder}
    [ ${?} != 0 ] && error "Failed to cmake" && return -1
    make -j
    [ ${?} != 0 ] && error "Failed to cmake" && return -1
    return 0
}

build()
{
    info "Begin build"
    clean_build_env
    [ ${?} != 0 ] && error "Failed to clean build env" && return -1
    check_env
    [ ${?} != 0 ] && return -1

    mkdir -p build/Utils
    cd build/Utils
    ndk_compile arm64-v8a ${cur_file_path}/../../Common
    [ ${?} != 0 ] && error "Failed to compile Utils" && return -1
    cd ..
    mkdir opus
    cd opus
    ndk_compile arm64-v8a ${cur_file_path}/../../unpack_open_source/opus
    [ ${?} != 0 ] && error "Failed to compile opus" && return -1
    cd ../..
    mkdir -p jniLibs/arm64-v8a
    cp build/Utils/libDemoUtils.so jniLibs/arm64-v8a
    cp build/opus/libopus.so jniLibs/arm64-v8a
    ${AN_GRADLEDIR}/bin/gradle assembleDebug -x test -x lint --stacktrace -PenableAsan=${ASAN}
    [ ${?} != 0 ] && error "Failed to Incremental compile" &&  return -1
    if [ -n "${MODULE_OUTPUT_DIR}" ];then
        cp build/outputs/aar/AudioPlay-debug.aar ${MODULE_OUTPUT_DIR}/AudioEngineClient.aar
    fi
    info "build success"
}

ASAN="OFF"
if [ "$2" == "asan" ]; then
    ASAN="ON"
fi
if [ "$1" == "build" ] || [ "$1" == "inc" ]; then
    build
else
    error "Invalid input, please input build or inc"
    exit 1
fi