#!/bin/bash
# 说明：下载开源软件到open_source_download目录，并解压到unpack_open_source目录。
# 若open_source_download下存在已下载好的开源软件，则不会重复下载。

set -e
cur_file_path=$(realpath $(dirname ${0}))
cd ${cur_file_path}
download_path=${cur_file_path}/../open_source_download
unpack_path=${cur_file_path}/../unpack_open_source

source ${cur_file_path}/compile_log.sh
source ${cur_file_path}/opensource_repo.conf

open_sources=(libdrm libva opus openH264_32 openH264_64)

is_download_file_exist=0

function check_download_file_exist()
{
    local file_name=${1}
    if [ -f "${download_path}/${file_name}" ]; then
        info "${download_path}/${file_name} already exit, no need download again."
        is_download_file_exist=1
        return
    fi
    is_download_file_exist=0
}

function download_open_source()
{
    local url=${1}
    local file_name=${2}
    wget ${url} -O ${download_path}/${file_name}
    if [ $? -ne 0 ]; then
        error "Download ${file_name} failed!"
        exit 1
    fi
}

function unpack_source()
{
    local target_unpack_path=${1}
    local file_name=${2}
    local unpack_type=${3}

    mkdir -p ${target_unpack_path}
    if [ "${unpack_type}" == "2" ]; then
        cp -rf ${download_path}/${file_name} ${target_unpack_path}/
        info "Copy ${file_name} success"
        return
    fi
    info "Unpack ${file_name} ..."
    # 目前需要解压的文件只有zip格式，后面若有其他格式的文件需要解压请在此处添加
    unzip -qo ${download_path}/${file_name} -d ${target_unpack_path}/
    local file_list=`ls ${target_unpack_path}`
    local file_count=`ls ${target_unpack_path} | wc -l`
    if [ ${file_count} -eq 1 ] && [ -d ${target_unpack_path}/${file_list[0]} ]; then
        # 解压后只有一个文件夹，判断为压缩包内还嵌套了一层多余的文件夹，将此文件夹内的文件移动至上一级
        mv ${target_unpack_path}/${file_list[0]}/* ${target_unpack_path}/
        local tmp_count=`ls ${target_unpack_path}/${file_list[0]} | wc -l`
        if [ ${tmp_count} -eq 0 ]; then
            rm -rf ${target_unpack_path}/${file_list[0]}
        fi
    fi
    info "Unpack success"
}

for open_source in ${open_sources[@]}
do
    source_info=($(eval echo '${'${open_source}'[*]}'))
    if [ ${#source_info[@]} -ne 4 ];then
        error "${open_source} config info is incorrect"
        exit 1
    fi
    url=${source_info[0]}
    file_name=${source_info[1]}
    target_unpack_path=${unpack_path}/${source_info[2]}
    unpack_type=${source_info[3]}
    if [ "${unpack_type}" == "1" ]; then
        target_unpack_path=${target_unpack_path}/codes
    fi

    mkdir -p ${download_path}
    check_download_file_exist ${file_name}
    if [ ${is_download_file_exist} -ne 1 ]; then
        download_open_source ${url} ${file_name}
    fi

    unpack_source ${target_unpack_path} ${file_name} ${unpack_type}
done

