#!/bin/bash

ACTION=$1
MIN=$2
MAX=$3
SYSTEM_SIZE_MB=$4  # 新增：第四个参数，限定system分区大小(MB)
THISDIR=$(readlink -ef $(dirname $0))
if [ -z $3 ]; then
    MAX=$2
fi

if [ -z $4 ]; then
    SYSTEM_SIZE_MB=0 # 如果没有传入第四个参数，默认不使能配额限制
fi

if [ -z ${USER_DATA_PATH} ]; then
    USER_DATA_PATH="/home/mount"
fi

if [ -z ${NFS_DATA_PATH} ]; then
    NFS_DATA_PATH="/tmp/nfs"
fi

if [ -z ${STORAGE_SIZE_GB} ]; then
    STORAGE_SIZE_GB=16
fi

#===============================================================================
# Functions
#===============================================================================
function check_environment() {
    # root权限执行此脚本
    if [ "${UID}" -ne 0 ]; then
        echo  "请使用root权限执行"
        exit 1
    fi

    # 支持非当前目录执行
    CURRENT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
    cd ${CURRENT_DIR}

    # 如果是需要转码的机型，在使用脚本过程中检查转码
    local VENDOR_ID=$(lscpu | grep "Vendor ID:" | grep -v "BIOS" | awk '{print $3}')
    if [ x"$VENDOR_ID" == x"0x48" ] || [ x"$VENDOR_ID" == x"HiSilicon" ]; then
	    check_exagear
    fi

    # 清理/dev/目录下的全部loop device节点，并在/dev/loop_device/目录下提前生成足够数量的设备节点
    if [ $1 == "start" ] || [ $1 == "restart" ]; then
        prepare_loop_device
    fi

    # 检查必要的环境配置
    check_selinux
    check_max_user_instances
    check_cgroup_v2
}

function check_exagear() {
    # 未注册
    if [ ! -e "/proc/sys/fs/binfmt_misc/ubt_a32a64" ]; then
        if [ ! -d "/proc/sys/fs/binfmt_misc/" ]; then
            mount -t binfmt_misc none /proc/sys/fs/binfmt_misc
        fi

        # 在归档路径下模糊查找
        local UBT_PATHS=($(ls /root/dependency/*/ubt_a32a64))
        if [ ${#UBT_PATHS[@]} -lt 1 ]; then
            echo "No ubt_a32a64 file!"
            exit 1
        elif [ ${#UBT_PATHS[@]} -gt 1 ]; then
            echo "Many ubt_a32a64 files exist! Please check:"
            for PA in ${UBT_PATHS[@]}; do
                echo "${PA}"
            done
            exit 1
        fi

        # 恢复exgear文件
        mkdir -p /opt/exagear
        chmod -R 700 /opt/exagear
        cp -rf ${UBT_PATHS[0]} /opt/exagear/
        cd /opt/exagear
        chmod +x ubt_a32a64

        # 注册转码 续行符后字符串顶格
        echo ":ubt_a32a64:M::\x7fELF\x01\x01\x01\x00\x00\x00\x0"\
"0\x00\x00\x00\x00\x00\x02\x00\x28\x00:\xff\xff\xf"\
"f\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00"\
"\x00\xfe\xff\xff\xff:/opt/exagear/ubt_a32a64:POCF" > /proc/sys/fs/binfmt_misc/register
    fi

    # 检查ubt_a32a64版本
    local UBT_VER=($(/opt/exagear/ubt_a32a64 -V |grep -w binary | sed 's/^ExaGear binary translator version: \([^\s]*\)/\1/g'  | sed 's/^v//g'))
    if [ $UBT_VER \> "2.4.1" ]; then
        if [ ! -e "/dev/tango32" ]; then
            echo "No tango32!"
            exit 1
        fi
    fi
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

function check_selinux() {
    local SELINUX_STATUS=$(getenforce)
    if [ "$SELINUX_STATUS" != "Disabled" ]; then
        echo "SELinux is not disabled! Please check! Current status: $SELINUX_STATUS"
        exit 1
    fi
}

function check_max_user_instances() {
    local TARGET_VALUE=8192
    local CURRENT_VALUE=$(cat /proc/sys/fs/inotify/max_user_instances)
    if [ "$CURRENT_VALUE" -ne "$TARGET_VALUE" ]; then
        echo "Set fs.inotify.max_user_instances to $TARGET_VALUE"
        sysctl -w fs.inotify.max_user_instances=$TARGET_VALUE > /dev/null 2>&1
    fi
}

function check_cgroup_v2() {
    if ! mount | grep -q "cgroup2 on /sys/fs/cgroup"; then
        echo "WARNING: cgroup v2 is disabled, which may lead to functional issues."
    fi
}

function get_lxcfs_path() {
    local value
    if [ -d "/var/lib/lxc/lxcfs" ]; then
        value="/var/lib/lxc/lxcfs"
    elif [ -d "/var/lib/lxcfs" ]; then
        value="/var/lib/lxcfs"
    else
        echo "error, fail to get lxcfs path"
        exit 1
    fi

    echo ${value}
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
    for ((i=0; i<8; i++));
    do
        mkdir -p ${CPU_PATH}/cpu$i
        chmod 755 ${CPU_PATH}/cpu$i
        #其他文件的挂载
        mkdir -p ${CPU_PATH}/cpu$i/hotplug ${CPU_PATH}/cpu$i/power ${CPU_PATH}/cpu$i/regs ${CPU_PATH}/cpu$i/topology
        cp /sys/devices/system/cpu/cpu$i/cpu_capacity ${CPU_PATH}/cpu$i/cpu_capacity
        mount --bind /sys/devices/system/cpu/cpu$i/hotplug ${CPU_PATH}/cpu$i/hotplug
        echo "1" >${CPU_PATH}/cpu$i/online
        mount --bind /sys/devices/system/cpu/cpu$i/power ${CPU_PATH}/cpu$i/power
        mount --bind /sys/devices/system/cpu/cpu$i/regs ${CPU_PATH}/cpu$i/regs
        mount --bind /sys/devices/system/cpu/cpu$i/topology ${CPU_PATH}/cpu$i/topology
        mkdir -p ${CPU_PATH}/cpu$i/cpufreq ${CPU_PATH}/cpu$i/cpuidle
        mount --bind ${CPU_PATH}/cpufreq/policy0 ${CPU_PATH}/cpu$i/cpufreq
        mount --bind ${CPU_PATH}/cpufreq/cpuidle ${CPU_PATH}/cpu$i/cpuidle
    done

    yq eval -i '
        .spec.volumes += {
            "name": "localcpu",
            "hostPath": {
                "path": "/var/lib/kbox/cpus/${POD_NAME}/cpu"
            }
        }
        |
        .spec.containers[0].volumeMounts += {
            "mountPath": "/data/local/cpu",
            "name": "localcpu",
            "readOnly": true
        }
    ' k8s-video-apply.yaml
}

function start(){
    cd $THISDIR
    export LXCFS_PATH=$(get_lxcfs_path)
    export THIS_DIR=$THISDIR
    export USER_DATA_PATH=$USER_DATA_PATH
    if [ "$ENABLE_F2FS" == "1" ]; then  
	    check_f2fs_partition "${USER_DATA_PATH}/data"
	    if [ $? -ne 0 ]; then
	        return 1 # 校验失败，直接退出拉起流程
	    fi
	fi
    for ((j=$MIN; j<=$MAX; j++))
    do
        IMG=${USER_DATA_PATH}/img/video$j.img
        if [ ! -e $IMG ]; then
            fallocate -l ${STORAGE_SIZE_GB}G $IMG
            if [ "$ENABLE_F2FS" == "1" ]; then
	             yes | mkfs.f2fs $IMG
	         else
	             yes | mkfs -t ext4 $IMG
	         fi
        fi
        DATA_PATH="${USER_DATA_PATH}/data/video$j"
        mkdir -p $DATA_PATH
        if [ "$ENABLE_F2FS" == "1" ]; then
	        mount -t f2fs -o loop $IMG $DATA_PATH
        else
            mount $IMG $DATA_PATH
	    fi
        echo $(($STORAGE_SIZE_GB * 2 * 1024 * 1024)) >$DATA_PATH/storage_size
        export YAML_PATH=$j
        export POD_NAME=video$j
        touch /var/run/containerd/video$j
        export HOSTPORT=$(expr $j + 8000)
        export HOSTPORT2=$(expr $j + 8500)
        cp k8s-video.yaml k8s-video-apply.yaml
        mock_cpu
        if [ -f $THISDIR/default.prop_$POD_NAME ]; then
            yq eval -i '
                .spec.volumes += {
                    "name": "default-prop",
                    "hostPath": {
                        "path": "${THIS_DIR}/default.prop_$POD_NAME",
                        "type": "File"
                    }
                }
                |
                .spec.containers[0].volumeMounts += {
                    "mountPath": "/kbox_prop/default.prop",
                    "name": "default-prop"
                }
            ' k8s-video-apply.yaml
        fi
        if [ -f $THISDIR/build.prop ]; then
            yq eval -i '
                .spec.volumes += {
                    "name": "build-prop",
                    "hostPath": {
                        "path": "${THIS_DIR}/build.prop",
                        "type": "File"
                    }
                }
                |
                .spec.containers[0].volumeMounts += {
                    "mountPath": "/kbox_prop/build.prop",
                    "name": "build-prop"
                }
            ' k8s-video-apply.yaml
fi
        
        # 启动容器
        export SYSTEM_SIZE_MB
        envsubst < k8s-video-apply.yaml | kubectl apply -f -
        
        if [ "$SYSTEM_SIZE_MB" -gt 0 ]; then
            echo "Pod ${POD_NAME} 创建成功，配额(${SYSTEM_SIZE_MB}MB)将由NRI插件自动应用"
        fi
    done
}

function delete(){
    local keep_data=0
    if [ $1 ] && [ $1 -eq 1 ]; then
        keep_data=1
    fi
    for ((i=$MIN; i<=$MAX; i++))
    do
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
        if [ -d "$USER_DATA_PATH/data/$BOX_NAME" ]; then
            while [ $umount_try -gt 1 ]
            do
                umount $USER_DATA_PATH/data/$BOX_NAME > /dev/null 2>&1
                [ $? -ne 0 ] && echo "$BOX_NAME is already umounted!"
                mount | grep -w "$USER_DATA_PATH/data/$BOX_NAME"
                if [ $? -eq 0 ]; then
                    umount_try=$((umount_try - 1))
                    sleep 1
                else
                    echo "umounted $BOX_NAME OK"
                    break
                fi
            done
            rm -rf $USER_DATA_PATH/data/$BOX_NAME > /dev/null 2>&1
            [ $? -ne 0 ] && echo "fail to remove data files $USER_DATA_PATH/data/$BOX_NAME !"
        fi

        # 删除数据img文件
        if [ -e "$USER_DATA_PATH/img/$BOX_NAME.img" ] && [ $keep_data -ne 1 ]; then
            rm -rf $USER_DATA_PATH/img/$BOX_NAME.img > /dev/null 2>&1
            [ $? -ne 0 ] && echo "fail to remove image file $USER_DATA_PATH/img/$BOX_NAME.img !"
        fi

        # 删除input event path
        if [ -d /var/run/$BOX_NAME ]; then
            rm -rf /var/run/${BOX_NAME} > /dev/null 2>&1
            [ $? -ne 0 ] && echo "fail to remove event path /var/run/${BOX_NAME} !"
        fi
    done
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
        echo -e "\033[1;31m[ERROR] 校验失败: 容器配置为使能 f2fs (ENABLE_F2FS=1)\033[0m"
        echo -e "\033[1;31m[ERROR] 但目标挂载目录 ${target_path} 所在的分区格式为 ${host_fs_type}，并非 f2fs。\033[0m"
        echo -e "\033[1;31m[ERROR] 请检查底层硬盘分区格式是否已正确格式化并挂载！\033[0m"
        has_error=1
    fi

    # 3. 最终判断逻辑：只要有一个校验未通过，就中止并返回 1
    if [ "$has_error" -ne 0 ]; then
        echo -e "\033[1;31m[ERROR] f2fs 运行环境检查未通过，操作已中止。\033[0m"
        return 1
    fi

    return 0
 }

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

function nstart(){
    check_nfs_mount "${NFS_DATA_PATH}"
    USER_DATA_PATH=${NFS_DATA_PATH}
    start "$@"
}

function ndelete(){
    USER_DATA_PATH=${NFS_DATA_PATH}
    delete "$@"
}

check_environment $1

case "$ACTION" in
    start) start "$@";;
    nstart) nstart "$@";;
    delete) delete;;
    ndelete) ndelete;;
    tdelete) delete 1;;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac