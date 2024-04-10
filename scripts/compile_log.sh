#!/bin/bash
# print build log
# Copyright © Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.

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
