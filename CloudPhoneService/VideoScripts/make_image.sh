#!/bin/bash
# 本脚本将基于kbox基础云手机镜像和视频流云手机相关的二进制制作视频流云手机镜像，方便用户使用
# 操作命令
# ./make_images.sh [kbox基础云手机镜像名称] [视频流云手机镜像名称]
# 参数可选，默认为kbox基础云手机镜像名称：kbox:latest，视频流云手机镜像名称：video:latest
# 例：
# ./make_image.sh
# ./make_image.sh kbox:latest
# ./make_image.sh kbox:latest video:latest

set -e

CUR_PATH=$(cd $(dirname "${0}");pwd)

# kbox基础云手机和视频流云手机默认镜像名称，可根据需要自行修改
DOCKER_IMAGE_KBOX=kbox:latest
DOCKER_IMAGE_VIDEO=video:latest

DOCKER_FILE=${CUR_PATH}/Dockerfile
DOCKER_FILE_CANDIDATE=${CUR_PATH}/Dockerfile_NoVPU

VIDEO_DEMO_PACKAGE=${CUR_PATH}/DemoVideoEngine.tar.gz
VIDEO_BINARY_PACKAGE="BoostKit-videoengine_*.zip"
VIDEO_ENGINE_PACKAGE=VideoEngine.tar.gz
ENC_PACKAGE=${CUR_PATH}/NETINT.tar.gz

TMP_VIDEO_DEMO_DID=${CUR_PATH}/DemoVideoEngine
TMP_VIDEO_BINARY_DIR=${CUR_PATH}/BoostKit-videoengine_unpack
TMP_ENC_DIR=${CUR_PATH}/NETINT

# 检查nvme指令
function check_nvme_command()
{
    cmd="nvme --help"
    $cmd >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "\033[1;31m[ERROR] nvme command unavailable, please install nvme-cli. \033[0m"
        exit -1
    fi
}

function check_netint_package()
{
    # 判断编码包是否存在
    if [ ! -f "${ENC_PACKAGE}" ]; then
        echo -e "\033[1;33m[WARNING] ${ENC_PACKAGE} not exist, making image without encode package. \033[0m"
    fi
}

function check_vpu()
{
    check_nvme_command
    cmd="nvme list"
    output=$($cmd)

    if echo "$output" | grep -q "T432"; then
        echo -e "\033[1;36m[INFO] make image for T432 \033[0m"
        DOCKER_FILE_CANDIDATE=${CUR_PATH}/Dockerfile_T432
        check_netint_package
    elif echo "$output" | grep -q "QuadraT2A"; then
        echo -e "\033[1;36m[INFO] make image for QuadraT2A \033[0m"
        DOCKER_FILE_CANDIDATE=${CUR_PATH}/Dockerfile_QuadraT2A
        check_netint_package
    else
        echo -e "\033[1;36m[INFO] make image without vpu \033[0m"
    fi

    # 判断Dockerfile是否存在
    if [ ! -f "${DOCKER_FILE_CANDIDATE}" ]; then
        echo -e "\033[1;31m[ERROR] ${DOCKER_FILE_CANDIDATE} is not exist. \033[0m" && return -1
    fi

    cp ${DOCKER_FILE_CANDIDATE} ${DOCKER_FILE}
}

# 前置条件验证
function check_env()
{

    # 判断视频流云手机原型包是否存在
    if [ ! -f "${VIDEO_DEMO_PACKAGE}" ]; then
        echo -e "\033[1;31m[ERROR] ${VIDEO_DEMO_PACKAGE} is not exist. \033[0m" && return -1
    fi
    # 判断视频流云手机商用二进制包或VideoEngine.tar.gz是否存在
    if [ -z "$(ls ${CUR_PATH}/${VIDEO_BINARY_PACKAGE} 2>/dev/null)" ] && [ ! -f "${CUR_PATH}/${VIDEO_ENGINE_PACKAGE}" ]; then
        echo -e "\033[1;31m[ERROR] ${VIDEO_BINARY_PACKAGE} or ${VIDEO_ENGINE_PACKAGE} is not exist. \033[0m" && return -1
    fi
    if [ ! -f "${CUR_PATH}/${VIDEO_ENGINE_PACKAGE}" ] && [ $(ls ${CUR_PATH}/${VIDEO_BINARY_PACKAGE} | wc -l) -gt 1 ]; then
        echo -e "\033[1;31m[ERROR] ${VIDEO_BINARY_PACKAGE} is more than 1!. \033[0m" && return -1
    fi

    check_vpu

    # 判断视频流云手机原型包目录是否存在，若存在则删除重建
    if [ -d "${TMP_VIDEO_DEMO_DID}" ]; then
        rm -rf ${TMP_VIDEO_DEMO_DID}
    fi
    mkdir -p ${TMP_VIDEO_DEMO_DID}

    # 若当前目录不存在VideoEngine.tar.gz，判断二进制包目录是否存在，若存在则删除重建
    if [ ! -f "${CUR_PATH}/${VIDEO_ENGINE_PACKAGE}" ] && [ -d "${TMP_VIDEO_BINARY_DIR}" ]; then
        rm -rf ${TMP_VIDEO_BINARY_DIR}
        mkdir -p ${TMP_VIDEO_BINARY_DIR}
    fi

    if [ -f "${ENC_PACKAGE}" ]; then
        # 判断编码包目录是否存在，若存在则删除重建
        if [ -d "${TMP_ENC_DIR}" ]; then
            rm -rf ${TMP_ENC_DIR}
        fi
        mkdir -p ${TMP_ENC_DIR}
    fi

    # 解压视频流云手机原型包（注意：通过--no-same-owner保证解压后的文件属组为当前操作用户（root）文件属组）
    cmd="tar --no-same-owner -zxvf ${VIDEO_DEMO_PACKAGE} -C ${TMP_VIDEO_DEMO_DID}"
    echo -e "\033[1;36m[INFO] "$cmd" \033[0m"
    $cmd > /dev/null

    # 解压商用包到${TMP_VIDEO_DEMO_DID}，如果当前目录存在VideoEngine.tar.gz直接解压，否则先从二进制包中获取VideoEngine.tar.gz
    if [ -f "${CUR_PATH}/${VIDEO_ENGINE_PACKAGE}" ]; then
        cmd="tar --no-same-owner -zxvf ${CUR_PATH}/${VIDEO_ENGINE_PACKAGE} -C ${TMP_VIDEO_DEMO_DID}"
        echo -e "\033[1;36m[INFO] "$cmd" \033[0m"
        $cmd > /dev/null
    else
        local real_binary_name=$(ls ${CUR_PATH}/${VIDEO_BINARY_PACKAGE})
        cmd="unzip -d ${TMP_VIDEO_BINARY_DIR} ${real_binary_name}"
        echo -e "\033[1;36m[INFO] "$cmd" \033[0m"
        $cmd > /dev/null
        cmd="tar --no-same-owner -zxvf ${TMP_VIDEO_BINARY_DIR}/${VIDEO_ENGINE_PACKAGE} -C ${TMP_VIDEO_DEMO_DID}"
        echo -e "\033[1;36m[INFO] "$cmd" \033[0m"
        $cmd > /dev/null
    fi

    if [ -f "${ENC_PACKAGE}" ]; then
        # 解压编码包（注意：通过--no-same-owner保证解压后的文件属组为当前操作用户（root）文件属组）
        cmd="tar --no-same-owner -zxvf ${ENC_PACKAGE} -C ${TMP_ENC_DIR}"
        echo -e "\033[1;36m[INFO] "$cmd" \033[0m"
        $cmd > /dev/null
    fi
}

# 制作镜像
function make_image()
{
    if [ -n "$1" ]; then
        DOCKER_IMAGE_KBOX="$1"
    fi
    if [ -n "$2" ]; then
        DOCKER_IMAGE_VIDEO="$2"
    fi
    cmd="docker build --build-arg KBOX_IMAGE=${DOCKER_IMAGE_KBOX} -t ${DOCKER_IMAGE_VIDEO} ."
    echo -e "\033[1;36m[INFO] "$cmd" \033[0m"
    $cmd
    wait
}

function main()
{
    echo -e "\033[1;36m[INFO] Begin to make image \033[0m"
    # 构建镜像
    local start_time=$(date +%s)
    local end_time=0

    check_env
    [ ${?} != 0 ] && echo -e "\033[1;31m[ERROR] Failed to check environment. \033[0m" && exit -1

    make_image "$@"
    [ ${?} != 0 ] && echo -e "\033[1;31m[ERROR] Failed to make image. \033[0m" && exit -1

    # 制作镜像时间统计
    end_time=$(date +%s)
    time_interval=$((${end_time}-${start_time}))
    echo -e "\033[1;36m[INFO] End to make image from ${DOCKER_IMAGE_KBOX} to ${DOCKER_IMAGE_VIDEO} \033[0m"
    echo -e "\033[1;36m[INFO] Make image takes ${time_interval} seconds. \033[0m"
}

main "$@"
