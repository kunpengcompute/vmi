#!/bin/bash
# Instruction engine client build shell
# Copyright © Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

set -e

cur_file_path=$(cd $(dirname "${0}");pwd)
REPO_ROOT_DIR=$(realpath ${cur_file_path}/../..)
cd "${cur_file_path}"

build_type=${2}
AN_JOBS="$(grep processor /proc/cpuinfo | wc -l)"
source ${cur_file_path}/../../scripts/compile_log.sh
source ${REPO_ROOT_DIR}/scripts/cloud_ndk_build_func.sh ${REPO_ROOT_DIR}

system_so_list="
    libVmiOpus.so "

clean_build_env()
{
    rm -rf ${cur_file_path}/output
    if [ "${build_type}" == "Client" ]; then
        rm -rf ${cur_file_path}/build
    else
        rm -rf ${cur_file_path}/libs
        rm -rf ${cur_file_path}/obj
    fi
}

cloud_package(){
    output_dir=${MODULE_OUTPUT_DIR}
    output_symbols_dir=${MODULE_SYMBOL_DIR}
    [ -z "${output_dir}" ] && output_dir=${cur_file_path}/output && rm -rf ${output_dir} && mkdir -p ${output_dir}
    [ -z "${output_symbols_dir}" ] && output_symbols_dir=${cur_file_path}/output/symbols && rm -rf ${output_symbols_dir} && mkdir -p ${output_symbols_dir}

    copy_lib_lib64_and_symbols_so ${cur_file_path}/libs ${output_dir} ${output_symbols_dir} "${system_so_list[@]}"
    [ ${?} != 0 ] && error "Failed to copy so" && return -1

    if [ -z "${MODULE_OUTPUT_DIR}" ]; then # MODULE_OUTPUT_DIR为空，直接在本地output目录打包
        cd ${output_dir}
        tar -zcvf OpusCloud.tar.gz system
        cd -
    fi

    if [ -z "${MODULE_SYMBOL_DIR}" ]; then # MODULE_SYMBOL_DIR为空，直接在本地output/symbols目录打包
        cd ${output_symbols_dir}
        tar -zcvf OpusCloudSymbol.tar.gz system
        cd -
    fi

    info "copy output and symbol success"
}

build()
{
    clean_build_env
    [ ${?} != 0 ] && return -1
    check_ndk_env
    [ ${?} != 0 ] && return -1

    if [ "${build_type}" == "Client" ]; then
        info "opus client build"
        mkdir -p build
        cd build
        client_ndk_compile arm64-v8a ${cur_file_path}
        [ ${?} != 0 ] && error "Failed to compile opus client" && return -1
        cd -
        client_package
        [ ${?} != 0 ] && error "Failed to package opsu client" && return -1
    else
        info "opus cloud build"
        cloud_ndk_compile ${cur_file_path} ${ASAN}

        save_ndk_symbols_and_strip ${cur_file_path}
        [ ${?} != 0 ] && error "Failed to save symbols and strip" && return -1

        cloud_package
        [ ${?} != 0 ] && error "Failed to copy output and symbol" && return -1
    fi
    return 0
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