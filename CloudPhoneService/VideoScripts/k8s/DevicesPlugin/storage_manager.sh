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

case "$ACTION" in
    create) check_param "$@";;
    delete) delete "$@";;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac