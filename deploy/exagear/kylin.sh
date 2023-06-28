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
    cd "$(find . -type d -iname "*centos7")/release"
    rpm -ivh *.rpm
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

    yum install -y wget

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

    yum install -y which gcc gcc-c++ pkgconfig libglvnd-devel
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

function upgrade_git()
{
    echo "Upgrade git ..."

    cd $DEPLOY_DIR
    local git_url="https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.9.5.tar.xz"
    local git_package="${git_url##*/}"
    
    yum remove -y git
    if [ ! -f ${git_package} ]; then
        wget "${git_url}"
        if [ $? -ne 0 ]; then
            echo "Failed to download git"
            return 1
        fi
    fi

    tar xf ${git_package}
    cd "${git_package%.tar.xz}"
    yum install -y \
        curl-devel expat-devel gettext-devel openssl-devel zlib-devel perl-ExtUtils-MakeMaker
    ./configure --prefix=/usr/local
    make -j "$(nproc)" && make install
    if [ $? -ne 0 ]; then
        echo "Failed to compile git"
        return 1
    fi

    ln -s /usr/local/bin/git /usr/bin/git
    echo "success"
    return 0
}

function upgrade_cmake()
{
    echo "Upgrade cmake ..."

    cd $DEPLOY_DIR
    local cmake_url="https://cmake.org/files/v3.26/cmake-3.26.4-linux-x86_64.sh"
    local cmake_package="${cmake_url##*/}"

    yum remove -y cmake
    if [ ! -f ${cmake_package} ]; then
        wget "${cmake_url}"
        if [ $? -ne 0 ]; then
            echo "Failed to download cmake"
            return 1
        fi
    fi
    sh ${cmake_package} --prefix=/usr/local --exclude-subdir

    echo "success"
    return 0
}

function upgrade_gcc()
{
    echo "Upgrade gcc ..."
    yum install -y centos-release-scl
    yum install -y devtoolset-7-gcc* devtoolset-7-libstdc++*
    echo "success"
}

function install_exagear_vulkan()
{
    echo "Install vulkan ..."
    yum install -y wget unzip make numactl mesa-dri-drivers

    upgrade_git || return 1
    upgrade_cmake || return 1
    upgrade_gcc || return 1

    yum groupinstall -y "Development Tools"
    yum install -y \
        glm-devel libpng-devel wayland-devel wayland-protocols-devel \
        libpciaccess-devel libX11-devel libxcb-devel libXrandr-devel \
        xcb-util xcb-util-keysyms-devel xcb-util-wm-devel python3  \
        lz4-devel libzstd-devel qt qt5-qtbase-devel
    
    source /opt/rh/devtoolset-7/enable

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
    if [ -z "$1" -o "$1" == "-h" -o "$1" == "--help" ]; then
        usage
        return 0
    elif [ "$1" == "--install-exagear" ]; then
        install_exagear || return 1
    elif [ "$1" == "--install" ]; then
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