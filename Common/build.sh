#!/bin/bash
# Communication build shell
# Copyright © Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.

cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"

build_type=${2}
build_platform=android
if [ $# -eq 3 ];then
    build_platform=${3}
fi
ENGINE_ROOT_DIR=${cur_file_path}/../
ENGINE_RELEASEIMGS_DIR=${VMIENGINE_OUTPUT_DIR}
CLIENT_INCLUDE_DIR=${cur_file_path}/Libs/Client/include
CLOUD_INCLUDE_DIR=${cur_file_path}/Libs/Cloud/include
TMP_FOLDER=${cur_file_path}/Tmp

source ${ENGINE_ROOT_DIR}/scripts/cloud_ndk_build_func.sh ${ENGINE_ROOT_DIR}
AN_JOBS="$(grep processor /proc/cpuinfo | wc -l)"
if [ "${build_platform}" == "linux" ];then
    strip_tool=strip
fi
if [ "${STRIP}" ];then
    strip_tool=${STRIP}
fi
source ${cur_file_path}/../scripts/compile_log.sh

client_ndk_compile()
{
    info "libsCommunication client compilation"
    target=${1}
    folder=${2}
    if [ "${build_platform}" == "android" ];then
        echo "AN_NDKDIR:${AN_NDKDIR}"
        cmake -DCMAKE_TOOLCHAIN_FILE=${AN_NDKDIR}/build/cmake/android.toolchain.cmake -DANDROID_ABI="${target}" -DANDROID_NDK=${AN_NDKDIR} -DANDROID_PLATFORM=android-22 ${folder}
        [ ${?} != 0 ] && error "Failed to cmake" && return -1
    else
        echo "linux build"
        cmake ${folder}
        [ ${?} != 0 ] && error "Failed to cmake" && return -1
    fi
    make -j ${AN_JOBS}
    [ ${?} != 0 ] && error "Failed to cmake" && return -1
    rm -rf symbols
    mkdir -p symbols
    #cd ..
    so_lists=$(find -name "*.so")
    for each_so in ${so_lists[@]}
    do
        cp ${each_so} ./symbols
        [ ${?} != 0 ] && error "Failed to cp ${each_so} to symbol" && return -1
        ${strip_tool} -s ${each_so}
        [ ${?} != 0 ] && error "Failed to strip ${each_so}" && return -1
    done
    #cd -
    return 0
}

client_package()
{
    output_dir=${MODULE_OUTPUT_DIR}
    output_symbols_dir=${MODULE_SYMBOL_DIR}
    [ -z "${output_dir}" ] && output_dir=${cur_file_path}/output && rm -rf ${output_dir} && mkdir -p ${output_dir}
    [ -z "${output_symbols_dir}" ] && output_symbols_dir=${cur_file_path}/output/symbols && rm -rf ${output_symbols_dir} && mkdir -p ${output_symbols_dir}
    cp ${cur_file_path}/build/*.so ${output_dir}
    [ ${?} != 0 ] && error "Failed to copy libCommunication" && return -1
    cp ${cur_file_path}/build/symbols/*.so ${output_symbols_dir}
    [ ${?} != 0 ] && error "Failed to copy libCommunication symbol" && return -1
    cp ${cur_file_path}/build/Communication/*.so ${output_dir}
    [ ${?} != 0 ] && error "Failed to copy libCommunication" && return -1
    cp ${cur_file_path}/build/Communication/symbols/*.so ${output_symbols_dir}
    [ ${?} != 0 ] && error "Failed to copy libCommunication symbol" && return -1
 
    if [ -z "${MODULE_SYMBOL_DIR}" ]; then # MODULE_SYMBOL_DIR为空，直接在本地output/symbols目录打包
        cd ${output_symbols_dir}
        tar -zcvf ../VmiCommunicationClientSymbol.tar.gz *
        cd -
    fi
    if [ -z "${MODULE_OUTPUT_DIR}" ]; then # MODULE_OUTPUT_DIR为空，直接在本地output目录打包
        cd ${output_dir}
        tar -zcvf VmiCommunicationClient.tar.gz *.so
        cd -
    fi

}

cloud_package()
{
    # copy output
    output_dir=${MODULE_OUTPUT_DIR}
    output_symbols_dir=${MODULE_SYMBOL_DIR}

    [ -z "${output_dir}" ] &&  output_dir=${cur_file_path}/output && rm -rf ${output_dir}
    [ -z "${output_symbols_dir}" ] && output_symbols_dir=${cur_file_path}/output/symbols && rm -rf ${output_symbol_dir}
    copy_lib_lib64_and_symbols_so ${cur_file_path}/libs ${output_dir} ${output_symbols_dir}
    [ ${?} != 0 ] && error "Failed to copy Common so" && return -1
    copy_lib_lib64_and_symbols_so ${cur_file_path}/Communication/libs ${output_dir} ${output_symbols_dir}
    [ ${?} != 0 ] && error "Failed to copy Common/Communication so" && return -1

    if [ -z "${MODULE_OUTPUT_DIR}" ]; then # MODULE_OUTPUT_DIR为空，直接在本地output目录打包
        cd ${output_dir}
        tar -zcvf VmiCommunicationCloud.tar.gz vendor
        cd -
    fi
    if [ -z "${MODULE_SYMBOL_DIR}" ]; then # MODULE_SYMBOL_DIR为空，直接在本地output/symbols目录打包
        cd ${output_symbols_dir}
        tar -zcvf VmiCommunicationCloudSymbol.tar.gz vendor
        cd -
    fi

    info "copy output and symbol success"
}

clean_build_env()
{
    rm -rf ${cur_file_path}/output
    rm -rf ${cur_file_path}/build
    rm -rf ${TMP_FOLDER}
    rm -rf ${cur_file_path}/Libs
    rm -rf ${cur_file_path}/libs
    rm -rf ${cur_file_path}/obj
    rm -rf ${cur_file_path}/Communication/libs
    rm -rf ${cur_file_path}/Communication/obj
}

build()
{
    clean_build_env
    [ ${?} != 0 ] && return -1
    check_ndk_env
    [ ${?} != 0 ] && return -1

    if [ "${build_type}" == "Client" ]; then
        mkdir -p build
        cd build
        client_ndk_compile arm64-v8a ${cur_file_path}
        [ ${?} != 0 ] && error "Failed to compile client libCommunication" && return -1
        mkdir -p Communication
        cd Communication
        client_ndk_compile arm64-v8a ${cur_file_path}/Communication
        [ ${?} != 0 ] && error "Failed to compile client libVmiCommonCommunication" && return -1
        cd ../../
        client_package
        [ ${?} != 0 ] && error "Failed to package client libCommunication" && return -1
    else
        info "libCommunication server build"

        cloud_ndk_compile ${cur_file_path}
        cloud_ndk_compile ${cur_file_path}/Communication

        save_ndk_symbols_and_strip ${cur_file_path}
        [ ${?} != 0 ] && error "Failed to save Common symbols and strip" && return -1
        save_ndk_symbols_and_strip ${cur_file_path}/Communication
        [ ${?} != 0 ] && error "Failed to save Common/Communication symbols and strip" && return -1

        cloud_package
        [ ${?} != 0 ] && error "Failed to copy output and symbol" && return -1
    fi
    return 0
}

if [ "$1" == "build" ] || [ "$1" == "inc" ]; then
    build
else
    error "Invalid input, please input build or inc"
    exit 1
fi