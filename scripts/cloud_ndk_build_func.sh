#!/bin/bash
# 存放服务端ndk编译需要使用的函数
# 使用方法：source ${VMIENGINE_ROOT_DIR}/scripts/cloud_ndk_build_func.sh ${VMIENGINE_ROOT_DIR}
# 上述命令中${VMIENGINE_ROOT_DIR}是VMIEngine代码的根目录，否则此脚本可能获取不到正确的路径导致执行出错。

set -e

VMIENGINE_ROOT_DIR=${1}
strip_tool=${AN_NDKDIR}/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip
source ${VMIENGINE_ROOT_DIR}/scripts/compile_log.sh

function check_ndk_env()
{
    [ -z "${AN_NDKDIR}" ] && error "please set AN_NDKDIR is the path of NDK" && return -1
    return 0
}

function cloud_ndk_compile()
{
    local build_path=${1}
    local asan=${2}

    cd ${build_path}
    ${AN_NDKDIR}/ndk-build \
        NDK_PROJECT_PATH=${build_path} \
        NDK_APPLICATION_MK=${build_path}/Application.mk \
        APP_BUILD_SCRIPT=${build_path}/Android.mk \
        ENABLE_ASAN=${asan}
    cd -
}

function save_ndk_symbols_and_strip()
{
    local build_path=${1}

    if [ ! -d "${build_path}/libs" ]; then
        error "Can't find ${build_path}/libs!"
    fi
    cd ${build_path}/libs
    file_lists=$(find -type f)
    mkdir symbols
    [ ${?} != 0 ] && error "Failed to create dir:${build_path}/libs/symbols" && return -1
    cp -r arm64-v8a/ symbols/
    [ ${?} != 0 ] && error "Failed to cp arm64-v8a to symbols" && return -1
    cp -r armeabi-v7a/ symbols/
    [ ${?} != 0 ] && error "Failed to cp armeabi-v7a to symbols" && return -1
    for each_file in ${file_lists[@]}
    do
        ${strip_tool} -s ${each_file}
        [ ${?} != 0 ] && error "Failed to strip ${each_file}" && return -1
    done
    cd -
}

function copy_lib_lib64_and_symbols_so()
{
    local libs_dir=${1}
    local output_dir=${2}
    local output_symbols_dir=${3}
    local system_list=${4}

    # copy so
    if [ -d "${libs_dir}/armeabi-v7a" ]; then
        cd ${libs_dir}/armeabi-v7a
        local so_list=$(find -iname "*.so")
        mkdir -p ${output_dir}/vendor/lib/
        mkdir -p ${output_dir}/system/lib/
        for so_name in ${so_list[@]}
        do
            local base_so_name=$(basename ${so_name})
            if [ "${base_so_name}" == "libc++_shared.so" ]; then
                continue
            fi
            local is_system=0
            for system_so in ${system_list[@]}
            do
                if [ "${base_so_name}" == "${system_so}" ]; then
                    is_system=1
                    cp ${base_so_name} ${output_dir}/system/lib/
                    [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_dir}/system/lib/" && return -1
                    cp libc++_shared.so ${output_dir}/system/lib/
                    [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_dir}/system/lib/" && return -1
                    break
                fi
            done
            if [ ${is_system} -eq 0 ]; then
                cp ${base_so_name} ${output_dir}/vendor/lib/
                [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_dir}/vendor/lib/" && return -1
                cp libc++_shared.so ${output_dir}/vendor/lib/
                [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_dir}/vendor/lib/" && return -1
            fi
        done
        cd -
    fi
    if [ -d "${libs_dir}/arm64-v8a" ]; then
        cd ${libs_dir}/arm64-v8a
        local so_list=$(find -iname "*.so")
        mkdir -p ${output_dir}/vendor/lib64/
        mkdir -p ${output_dir}/system/lib64/
        for so_name in ${so_list[@]}
        do
            local base_so_name=$(basename ${so_name})
            if [ "${base_so_name}" == "libc++_shared.so" ]; then
                continue
            fi
            local is_system=0
            for system_so in ${system_list[@]}
            do
                if [ "${base_so_name}" == "${system_so}" ]; then
                    is_system=1
                    cp ${base_so_name} ${output_dir}/system/lib64/
                    [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_dir}/system/lib64/" && return -1
                    cp libc++_shared.so ${output_dir}/system/lib64/
                    [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_dir}/system/lib64/" && return -1
                    break
                fi
            done
            if [ ${is_system} -eq 0 ]; then
                cp ${base_so_name} ${output_dir}/vendor/lib64/
                [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_dir}/vendor/lib64/" && return -1
                cp libc++_shared.so ${output_dir}/vendor/lib64/
                [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_dir}/vendor/lib64/" && return -1
            fi
        done
        cd -
    fi
    # copy symbols
    if [ -z "${output_symbols_dir}" ]; then
        return 0
    fi
    if [ -d "${libs_dir}/symbols/armeabi-v7a" ]; then
        cd ${libs_dir}/symbols/armeabi-v7a
        local so_list=$(find -iname "*.so")
        mkdir -p ${output_symbols_dir}/vendor/lib/
        mkdir -p ${output_symbols_dir}/system/lib/
        for so_name in ${so_list[@]}
        do
            local base_so_name=$(basename ${so_name})
            if [ "${base_so_name}" == "libc++_shared.so" ]; then
                continue
            fi
            local is_system=0
            for system_so in ${system_list[@]}
            do
                if [ "${base_so_name}" == "${system_so}" ]; then
                    is_system=1
                    cp ${base_so_name} ${output_symbols_dir}/system/lib/
                    [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_symbols_dir}/system/lib/" && return -1
                    cp libc++_shared.so ${output_symbols_dir}/system/lib/
                    [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_symbols_dir}/system/lib/" && return -1
                    break
                fi
            done
            if [ ${is_system} -eq 0 ]; then
                cp ${base_so_name} ${output_symbols_dir}/vendor/lib/
                [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_symbols_dir}/vendor/lib/" && return -1
                cp libc++_shared.so ${output_symbols_dir}/vendor/lib/
                [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_symbols_dir}/vendor/lib/" && return -1
            fi
        done
        cd -
    fi
    if [ -d "${libs_dir}/symbols/arm64-v8a" ]; then
        cd ${libs_dir}/symbols/arm64-v8a
        local so_list=$(find -iname "*.so")
        mkdir -p ${output_symbols_dir}/vendor/lib64/
        mkdir -p ${output_symbols_dir}/system/lib64/
        for so_name in ${so_list[@]}
        do
            local base_so_name=$(basename ${so_name})
            if [ "${base_so_name}" == "libc++_shared.so" ]; then
                continue
            fi
            local is_system=0
            for system_so in ${system_list[@]}
            do
                if [ "${base_so_name}" == "${system_so}" ]; then
                    is_system=1
                    cp ${base_so_name} ${output_symbols_dir}/system/lib64/
                    [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_symbols_dir}/system/lib64/" && return -1
                    cp libc++_shared.so ${output_symbols_dir}/system/lib64/
                    [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_symbols_dir}/system/lib64/" && return -1
                    break
                fi
            done
            if [ ${is_system} -eq 0 ]; then
                cp ${base_so_name} ${output_symbols_dir}/vendor/lib64/
                [ ${?} != 0 ] && error "Failed to copy ${base_so_name} to ${output_symbols_dir}/vendor/lib64/" && return -1
                cp libc++_shared.so ${output_symbols_dir}/vendor/lib64/
                [ ${?} != 0 ] && error "Failed to copy libc++_shared.so to ${output_symbols_dir}/vendor/lib64/" && return -1
            fi
        done
        cd -
    fi
    return 0
}