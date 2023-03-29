#!/bin/bash

function usage()
{
    printf "usage: %s [options]" "$0"
    printf "\n\t-h 显示帮助"
    printf "\n\t-r"
    printf "\n\t\tpcie: 记录显卡的相关数据"
    printf "\n\t\tperf: 记录测试程序的调用栈和 cache miss 率"
    printf "\n\t-t\trun_time: 每个 unity demo 的运行时长, 默认为60s"
    printf "\n\t-d\tdst_dir: 输出文件夹, 默认为 %s" "/tmp/$(date +%m%d)/unity"
    printf "\n\t-s\tsrc_dir: unity demo 文件夹, 默认为 %s" "$(cd $(dirname $0) && pwd)"
    printf "\n"
}

while getopts 'hr:t:d:s:' opt; do
    case "$opt" in
    h)
        usage "$0"
        exit 0
        ;;
    r)
        [[ "${OPTARG}" == "pcie" ]] && pcie="yes"
        [[ "${OPTARG}" == "perf" ]] && perf="yes"
        ;;
    t)
        time="${OPTARG}"
        ;;
    d)
        dst_dir="${OPTARG}"
        ;;
    s)
        src_dir="${OPTARG}"
        ;;
    ?)
        usage "$0"
        exit 1
    esac
done

default_dst_dir="/tmp/$(date +%m%d)/unity"
default_src_dir="$(dirname $0)"

dst_dir="${dst_dir-${default_dst_dir}}"
src_dir="${src_dir-${default_src_dir}}"
pcie=${pcie-"no"}
perf=${perf-"no"}
time=${time-60}

if [ ! -d "${dst_dir}" ]; then
	mkdir -p "${dst_dir}"
fi
if [ ! -d "${src_dir}" ]; then
	mkdir -p "${src_dir}"
fi

dst_dir="$(cd ${dst_dir} && pwd)"
src_dir="$(cd ${src_dir} && pwd)"

echo "输出文件夹: ${dst_dir}"
echo "unity demo 所在文件夹: ${src_dir}"
echo "运行时长: ${time}s"
echo "是否记录 pcie: ${pcie}"
echo "是否记录 perf: ${perf}"
echo ""

function get_base_name()
{
    base_name=${1##*/}
    base_name=${base_name#*linuxr-}
    base_name=${base_name%.zip}
}

function get_pid_by_name()
{
    local cmd="$1"
    local pids=$(ps u | grep "${cmd}" | grep -v "grep" | awk '{print $11,$2}' 2>/dev/null)
    pid=$(echo "${pids}" | sed -n '/^perf/! p' | awk '{print $2}')
}

function wait()
{
    local cur=0
    while [ $cur -lt $1 ]; do
            local remain=$(($1-$cur))
            echo -ne "\rremain: ${remain}s        "
            sleep 1
            cur=$(($cur+1))
    done
    echo ""
}

function run()
{
    local pcie_log_file="$1"
    local perf_log_file="$2"
    local exec="$3"
    shift 3
    local args="$@"

    echo "pcie_log_file = ${pcie_log_file}"
    echo "perf_log_file = ${perf_log_file}"
    echo "exec = ${exec}"
    echo "args = ${args[@]}"

    if [ "${pcie}" == "yes" ]; then
        nvidia-smi dmon -i 0 -s pcut -f "${pcie_log_file}" &
    fi

    if [ "${perf}" == "yes" ]; then
        perf record -g -o "${perf_log_file}" numactl -N0 -m0 "${exec}" "${args}" > /dev/null &
    else
        numactl -N0 -m0 "${exec}" "${args}" > /dev/null &
    fi

    wait "${time}"
    get_pid_by_name "${exec}"
    kill -2 "${pid}"

    if [ "${pcie}" == "yes" ]; then
        get_pid_by_name "nvidia-smi dmon -i 0 -s pcut -f ${pcie_log_file}"

        kill -2 "${pid}"
    fi
}

function main()
{
    local dst_dir="$1"
    local src_dir="$2"
	
    local demos=$(find "${src_dir}" -type f -name "*.zip" | sort)
    echo "运行的 demo 有: "
    echo "${demos[@]}"
    echo ""

	rm -rf ~/.config/unity3d

    for demo in ${demos[@]}; do
        echo "运行 ${demo} ..."

        echo "解压 ${demo} ..."
        get_base_name "${demo}"
        unzip -o "${demo}" -d "/tmp/${base_name}" > /dev/null

        echo "赋予执行权限 ..."
        local executable=$(find "/tmp/${base_name}" -type f -name "*.x86_64")
        chmod +x "${executable}"

        echo "执行 ..."
		rm -rf "${dst_dir}/${base_name}"
		mkdir -p "${dst_dir}/${base_name}"
        run \
          "${dst_dir}/${base_name}/pcie.log"  \
          "${dst_dir}/${base_name}/perf.data" \
          "${executable}"        \
          "-force-vulkan"

        echo ""
    done

	rm -rf "${dst_dir}/unity3d"
    cp -r ~/.config/unity3d "${dst_dir}"
}

main "${dst_dir}" "${src_dir}"

