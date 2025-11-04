#!/bin/bash

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
if [ -n "$LOG_FILE_PATH" ]; then
    TASK_PATH=$(dirname "$LOG_FILE_PATH")
    CONFIG_FILE="$TASK_PATH/config.json"

    if [ -f "$CONFIG_FILE" ]; then
            # 输出文件内容
            jq '.linux.resources.devices = [{"allow": false, "access": "rwm"}, {"allow": true,"type": "c", "major": -1, "minor": -1, "access": "rwm"}, {"allow": true, "type": "b", "major": 7, "minor": -1, "access": "rwm"}]' $CONFIG_FILE > config.json.tmp
            mv config.json.tmp $CONFIG_FILE
    fi
fi

/usr/bin/va-container-runtime "$@"