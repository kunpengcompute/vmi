#!/bin/bash
# Script for setting up docker env in Kylin OS

cur_file_path=$(pwd)

install_docker()
{
    if [ -n $(which docker) ]; then
        echo "Existing docker found, skip installing..."
        return 0
    fi
    DOCKER_INSTALL_LOC=/root/test/docker-19.03
    if [ ! -d ${DOCKER_INSTALL_LOC} ]; then
        mkdir -p ${DOCKER_INSTALL_LOC}
    fi

    cd ${DOCKER_INSTALL_LOC}
    rm -rf ${DOCKER_INSTALL_LOC}/*

    # Download docker19.03 rpms since nvidia-docker required docker ver >= 19.03

    wget https://download.docker.com/linux/centos/8/aarch64/stable/Packages/containerd.io-1.6.16-3.1.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "Download containerd.io failed" && exit
    wget https://download.docker.com/linux/centos/8/aarch64/stable/Packages/docker-ce-19.03.15-3.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "Download docker-ce failed" && exit
    wget https://download.docker.com/linux/centos/8/aarch64/stable/Packages/docker-ce-cli-19.03.15-3.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "Download docker-ce-cli failed" && exit
    wget https://download.docker.com/linux/centos/8/aarch64/stable/Packages/docker-compose-plugin-2.6.0-3.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "Download docker-compose-plugin failed" && exit
    #wget https://download.docker.com/linux/centos/8/aarch64/stable/Packages/docker-buildx-plugin-0.10.2-1.el8.aarch64.rpm
    #[ $? -ne 0 ] && echo "Download docker-buildx-plugin failed" && exit

    echo "Installing docker rpms"
    sudo rpm -ivh *.rpm
    [ $? -ne 0 ] && echo "Installation failed" && exit
    echo '{"experimental":true}' > /etc/docker/daemon.json
    # Set up docker service
    echo "Enabling docker service..."
    systemctl enable docker.service
    echo "Starting docker service..."
    systemctl start docker.service
   
    cd -
    return 0
}

install_nvidia_container_toolkit()
{
    BASE_DIR=/root/nvidia-docker
    if [ ! -d ${BASE_DIR} ]; then
        mkdir -p ${BASE_DIR}
    fi

    LIBNVIDIA_CONTAINER_LOC=${BASE_DIR}/libnvidia-container
      
    cd ${BASE_DIR}
    rm -rf ${BASE_DIR}/*

    # Download and compile libnvidia-container
    git clone https://github.com/NVIDIA/libnvidia-container.git
    [ $? -ne 0 ] && echo "clone libnvidia-container failed" && exit
    cd libnvidia-container
    make centos8
    cd -

    # Download and compile container-toolkit
    CONTAINER_TOOLKIT_LOC=${BASE_DIR}/container-toolkit

    cd ${BASE_DIR}
    git clone https://gitlab.com/nvidia/container-toolkit/container-toolkit.git
    cd container-toolkit
    make centos8
    cd -

    # install libnvidia-container
    cd ${LIBNVIDIA_CONTAINER_LOC}/dist/centos8/aarch64
    echo "- - - - - Installing libnvidia-container - - - - - "
    sudo rpm -ivh *.rpm
    [ $? -ne 0 ] && echo "install libnvidia-container failed" && exit

    # install container-toolkit
    cd ${CONTAINER_TOOLKIT_LOC}/dist/centos8/aarch64
    echo "- - - - - Installing container-toolkit - - - - - "
    sudo rpm -ivh *.rpm
    [ $? -ne 0 ] && echo "install container-toolkit failed" && exit

    # register nvidia runtime
    echo "registering nvidia runtime"
    sudo nvidia-ctk runtime configure --runtime=docker --set-as-default

    sudo systemctl restart docker
    return 0
}

build_ubuntu_image()
{
    # build ubuntu based on dockerfile
    cd ${cur_file_path}/dockerfile
    
    echo "Building ubuntu image..."
    docker build --pull -t \
       container:2004 \
       --build-arg BASE_DIST=ubuntu20.04 \
       --build-arg CUDA_VERSION=11.7.0 \
       --file dockerfile .
    [ $? -eq 0 ] && echo "Build ubuntu image success" && cd - && return 0
    echo "Build ubuntu image failed" && exit
}

create_container()
{
    echo "running container..."
    docker run --gpus all --privileged \
        -e NVIDIA_DISABLE_REQUIRE=1 \
	-v $HOME/.Xauthority:/root/.Xauthority \
	-e DISPLAY -e NVIDIA_DRIVER_CAPABILITIES=all \
	--device /dev/dri --net host \
	-v /etc/vulkan/icd.d/nvidia_icd.json:/etc/vulkan/icd.d/nvidia_icd.json \
	-v /etc/vulkan/implicit_layer.d/nvidia_layers.json:/etc/vulkan/implicit_layer.d/nvidia_layers.json \
	-v /usr/share/glvnd/egl_vendor.d/10_nvidia.json:/usr/share/glvnd/egl_vendor.d/10_nvidia.json -itd container:2004
    [ $? -eq 0 ] && echo "Create container success" && return 0
    echo "Create container failed" && exit
}


echo "- - - - - Installing docker 19.03 for Kylin OS - - - - -"
install_docker

echo "- - - - - Installing nvidia container toolkit - - - - - "
install_nvidia_container_toolkit

build_ubuntu_image
create_container
