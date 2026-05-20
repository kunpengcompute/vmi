#!/bin/bash

ACTION=$1
MIN=$2
MAX=$3
ENABLE_F2FS=$4
SYSTEM_SIZE_MB=$5
if [ -z $3 ]; then
    MAX=$2
fi
if [ -z $4 ]; then
    ENABLE_F2FS=0 # 如果没有传入第四个参数，默认不使能F2FS
fi
if [ -z $5 ]; then
    SYSTEM_SIZE_MB=0 # 如果没有传入第五个参数，默认不使能配额限制
fi

if [ -z "$DATA_BASE_PATH" ]; then
        export DATA_BASE_PATH="/home/mount"
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

function check_f2fs_partition () {
    local target_path=$1
    local has_error=0
    
    # 1. 校验当前内核是否支持 f2fs
    # 使用 grep -q 静默匹配，如果找到了会返回 0 (true)
    if ! grep -q "f2fs" /proc/filesystems; then
        echo -e "\033[1;31m[ERROR] 校验失败: 当前系统内核不支持 f2fs 文件系统。\033[0m"
        echo -e "\033[1;31m[ERROR] 执行 'cat /proc/filesystems | grep f2fs' 未检测到回显。\033[0m"
        echo -e "\033[1;31m[ERROR] 请检查内核是否编译了 f2fs 支持，或尝试手动加载模块 (modprobe f2fs)。\033[0m"
        has_error=1
    fi

    # 2. 校验目标目录所在的分区是否为 f2fs 格式
    mkdir -p "${target_path}"
    local host_fs_type=$(df -T "${target_path}" | tail -1 | awk '{print $2}')
    if [ "$host_fs_type" != "f2fs" ]; then
        echo  "当前容器配置为使能 f2fs (ENABLE_F2FS=1)"
        echo  "但目标挂载目录 ${target_path} 所在的分区格式为 ${host_fs_type}，并非 f2fs。"
        echo  "这可能会导致性能受到影响，请注意！"
    fi

    # 3. 最终判断逻辑：如果第一个校验未通过，就中止并返回 1
    if [ "$has_error" -ne 0 ]; then
        echo -e "\033[1;31m[ERROR] f2fs 运行环境检查未通过，操作已中止。\033[0m"
        return 1
    fi

    return 0
 }

function start(){
    export DATA_BASE_PATH=$DATA_BASE_PATH
    if [ ! -d "${DATA_BASE_PATH}/img" ]; then
        mkdir -p ${DATA_BASE_PATH}/img
    fi
    if [ "$ENABLE_F2FS" == "1" ]; then  
	    check_f2fs_partition "${DATA_BASE_PATH}/data"
	    if [ $? -ne 0 ]; then
	        return 1 # 校验失败，直接退出拉起流程
	    fi
	fi
    for ((i=$MIN; i<=$MAX; i++))
    do
        IMG=${DATA_BASE_PATH}/img/video$i.img
        if [ ! -e $IMG ]; then
            fallocate -l ${STORAGE_SIZE_GB}G $IMG
            if [ "$ENABLE_F2FS" == "1" ]; then
	             yes | mkfs.f2fs $IMG
	         else
	             yes | mkfs -t ext4 $IMG
	         fi
        fi
        DATA_PATH="${DATA_BASE_PATH}/data/video$i"
        mkdir -p $DATA_PATH
        if [ "$ENABLE_F2FS" == "1" ]; then
	        mount -t f2fs -o loop $IMG $DATA_PATH
        else
            mount $IMG $DATA_PATH
	    fi
        echo $(($STORAGE_SIZE_GB * 2 * 1024 * 1024)) >$DATA_PATH/storage_size

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
    
    DATA_BASE_PATH=${NFS_DATA_PATH}
    start "$@"
}

function ndelete(){
    DATA_BASE_PATH=${NFS_DATA_PATH}
    delete "$@"
}

function delete(){
    local keep_data=0
    if [ -n "$1" ] && [ "$1" == "1" ]; then
        keep_data=1
    fi
    for ((i=$MIN; i<=$MAX; i++))
    do
        # kubectl delete pod video$i
        local umount_try=30
        local BOX_NAME=video$i
        # 删除容器
        rm -f /var/run/containerd/video$i
        kubectl delete pod video$i 

        # 删除cpu/文件
        umount /var/lib/kbox/cpus/video$i/cpu/cpu*/* > /dev/null 2>&1
        rm -rf /var/lib/kbox/cpus/video$i
        [ $? -ne 0 ] && echo "fail to remove data files /var/lib/kbox/cpus/$BOX_NAME !"

        # 删除数据文件
        if [ -d "$DATA_BASE_PATH/data/$BOX_NAME" ]; then
            while [ $umount_try -gt 1 ]
            do
                umount $DATA_BASE_PATH/data/$BOX_NAME > /dev/null 2>&1
                [ $? -ne 0 ] && echo "$BOX_NAME is already umounted!"
                mount | grep -w "$DATA_BASE_PATH/data/$BOX_NAME"
                if [ $? -eq 0 ]; then
                    umount_try=$((umount_try - 1))
                    sleep 1
                else
                    echo "umounted $BOX_NAME OK"
                    break
                fi
            done
            rm -rf $DATA_BASE_PATH/data/$BOX_NAME > /dev/null 2>&1
            [ $? -ne 0 ] && echo "fail to remove data files $DATA_BASE_PATH/data/$BOX_NAME !"
        fi

        # 删除数据img文件
        if [ -e "$DATA_BASE_PATH/img/$BOX_NAME.img" ] && [ $keep_data -ne 1 ]; then
            rm -rf $DATA_BASE_PATH/img/$BOX_NAME.img > /dev/null 2>&1
            [ $? -ne 0 ] && echo "fail to remove image file $DATA_BASE_PATH/img/$BOX_NAME.img !"
        fi
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
