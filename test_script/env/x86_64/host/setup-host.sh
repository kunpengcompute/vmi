#!/bin/bash
####################################################################################
# 设置 x86_64 host 端 unity/ue4 运行环境
# 1. CPU 锁频 2.5 GHz
# 2. GPU 核心锁频 2100 MHz; GPU 显存锁频 8001 MHz
# 3. 设置透明大页为默认 
####################################################################################

echo "设置 x86_64 host 端 unity/ue4 运行环境"

which cpupower
if [ $? -ne 0 ]; then
    echo "没有 cpupower 命令, 请先安装相关应用包"
    exit 1
fi
which nvidia-smi
if [ $? -ne 0 ]; then
    echo "没有 nvidia-smi 命令, 请先安装 nvidia 驱动"
    exit 1
fi

# 1. x86_64 CPU 锁频 2.5 GHz
cpupower frequency-set -g userspace
cpupower frequency-set -f 2.5GHz
# 2. GPU 核心锁频 2100 MHz; GPU 显存锁频 8001 MHz
nvidia-smi -pm 1
nvidia-smi -lgc 2100
nvidia-smi -lmc 8001
# 3. x86_64 设置透明大页为 madvise
echo madvise >/sys/kernel/mm/transparent_hugepage/enabled