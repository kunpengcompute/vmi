#!/bin/bash
set -e

# 收集性能数据的后台进程
declare -a RECORD_PID_ARRAY
# unity 应用后台进程
declare UNITY_APP_PID

function usage()
{
    echo "usage: $0 -path=... -quittime=... [options]"
    echo -e "\t" "-p|-path: 指定citypark应用的路径"
    echo -e "\t" "-t|-quittime: 指定运行时长"
    echo -e "\t" "-N 绑NUMA或绑核, -C 选项指定绑核"
    echo -e "\t" "-C 绑核"
    echo -e "\t" "-record 记录性能数据"
    echo -e "\t\t" "perf 记录 perf 数据"
    echo -e "\t\t" "pcie 记录 pcie 数据"
    echo -e "\t\t" "nmon 记录 nmon 数据"
    echo -e "\t\t" "thread 记录 top thread 数据"
    echo -e "\t\t" "示例,记录perf和pcie数据: -record perf,pcie"
}

ARGS=$(getopt -a \
    --options "hp:t:N:C:o:r:" \
    --longoptions "help,path:,quittime:,node:,cpus:,output-dir:,record:,debug" \
    -n "$0" -- "$@")
eval set -- $ARGS

while true; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -p|--path)
            UNITY_APP_PATH="$2"
            shift 2
            ;;
        -t|--quittime)
            QUIT_TIME="$2"
            shift 2
            ;;
        -N|--node)
            NODE="$2"
            shift 2
            ;;
        -C|--cpus)
            CPUS="$2"
            shift 2
            ;;
        -o|--output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -r|--record)
            RECORD="$2"
            shift 2
            ;;
        --debug)
            DEBUG="yes"
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "invalid args"
            usage
            exit 1
            ;;
    esac
done

# 确认 unity app 路径和输出路径
if [ x"$UNITY_APP_PATH" == "x" -o x"$QUIT_TIME" == "x" ]; then
    usage
    exit 1
fi
OUTPUT_DIR="${OUTPUT_DIR-/tmp/$(date +%m%d)/unity}"

# 绑核或绑NUMA
if [ x"$NODE" == "x" ]; then
    CPUS="null"
else
    CPUS="${CPUS-null}"
fi
NODE="${NODE-null}"

# 记录相关性能数据
if [ x"$RECORD" != "x" ]; then
    for item in $(echo "$RECORD" | tr -s "," " "); do
        [[ $item == perf ]] && PERF="yes"
        [[ $item == pcie ]] && PCIE="yes"
        [[ $item == nmon ]] && NMON="yes"
        [[ $item == thread ]] && THREAD="yes"
    done
fi
PERF="${PERF-no}"
PCIE="${PCIE-no}"
NMON="${NMON-no}"
THREAD="${THREAD-no}"

# 打印到控制台, 方便开发者调试
if [ x"$DEBUG" == "xyes" ]; then
    # print unity app and output dir
    echo "unity app path: $UNITY_APP_PATH"
    echo "output dir: $OUTPUT_DIR"
    # print record items
    echo -n "record: "
    if [ "${PERF}" == "yes" ]; then
        echo -n "perf "
    fi
    if [ "${PCIE}" == "yes" ]; then
        echo -n "pcie "
    fi
    if [ "${NMON}" == "yes" ]; then
        echo -n "nmon "
    fi
    if [ "${THREAD}" == "yes" ]; then
        echo -n "thread"
    fi
    echo ""
    # print band cpus or band numa
    if [ $NODE != null ]; then
        if [ $CPUS != null ]; then
            echo "band cpus: $CPUS"
        else
            echo "band numa: $NODE"
        fi
    fi
fi

function get_base_name()
{
    base_name=${1##*/}
    base_name=${base_name#*linuxr-}
    base_name=${base_name%.zip}
}

function run()
{
    get_base_name "$UNITY_APP_PATH"
    EXTRA_DIR="/tmp/unity/$base_name"
    if [ ! -d $EXTRA_DIR ]; then
        mkdir -p $EXTRA_DIR
        unzip -o "$UNITY_APP_PATH" -d "$EXTRA_DIR" > /dev/null
    fi
    local exec="$(find "$EXTRA_DIR" -type f -name "*.x86_64")"
    chmod +x "$exec"

    CMD=""
    if [ $NODE != null -a $CPUS != null ]; then
        CMD="numactl -C $CPUS -m $NODE"
    elif [ $NODE != null ]; then
        CMD="numactl -N $NODE -m $NODE"
    fi
    CMD="$CMD $exec -force-vulkan -logFile $OUTPUT_DIR/${base_name}.log"
    echo "cmd: $CMD"

    eval $CMD >/dev/null &
    sleep 1
    UNITY_APP_PID=$(pstree -p $! | head -1 | tr -s "(" " " | tr -s ")" " " | awk '{print $4}')
    if [ x"$DEBUG" == "xyes" ]; then
        echo "unity app pid: $UNITY_APP_PID"
    fi
}

function record()
{
    local index=0

    if [ $NMON == yes ]; then
        nmon -f -t -m "${OUTPUT_DIR}" -s 1 -c "$((QUIT_TIME-3))" &
    fi

    if [ $PCIE == yes ]; then
        nvidia-smi dmon -i 0 -s pcut -f "${OUTPUT_DIR}"/pcie.log &
        RECORD_PID_ARRAY[$index]=$!
        index=$((index+1))
    fi

    if [ $PERF == yes ]; then
        perf record -g -o "${OUTPUT_DIR}"/perf.data -p $UNITY_APP_PID -F 99 2>/dev/null &
    fi

    if [ $THREAD == yes ]; then
        top -b -H -p $UNITY_APP_PID >"${OUTPUT_DIR}"/threads.log &
        RECORD_PID_ARRAY[$index]=$!
        index=$((index+1))
    fi
}

function mywait()
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

run
record

mywait $QUIT_TIME
kill -2 $UNITY_APP_PID
for p in ${RECORD_PID_ARRAY[@]}; do
    kill -2 $p
done