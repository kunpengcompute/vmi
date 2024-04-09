#!/bin/bash
# build VmiAgent
# Copyright © Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=VmiAgent
source ${cur_file_path}/../../scripts/compile_log.sh

AN_JOBS="$(grep processor /proc/cpuinfo | wc -l)"
REPO_ROOT_DIR=$(realpath ${cur_file_path}/../..)
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
    output_symbols_dir=${MODULE_SYMBOL_DIR}
    [ -z "${output_dir}" ] && output_dir=${cur_file_path}/output && rm -rf ${output_dir} && mkdir -p ${output_dir}
    [ -z "${output_symbols_dir}" ] && output_symbols_dir=${cur_file_path}/output/symbols && rm -rf ${output_symbols_dir} && mkdir -p ${output_symbols_dir}

    copy_lib_lib64_and_symbols_so ${cur_file_path}/libs ${output_dir} ${output_symbols_dir}
    [ ${?} != 0 ] && error "Failed to copy so" && return -1

    mkdir -p ${output_dir}/vendor/bin/
    cp ${cur_file_path}/libs/arm64-v8a/VmiAgent ${output_dir}/vendor/bin/
    [ ${?} != 0 ] && error "Failed to copy VmiAgent" && return -1
    mkdir -p ${output_symbols_dir}/vendor/bin/
    cp ${cur_file_path}/libs/symbols/arm64-v8a/VmiAgent ${output_symbols_dir}/vendor/bin/
    [ ${?} != 0 ] && error "Failed to copy symbol VmiAgent" && return -1
    mkdir -p ${output_dir}/vendor/etc/init/
    cp ${cur_file_path}/VmiAgentAndroidP.rc ${output_dir}/vendor/etc/init/
    [ ${?} != 0 ] && error "Failed to copy VmiAgentAndroidP.rc" && return -1

    if [ -z "${MODULE_SYMBOL_DIR}" ]; then # MODULE_SYMBOL_DIR为空，直接在本地output/symbols目录打包
        cd ${output_symbols_dir}
        tar -zcvf ../VmiAgentSymbol.tar.gz vendor
        cd -
    fi
    if [ -z "${MODULE_OUTPUT_DIR}" ]; then # MODULE_OUTPUT_DIR为空，直接在本地output目录打包
        cd ${output_dir}
        tar -zcvf VmiAgent.tar.gz vendor
        cd -
    fi
}

clean_build_env()
{
    info "Begin Clean"
    rm -rf output
    rm -rf Include
    rm -rf Libs
    rm -rf libs
    rm -rf obj
    info "Clean success"
}

build()
{
    clean_build_env
    [ ${?} != 0 ] && error "Failed to clean build env" && return -1
    check_ndk_env
    [ ${?} != 0 ] && return -1

    cloud_ndk_compile ${cur_file_path} ${ASAN}

    save_ndk_symbols_and_strip ${cur_file_path}
    [ ${?} != 0 ] && error "Failed to save symbols and strip" && return -1

    package
    [ ${?} != 0 ] && error "Failed to package" && return -1
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
