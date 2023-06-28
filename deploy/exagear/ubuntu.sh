#!/bin/bash

ARCH=$(arch)
source ../url.conf

function usage()
{
    echo "usage:"
    echo -e "--install-exagear 安装 exagear 转码环境"
    echo -e "--install opts"
    echo -e "opts:"
    echo -e "\t" "nvidia 安装 nvidia driver"
    echo -e "\t" "vulkan 安装 vulkan"
}

function install_exagear()
{
    echo "Install exagear ..."

    mkdir -p $DEPLOY_DIR/exagear && cd $DEPLOY_DIR/exagear
    local exagear_package="${EXAGEAR_URL##*/}"
    if [ ! -f "${exagear_package}" ]; then
        wget "$EXAGEAR_URL"
        if [ $? -ne 0 ]; then
            echo "Failed to download exagear"
            return 1
        fi
    fi
    
    tar xf "${exagear_package}"
    cd "$(find . -type d -iname "*ubuntu18")/release"
    dpkg -i *.deb
    sed --follow-symlinks -i -e 's@EXAGEAR_USE_OPT="y"@EXAGEAR_USE_OPT="n"@g' -e 's@fbase@all@g' /etc/exagear-x86_32.conf
    sed --follow-symlinks -i -e 's@EXAGEAR_USE_OPT="y"@EXAGEAR_USE_OPT="n"@g' -e 's@fbase@all@g' /etc/exagear-x86_64.conf

    echo "success"
    return 0
}

function install_exagear_nvidia()
{
    echo "Install nvidia driver ..."

    nvidia-smi > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "Already install nvidia driver"
        return 0
    fi

    apt install -y wget

    local download_dir="$DEPLOY_DIR/nvidia/$(arch)"
    mkdir -p $download_dir && cd $download_dir
    local nvidia_package="${NVIDIA_DRIVER_URL_X86##*/}"
    if [ ! -f ${nvidia_package} ]; then
        wget "$NVIDIA_DRIVER_URL_X86"
        if [ $? -ne 0 ]; then
            echo "Failed to download nvidia driver"
            return 1
        fi
    fi

    apt install -y build-essential pkg-config libglvnd* libxml2 kmod
    bash ${nvidia_package} --extract=$(pwd)/cuda
    cd cuda
    eval ./*.run --silent --no-kernel-modules --no-x-check

    nvidia-smi > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Failed to install nvidia driver"
        return 1
    fi

    echo "success"
    return 0    
}

function install_exagear_vulkan()
{
    echo "Install vulkan ..."

    apt install -y \
            wget libglm-dev cmake libxcb-dri3-0 libxcb-present0 libpciaccess0 \
            libpng-dev libxcb-keysyms1-dev libxcb-dri3-dev libx11-dev g++ gcc \
            libmirclient-dev libwayland-dev libxrandr-dev libxcb-randr0-dev libxcb-ewmh-dev \
            git python3 bison libx11-xcb-dev liblz4-dev libzstd-dev python3-distutils qt5-default
    apt install -y numactl unzip

    local download_dir="/usr/local/vulkan/${ARCH}"
    mkdir -p  "${download_dir}" && cd "${download_dir}"

    local package="${VULKAN_URL##*/}"
    if [ ! -f "${package}" ]; then
        echo "Downloading vulkan ..."
        wget "${VULKAN_URL}"
        if [ $? -ne 0 ]; then
            echo "Failed to download vulkan"
            return 1
        fi
    fi

    local version="$(basename ${package##*-} .tar.gz)"
    if [ ! -d "${version}" ]; then
        tar xf "${package}"
    fi
    cd "${version}"

    # ubuntu x86 的 vulkan 不需要编译
cat << EOF > /etc/profile.d/vulkan-$ARCH.sh
if [ \$(arch) == $ARCH ]; then
    . ${download_dir}/${version}/setup-env.sh
fi
EOF
    chmod 444 /etc/profile.d/vulkan-$ARCH.sh

    echo "success"
    return 0
}

function main()
{
    if [ -z "$1" -o "$1" == "-h" -o "$1" == "--help" ]; then
        usage
        return 0
    elif [ "$1" == "--install-exagear" ]; then
        install_exagear || return 1
    elif [ "$1" == "--install" ]; then

        if [ ! -f /etc/apt/sources.list.bak ]; then
            # 更换软件源为 华为源
            cp /etc/apt/sources.list /etc/apt/sources.list.bak
            sed -i "s@http://.*archive.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list
            sed -i "s@http://.*security.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list
        fi
        apt update

        shift 1
        local install_nvidia=0
        local install_vulkan=0
        for opt in $(echo $@ | tr "," " "); do
            if [ $opt == "nvidia" ]; then
                install_nvidia=1
            elif [ $opt == "vulkan" ]; then
                install_vulkan=1
            fi
        done

        if [ $install_nvidia -eq 1 ]; then
            install_exagear_nvidia || return 1
        fi

        if [ $install_vulkan -eq 1 ]; then
            install_exagear_vulkan || return 1
        fi
    else
        usage
        return 1
    fi
}

main $@