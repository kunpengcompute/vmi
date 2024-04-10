#!/bin/bash
# 用于存放编译各个模块的函数
# 使用方法：source ${VMIENGINE_ROOT_DIR}/scripts/cloud_ndk_build_func.sh ${root_dir}
# 上述命令中${root_dir}是VMIEngine代码的根目录，否则此脚本可能获取不到正确的路径导致执行出错。

set -e
root_dir=${1}
env_list=(JAVA_HOME AN_SDKDIR CMAKE AN_NDKDIR)
unpack_open_source=${root_dir}/unpack_open_source
source ${root_dir}/scripts/build_env.conf
source ${root_dir}/scripts/compile_log.sh
check_env(){
    for var in ${env_list[@]}
    do
    [ -z $(eval echo '$'${var}) ] && error "please set ${var}" && return -1
    done
    return 0
}

video_server_compile()
{
    #OpusCloud
    info "begin build OpusCloud"
    bash ${unpack_open_source}/opus/build.sh build Cloud
    cp ${unpack_open_source}/opus/output/OpusCloud.tar.gz ${VMIENGINE_OUTPUT_DIR}/OpusCloud.tar.gz
    cp ${unpack_open_source}/opus/output/symbols/OpusCloudSymbol.tar.gz ${VMIENGINE_OUTPUT_DIR}/OpusCloudSymbol.tar.gz
    info "build OpusCloud success"
    #VmiCommunicationCloud
    info "begin build VmiCommunicationCloud"
    cd ${root_dir}/Common/
    bash ${root_dir}/Common/build.sh build Cloud
    cd output
    cp VmiCommunicationCloud.tar.gz ${VMIENGINE_OUTPUT_DIR}
    cd symbols
    tar -zcvf VmiCommunicationCloudSymbol.tar.gz vendor
    cp VmiCommunicationCloudSymbol.tar.gz ${VMIENGINE_OUTPUT_DIR}
    info "build VmiCommunicationCloud success"
    #VideoCodec
    info "begin build VideoCodec"
    bash ${root_dir}/CloudPhoneService/VideoEngine/Media/build.sh build
    cp ${root_dir}/CloudPhoneService/VideoEngine/Media/output/VideoCodec.tar.gz ${VMIENGINE_OUTPUT_DIR}/VideoCodec.tar.gz
    info "build VideoCodec success"
    info "begin build GpuEncTurbo"
    bash ${root_dir}/CloudPhoneService/VideoEngine/GpuEncTurbo/build.sh build
    cp  ${root_dir}/CloudPhoneService/VideoEngine/GpuEncTurbo/output/GpuEncTurbo.tar.gz ${VMIENGINE_OUTPUT_DIR}/GpuEncTurbo.tar.gz
    cp  ${root_dir}/CloudPhoneService/VideoEngine/GpuEncTurbo/output/symbols/GpuEncTurboSymbol.tar.gz ${VMIENGINE_OUTPUT_DIR}/GpuEncTurboSymbol.tar.gz
    info "build GpuEncTurbo success"
    #VmiAgent
    info "begin build VmiAgent"
    bash ${root_dir}/CloudPhoneService/VmiAgent/build.sh build
    cp ${root_dir}/CloudPhoneService/VmiAgent/output/VmiAgent.tar.gz ${VMIENGINE_OUTPUT_DIR}/VmiAgent.tar.gz
    cp ${root_dir}/CloudPhoneService/VmiAgent/output/VmiAgentSymbol.tar.gz ${VMIENGINE_OUTPUT_DIR}/VmiAgentSymbol.tar.gz
    info "build VmiAgent success"
    info "begin build DemoVideoEngine"
    bash ${root_dir}/CloudPhoneService/VideoEngine/build.sh build
    cd ${VMIENGINE_OUTPUT_DIR}/DemoVideoEngine
    tar -zcvf ../DemoVideoEngine.tar.gz *
    cd ${VMIENGINE_OUTPUT_DIR}
    rm -rf DemoVideoEngine
    info "build DemoVideoEngine success"
}


video_client_compile()
{
    # VmiCommunicationClient
    info "begin build VmiCommunicationClient"
    cd ${root_dir}/Common/
    bash ${root_dir}/Common/build.sh build Client
    cd ${root_dir}/Common/output 
    cp ${root_dir}/Common/output/VmiCommunicationClient.tar.gz ${VMIENGINE_OUTPUT_DIR}
    cp ${root_dir}/Common/output/VmiCommunicationClientSymbol.tar.gz ${VMIENGINE_OUTPUT_DIR}
    info "build VmiCommunicationClient success"
    AudioEngineClient_compile
    TouchEngineClient_compile
    VideoEngineClient_compile
    # NdkDecoder
    info "begin build NdkDecoder"
    bash ${root_dir}/CloudPhoneClient/VideoEngine/NdkDecoder/build.sh build
    info "begCloudPhoneApkin build NdkDecoder success"
    # CloudPhoneApk
    info "begin build CloudPhoneApk"
    bash ${root_dir}/CloudPhoneClient/VideoEngine/CloudPhoneUI/build.sh build
    cd ${VMIENGINE_OUTPUT_DIR}
    tar -zcvf CloudPhoneApk.tar.gz CloudPhone.apk
    info "build CloudPhoneApk success"
}

AudioEngineClient_compile(){
    # AudioEngineClient
    info "begin build AudioEngineClient"
    bash ${root_dir}/CloudPhoneClient/AudioPlay/build.sh build
    pushd ${root_dir}/CloudPhoneClient/AudioPlay/build/outputs/aar/
    mv AudioPlay-debug.aar AudioEngineClient.aar
    tar -zcvf AudioEngineClient.tar.gz AudioEngineClient.aar
    cp AudioEngineClient.tar.gz ${VMIENGINE_OUTPUT_DIR}
    popd
    info "build AudioEngineClient success"
}

TouchEngineClient_compile(){
    # AudioEngineClient
    info "begin build TouchEngineClient"
    bash ${root_dir}/CloudPhoneClient/TouchCapture/build.sh build
    pushd ${root_dir}/CloudPhoneClient/TouchCapture/build/outputs/aar/
    mv TouchCapture-debug.aar TouchEngineClient.aar
    tar -zcvf TouchEngineClient.tar.gz TouchEngineClient.aar
    mv TouchEngineClient.tar.gz ${VMIENGINE_OUTPUT_DIR}
    popd
    info "build TouchEngineClient success"
}

VideoEngineClient_compile(){
    VideoEngineDecoder_compile
    # AudioEngineClient
    info "begin build VideoEngineClient"
    mkdir -p ${VMIENGINE_OUTPUT_DIR}/VideoEngineClient/include
    bash ${root_dir}/CloudPhoneClient/VideoDecoder/build.sh build
    cp -r ${root_dir}/CloudPhoneClient/VideoDecoder/libs/arm64-v8a/* ${VMIENGINE_OUTPUT_DIR}/VideoEngineClient/
    cp -r ${root_dir}/CloudPhoneClient/VideoDecoder/Decoder/libs/arm64-v8a/* ${VMIENGINE_OUTPUT_DIR}/VideoEngineClient/
    cp ${root_dir}/CloudPhoneClient/VideoDecoder/Control/VideoEngineClient.h ${VMIENGINE_OUTPUT_DIR}/VideoEngineClient/include/
    cd ${VMIENGINE_OUTPUT_DIR}/VideoEngineClient
    tar -zcvf VideoEngineClient.tar.gz *
    mv VideoEngineClient.tar.gz ${VMIENGINE_OUTPUT_DIR}
    info "build VideoEngineClient success"
}

VideoEngineDecoder_compile(){
    # VideoEngineDecoder
    info "begin build VideoEngineDecoder"
    mkdir -p ${VMIENGINE_OUTPUT_DIR}/VideoEngineDecoder
    bash ${root_dir}/CloudPhoneClient/VideoDecoder/Decoder/build.sh build
    pushd ${root_dir}/CloudPhoneClient/VideoDecoder/Decoder/libs/
    tar -zcvf VideoEngineDecoder.tar.gz *
    mv VideoEngineDecoder.tar.gz ${VMIENGINE_OUTPUT_DIR}
    popd
    info "build VideoEngineDecoder success"
}