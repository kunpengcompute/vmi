#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

# print build log

set -e

function error()
{
    data_info=$(date "+%Y-%m-%d %H:%M:%S")
    log_data="[${data_info}][${MODULE_NAME}][error]${1}"
    echo -e  "\033[1;31m${log_data}\033[0m"
}

function info()
{
    data_info=$(date "+%Y-%m-%d %H:%M:%S")
    log_data="[${data_info}][${MODULE_NAME}][info]${1}"
    echo -e "\033[1;36m${log_data}\033[0m"
}

function warning()
{
    data_info=$(date "+%Y-%m-%d %H:%M:%S")
    log_data="[${data_info}][${MODULE_NAME}][warning]${1}"
    echo -e "\033[1;32m${log_data}\033[0m"
}
