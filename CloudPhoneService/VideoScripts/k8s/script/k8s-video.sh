#!/bin/bash

ACTION=$1
MIN=$2
MAX=$3
if [ -z $3 ]; then
    MAX=$2
fi
function start(){
    for ((i=$MIN; i<=$MAX; i++))
    do
        export YAML_PATH=$i  #yaml配置文件位置
        export POD_NAME=video$i  # pod名称及容器名称
        export HOSTPORT=$(expr $i + 8000)   # 宿主机端口号
        export HOSTPORT2=$(expr $i + 8500)
        envsubst < k8s-video.yaml | kubectl apply -f -
    done
}

function delete(){
    for ((i=$MIN; i<=$MAX; i++))
    do
        kubectl delete pod video$i
    done
}

function prepare_loop_device() {
    # 清空/dev/目录下现有的loop device
    for dev in /dev/loop[0-9]*; do
        # 不删除loop device软链接
        if [ -L "$dev" ]; then
            continue
        fi

        if [ -b "$dev" ]; then
            echo "Removing $dev"
            rm -f "$dev"
        fi
    done

    local TARGET_DIR="/dev/loop_device"
    local LOOP_NUM=20000

    mkdir -p "$TARGET_DIR"

    # 统计已有loop device节点数量
    count=$(ls -1 "$TARGET_DIR"/loop* 2>/dev/null | wc -l)
    # 如果少于预期节点个数LOOP_NUM，则补齐
    if [ "$count" -lt $LOOP_NUM ]; then
        echo "Creating loop device... This process will take about 1 min."

        for i in $(seq 0 $((LOOP_NUM - 1))); do
            local guest_node="$TARGET_DIR/loop$i"
            local host_node="/dev/loop$i"

            if [ ! -e "$guest_node" ]; then
                mknod -m 0666 "$guest_node" b 7 $i
            fi

            if [ ! -e "$host_node" ]; then
                ln -s "$guest_node" "$host_node"
            fi
        done

        echo "Loop device created successfully."
    else
        echo "Skip creating loop device."
    fi
}

prepare_loop_device

case "$ACTION" in
    start) start "$@";;
    delete) delete "$@";;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac
