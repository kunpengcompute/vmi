#!/bin/bash
# build VideoEngineDecoder
# Copyright © Huawei Technologies Co., Ltd. 2018-2023. All rights reserved.
set -e
cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=NdkDecoder
source ${cur_file_path}/../../../scripts/compile_log.sh

AN_JOBS="$(grep processor /proc/cpuinfo | wc -l)"
REPO_ROOT_DIR=${cur_file_path}/../../../

ANDROID_NDK_HOME="${ANDROID_NDK_HOME:-/opt/buildtools/android-sdk-linux/ndk/25.1.8937393}"

so_list=(
    ${cur_file_path}/libs/*
)

clean_build_env()
{
    info "Begin Clean"
    rm -rf ./obj
    rm -rf ./libs
    info "Clean success"
}

build()
{
    info "Begin build"
    clean_build_env

    ${ANDROID_NDK_HOME}/ndk-build \
    NDK_PROJECT_PATH=${cur_file_path} \
    NDK_APPLICATION_MK=${cur_file_path}/Application.mk \
    APP_BUILD_SCRIPT=${cur_file_path}/Android.mk \
    ENABLE_ASAN=${ASAN}
    if [ -n "${MODULE_OUTPUT_DIR}" ];then
        mkdir -p ${MODULE_OUTPUT_DIR}/libs
        for so_name in ${so_lists[@]}
        do
            info "cp ${so_name} to  ${MODULE_OUTPUT_DIR}"
            cp -r ${so_name} ${MODULE_OUTPUT_DIR}
        done
        cd -
    fi
    info "build success"
}

ASAN=0
if [ "$2" == "asan" ]; then
    ASAN=1
fi
if [ "$1" == "build" ] || [ "$1" == "inc" ]; then
    build
else
    error "Invalid input, please input build or inc"
    exit 1
fi
