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
        cp k8s-video.yaml k8s-video-apply.yaml
        mock_cpu
        envsubst < k8s-video-apply.yaml | kubectl apply -f -
        if [ "$SYSTEM_SIZE_MB" -gt 0 ]; then
            echo "Pod ${POD_NAME} 创建成功，配额(${SYSTEM_SIZE_MB}MB)将由NRI插件自动应用"
        fi
    done
}

function nstart(){
    check_nfs_mount "${NFS_DATA_PATH}"
    
    # 检查是否已存在f2fs镜像
    for ((i=$MIN; i<=$MAX; i++))
    do
        IMG=${NFS_DATA_PATH}/img/video$i.img
        if [ -e $IMG ]; then
            # 检查镜像文件系统类型
            local fs_type=$(file -s $IMG 2>/dev/null | grep -o "f2fs" || echo "")
            if [ "$fs_type" == "f2fs" ]; then
                echo -e "\033[1;31m[ERROR] 不可以同时使能F2FS和NFS！\033[0m"
                echo -e "\033[1;31m[ERROR] 检测到镜像 ${IMG} 为f2fs格式，使用nstart命令（NFS挂载）\033[0m"
                echo -e "\033[1;31m[ERROR] 请删除现有f2fs镜像，或使用storage_manager.sh create创建ext4镜像\033[0m"
                exit 1
            fi
        fi
    done
    
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

#CPU文件模拟
function mock_cpu() {
    local CPU_PATH="/var/lib/kbox/cpus/${POD_NAME}/cpu"
    local CPU_NUM=8
    if [ -d "${CPU_PATH}" ];then
        umount /var/lib/kbox/cpus/$POD_NAME/cpu/cpu*/* > /dev/null 2>&1
        rm -rf /var/lib/kbox/cpus/$POD_NAME
        [ $? -ne 0 ] && echo "fail to remove data files /var/lib/kbox/cpus/$POD_NAME !" && RET="fail"
    fi

    mkdir -p ${CPU_PATH}
    chmod 755 ${CPU_PATH}

    echo "7" >${CPU_PATH}"/kernel_max"
    echo "0-7" >${CPU_PATH}"/possible"
    echo "0-7" >${CPU_PATH}"/present"
    echo "0-7" >${CPU_PATH}"/online"

    chmod 444 ${CPU_PATH}/kernel_max ${CPU_PATH}/possible ${CPU_PATH}/present ${CPU_PATH}/online

    # mock_cpufreq
    mkdir -p ${CPU_PATH}/cpufreq/policy0 ${CPU_PATH}/cpufreq/cpuidle
    chmod 755 ${CPU_PATH}/cpufreq ${CPU_PATH}/cpufreq/policy0 ${CPU_PATH}/cpufreq/cpuidle

    echo "$(seq 0 $(($CPU_NUM - 1))|tr 'n' ' ')" >${CPU_PATH}"/cpufreq/policy0/affected_cpus"
    echo "1954000" >${CPU_PATH}"/cpufreq/policy0/cpuinfo_max_freq"
    echo "1954000" >${CPU_PATH}"/cpufreq/policy0/cpuinfo_cur_freq"
    echo "554000" >${CPU_PATH}"/cpufreq/policy0/cpuinfo_min_freq"
    echo "0" >${CPU_PATH}"/cpufreq/policy0/cpuinfo_transition_latency"
    cat ${CPU_PATH}"/cpufreq/policy0/affected_cpus" >${CPU_PATH}"/cpufreq/policy0/related_cpus"
    echo "554000 860000 956000 1042000 1128000 1224000 1320000 1397000 1512000 1628000 1748000 1858000 1954000" >${CPU_PATH}"/cpufreq/policy0/scaling_available_frequencies"
    echo "interacitve userspace powersave performance schedutil" >${CPU_PATH}"/cpufreq/policy0/scaling_available_governors"
    cat ${CPU_PATH}"/cpufreq/policy0/cpuinfo_cur_freq" >${CPU_PATH}"/cpufreq/policy0/scaling_cur_freq"
    echo "cpufreq-dt" >${CPU_PATH}"/cpufreq/policy0/scaling_driver"
    echo "performance" >${CPU_PATH}"/cpufreq/policy0/scaling_governor"
    cat ${CPU_PATH}"/cpufreq/policy0/cpuinfo_max_freq" >${CPU_PATH}"/cpufreq/policy0/scaling_max_freq"
    cat ${CPU_PATH}"/cpufreq/policy0/cpuinfo_min_freq" >${CPU_PATH}"/cpufreq/policy0/scaling_min_freq"
    echo "<unsupported>" >${CPU_PATH}"/cpufreq/policy0/scaling_setspeed"

    chmod 444 ${CPU_PATH}/cpufreq/policy0/*
    chmod 400 ${CPU_PATH}/cpufreq/policy0/cpuinfo_cur_freq
    chmod 644 ${CPU_PATH}/cpufreq/policy0/scaling_governor ${CPU_PATH}/cpufreq/policy0/scaling_setspeed
    chmod 660 ${CPU_PATH}/cpufreq/policy0/scaling_max_freq ${CPU_PATH}/cpufreq/policy0/scaling_min_freq

    # mock_cpuidle
    mkdir -p ${CPU_PATH}/cpufreq/cpuidle/driver ${CPU_PATH}/cpufreq/cpuidle/state0 ${CPU_PATH}/cpufreq/cpuidle/state1
    chmod 755 ${CPU_PATH}/cpufreq/cpuidle/*

    echo "hisi_cluster0_idle_driver" >${CPU_PATH}"/cpufreq/cpuidle/driver/name"
    chmod 444 ${CPU_PATH}"/cpufreq/cpuidle/driver/name" 

    echo "ARM64 WFI" >${CPU_PATH}"/cpufreq/cpuidle/state0/desc"
    echo "0" >${CPU_PATH}"/cpufreq/cpuidle/state0/disable"
    echo "1" >${CPU_PATH}"/cpufreq/cpuidle/state0/latency"
    echo "WFI" >${CPU_PATH}"/cpufreq/cpuidle/state0/name"
    echo "0" >${CPU_PATH}"/cpufreq/cpuidle/state0/power"
    echo "1" >${CPU_PATH}"/cpufreq/cpuidle/state0/residency"
    echo "$((RANDOM*4+11111))" >${CPU_PATH}"/cpufreq/cpuidle/state0/usage"
    echo "$(($(cat ${CPU_PATH}/cpufreq/cpuidle/state0/usage)*666))" >${CPU_PATH}"/cpufreq/cpuidle/state0/time"

    chmod 444 ${CPU_PATH}/cpufreq/cpuidle/state0/*
    chmod 644 ${CPU_PATH}/cpufreq/cpuidle/state0/disable

    echo "cpu-sleep-0" >${CPU_PATH}"/cpufreq/cpuidle/state1/desc"
    echo "0" >${CPU_PATH}"/cpufreq/cpuidle/state1/disable"
    echo "110" >${CPU_PATH}"/cpufreq/cpuidle/state1/latency"
    echo "cpu-sleep-0" >${CPU_PATH}"/cpufreq/cpuidle/state1/name"
    echo "0" >${CPU_PATH}"/cpufreq/cpuidle/state1/power"
    echo "3000" >${CPU_PATH}"/cpufreq/cpuidle/state1/residency"
    echo "$((RANDOM*+11111))" >${CPU_PATH}"/cpufreq/cpuidle/state1/usage"
    echo "$(($(cat ${CPU_PATH}/cpufreq/cpuidle/state1/usage)*22222))" >${CPU_PATH}"/cpufreq/cpuidle/state0/time"


    chmod 444 ${CPU_PATH}/cpufreq/cpuidle/state1/*
    chmod 644 ${CPU_PATH}/cpufreq/cpuidle/state1/disable

    # mock_cpu*
    for ((j=0; j<8; j++));
    do
        mkdir -p ${CPU_PATH}/cpu$j
        chmod 755 ${CPU_PATH}/cpu$j
        #其他文件的挂载
        mkdir -p ${CPU_PATH}/cpu$j/hotplug ${CPU_PATH}/cpu$j/power ${CPU_PATH}/cpu$j/regs ${CPU_PATH}/cpu$j/topology
        cp /sys/devices/system/cpu/cpu$j/cpu_capacity ${CPU_PATH}/cpu$j/cpu_capacity
        mount --bind /sys/devices/system/cpu/cpu$j/hotplug ${CPU_PATH}/cpu$j/hotplug
        echo "1" >${CPU_PATH}/cpu$j/online
        mount --bind /sys/devices/system/cpu/cpu$j/power ${CPU_PATH}/cpu$j/power
        mount --bind /sys/devices/system/cpu/cpu$j/regs ${CPU_PATH}/cpu$j/regs
        mount --bind /sys/devices/system/cpu/cpu$j/topology ${CPU_PATH}/cpu$j/topology
        mkdir -p ${CPU_PATH}/cpu$j/cpufreq ${CPU_PATH}/cpu$j/cpuidle
        mount --bind ${CPU_PATH}/cpufreq/policy0 ${CPU_PATH}/cpu$j/cpufreq
        mount --bind ${CPU_PATH}/cpufreq/cpuidle ${CPU_PATH}/cpu$j/cpuidle
    done

   # 1. 替换基础的公共系统挂载点
    yq eval -i '
        (.spec.volumes[] | select(.name == "cpusys") | .hostPath.path) = "/var/lib/kbox/cpus/'"${POD_NAME}"'/cpu" |
        (.spec.volumes[] | select(.name == "cpufreq") | .hostPath.path) = "/var/lib/kbox/cpus/'"${POD_NAME}"'/cpu/cpufreq" |
        (.spec.volumes[] | select(.name == "cpuidle") | .hostPath.path) = "/var/lib/kbox/cpus/'"${POD_NAME}"'/cpu/cpufreq/cpuidle"
    ' k8s-video-apply.yaml

    # 2. 借助 Bash 循环动态处理所有 cpuX 挂载点
    for ((j=0; j<CPU_NUM; j++));
    do
        yq eval -i '
            (.spec.volumes[] | select(.name == "cpu'"$j"'") | .hostPath.path) = "/var/lib/kbox/cpus/'"${POD_NAME}"'/cpu/cpu'"$j"'"
        ' k8s-video-apply.yaml
    done
}

case "$ACTION" in
    start) start "$@";;
    nstart) nstart "$@";;
    delete) delete "$@";;
    ndelete) ndelete "$@";;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac
