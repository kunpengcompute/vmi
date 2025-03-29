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

case "$ACTION" in
    start) start "$@";;
    delete) delete "$@";;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac
