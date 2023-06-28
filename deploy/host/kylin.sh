#!/bin/bash

ARCH="$(arch)"
source ../url.conf

function usage()
{
    echo "usage:"
    echo -e "--install-4kb-kernel 在 aarch64 服务器上安装 4kb 内核"
    echo -e "--install opts"
    echo -e "opts:"
    echo -e "\t" "nvidia 安装 nvidia driver"
    echo -e "\t" "xrdp 安装 xrdp 远程桌面"
    echo -e "\t" "vulkan 安装 vulkan"
}

:<<!
安装 4kb 内核
kylin 暂未提供官方的下载地址, 这里暂时实现从本地文件安装内核的功能
默认 4kb 内核安装包在 /root 目录下
!
function install_4kb_kernel()
{
    echo "Install 4kb kernel ..."

    if [ "$(getconf PAGE_SIZE)" == 4096 ]; then
        echo "Already 4kb kernel"
        return 0
    fi

    if [ ! -f "/root/kernel-4.19.90-25.1.hwy.ctm-aarch64.tar.gz" ]; then
        echo "cannot access '/root/kernel-4.19.90-25.1.hwy.ctm-aarch64.tar.gz': No such file"
        return 1
    fi

    mkdir -p $DEPLOY_DIR && cd $DEPLOY_DIR
    tar xf /root/kernel-4.19.90-25.1.hwy.ctm-aarch64.tar.gz -C .
    tar xf kernel-4.19.90-25.1.hwy.ctm-aarch64/base-version.tar.gz
    cd base-version
    rpm -ivh \
        kernel-4.19.90-25.1.hwy.ctm.v2101.ky10.aarch64.rpm \
        kernel-core-4.19.90-25.1.hwy.ctm.v2101.ky10.aarch64.rpm \
        kernel-modules-*.rpm
    if [ $? -ne 0 ]; then
        echo "Failed to install kernel"
        return 1
    fi

    echo "success, please reboot"
    return 0
}

function install_host_nvidia_x86_64()
{
    echo "Install nvidia driver ..."

    nvidia-smi > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "Already install nvidia driver"
        return 0
    fi
    
    local download_dir="$DEPLOY_DIR/nvidia/$(arch)"
    mkdir -p $download_dir && cd $download_dir
    if [ ! -f "${NVIDIA_DRIVER_URL_X86##*/}" ]; then
        wget "$NVIDIA_DRIVER_URL_X86"
        if [ $? -ne 0 ]; then
            echo "Failed to download nvidia driver"
            return 1
        fi
    fi

    yum install -y which gcc gcc-c++ pkgconfig libglvnd-devel
    bash "${NVIDIA_DRIVER_URL_X86##*/}" --silent --driver


    nvidia-smi > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Failed to install nvidia driver"
        return 1
    fi

    echo "success"
    return 0
}

function install_host_nvidia_aarch64()
{
    nvidia-smi > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "Already install nvidia driver"
        return 0
    fi

    local download_dir="$DEPLOY_DIR/nvidia/$(arch)"
    mkdir -p $download_dir && cd $download_dir
    if [ ! -f "${NVIDIA_DRIVER_URL_AARCH64##*/}" ]; then
        wget "$NVIDIA_DRIVER_URL_AARCH64"
        if [ $? -ne 0 ]; then
            echo "Failed to download nvidia driver"
            return 1
        fi
    fi

    # get 4kb kernel source
    cd $DEPLOY_DIR/base-version
    rpm2cpio kernel-devel-* | cpio -div > /dev/null
    rpm2cpio kernel-headers-* | cpio -div > /dev/null

    cd $download_dir
    yum install -y which gcc gcc-c++ pkgconfig libglvnd-devel
    local kernel_source_path="$DEPLOY_DIR/base-version/usr/src/kernels/4.19.90-25.1.hwy.ctm.v2101.ky10.aarch64"
    bash "${NVIDIA_DRIVER_URL_AARCH64##*/}" --extract=$(pwd)/cuda
    cd cuda
    eval ./*.run --silent --kernel-source-path="${kernel_source_path}"

    nvidia-smi > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Failed to install nvidia driver"
        return 1
    fi

    echo "success"
    return 0
}

function install_host_nvidia()
{
    if [ $ARCH == "aarch64" ]; then
        install_host_nvidia_aarch64
    elif [ $ARCH == "X86_64" ]; then
        install_host_nvidia_x86_64
    else
        echo "Unsupport archticture"
        return 1
    fi
}

function install_xrdp()
{
    echo "Install xrdp ..."

    which xrdp
    if [ $? -eq 0 ]; then
        echo "Already install xrdp"
        return 0
    fi

    yum install -y \
        cmake patch gcc make autoconf libtool automake pkgconfig openssl-devel gettext file \
        pam-devel libX11-devel libXfixes-devel libjpeg-devel libXrandr-devel nasm \
        flex bison gcc-c++ libxslt perl-libxml-perl xmlto-tex \
        xorg-x11-font-utils xorg-x11-server-devel
    
    mkdir -p $DEPLOY_DIR/git/neutrinolabs && cd $DEPLOY_DIR/git/neutrinolabs

    local times=1
    while true; do
        echo "Try Download xrdp source, ${times} times ..."
        git clone --recursive https://github.com/neutrinolabs/xrdp.git && break

        times=$((times+1))
        if [ times -ge 10 ]; then
            echo "Failed to download xrdp source"
            return 1
        fi
    done

    cd xrdp
    ./bootstrap
    ./configure
    make -j $(nproc) && make install

    cd $DEPLOY_DIR/git/neutrinolabs
    times=1
    while true; do
        echo "Try Download xorgxrdp source, ${times} times ..."
        git clone https://github.com/neutrinolabs/xorgxrdp.git && break

        times=$((times+1))
        if [ times -ge 10 ]; then
            echo "Failed to download xorgxrdp source"
            return 1
        fi
    done

    cd xorgxrdp
    ./bootstrap
    ./configure \
        XRDP_CFLAGS=-I${DEPLOY_DIR}/git/neutrinolabs/xrdp/common \
        XRDP_LIBS=/usr/local/lib
    make -j $(npoc) && make install

    yum install -y firewalld
    firewall-cmd --add-port=3389/tcp --permanent
    firewall-cmd --reload

    echo mate-session > /root/.xsession
    chmod 700 /root/.xsession
    systemctl enable xrdp
    systemctl start xrdp

    return 0
}

function install_vulkan()
{
    echo "Install vulkan ..."

    yum groupinstall -y "Development Tools"
    yum install -y \
        glm-devel libpng-devel wayland-devel wayland-protocols-devel \
        libpciaccess-devel libX11-devel libxcb-devel libXrandr-devel \
        xcb-util xcb-util-keysyms-devel xcb-util-wm-devel python3  \
        lz4-devel libzstd-devel qt qt5-qtbase-devel
    
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
    if [ "$1" == "-h" -o "$1" == "--help" ]; then
        usage
        return 0
    elif [ "$1" == "--install-4kb-kernel" ]; then
        install_4kb_kernel || return 1
    elif [ "$1" == "--install" ]; then
        shift 1
        local install_nvidia=0
        local install_xrdp=0
        local install_vulkan=0
        for opt in $(echo $@ | tr "," " "); do
            if [ $opt == "nvidia" ]; then
                install_nvidia=1
            elif [ $opt == "xrdp" ]; then
                install_xrdp=1
            elif [ $opt == "vulkan" ]; then
                install_vulkan=1
            fi
        done

        if [ $install_nvidia -eq 1 ]; then
            install_host_nvidia || return 1
        fi

        if [ $install_xrdp -eq 1 ]; then
            install_xrdp || return 1
        fi

        if [ $install_vulkan -eq 1 ]; then
            install_vulkan || return 1
        fi
    fi
}

main $@