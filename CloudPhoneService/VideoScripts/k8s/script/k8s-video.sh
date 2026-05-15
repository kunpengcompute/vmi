#!/bin/bash

ACTION=$1
MIN=$2
MAX=$3
SYSTEM_SIZE_MB=$4
if [ -z $3 ]; then
    MAX=$2
fi
if [ -z $4 ]; then
    SYSTEM_SIZE_MB=0 # 如果没有传入第五个参数，默认不使能配额限制
fi
NFS_DATA_PATH="/tmp/nfs"
STORAGE_SIZE_GB=32

function check_nfs_mount() {
    local nfs_dir=$1
    
    if [ ! -d "$nfs_dir" ]; then
        echo -e "\033[1;31m[ERROR] NFS目录 ${nfs_dir} 不存在！\033[0m"
        exit 1
    fi
    
    if ! mountpoint -q "$nfs_dir"; then
        echo -e "\033[1;31m[ERROR] NFS目录 ${nfs_dir} 不是挂载点！\033[0m"
        exit 1
    fi
    
    if ! mount | grep " ${nfs_dir} " | grep -qE " type nfs| type nfs4"; then
        echo -e "\033[1;31m[ERROR] NFS目录 ${nfs_dir} 不是NFS远端挂载目录！\033[0m"
        exit 1
    fi
    
    echo "NFS目录 ${nfs_dir} 检查通过"
    return 0
}

function start(){
    if [ -z "$DATA_BASE_PATH" ]; then
        export DATA_BASE_PATH="/home/mount"
    fi
    for ((i=$MIN; i<=$MAX; i++))
    do
        export YAML_PATH=$i # yaml配置文件位置
        export POD_NAME=video$i # pod名称及容器名称
        touch /var/run/containerd/video$i # 存放容器id，挂载到容器内
        export HOSTPORT=$(expr $i + 8000) # 宿主机端口号
        export HOSTPORT2=$(expr $i + 8500)
        export SYSTEM_SIZE_MB
        envsubst < k8s-video.yaml | kubectl apply -f -
        
        if [ "$SYSTEM_SIZE_MB" -gt 0 ]; then
            echo "Pod ${POD_NAME} 创建成功，配额(${SYSTEM_SIZE_MB}MB)将由NRI插件自动应用"
        fi
    done
}

function nstart(){
    check_nfs_mount "${NFS_DATA_PATH}"
    echo "Creating storage: MIN=$MIN, MAX=$MAX, SIZE=${STORAGE_SIZE_GB}G"
    if [ ! -d "${NFS_DATA_PATH}/img" ]; then
        mkdir -p ${NFS_DATA_PATH}/img
    fi
    for ((i=$MIN; i<=$MAX; i++))
    do
        IMG=${NFS_DATA_PATH}/img/video$i.img
        if [ ! -e $IMG ]; then
            echo "Creating image file: $IMG"
            fallocate -l ${STORAGE_SIZE_GB}G $IMG
            yes | mkfs -t ext4 $IMG
        fi
        DATA_PATH="${NFS_DATA_PATH}/data/video$i/data"
        mkdir -p $DATA_PATH
        if ! mountpoint -q $DATA_PATH 2>/dev/null; then
            mount $IMG $DATA_PATH
            if [ $? -eq 0 ]; then
                echo $(($STORAGE_SIZE_GB * 2 * 1024 * 1024)) >$DATA_PATH/storage_size
                echo "Mounted $IMG to $DATA_PATH"
            else
                echo -e "\033[1;31m[ERROR] Failed to mount $IMG to $DATA_PATH\033[0m"
            fi
        else
            echo "Already mounted: $DATA_PATH"
        fi
    done
    export DATA_BASE_PATH=${NFS_DATA_PATH}
    start "$@"
}

function ndelete(){
    for ((i=$MIN; i<=$MAX; i++))
    do
        kubectl delete pod video$i
        DATA_PATH="${NFS_DATA_PATH}/data/video$i/data"
        if [ -d ${DATA_PATH} ]; then
            umount ${DATA_PATH} 2>/dev/null
            rm -rf ${NFS_DATA_PATH}/data/video$i
        fi
    done
    echo "Storage unmounted for video$MIN to video$MAX"
}

function delete(){
    for ((i=$MIN; i<=$MAX; i++))
    do
        kubectl delete pod video$i
    done
}

case "$ACTION" in
    start) start "$@";;
    nstart) nstart "$@";;
    delete) delete "$@";;
    ndelete) ndelete "$@";;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac
