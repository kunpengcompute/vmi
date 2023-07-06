#!/bin/bash
# Script for setting up docker env in Kylin v10 SP2/openEuler 22.03 LTS SP1

cur_file_path=$(pwd)

# default image info
IMAGE_NAME="ubuntu:2004"
IMAGE_VERSION="ubuntu20.04"
IMAGE_CUDA_VERSION="11.7.0"

# support customized image
IMAGE_CUSTOMIZE_NAME="$2"
IMAGE_CUSTOMIZE_VERSION="$3"
IMAGE_CUSTOMIZE_CUDA_VERSION="$4"

if [ ! -z "$IMAGE_CUSTOMIZE_NAME" ]; then
    echo "Image name specified: ${IMAGE_CUSTOMIZE_NAME}"
    IMAGE_NAME=$IMAGE_CUSTOMIZE_NAME
fi

if [ ! -z "$IMAGE_CUSTOMIZE_VERSION" ]; then
    echo "Image base version specified: ${IMAGE_CUSTOMIZE_VERSION}"
    IMAGE_VERSION=$IMAGE_CUSTOMIZE_VERSION
fi

if [ ! -z "$IMAGE_CUSTOMIZE_CUDA_VERSION" ]; then
    echo "Image cuda version specified: ${IMAGE_CUSOMIZE_CUDA_VERSION}"
    IMAGE_CUDA_VERSION=$IMAGE_CUSTOMIZE_CUDA_VERSION
fi

install_docker()
{
    echo "Installing docker 19.03"
    which docker && echo "Existing docker found, skip installing" && return 0
    DOCKER_INSTALL_LOC=/root/test/docker-19.03
    if [ ! -d ${DOCKER_INSTALL_LOC} ]; then
        mkdir -p ${DOCKER_INSTALL_LOC}
    fi

    cd ${DOCKER_INSTALL_LOC}
    rm -rf ${DOCKER_INSTALL_LOC}/*

    # Download docker19.03 rpms since nvidia-docker required docker ver >= 19.03
    wget -t 0 -c https://download.docker.com/linux/centos/8/aarch64/stable/Packages/containerd.io-1.6.16-3.1.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "ERROR: Download containerd.io failed" && exit
    wget -t 0 -c https://download.docker.com/linux/centos/8/aarch64/stable/Packages/docker-ce-19.03.15-3.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "ERROR: Download docker-ce failed" && exit
    wget -t 0 -c https://download.docker.com/linux/centos/8/aarch64/stable/Packages/docker-ce-cli-19.03.15-3.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "ERROR: Download docker-ce-cli failed" && exit
    wget -t 0 -c https://download.docker.com/linux/centos/8/aarch64/stable/Packages/docker-compose-plugin-2.6.0-3.el8.aarch64.rpm
    [ $? -ne 0 ] && echo "ERROR: Download docker-compose-plugin failed" && exit

    echo "Installing docker rpms"
    sudo rpm -ivh *.rpm
    [ $? -ne 0 ] && echo "ERROR: Installation failed" && exit
    # Set up docker service
    echo "Enabling docker service..."
    systemctl enable docker.service || echo "ERROR: enable docker service failed" && exit 1
    echo "Starting docker service..."
    systemctl start docker.service || echo "ERROR: start docker service failed" && exit 1

    # daemon.json created after enabling docker service
    echo '{"experimental":true}' > /etc/docker/daemon.json
    echo "Restarting docker service..."
    systemctl restart docker.service || echo "ERROR: restart docker service failed" && exit 1

    # test if docker is functionable
    docker run hello-world || echo "ERROR: docker error" && exit 1
   
    cd -
    return 0
}

install_nvidia_container_toolkit()
{
    echo "Installing nvidia container toolkit"
    BASE_DIR=/root/nvidia-container-toolkit
    if [ ! -d ${BASE_DIR} ]; then
        mkdir -p ${BASE_DIR}
    fi
      
    cd ${BASE_DIR}
    rm -rf ${BASE_DIR}/*

    # Download and compile container-toolkit
    CONTAINER_TOOLKIT_LOC=${BASE_DIR}/container-toolkit

    cd ${BASE_DIR}
    git clone https://gitlab.com/nvidia/container-toolkit/container-toolkit.git
    if [ $? -ne 0 ]; then
        echo "ERROR: Git clone failed"
        exit 1
    fi
    cd container-toolkit
    local sum=0
    echo "Building rpms"
    bash ./scripts/build-packages.sh centos8-aarch64
    while [ $? -ne 0 ]
    do
        let sum++
        [ ${sum} -eq 10 ] && echo "ERROR: Reached maximum retry, make failed" && exit 1
        echo "retrying make for container-toolkit ${sum} times"
        bash ./scripts/build-packages.sh centos8-aarch64 && break
    done
    cd -

    # install rpms after success compilation
    if [ ! -d "${CONTAINER_TOOLKIT_LOC}/dist/centos8/aarch64" ]; then
        echo "Directory does not exists!" && exit 1
    fi
    cd ${CONTAINER_TOOLKIT_LOC}/dist/centos8/aarch64
    echo "Installing container-toolkit and related tools"
    sudo rpm -ivh *.rpm
    [ $? -ne 0 ] && echo "ERROR: install container-toolkit failed" && exit 1

    # register nvidia runtime
    echo "registering nvidia runtime"
    sudo nvidia-ctk runtime configure --runtime=docker --set-as-default
    [ $? -ne 0 ] && echo "ERROR: configure container-toolkit runtime failed" && exit 1
    sudo systemctl restart docker
    return 0
}

build_image()
{
    # build ubuntu based on dockerfile
    cd ${cur_file_path}/dockerfile
    
    echo "Building ${IMAGE_VERSION} image..."
    docker build --pull -t \
       ${IMAGE_NAME} \
       --build-arg BASE_DIST=${IMAGE_VERSION} \
       --build-arg CUDA_VERSION=${IMAGE_CUDA_VERSION} \
       --file dockerfile .
    [ $? -eq 0 ] && echo "Build image success" && cd - && return 0
    echo "ERROR: Build image failed" && exit 1
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
	-v /usr/share/glvnd/egl_vendor.d/10_nvidia.json:/usr/share/glvnd/egl_vendor.d/10_nvidia.json -itd ${IMAGE_NAME})
    [ $? -eq 0 ] && echo "Create container success, container id: ${CONTAINER_ID}" && return 0
    echo "ERROR:Create container failed" && exit 1
}

default_set_up()
{
    install_docker
    [ $? -ne 0 ] && echo "ERROR: install docker 19.03 failed" && exit 1
    install_nvidia_container_toolkit
    [ $? -ne 0 ] && echo "ERROR: install nv docker failed" && exit 1
    build_image
    [ $? -ne 0 ] && echo "ERROR: build ubuntu image failed" && exit 1
    create_container
    [ $? -ne 0 ] && echo "ERROR: create container failed" && exit 1
    return 0
}

setup_new_container_with_exagear()
{
    set_up_new_container
    cd cur_file_path
    if [ ! -f "ExaGear*.tar.gz" ]; then
        echo "ERROR: ExaGear package not exists" && exit 1
    fi
    docker cp ExaGear*.tar.gz ${CONTAINER_ID}:/root
    if [ $? -ne 0 ]; then
       echo "ERROR: copy exagear package failed, remove container"
       docker stop ${CONTAINER_ID}
       docker rm ${CONTAINER_ID}
       exit
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

print_help()
{
    echo "Options:"
    echo "--help: Display help message"
    echo "--default: default set up, including docker and nvidia container toolkit installation & create container"
    echo "--build: build nvidia/cuda image based on given image name or default, e.g. ./setup.sh build {image_name} {image_version} {image_cuda_version}"
    echo "--exagear: create a new container and install exagear (put the exagear tar file and this script in the same folder)"
    echo "--new: create a new container with given image name or by default"
    echo "--clean: remove all existing containers"
}

ACTION=$1; shift
echo "Preferred action is: ${ACTION}"
case "$ACTION" in
    --default) default_set_up
    ;;
    --new) create_container
    ;;
    --exagear) setup_new_container_with_exagear
    ;;
    --build) build_image
    ;;
    --clean) clean_up
    ;;
    --help) print_help
    ;;
    *) echo "Incorrect input, ${ACTION} is not supported"
    ;;
esac
