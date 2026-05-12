# 执行 ./storage_manager.sh $ACTION $STORAGE_NUM $STORAGE_SIZE_GB $IMG_BASE
# ACTION参数值为create 或者 delete  ，创建或者删除
# STORAGE_NUM: 数据卷数量,默认为200.需比pod数量大
# STORAGE_SIZE_GB ：存储大小 ，单位G
# IMG_BASE: 基础数据卷img文件
# 例如：
# ./storage_manager.sh create 100 32   创建100个存储大小为32G的存储隔离数据卷。名称为video1 .. video100
# ./storage_manager.sh delete 100   删除名称为video1 .. video100数据卷
# ./storage_manager.sh create 100 32 /home/mount/img/videobase.img 通过videobase.img为基础制作名为video1 .. video100的数据卷

#/bin/bash
ACTION=$1
STORAGE_NUM_MIN=$2
STORAGE_NUM_MAX=$3
STORAGE_SIZE_GB=$4
IMG_BASE=$4

if [ -z ${STORAGE_SIZE_GB} ]; then
    STORAGE_SIZE_GB=16
fi

if [ -z ${STORAGE_NUM} ]; then
    STORAGE_NUM=200
fi

if [ -z ${USER_DATA_PATH} ]; then
    USER_DATA_PATH="/home/mount/"
fi

if [ ! -d "${USER_DATA_PATH}/img" ]; then
    mkdir -p ${USER_DATA_PATH}/img
fi

function create(){
    echo "MIN=$STORAGE_NUM_MIN,MAX=$STORAGE_NUM_MAX"
    for i in $(seq $STORAGE_NUM_MIN $STORAGE_NUM_MAX);
    do
        IMG=${USER_DATA_PATH}/img/video$i.img
        if [ ! -e $IMG ]; then
            fallocate -l ${STORAGE_SIZE_GB}G $IMG
            yes | mkfs -t ext4 $IMG
        fi
        DATA_PATH="${USER_DATA_PATH}/data/video$i"
        mkdir -p $DATA_PATH
        mount $IMG $DATA_PATH
        echo $(($STORAGE_SIZE_GB * 2 * 1024 * 1024)) >$DATA_PATH/storage_size
    done 
}

function fcreate(){
    echo "MIN=$STORAGE_NUM_MIN,MAX=$STORAGE_NUM_MAX"
    check_f2fs_partition "${USER_DATA_PATH}/data"
    if [ $? -ne 0 ]; then
        return 1 # 校验失败，直接退出拉起流程
    fi
    for i in $(seq $STORAGE_NUM_MIN $STORAGE_NUM_MAX);
    do
        IMG=${USER_DATA_PATH}/img/video$i.img
        if [ ! -e $IMG ]; then
            fallocate -l ${STORAGE_SIZE_GB}G $IMG           
 	        yes | mkfs.f2fs $IMG
 	         
        fi
        DATA_PATH="${USER_DATA_PATH}/data/video$i"
        mkdir -p $DATA_PATH    
 	    mount -t f2fs -o loop $IMG $DATA_PATH
        echo $(($STORAGE_SIZE_GB * 2 * 1024 * 1024)) >$DATA_PATH/storage_size
    done 
}

function delete(){
    for i in $(seq $STORAGE_NUM_MIN $STORAGE_NUM_MAX);
    do
        IMG=${USER_DATA_PATH}/img/video$i.img
        rm -rf ${USER_DATA_PATH}/img/video$i.img
        DATA_PATH="${USER_DATA_PATH}/data/video$i"
        if [ -d ${DATA_PATH} ]; then 
            umount ${DATA_PATH}
            rm -rf $DATA_PATH
        fi
    done 
}

function createByImg(){
    echo "MIN=$STORAGE_NUM_MIN,MAX=$STORAGE_NUM_MAX"
    for i in $(seq $STORAGE_NUM_MIN $STORAGE_NUM_MAX);
    do
        DATA_PATH="${USER_DATA_PATH}/data/video$i"
        mkdir -p $DATA_PATH
        cp $IMG_BASE ${USER_DATA_PATH}/img/video$i.img
        mount ${USER_DATA_PATH}/img/video$i.img $DATA_PATH
    done
}

function fcreateByImg(){
    echo "MIN=$STORAGE_NUM_MIN,MAX=$STORAGE_NUM_MAX"
    for i in $(seq $STORAGE_NUM_MIN $STORAGE_NUM_MAX);
    do
        DATA_PATH="${USER_DATA_PATH}/data/video$i"
        mkdir -p $DATA_PATH
        cp $IMG_BASE ${USER_DATA_PATH}/img/video$i.img
        mount -t f2fs -o loop ${USER_DATA_PATH}/img/video$i.img $DATA_PATH
    done
}

check_param(){
    if [ -z ${IMG_BASE} ]; then
        echo -e  "\033[1;31mEnter the fourth parameter, which indicates the storage size or the image file.\033[0m"
        exit 0
    fi
    if [ -f ${IMG_BASE} ]; then
        createByImg
    else
        create
    fi
}

fcheck_param(){
    if [ -z ${IMG_BASE} ]; then
        echo -e  "\033[1;31mEnter the fourth parameter, which indicates the storage size or the image file.\033[0m"
        exit 0
    fi
    if [ -f ${IMG_BASE} ]; then
        fcreateByImg
    else
        fcreate
    fi
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

case "$ACTION" in
    create) check_param "$@";;
    fcreate) fcheck_param "$@";;
    delete) delete "$@";;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac