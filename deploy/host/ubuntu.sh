#!/bin/bash

ARCH=$(arch)
source ../url.conf

function usage()
{
    echo "usage:"
    echo -e "--install opts"
    echo -e "opts:"
    echo -e "\t" "nvidia 安装 nvidia driver"
    echo -e "\t" "xdesktop 安装 xubuntu 桌面"
    echo -e "\t" "xrdp 安装 xrdp 远程桌面"
    echo -e "\t" "vulkan 安装 vulkan"
}

function install_host_nvidia()
{
    echo "install nvidia ..."

    nvidia-smi > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "Already install nvidia driver"
        return 0
    fi

    if [ $ARCH == aarch64 ]; then
        URL="$NVIDIA_DRIVER_URL_AARCH64"
    else
        URL="$NVIDIA_DRIVER_URL_X86"
    fi

    local download_dir="$DEPLOY_DIR/nvidia/$(arch)"
    mkdir -p $download_dir && cd $download_dir
    local nvidia_package="${URL##*/}"
    if [ ! -f "${nvidia_package}" ]; then
        wget "$URL"
        if [ $? -ne 0 ]; then
            echo "Failed to download nvidia driver"
            return 1
        fi
    fi

    apt install -y build-essential pkg-config libglvnd*
    bash "${nvidia_package}" --silent --driver

    nvidia-smi > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Failed to install nvidia driver"
        return 1
    fi

    echo "success"
    return 0
}

function install_xubuntu_desktop()
{
    echo "install xubuntu-desktop ..."

    apt install -y tasksel
    tasksel install xubuntu-desktop

    echo "success"
}

function install_xrdp()
{
    echo "Install xrdp ..."

    apt install -y xrdp firewalld
    echo "startxfce4" >> /etc/xrdp/xrdp.ini
    echo "xfce4-session" > /root/.xsession
    chmod 500 /root/.xsession

    firewall-cmd --add-port=3389/tcp --permanent
    firewall-cmd --reload

    systemctl enable xrdp
    systemctl start xrdp

    echo "success"
}

function install_vulkan()
{
    echo "Install vulkan ..."

    apt install -y \
            wget libglm-dev cmake libxcb-dri3-0 libxcb-present0 libpciaccess0 \
            libpng-dev libxcb-keysyms1-dev libxcb-dri3-dev libx11-dev g++ gcc \
            libmirclient-dev libwayland-dev libxrandr-dev libxcb-randr0-dev libxcb-ewmh-dev \
            git python3 bison libx11-xcb-dev liblz4-dev libzstd-dev python3-distutils qt5-default
    apt install -y numactl

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

    # x86 ubuntu 不需要编译 vulkan
    if [ $ARCH == aarch64 ]; then
        cd source/shaderc
        local times=1
        while true; do
            echo "Try to download shaderc ${times} times"
            python3 update_shaderc_sources.py && break

            times=$((times+1))
            if [ "${times}" -ge 10 ]; then
                echo "Failed to download vulkan source"
                return 1
            fi
        done
        cd -

        echo "Compiling vulkan ..."
        ./vulkansdk
        if [ $? -ne 0 ]; then
            echo "Failed to compile vulkan"
            return 1
        fi
    fi

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
    if [ $1 == "-h" -o $1 == "--help" ]; then
        usage
        return 0
    elif [ $1 == "--install" ]; then

        if [ ! -f /etc/apt/sources.list.bak ]; then
            # 更换软件源为 华为源
            cp /etc/apt/sources.list /etc/apt/sources.list.bak
            wget -O /etc/apt/sources.list https://repo.huaweicloud.com/repository/conf/Ubuntu-Ports-bionic.list
            apt update
        fi

        shift 1
        for opt in $(echo $@ | tr "," " "); do
            [ $opt == "nvidia" ] && INSTALL_NVIDIA="yes"
            [ $opt == "xdesktop" ] && INSTALL_XUBUNTU_DESKTOP="yes"
            [ $opt == "xrdp" ] && INSTALL_XRDP="yes"
            [ $opt == "vulkan" ] && INSTALL_VULKAN="yes"
        done
    else
        usage
        return 1
    fi

    if [ "${INSTALL_NVIDIA-no}" == "yes" ]; then
        install_host_nvidia || return 1
    fi
    if [ "${INSTALL_XUBUNTU_DESKTOP-no}" == "yes" ]; then
        install_xubuntu_desktop || return 1
    fi
    if [ "${INSTALL_XRDP-no}" == "yes" ]; then
        install_xrdp || return 1
    fi
    if [ "${INSTALL_VULKAN-no}" == "yes" ]; then
        install_vulkan || return 1
    fi
}

main $@