#!/bin/bash

function usage()
{
	printf "usage: $0 [options] -c cnt"
	printf "\n-c cnt: 摄像机镜头数"
	printf "\noptions:"
	printf "\n\t-h 显示帮助"
	printf "\n\t-r"
	printf "\n\t\tpcie: 记录显卡的相关数据"
	printf "\n\t\tperf: 记录测试程序的调用栈和cache miss率"
	printf "\n\t\tnmon: 记录系统的性能数据"
	printf "\n\t-t\tquit_time: 运行时长, 默认为100"
	printf "\n\t-R\t在屏渲染"
	printf "\n\t-d\tdst_dir: 输出文件夹, 默认为 %s" "tmp/$(date +%m%d)/citypark"
	printf "\n\t-s\tcitypark_path: citypark 的路径, 默认为 %s" "$(cd $(dirname $0) && pwd)/citypark.tar.gz"
	printf "\n"
}

while getopts 'hr:c:t:Rd:s:' opt; do
	case "$opt" in
	h)
		usage
		exit 0
		;;
	r)
		[[ "${OPTARG}" == "pcie" ]] && pcie="yes"
		[[ "${OPTARG}" == "perf" ]] && perf="yes"
		[[ "${OPTARG}" == "nmon" ]] && nmon="yes"
		;;
	c)
		cnt="${OPTARG}"
		;;
	t)
		quit_time="${OPTARG}"
		;;
	R)
		render_off_screen="no"
		;;
	d)
		dst_dir="${OPTARG}"
		;;
	s)
		citypark_path="${OPTARG}"
		;;
	?)
		usage
		exit 1
	esac
done

[[ -z "${cnt}" ]] && usage && exit 1

default_quit_time=100
default_dst_dir="/tmp/$(date +%m%d)/citypark"
default_citypark_path="$(cd $(dirname $0) && pwd)/citypark.tar.gz"

pcie="${pcie-"no"}"
perf="${perf-"no"}"
nmon="${nmon-"no"}"
quit_time="${quit_time-${default_quit_time}}"
render_off_screen="${render_off_screen-"yes"}"
dst_dir="${dst_dir-${default_dst_dir}}"
citypark_path="${citypark_path-${default_citypark_path}}"

if [ ! -d "${dst_dir}" ]; then
	mkdir -p "${dst_dir}"
fi
dst_dir="$(cd ${dst_dir} && pwd)"

echo "镜头数: ${cnt}"
echo "运行时长: ${quit_time}s"
echo "是否是离屏渲染: ${render_off_screen}"
echo "输出目录: ${dst_dir}"
echo "citypark 路径: ${citypark_path}"
echo "是否记录 pcie: ${pcie}"
echo "是否记录 perf: ${perf}"
echo "是否记录 nmon: ${nmon}"
echo ""

function get_pid_by_name()
{
	local cmd="$1"
	local pids=$(ps u | grep "${cmd}" | grep -v "grep" | awk '{print $11,$2}' 2>/dev/null)

	pid=$(echo "${pids}" | sed -n '/^perf/! p' | awk '{print $2}')
}

function get_prefix()
{
	prefix="${dst_dir}"

	if [ "${render_off_screen}" == "yes" ]; then
		prefix="${prefix}/off"
	else
		prefix="${prefix}/on"
	fi

	prefix="${prefix}/${cnt}"
	echo "prefix = ${prefix}"
}

function get_args()
{
	args="-cameras=${cnt} -quittime=${quit_time}"
	if [ "${render_off_screen}" == "yes" ]; then
		args="${args} -renderoffscreen"
	fi
	args="-ResX=1920 -ResY=1080 -ForceRes ${args}"
	echo "args = ${args}"
}

function main()
{
	echo "解压 ${citypark_path} ..."
	if [ ! -d /tmp/citypark ]; then
		mkdir /tmp/citypark
		tar xf "${citypark_path}" -C "/tmp/citypark" > /dev/null 2>&1
	fi

	echo "添加执行权限 ..."
	local executable=$(find /tmp/citypark -type f -name "*.sh")
	echo "executable = ${executable}"
	chmod +x "${executable}"

	echo "运行 ..."
	get_prefix
	mkdir -p "${prefix}"
	get_args

	if [ "${pcie}" == "yes" ]; then
		nvidia-smi dmon -i 0 -s pcut -f "${prefix}/pcie.log" &
	fi

	if [ "${nmon}" == "yes" ]; then
		nmon -f -t -m "${prefix}" -s 1 -c "${quittime}" &
	fi

	if [ "${perf}" == "yes" ]; then
		perf record -g -o "${prefix}/perf.data" numactl -N0 -m0 "${executable}" ${args} > "${prefix}/citypark.log" 
	else
		numactl -N0 -m0 "${executable}" ${args} > "${prefix}/citypark.log" 
	fi

	if [ "${pcie}" == "yes" ]; then
		get_pid_by_name "nvidia-smi dmon -i 0 -s pcut -f ${prefix}/pcie.log"
		kill -2 "${pid}"
	fi

	echo ""
}

main
