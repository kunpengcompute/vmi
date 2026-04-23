#!/bin/bash

ACTION=$1
MIN=$2
MAX=$3
SYSTEM_SIZE_MB=$4
if [ -z $3 ]; then
    MAX=$2
fi
if [ -z $4 ]; then
    SYSTEM_SIZE_MB=0 # 如果没有传入第五个参数，默认不使能配额限制
fi
function start(){
    for ((i=$MIN; i<=$MAX; i++))
    do
        export YAML_PATH=$i  #yaml配置文件位置
        export POD_NAME=video$i  # pod名称及容器名称
        export HOSTPORT=$(expr $i + 8000)   # 宿主机端口号
        export HOSTPORT2=$(expr $i + 8500)
        envsubst < k8s-video.yaml | kubectl apply -f -
        sleep 5
         if [ "$SYSTEM_SIZE_MB" -gt 0 ]; then
            echo "等待容器 ${POD_NAME} 启动以获取 Container ID 进行配额限制..."
            local max_wait=30
            local count=0
            local container_id=""

            # 轮询等待 K8s 分配 containerd 的运行 ID
            while [ $count -lt $max_wait ]; do
                # 提取形如 containerd://xxxxxxxx 的真实 ID 字符串
                container_id=$(crictl ps --name video$i --state running -q)
                if [ -n "$container_id" ]; then
                    break
                fi
                sleep 1
                count=$((count+1))
            done

            if [ -n "$container_id" ]; then
                # 1. 截取前 8 位并转换为 10 进制的 Project ID
                local hex_prefix=${container_id:0:8}
                local project_id=$((16#$hex_prefix))
                echo "截取前八位转换得到的 Project ID 为: $project_id"

                # 2. 轮询查找 upperdir (因为挂载也需要一丝丝时间)
                local upperdir=""
                local mount_wait=10
                local mount_count=0
                while [ $mount_count -lt $mount_wait ]; do
                    upperdir=$(mount | grep "${container_id}.*/rootfs" | sed -n "s/.*upperdir=\([^,]*\).*/\1/p")
                    if [ -n "$upperdir" ]; then
                        break
                    fi
                    sleep 1
                    mount_count=$((mount_count+1))
                done

                if [ -n "$upperdir" ]; then
                    # 动态获取 upperdir 所在的底层挂载点及文件系统类型
                    local base_mount=$(df -P "$upperdir" | tail -1 | awk '{print $6}')
                    local fs_type=$(df -T "$upperdir" | tail -1 | awk '{print $2}')
                    
                    if [ "$fs_type" != "xfs" ]; then
                        echo -e "\033[1;33m[WARNING] 容器 ${POD_NAME} 的底层挂载点 ${base_mount} 格式为 ${fs_type}，并非 xfs。已跳过 system 分区大小限制。\033[0m"
                    else
                        # 3. 施加 XFS Quota (单位转换: 命令接收 m 代表 MB)
                        echo "针对路径 [${upperdir}] 绑定 XFS Project 配额: ${SYSTEM_SIZE_MB}M"
                        xfs_quota -x -c "project -s -p ${upperdir} ${project_id}" "${base_mount}"
                        xfs_quota -x -c "limit -p bsoft=${SYSTEM_SIZE_MB}m bhard=${SYSTEM_SIZE_MB}m ${project_id}" "${base_mount}"
                        
                        if [ $? -eq 0 ]; then
                            echo -e "\033[1;32m[SUCCESS] 容器 ${POD_NAME} 的 system 分区配额(${SYSTEM_SIZE_MB}M) 已成功生效！\033[0m"
                        else
                            echo -e "\033[1;31m[ERROR] XFS 配额执行失败，请检查 ${base_mount} 是否开启了 pquota 挂载参数。\033[0m"
                        fi
                    fi
                else
                    echo -e "\033[1;31m[ERROR] 获取 upperdir 路径失败，跳过限制。\033[0m"
                fi
            else
                echo -e "\033[1;31m[ERROR] 等待容器就绪超时，未获取到 Container ID。\033[0m"
            fi 
        fi
    done
}

function delete(){
    for ((i=$MIN; i<=$MAX; i++))
    do
        kubectl delete pod video$i
    done
}

case "$ACTION" in
    start) start "$@";;
    delete) delete "$@";;
    *) echo -e  "\033[1;31minput command[$ACTION] not support.\033[0m"
esac
