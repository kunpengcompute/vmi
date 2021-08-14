#!/bin/bash
# build CloudGame Apk
# Copyright © Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=CloudGameApk
source ${cur_file_path}/../../../build/tools/compile_log.sh
BASE_ENGINE_PATH=${cur_file_path}/../../../output/native/release_imgs/
BUILD_TIME=`date "+%Y%m%d"`
BUILD_TYPE="smokeApk"

inc()
{
    info "Begin Incremental compile"
    if [ ${1} == "smokeApk" ];then
        BUILD_TYPE="smokeApk"
    fi

    if [ -f "${BASE_ENGINE_PATH}/BaseEngine.tar.gz" ]; then
        tar -zvxf ${BASE_ENGINE_PATH}/BaseEngine.tar.gz -C ${BASE_ENGINE_PATH}/
    fi

    mkdir -p ${cur_file_path}/../../Libs
    tar -zvxf ${MODULE_OUTPUT_DIR}/../InstructionEngineClient.tar.gz -C ${cur_file_path}/../../Libs/
    tar -zvxf ${MODULE_OUTPUT_DIR}/../AudioEngineClient.tar.gz -C ${cur_file_path}/../../Libs/
    tar -zvxf ${MODULE_OUTPUT_DIR}/../TouchEngineClient.tar.gz -C ${cur_file_path}/../../Libs/
    tar -zvxf ${MODULE_OUTPUT_DIR}/../VmiCommunication.tar.gz -C ${cur_file_path}/../../Libs/
    tar -zvxf ${MODULE_OUTPUT_DIR}/../EmuGLRender.tar.gz -C ${cur_file_path}/../../Libs/

    cp ${cur_file_path}/../../Libs/AudioEngineClient.aar ${cur_file_path}/app/libs/
    cp ${cur_file_path}/../../Libs/TouchEngineClient.aar ${cur_file_path}/app/libs/
    mkdir -p ${cur_file_path}/app/src/main/jniLibs/armeabi-v7a
    cp ${cur_file_path}/../../Libs/libInstructionEngineClient.so ${cur_file_path}/app/src/main/jniLibs/armeabi-v7a
    cp ${cur_file_path}/../../Libs/libVmiInstrCommon.so ${cur_file_path}/app/src/main/jniLibs/armeabi-v7a
    cp ${cur_file_path}/../../Libs/libVmiInstructionCommon.so ${cur_file_path}/app/src/main/jniLibs/armeabi-v7a
    cp ${cur_file_path}/../../Libs/libEmuGLRender.so ${cur_file_path}/app/src/main/jniLibs/armeabi-v7a
    cp ${cur_file_path}/../../Libs/libCommunication.so ${cur_file_path}/app/src/main/jniLibs/armeabi-v7a
    cp ${cur_file_path}/../../Libs/libunitrans-hmtp.so ${cur_file_path}/app/src/main/jniLibs/armeabi-v7a
    cp ${cur_file_path}/../../Libs/InstructionEngineClient.h  ${cur_file_path}/app/src/main/cpp
    cp ${cur_file_path}/../../Libs/InstructionEngine.h  ${cur_file_path}/app/src/main/cpp
    sed -i "s/V200B001/${BUILD_TIME}/g" ${cur_file_path}/app/build.gradle

    ${AN_GRADLEDIR}/bin/gradle --no-daemon assembleDebug -x test -x lint --stacktrace 
    [ ${?} != 0 ] && error "Failed to Incremental compile" && return -1

    if [ -n "${MODULE_OUTPUT_DIR}" ]; then
        if [ ${BUILD_TYPE} == "smokeApk" ]; then
            mv ${cur_file_path}/app/build/outputs/apk/app1/debug/cloudphone_client.apk ${cur_file_path}/app/build/outputs/apk/CloudGame_Smoke_${BUILD_TIME}.apk
            cp ${cur_file_path}/app/build/outputs/apk/CloudGame_Smoke_${BUILD_TIME}.apk ${MODULE_OUTPUT_DIR}
            cp ${cur_file_path}/app/build/outputs/apk/CloudGame_Smoke_${BUILD_TIME}.apk ${MODULE_OUTPUT_DIR}/..
        fi
    fi
    rm -rf ${BASE_ENGINE_PATH}/AudioEngine*
    rm -rf ${BASE_ENGINE_PATH}/TouchEngine*
    info "Incremental compile success"
}

cleanprebuilt()
{
    info "Begin clean prebuilt"
    rm -rf ${cur_file_path}/../../Libs
    info "End clean prebuilt"
}

clean()
{
    info "Begin Clean"
    rm -rf app/build
    rm -rf app/.externalNativeBuild
    rm -rf ${cur_file_path}/app/src/main/cpp/InstructionEngine.h
    rm -rf ${cur_file_path}/app/src/main/cpp/InstructionEngineClient.h
    cleanprebuilt
    info "Clean success"
}

build()
{
    info "Begin build ClientPhone Apk"
    clean
    [ ${?} != 0 ] && error "Failed to clean" && return -1
    inc ${1}
    [ ${?} != 0 ] && error "Failed to build" && return -1

    rm -rf ${cur_file_path}/app/build/outputs/apk/*
    rm -rf ${cur_file_path}/app/libs/AudioEngineClient.aar
    rm -rf ${cur_file_path}/app/libs/TouchEngineClient.aar
    rm -rf ${cur_file_path}/app/libs/VMI*.aar
    info "End build ClientPhone Apk"
    info "build success"
}

ACTION=$1; shift
case "$ACTION" in
    build) build "$@";;
    clean) clean "$@";;
    inc) inc "$@";;
    *) error "input command[$ACTION] not support.";;
esac
