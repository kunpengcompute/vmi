#!/bin/bash
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# vmi is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

# build script

# 1.检查所需变量是否存在
# 2.构建
# android 11 demo仓 构建脚本
# ./build_video.sh video_server 视频流服务端
# ./build_video.sh video_client 视频流客户端

set -e
root_dir=$(realpath $(dirname ${0}))
cd ${root_dir}
moudle_name_list=(video_server video_client)
moudle_name=$1
export VMIENGINE_OUTPUT_DIR=${root_dir}/output/
source ${root_dir}/scripts/build_module_func.sh ${root_dir}
source ${root_dir}/scripts/compile_log.sh
check_parame(){
    for name in ${moudle_name_list[@]}
    do
        [ "$moudle_name" == "$name" ] && return 0
    done
    error "command ${moudle_name} not found"
    return -1
}

mkdir -p ${VMIENGINE_OUTPUT_DIR}
check_parame
[ $? != 0 ] && exit 0
check_env
[ $? != 0 ] && exit 0

#下载开源软件
bash ${root_dir}/scripts/download_open_source.sh


ACTION=$1; shift
case "$ACTION" in
    video_server) video_server_compile "$@";;
    video_client) video_client_compile "$@";;
    *) error "input command[$ACTION] not support.";;
esac