#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

LOG_FILE_PATH=$(echo "$@" | awk -v search="--log" '
  {
    for (i=1; i<=NF; i++) {
      # 找到 --log 参数
      if ($i == search) {
        # 打印下一个字段（日志文件路径），并退出
        print $(i+1)
        exit
      }
    }
  }'
)

CONTAINER_ID=$(echo "$@" | awk -v search="start" '
  {
    for (i=1; i<=NF; i++) {
      # 找到 start 参数
      if ($i == search) {
        # 打印下一个字段（container id），并退出
        print $(i+1)
        exit
      }
    }
  }'
)

if [ -n "$LOG_FILE_PATH" ]; then
    TASK_PATH=$(dirname "$LOG_FILE_PATH")
    CONFIG_FILE="$TASK_PATH/config.json"

    if [ -f "$CONFIG_FILE" ]; then
            # 输出文件内容
            jq '.linux.resources.devices = [{"allow": false, "access": "rwm"}, {"allow": true,"type": "c", "major": -1, "minor": -1, "access": "rwm"}, {"allow": true, "type": "b", "major": 7, "minor": -1, "access": "rwm"}]' $CONFIG_FILE > config.json.tmp
            if [ -n "$CONTAINER_ID" ]; then
                sandbox_name=$(jq -r '.annotations["io.kubernetes.cri.sandbox-name"]' $CONFIG_FILE)
                echo $CONTAINER_ID > /var/run/containerd/$sandbox_name
            fi
            mv config.json.tmp $CONFIG_FILE
    fi
fi

/usr/bin/va-container-runtime "$@"