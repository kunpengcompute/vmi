#!/bin/bash
####################################################################################
# 设置 aarch64 虚拟机 unity/ue4 运行环境
# 1. GPU 核心锁频 2100 MHz; GPU 显存锁频 8001 MHz
# 2. 开启透明大页
# PS: 运行虚拟机前请先在 host 端锁定 CPU 频率和禁用透明大页
####################################################################################

echo "设置 aarch64 虚拟机 unity/ue4 运行环境"
echo "PS: 运行虚拟机前请先在 host 端锁定 CPU 频率和禁用透明大页"

which nvidia-smi
if [ $? -ne 0 ]; then
    echo "没有 nvidia-smi 命令, 请先安装 nvidia 驱动"
    exit 1
fi

# 2. GPU 核心锁频 2100 MHz; GPU 显存锁频 8001 MHz
nvidia-smi -pm 1
nvidia-smi -lgc 2100
nvidia-smi -lmc 8001
# 3. aarch64 设置透明大页为 always
echo always >/sys/kernel/mm/transparent_hugepage/enabled