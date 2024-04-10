#!/bin/bash
# build VideoEncoder module.
# Copyright © Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.

set -e

cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=GpuEncTurbo
export ALLOW_MISSING_DEPENDENCIES=true
source ${cur_file_path}/../../../scripts/compile_log.sh

REPO_ROOT_DIR=$(realpath ${cur_file_path}/../../..)
THIRDPARTY_DIR=${REPO_ROOT_DIR}/unpack_open_source
LIBVA_CODES_PATH=${THIRDPARTY_DIR}/libva/codes

source ${REPO_ROOT_DIR}/scripts/build_env.conf
source ${REPO_ROOT_DIR}/scripts/cloud_ndk_build_func.sh ${REPO_ROOT_DIR}

source_dirs="
    unpack_open_source/libdrm \
    unpack_open_source/libva \
    CloudPhoneService/VideoEngine/GpuEncTurbo "

system_so_list="
    libVmiEncTurboSys.so \
    libVmiEncTurboAcard.so \
    libVmiEncTurboInno.so \
    libVmiEncTurboCpuSys.so "

gen_head_files()
{
    cd ${THIRDPARTY_DIR}/libdrm/codes
    python3 gen_table_fourcc.py include/drm/drm_fourcc.h generated_static_table_fourcc.h
    cd -

    if [ ! -f "${LIBVA_CODES_PATH}/build/gen_version.sh" ] || \
        [ ! -f "${LIBVA_CODES_PATH}/va/va_version.h.in" ] || \
        [ ! -f "${LIBVA_CODES_PATH}/configure.ac" ]; then
        error "Can't find libva file: gen_version.sh va_version.h.in configure.ac"
        return -1
    fi
    mkdir -p ${LIBVA_CODES_PATH}/../Include/va
    bash ${LIBVA_CODES_PATH}/build/gen_version.sh ${LIBVA_CODES_PATH} ${LIBVA_CODES_PATH}/va/va_version.h.in > ${LIBVA_CODES_PATH}/../Include/va/va_version.h
}

package()
{
    output_dir=${MODULE_OUTPUT_DIR}
    output_symbols_dir=${MODULE_SYMBOL_DIR}
    [ -z "${output_dir}" ] && output_dir=${cur_file_path}/output && rm -rf ${output_dir} && mkdir -p ${output_dir}
    [ -z "${output_symbols_dir}" ] && output_symbols_dir=${cur_file_path}/output/symbols && rm -rf ${output_symbols_dir} && mkdir -p ${output_symbols_dir}

    copy_lib_lib64_and_symbols_so ${cur_file_path}/libs ${output_dir} ${output_symbols_dir} "${system_so_list[@]}"
    [ ${?} != 0 ] && error "Failed to copy so" && return -1

    if [ -z "${MODULE_OUTPUT_DIR}" ]; then
        cd ${output_dir}
        tar -zcvf GpuEncTurbo.tar.gz system vendor
        cd -
    fi
    if [ -z "${MODULE_SYMBOL_DIR}" ]; then
        cd ${output_symbols_dir}
        tar -zcvf GpuEncTurboSymbol.tar.gz *
        cd -
    fi
}

clean_build_env()
{
    info "Begin Clean"
    for source_dir in ${source_dirs[@]}
    do
        rm -rf ${REPO_ROOT_DIR}/${source_dir}/libs/
        rm -rf ${REPO_ROOT_DIR}/${source_dir}/obj/
    done
    info "Clean success"
}

build()
{
    clean_build_env
    [ ${?} != 0 ] && error "Failed to clean build env" && exit 1
    check_ndk_env
    [ ${?} != 0 ] && error "Failed to check ndk env" && return -1
    gen_head_files
    [ ${?} != 0 ] && error "Failed to gen head files" && return -1

    compile_source_dirs=${source_dirs}
    for source_dir in ${compile_source_dirs[@]}
    do
        cloud_ndk_compile ${REPO_ROOT_DIR}/${source_dir} ${ASAN}
    done
    save_ndk_symbols_and_strip ${cur_file_path}
    [ ${?} != 0 ] && error "Failed to save symbols and strip" && return -1
    package
    [ ${?} != 0 ] && error "Failed to Package" && return -1
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