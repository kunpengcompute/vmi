#!/bin/bash
# 虚拟机调优

:<<!
cpu 绑核
@参数1 虚拟机名称
@参数2 虚拟CPU的数量
@参数3 物理机逻辑CPU的编号列表
!
function band_cpu()
{
    if [ $# -ne 3 ]; then
cat <<EOF
usage: $0 name vcpus cpu_list
EOF
    return 1
    fi

    local name="$1"
    local vcpus="$2"
    local cpu_list="$3"

    declare -a cpu_groups
    local group_index=0
    for group in $(echo $cpu_list | tr -s "," " "); do
        eval cpu_groups[$group_index]="$group"
        group_index=$(( group_index + 1 ))
    done
    unset group_index

    local logic_cpus=0
    for group in ${cpu_groups[@]}; do
        local group_start=$(echo $group | awk -F '-' '{print $1}')
        local group_end=$(echo $group | awk -F '-' '{print $2}')
        logic_cpus=$(( logic_cpus + group_end - group_start + 1 ))
    done
    if [ $logic_cpus -lt $vcpus ]; then
        echo "the number of logic cpus is letter than virtual cpu."
        return 1
    fi

    local cmd=""
    local vcpupin=0
    local group_index=0
    while [ $vcpupin -lt $vcpus ]; do
        eval local group=\${cpu_groups[$group_index]}
        local group_start=$(echo $group | awk -F '-' '{print $1}')
        local group_end=$(echo $group | awk -F '-' '{print $2}')
        for no in $(seq $group_start $group_end); do
            if [ $vcpupin -ge $vcpus ]; then
                break
            fi
            cmd="${cmd}vcpupin${vcpupin}.vcpu=${vcpupin},vcpupin${vcpupin}.cpuset=$no,"
            # echo "vcpupin${vcpupin}.vcpu=${vcpupin},vcpupin${vcpupin}.cpuset=$no"
            vcpupin=$(( vcpupin + 1 ))
        done
        group_index=$(( group_index + 1 ))
    done
    cmd="${cmd%,}"

    set -x
    eval virt-xml $name --edit --cputune $cmd
    eval virsh emulatorpin --domain $name --cpulist $cpu_list --config
    set +x
}

:<<!
NUMA 调优
@参数1 虚拟机名称
@参数2 numa 节点列表
!
function numa_tune()
{
    if [ $# -ne 2 ]; then
cat <<EOF
usage: $0 name nodeset
EOF
    return 1
    fi
    local name="$1"
    local nodeset="$2"
    eval virt-xml $name --edit --numatune mode=strict,nodeset=$nodeset
}

:<<!
使用内存大页
@参数1 虚拟机名称
!
function use_memory_hugepage()
{
    if [ $# -ne 1 ]; then
cat <<EOF
usage: $0 name
EOF
    return 1
    fi
    local name="$1"
    eval virt-xml $name --edit --memorybacking hugepages=on
}

function usage()
{
cat <<EOF
usage: $0 command [args]
command:
    bandcpu CPULIST
    numatune
    memoryhugepage
EOF
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

case "$1" in
    bandcpu)
        shift 1
        band_cpu $@
        ;;
    numatune)
        shift 1
        numa_tune $@
        ;;
    memoryhugepage)
        shift 1
        use_memory_hugepage $@
        ;;
    *)
        usage
        exit 1
esac