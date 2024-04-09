#!/bin/bash
# build CloudPhoneApk
# Copyright © Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
cur_file_path=$(cd $(dirname "${0}");pwd)
cd "${cur_file_path}"
export MODULE_NAME=CloudPhoneApk
source ${cur_file_path}/../../../scripts/compile_log.sh
REPO_ROOT_DIR=${cur_file_path}/../../../

cleanprebuilt()
{
    info "Begin clean prebuilt"
    rm -rf ${cur_file_path}/Libs
    rm -rf app/libs/AudioEngineClient.aar
    rm -rf app/libs/TouchEngineClient.aar
    rm -rf app/src/main/jniLibs/arm64-v8a/*.so
    rm -rf app/src/main/cpp/VideoEngineClient.h
    rm -rf app/src/main/cpp/VmiEngine.h
    rm -rf app/src/main/cpp/VmiDef.h
    info "End clean prebuilt"
}

clean_build_env()
{
    info "Begin Clean"
    rm -rf .gradle
    rm -rf app/build
    rm -rf app/.cxx
    cleanprebuilt
    info "Clean success"
}

build()
{
    info "Begin build CloudPhoneApk"
    clean_build_env
    [ ${?} != 0 ] && error "Failed to clean build env" && return -1

    cleanprebuilt
    mkdir -p ${cur_file_path}/Libs
    if [ -z ${MODULE_OUTPUT_DIR} ];then
        MODULE_OUTPUT_DIR=${VMIENGINE_OUTPUT_DIR}/CloudPhoneApk
        mkdir -p ${MODULE_OUTPUT_DIR}
    fi
    tar -zvxf ${MODULE_OUTPUT_DIR}/../AudioEngineClient.tar.gz -C ${cur_file_path}/Libs/
    tar -zvxf ${MODULE_OUTPUT_DIR}/../TouchEngineClient.tar.gz -C ${cur_file_path}/Libs/
    tar -zvxf ${MODULE_OUTPUT_DIR}/../VmiCommunicationClient.tar.gz -C ${cur_file_path}/Libs/
    tar -zvxf ${MODULE_OUTPUT_DIR}/../VideoEngineClient.tar.gz -C ${cur_file_path}/Libs/

    cp ${cur_file_path}/Libs/AudioEngineClient.aar ${cur_file_path}/app/libs/
    cp ${cur_file_path}/Libs/TouchEngineClient.aar ${cur_file_path}/app/libs/

    cp ${cur_file_path}/Libs/libCommunication.so ${cur_file_path}/app/src/main/jniLibs/arm64-v8a
    cp ${cur_file_path}/Libs/libDemoUtils.so ${cur_file_path}/app/src/main/jniLibs/arm64-v8a
    cp ${cur_file_path}/Libs/libVideoEngineClient.so ${cur_file_path}/app/src/main/jniLibs/arm64-v8a
    cp ${cur_file_path}/Libs/libDecoder.so ${cur_file_path}/app/src/main/jniLibs/arm64-v8a
    cp ${cur_file_path}/../NdkDecoder/libs/arm64-v8a/libNdkDecoder.so ${cur_file_path}/app/src/main/jniLibs/arm64-v8a
    cp ${cur_file_path}/../NdkDecoder/libs/arm64-v8a/libc++_shared.so ${cur_file_path}/app/src/main/jniLibs/arm64-v8a
    cp ${cur_file_path}/Libs/include/VideoEngineClient.h  ${cur_file_path}/app/src/main/cpp
    cp ${cur_file_path}/../../../Common/Include/VmiEngine.h  ${cur_file_path}/app/src/main/cpp

    ${AN_GRADLEDIR}/bin/gradle --no-daemon assembleDebug -x test -x lint --stacktrace
    [ ${?} != 0 ] && error "Failed to Incremental compile" && return -1

    if [ -n "${MODULE_OUTPUT_DIR}" ];then
        cp ${cur_file_path}/app/build/outputs/apk/CloudPhone.apk ${MODULE_OUTPUT_DIR}/CloudPhone.apk
        cp ${cur_file_path}/app/build/outputs/apk/CloudPhone.apk ${MODULE_OUTPUT_DIR}/../CloudPhone.apk
    fi
    info "End build CloudPhoneApk"
    info "build success"
}

if [ "$1" == "build" ] || [ "$1" == "inc" ]; then
    build
else
    error "Invalid input, please input build or inc"
    exit 1
fi
