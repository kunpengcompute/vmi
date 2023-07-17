#!/bin/bash
####################################################################################
# 设置 x86_64 虚拟机在 host 端的运行环境
# 1. 在 host 端将 CPU 锁频 2.5 GHz
# 2. 禁用透明大页以提高虚拟机性能
####################################################################################

echo "设置 x86_64 虚拟机在 host 端的运行环境"

which cpupower
if [ $? -ne 0 ]; then
    echo "没有 cpupower 命令, 请先安装相关应用包"
    exit 1
fi

# 1. x86_64 CPU 锁频 2.5 GHz
cpupower frequency-set -g userspace
cpupower frequency-set -f 2.5GHz
# 2. 禁用透明大页以提高虚拟机性能
echo never >/sys/kernel/mm/transparent_hugepage/enabled

