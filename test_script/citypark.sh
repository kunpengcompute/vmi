#!/bin/bash
set -e

# 收集性能数据的后台进程
declare -a RECORD_PID_ARRAY
# citypark 进程
declare CITYPARK_PID_PARENTS
declare CITYPARK_PID

function usage()
{
    echo "usage: $0 -path=... -cameras=... -quittime=... [-renderoffscreen] [options]"
    echo -e "\t" "-p|-path: 指定citypark应用的路径"
    echo -e "\t" "-cameras: 指定镜头数"
    echo -e "\t" "-t|-quittime: 指定运行时长"
    echo -e "\t" "-renderoffscreen: 指定应用是否使用离屏渲染模式"
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
    --longoptions "help,path:,quittime:,node:,cpus:,output-dir:,record:,debug,cameras:,renderoffscreen" \
    -n "$0" -- "$@")
eval set -- $ARGS

while true; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -p|--path)
            CITYPARK_PATH="$2"
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
        --cameras)
            CAMERAS="$2"
            shift 2
            ;;
        --renderoffscreen)
            RENDER_OFF_SCREEN="yes"
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

if [ x"$CITYPARK_PATH" == "x" -o x"$QUIT_TIME" == "x" -o x"$CAMERAS" == "x" ]; then
    usage
    exit 1
fi
RENDER_OFF_SCREEN="${RENDER_OFF_SCREEN-no}"
OUTPUT_DIR="${OUTPUT_DIR-/tmp/$(date +%m%d)/unity}"

# band core or band numa
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
    echo "render off screen: $RENDER_OFF_SCREEN"
    echo "citypark path: $CITYPARK_PATH, cameras: $CAMERAS, run time: $QUIT_TIME"
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
        perf record -g -o "${OUTPUT_DIR}"/perf.data -p $CITYPARK_PID -F 99 2>/dev/null &
    fi
    if [ $THREAD == yes ]; then
        top -b -H -p $CITYPARK_PID >"${OUTPUT_DIR}"/threads.log &
        RECORD_PID_ARRAY[$index]=$!
        index=$((index+1))
    fi
}

function run()
{
    local extra_dir=/tmp/citypark
    if [ ! -d $extra_dir ]; then
        mkdir -p $extra_dir
        tar xf $CITYPARK_PATH -C $extra_dir
    fi
    local exec=$(find $extra_dir -type f -name "*.sh")
    chmod +x $exec
    
    CMD=""
    if [ $NODE != null -a $CPUS != null ]; then
        CMD="numactl -C $CPUS -m $NODE"
    elif [ $NODE != null ]; then
        CMD="numactl -N $NODE -m $NODE"
    fi
    CMD="$CMD $exec -ResX=1920 -ResY=1080 -ForceRes -cameras=$CAMERAS -quittime=$QUIT_TIME"
    if [ $RENDER_OFF_SCREEN == yes ]; then
        CMD="$CMD -renderoffscreen"
    fi
    echo "cmd: $CMD"

    if [ ! -d "${OUTPUT_DIR}" ]; then
        mkdir -p "${OUTPUT_DIR}"
    fi

    eval $CMD >${OUTPUT_DIR}/citypark.log  2>/dev/null &
    CITYPARK_PID_PARENTS=$!
}

run

sleep 2
CITYPARK_PID=$(pstree -p $CITYPARK_PID_PARENTS | head -1 | tr -s "(" " " | tr -s ")" " " | awk '{print $6}')
if [ x"$DEBUG" == "xyes" ]; then
    echo "citypark pid parent: $CITYPARK_PID_PARENTS"
    echo "citypark pid: $CITYPARK_PID"
fi
record

wait $CITYPARK_PID_PARENTS
for p in ${RECORD_PID_ARRAY[@]}; do
    kill -2 $p
done