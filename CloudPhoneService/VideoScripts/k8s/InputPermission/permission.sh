#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

directory="./host-files"
BINDER_MAJOR_ID=$(cat /proc/devices | grep binder | awk '{print $1}')
while true
do
    for sub_dir in "$directory"/*/; do
	subdir_name=$(basename "$sub_dir")
        echo "c 13:* rwm" >> "$sub_dir/devices.allow"
        echo "c $BINDER_MAJOR_ID:* rwm" >> "$sub_dir/devices.allow"
    done
    sleep 2
done

