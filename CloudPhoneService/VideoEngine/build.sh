#!/bin/bash
# build InstructionEngine Demo
# Copyright © Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=VideoEngineDemo
source ${cur_file_path}/../../scripts/compile_log.sh

REPO_ROOT_DIR=${cur_file_path}/../..
AN_JOBS="$(grep processor /proc/cpuinfo | wc -l)"
NSTACK_DIR=${REPO_ROOT_DIR}/third_party/nStack
if [ -z ${MODULE_OUTPUT_DIR} ];then
    MODULE_OUTPUT_DIR=${VMIENGINE_OUTPUT_DIR}/DemoVideoEngine
    mkdir -p ${MODULE_OUTPUT_DIR}
fi 
PACKAGE_DIR=${MODULE_OUTPUT_DIR}/../
DEMO_VIDEO_ENGINE_DIR=${PACKAGE_DIR}/DemoVideoEngine
OPEN_SOURCE_DEMO_DIR=${REPO_ROOT_DIR}/open_source_demo
VIDEO_SCRIPT=${cur_file_path}/../VideoScripts

engine_tars=(
    VmiCommunicationCloud.tar.gz
    GpuEncTurbo.tar.gz
    VideoCodec.tar.gz
    OpusCloud.tar.gz
    VmiAgent.tar.gz
)

engine_asan_tars=(
    VmiCommunicationCloud.tar.gz
    VideoEngineAsan.tar.gz
    GpuEncTurboAsan.tar.gz
    VideoCodec.tar.gz
    OpusCloud.tar.gz
    VmiAgentAsan.tar.gz
)

enter_dir() {
    info "Enter directory: $1"
    pushd $1 >/dev/null
}

leave_dir() {
    popd >/dev/null
}

copy_openh264()
{
    if [ ! -n "${MODULE_OUTPUT_DIR}" ]; then
        error "Not found: ${MODULE_OUTPUT_DIR}" && return -1
    fi
    lib_target_folder=${MODULE_OUTPUT_DIR}/vendor/lib
    lib64_target_folder=${MODULE_OUTPUT_DIR}/vendor/lib64
    [ ! -f ${lib_target_folder} ] && mkdir -p ${lib_target_folder}
    [ ! -f ${lib64_target_folder} ] && mkdir -p ${lib64_target_folder}
    [ ! -f ${PACKAGE_DIR}/openH264 ] && mkdir -p ${PACKAGE_DIR}/openH264
    enter_dir ${PACKAGE_DIR}/openH264
    cp -rf ${REPO_ROOT_DIR}/unpack_open_source/openH264/libopenh264-2.0.0-android.so.bz2 ./
    cp -rf ${REPO_ROOT_DIR}/unpack_open_source/openH264/libopenh264-2.0.0-android-arm64.so.bz2 ./
    bunzip2 -f libopenh264-2.0.0-android.so.bz2
    [ ${?} != 0 ] && return -1
    bunzip2 -kf libopenh264-2.0.0-android-arm64.so.bz2
    [ ${?} != 0 ] && return -1
    cp -rf libopenh264-2.0.0-android.so ${lib_target_folder}/libopenh264.so
    cp -rf libopenh264-2.0.0-android-arm64.so ${lib64_target_folder}/libopenh264.so
    leave_dir
    rm -rf ${PACKAGE_DIR}/openH264
    return 0
}

copy_script()
{
    mkdir -p ${MODULE_OUTPUT_DIR}/vendor/etc/vintf/
    cp -rf ${VIDEO_SCRIPT}/manifest.xml ${MODULE_OUTPUT_DIR}/vendor
    cp -rf ${VIDEO_SCRIPT}/manifest.xml ${MODULE_OUTPUT_DIR}/vendor/etc/vintf
    cp -rf ${VIDEO_SCRIPT}/cfct_config ${MODULE_OUTPUT_DIR}
    cp -rf ${VIDEO_SCRIPT}/cfct_video ${MODULE_OUTPUT_DIR}
    cp -rf ${VIDEO_SCRIPT}/make_image.sh ${MODULE_OUTPUT_DIR}
    cp -rf ${VIDEO_SCRIPT}/Dockerfile_* ${MODULE_OUTPUT_DIR}
    cp -rf ${VIDEO_SCRIPT}/vendor/default.prop ${MODULE_OUTPUT_DIR}/vendor
}

unpack_binary_lib()
{
    if [[ "${ASAN}" == 1 ]];then
        info "### Enable asan for ${MODULE_NAME}"
        mkdir -p ${DEMO_VIDEO_ENGINE_DIR}Asan
        info "engine_tars: "${engine_asan_tars[@]}
        for engine_tar in ${engine_asan_tars[@]}
        do
            cp ${PACKAGE_DIR}/${engine_tar} ${DEMO_VIDEO_ENGINE_DIR}Asan/
            [ ${?} != 0 ] && error "Failed to cp ${PACKAGE_DIR}/${engine_tar} ${DEMO_VIDEO_ENGINE_DIR}Asan/"
            cd ${DEMO_VIDEO_ENGINE_DIR}Asan
            info "decompress ${engine_tar}"
            tar -zxvf ${engine_tar}
            [ ${?} != 0 ] && error "Failed to decompress ${engine_tar}" && return -1
            rm -rf ${engine_tar}
            cd -
        done
    else
        mkdir -p ${DEMO_VIDEO_ENGINE_DIR}
        info "engine_tars: "${engine_tars[@]}
        for engine_tar in ${engine_tars[@]}
        do
            cp ${PACKAGE_DIR}/${engine_tar} ${DEMO_VIDEO_ENGINE_DIR}/
            [ ${?} != 0 ] && error "Failed to cp ${PACKAGE_DIR}/${engine_tar} ${DEMO_VIDEO_ENGINE_DIR}/"
            cd ${DEMO_VIDEO_ENGINE_DIR}
            info "decompress ${engine_tar}"
            tar -zxvf ${engine_tar}
            [ ${?} != 0 ] && error "Failed to decompress ${engine_tar}" && return -1
            rm -rf ${engine_tar}
            cd -
        done
    fi

    if [ "${AN_PRODUCT_PACKAGE_LIST}" == "system" ]; then
        cp -rf ${DEMO_VIDEO_ENGINE_DIR}/system/vendor/ ${DEMO_VIDEO_ENGINE_DIR}/
    fi
}

build()
{
    info "Begin build"
    unpack_binary_lib
    copy_openh264
    copy_script
    chmod -R 755 ${MODULE_OUTPUT_DIR}
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
