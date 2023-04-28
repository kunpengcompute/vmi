#!/bin/bash

function usage()
{
	printf "usage: %s options" "$0"
	printf "\n  -n, --name\t虚拟机的名称"
	printf "\n  -c, --cores\tCPU核心数"
	printf "\n  -m, --memory\t内存容量"
	printf "\n  -d, --disk\t硬盘容量"
	printf "\n  -i, --img\t安装镜像"
	printf "\n  --ks\t\tks文件路径"
	printf "\n  --node\t在哪个NUMA节点上分配内存"
	printf "\n  --pin\t\t将虚拟CPU与QEMU主线成绑定到物理CPU上"
	printf "\n"
}

ARGS=$(getopt -o hn:c:m:d:i: -l help,name:,cores:,memory:,disk:,img:,ks:,node:,pin: -n "$0" -- "$@")
eval set -- "${ARGS}"
while true; do
	case "$1" in
		-h|--help)
			usage
			exit 0
			;;
		-n|--name)
			name="$2"
			shift 2
			;;
		-c|--cores)
			cores="$2"
			shift 2
			;;
		-m|--memory)
			ram="$2"
			shift 2
			;;
		-d|--disk)
			disk="$2"
			shift 2
			;;
		-i|--img)
			img="$2"
			shift 2
			;;
		--ks)
			ks="$2"
			shift 2
			;;
		--node)
			node="$2"
			shift 2
			;;
		--pin)
			pin="$2"
			shift 2
			;;
		--)
			shift
			break
			;;
		*)
			usage
			exit 1
	esac
done

function is_valid_args()
{
	[ -z "${name}" ] && return 1
	[ -z "${cores}" ] && return 1
	[ -z "${ram}" ] && return 1
	[ -z "${disk}" ] && return 1
	[ -z "${img}" ] && return 1
	[ -z "${ks}" ] && return 1
	return 0
}
is_valid_args
if [ $? -gt 0 ]; then
	usage
	exit 1
fi

echo "name: ${name}"
echo "cores: ${cores}"
echo "ram: ${ram}"
echo "disk: ${disk}"
echo "img: ${img}"
echo "node: ${node-null}"
echo "pin: ${pin-null}"

function generate_band_cpu_cmds()
{
	BAND_CPU_CMDS=""
	local start="${pin%-*}"
	local end="${pin#*-}"
	echo "start: ${start}"
	echo "end: ${end}"
	BAND_CPU_CMDS="vcpupin0.vcpu=0,vcpupin0.cpuset=${start}"
	for i in $(seq 1 "$((end-start))"); do
		BAND_CPU_CMDS="${BAND_CPU_CMDS},vcpupin${i}.vcpu=${i},vcpupin${i}.cpuset=$((i+start))"
	done
	echo ""
	echo "cmd = ${BAND_CPU_CMDS}"
}

function install()
{
	local cmd
	cmd="virt-install"

	# 设置虚拟机名称
	cmd="${cmd} --name ${name}"
	# 设置虚拟机核数
	cmd="${cmd} --vcpus ${cores}"
	# CPU直通
	cmd="${cmd} --cpu host-passthrough"
	# 设置虚拟机内存
	cmd="${cmd} --memory ${ram}"
	# 设置虚拟机硬盘
	cmd="${cmd} --disk size=${disk},bus=scsi"
	# 设置虚拟机自动安装操作系统
	cmd="${cmd} --location ${img} --initrd-inject=${ks} --extra-args=\"inst.stage2=hd:LABEL=Kylin-Server-10 ks=file:/$(basename ${ks})\""
	# 添加输入设备
	cmd="${cmd} --input keyboard,bus=usb --input tablet"
	# 添加集显
	cmd="${cmd} --video virtio"
	# 添加vnc
	cmd="${cmd} --graphics vnc,port=5901,listen=0.0.0.0"
	# 设置虚拟机使用内存大页
	cmd="${cmd} --memorybacking hugepages=yes"
	# 设置虚拟机的 numa node
	if [ "${node}" != "null" ]; then
		cmd="${cmd} --numatune memory.mode=strict,memory.nodeset=${node}"
	fi
	# 设置虚拟机的CPU绑核
	if [ "${pin}" != "null" ]; then
		generate_band_cpu_cmds
		cmd="${cmd} --cputune ${BAND_CPU_CMDS}"
	fi
	
	# 安装虚拟机
	eval "${cmd}" 
	
	# 将QEMU主线程绑定到物理CPU上
	virsh emulatorpin "${name}" "${pin}" --current
}

install
