#!/bin/bash
# 简化安装虚拟机的流程

function usage()
{
cat <<EOF
usage: "$0" --name VM-NAME --img /path/to/imag [options]
options:
    --vcpus NUM
                Specify number of virtual cpu cores
                --vcpus 32
    --ram SIZE
                Specify capacity of ram
                --ram 32768 # 32768 MB
    --disk SIZE
                Specify capacity of disk
                --disk 256 # 256 GB
EOF
}

function generate_cmd()
{
    local cmd
    cmd="virt-install --name ${VM_NAME} --virt-type kvm --boot uefi"
    cmd="${cmd} --vcpus ${VCPU_NUM} --memory ${RAM_CAP} --disk size=${DISK_CAP}"
    cmd="${cmd} --cdrom ${IMG_PATH}"
    cmd="${cmd} --input tablet --input keyboard,bus=usb"
    cmd="${cmd} --video model=virtio --graphic vnc"

    echo "$cmd"
}

function install()
{
    local cmd="$(generate_cmd)"
    echo "$cmd"
    eval $cmd
}

while [ $# -gt 0 ]; do
    case "$1" in
        --name)
            VM_NAME="$2"
            ;;
        --img)
            IMG_PATH="$2"
            ;;
        --vcpus)
            VCPU_NUM=$2
            ;;
        --ram)
            RAM_CAP=$2
            ;;
        --disk)
            DISK_CAP=$2
            ;;
        *)
            usage
            exit 1
            ;;
    esac
    shift 2
done

if [ -z "$VM_NAME" -o -z "$IMG_PATH" ]; then
    usage
    exit 1
fi

# 虚拟机默认配置
VCPU_NUM=${VCPU_NUM-28}
RAM_CAP=${MEMORY_CAP-32768}
DISK_CAP=${DISK_CAP-256}

install
