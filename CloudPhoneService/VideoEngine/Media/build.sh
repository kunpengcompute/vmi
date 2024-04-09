#!/bin/bash
# build VideoEncoder module.
# Copyright © Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.

set -e

cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=VideoCodec
export ALLOW_MISSING_DEPENDENCIES=true
source ${cur_file_path}/../../../scripts/compile_log.sh

AN_JOBS="$(grep processor /proc/cpuinfo | wc -l)"
REPO_ROOT_DIR=$(realpath ${cur_file_path}/../../..)

source ${REPO_ROOT_DIR}/scripts/build_env.conf
source ${REPO_ROOT_DIR}/scripts/cloud_ndk_build_func.sh ${REPO_ROOT_DIR}

enter_dir() {
    info "Enter directory: $1"
    pushd $1 >/dev/null
}

leave_dir() {
    popd >/dev/null
}

package()
{
    output_dir=${MODULE_OUTPUT_DIR}
    [ -z "${output_dir}" ] && output_dir=${cur_file_path}/output && rm -rf ${output_dir} && mkdir -p ${output_dir}

    copy_lib_lib64_and_symbols_so ${cur_file_path}/libs ${output_dir}
    [ ${?} != 0 ] && error "Failed to copy so" && return -1
    
    if [ -z "${MODULE_OUTPUT_DIR}" ]; then
        cd ${output_dir}
        tar -zcvf VideoCodec.tar.gz vendor
        cd -
    fi
}

clean_build_env()
{
    info "Begin Clean"
    rm -rf ${cur_file_path}/output
    rm -rf ${cur_file_path}/libs
    rm -rf ${cur_file_path}/obj
    info "Clean success"
}

build()
{
    clean_build_env
    [ ${?} != 0 ] && error "Failed to clean build env" && return -1
    check_ndk_env
    [ ${?} != 0 ] && return -1

    cloud_ndk_compile ${cur_file_path}

    save_ndk_symbols_and_strip ${cur_file_path}
    [ ${?} != 0 ] && error "Failed to save ndk symbols and strip" && return -1

    package
    [ ${?} != 0 ] && error "Failed to Package" && return -1
    info "Incremental compile success"
    return 0
}

if [ "$1" == "build" ] || [ "$1" == "inc" ]; then
    build
else
    error "Invalid input, please input build or inc"
    exit 1
fi