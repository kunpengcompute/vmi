#!/bin/bash
# build TouchEngineClient
# Copyright © Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
set -e
cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=TouchEngineClient
source ${cur_file_path}/../../scripts/compile_log.sh

clean_build_env()
{
    info "Begin Clean"
    rm -rf build
    rm -rf .gradle
    rm -rf .externalNativeBuild
    info "Clean success"
}

build()
{
    info "Begin build"
    clean_build_env
    [ ${?} != 0 ] && error "Failed to clean build env" && return -1

    ${AN_GRADLEDIR}/bin/gradle assembleDebug -x test -x lint --stacktrace
    [ ${?} != 0 ] && error "Failed to Incremental compile" &&  return -1
    if [ -n "${MODULE_OUTPUT_DIR}" ];then
        cp build/outputs/aar/TouchCapture-debug.aar ${MODULE_OUTPUT_DIR}/TouchEngineClient.aar
    fi
    info "build success"
}

if [ "$1" == "build" ] || [ "$1" == "inc" ]; then
    build
else
    error "Invalid input, please input build or inc"
    exit 1
fi
