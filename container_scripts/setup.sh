#!/bin/bash
# Script for setting up docker env in Kylin OS

cur_file_path=$(pwd)

install_docker()
{
    echo "Installing docker 19.03 for Kylin OS"
    which docker && echo "Existing docker found, skip installing" && return 0
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
    # Set up docker service
    echo "Enabling docker service..."
    systemctl enable docker.service
    echo "Starting docker service..."
    systemctl start docker.service

    # daemon.json created after enabling docker service
    echo '{"experimental":true}' > /etc/docker/daemon.json
    echo "Restarting docker service..."
    systemctl restart docker.service
   
    cd -
    return 0
}

install_nvidia_container_toolkit()
{
    echo "Installing nvidia container toolkit"
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
    local sum=0
    make centos8
    # ensure network is ok, 10 tries maximum
    while [ $? -ne 0 ]
    do
        let sum++
        [ ${sum} -eq 10 ] && echo "Reached maximum retry, make failed" && exit
        echo "retrying make libnvidia-container for ${sum} times"
        make centos8 && break
    done

    cd -

    # Download and compile container-toolkit
    CONTAINER_TOOLKIT_LOC=${BASE_DIR}/container-toolkit

    cd ${BASE_DIR}
    git clone https://gitlab.com/nvidia/container-toolkit/container-toolkit.git
    cd container-toolkit
    sum=0
    make centos8
    while [ $? -ne 0 ]
    do
        let sum++
        [ ${sum} -eq 10 ] && echo "Reached maximum retry, make failed" && exit
        echo "retrying make for container-toolkit ${sum} times"
        make centos8 && break
    done
    cd -

    # install libnvidia-container
    cd ${LIBNVIDIA_CONTAINER_LOC}/dist/centos8/aarch64
    echo "Installing libnvidia-container"
    sudo rpm -ivh *.rpm
    [ $? -ne 0 ] && echo "install libnvidia-container failed" && exit

    # install container-toolkit
    cd ${CONTAINER_TOOLKIT_LOC}/dist/centos8/aarch64
    echo "Installing container-toolkit"
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
    echo "Running container..."
    CONTAINER_ID=$(docker run --gpus all --privileged \
        -e NVIDIA_DISABLE_REQUIRE=1 \
	-v $HOME/.Xauthority:/root/.Xauthority \
	-e DISPLAY -e NVIDIA_DRIVER_CAPABILITIES=all \
	--device /dev/dri --net host \
	-v /etc/vulkan/icd.d/nvidia_icd.json:/etc/vulkan/icd.d/nvidia_icd.json \
	-v /etc/vulkan/implicit_layer.d/nvidia_layers.json:/etc/vulkan/implicit_layer.d/nvidia_layers.json \
	-v /usr/share/glvnd/egl_vendor.d/10_nvidia.json:/usr/share/glvnd/egl_vendor.d/10_nvidia.json -itd container:2004)
    [ $? -eq 0 ] && echo "Create container success" && return 0
    echo "Create container failed" && exit
}

default_set_up()
{
    install_docker
    [ $? -ne 0 ] && echo "install docker 19.03 failed" && exit
    install_nvidia_container_toolkit
    [ $? -ne 0 ] && echo "install nv docker failed" && exit
    build_ubuntu_image
    [ $? -ne 0 ] && echo "build ubuntu image failed" && exit
    create_container
    [ $? -ne 0 ] && echo "create container failed" && exit
    return 0
}

setup_new_container()
{
    create_container
    [ $? -ne 0 ] && echo "create container failed" && exit
    echo "Installing dependency for container ${CONTAINER_ID}"
    docker exec -it ${CONTAINER_ID} /bin/bash -c 'export DEBIAN_FRONTEND=noninteractive'
    docker exec -it ${CONTAINER_ID} /bin/bash -c 'sudo apt-get update'
    docker exec -it ${CONTAINER_ID} /bin/bash -c 'sudo apt-get install -y\
        build-essential \
        numactl \
        *vulkan*'
   echo "Set up new container success"
   return 0
}

setup_new_container_with_exagear()
{
    set_up_new_container
    cd cur_file_path
    if [ ! -f "ExaGear*.tar.gz" ]; then
        echo "ExaGear package not exists" && exit
    fi
    docker cp ExaGear*.tar.gz ${CONTAINER_ID}:/root
    if [ $? -ne 0 ]; then
       echo "copy exagear package failed, remove container"
       docker stop ${CONTAINER_ID}
       docker rm ${CONTAINER_ID}
    fi
    echo "Installing ExaGear server for ubuntu"
    docker exec -it ${CONTAINER_ID} /bin/bash -c 'cd /root && tar -zxf ExaGear*.tar.gz'
    docker exec -it ${CONTAINER_ID} /bin/bash -c 'cd /root/ExaGear_2.0.0.1/ExaGear_Server_for_Ubuntu20/release && sudo dpkg -i \
        exagear-utils*.deb \
        exagear-core-x64*.deb \
        exagear-core-x32*.deb \
        exagear-guest*.deb \
        exagear-integration*.deb'
    [ $? -eq 0 ] && echo "Install ExaGear success" && return 0
}

clean_up()
{
    echo "Deleting all containers"
    docker stop $(docker ps -aq)
    docker rm $(docker ps -aq)
    return 0
}

ACTION=$1; shift
echo "Preferred action is: ${ACTION}"
case "$ACTION" in
    default) default_set_up "@"
    ;;
    new) setup_new_container "@"
    ;;
    exagear) setup_new_container_with_exagear "@"
    ;;
    build ) build_ubuntu_image "@"
    ;;
    clean) clean_up "@"
    ;;
    *) echo "Incorrect input, ${ACTION} is not supported"
    ;;
esac
    