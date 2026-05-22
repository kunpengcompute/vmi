# 编译指南

## 环境准备

### 编译服务器

系统版本：Ubuntu 22.04.3 LTS

系统架构：x86_64

使用自己的用户账号

### 网络要求

保证服务器正常联网，以确保可以正常gradle编译和下载开源软件。

### AOSP环境搭建

获取[AOSP源码](https://link.gitcode.com/?target=https%3A%2F%2Fandroid.googlesource.com%2Fplatform%2Fmanifest&from=https%3A%2F%2Fgitcode.com%2Ffuaniu%2FKbox%2Fblob%2FAOSP11%2Fdocs%2Fzh%2Fcompile_guide.md&lang=zh&theme=white)，上传至~/ARMNative目录下并解压

```shell
cd ~/ARMNative
tar -xvf aosp11r48.tar
```

### 项目代码下载

编译完整的视频流二进制包需要下载本仓库和[VMIEngine](https://gitcode.com/boostkit/VMIEngine)仓库。

```shell
git clone https://gitcode.com/boostkit/vmi.git
git clone https://gitcode.com/boostkit/VMIEngine.git
```

### 安装编译环境

进入vmi源码目录下执行auto_install_tools.sh，如果想了解各脚本的具体实现细节，请参考[编译脚本介绍](compile_scripts_introduction.md)文档

``` shell
cd ~/ARMNative/vmi
./scripts/auto_install_tools.sh ${安装目录} 
source ~/.bashrc 
```

其中，\${安装目录}可以自己指定，若不输入\${安装目录}，则脚本使用默认目录~/NativeCompileToolsDir。

并在~/.bashrc中添加以下环境变量：

```shell
# android
export AN_AOSPDIR=/home/newdisk/XXX/ARMNative/aosp11 # 改成实际存放aosp11源码的绝对路径
export AN_AOSPOUT=${AN_AOSPDIR}/out
```

更新后source使其生效

```shell
source ~/.bashrc
```

### ccache配置

配置ccahe加快编译速度，查看本地ccache安装位置

```shell
sudo apt install ccache
which ccache
```

在.bashrc中配置环境变量

```shell
export NDK_CCACHE=ccache
export CCACHE_DIR=~/.ccache
export PATH={ccache安装位置}:$PATH # 例如export PATH=/usr/bin:$PATH
export USR_CCACHE=1
```

更新后source使其生效

```shell
source ~/.bashrc
```

软链接所有的gcc和g++到ccache上

```shell
cd /usr/bin # ccache安装位置
ln -s ccache /usr/bin/gcc
ln -s ccache /usr/bin/g++
ln -s ccache /usr/bin/cc
ln -s ccache /usr/bin/c++
```

## 编译AOSP源码

### 安装依赖库

下载必要依赖，如果已安装，跳过即可

```shell
sudo apt install -y git     
sudo apt install -y libtool automake tclsh make openjdk-11-jdk git-core gnupg     
sudo apt install -y flex bison gperf build-essential zip curl zlib1g-dev     
sudo apt install -y gcc-multilib g++-multilib libc6-dev-i386 lib32ncurses5-dev     
sudo apt install -y x11proto-core-dev libx11-dev lib32z-dev ccache libgl1-mesa-dev     
sudo apt install -y libxml2-utils xsltproc unzip     
sudo apt install -y libx11-dev libreadline-dev     
sudo apt install -y libncurses5 libncurses5-dev     
sudo apt install -y tofrodos python-markdown-doc zlib1g-dev     
sudo apt install -y dpkg-dev libsdl1.2-dev     
sudo apt install -y m4 bc python3 python3-mako gettext     
sudo apt install -y expect
```

### 编译源码

进入安卓源码目录，执行如下编译指令

```shell
cd ~/ARMNative/aosp11
source build/envsetup.sh
lunch aosp_arm64-eng
make -j【线程数】
```

## 编译视频流

编译前需设置如下环境变量，不建议写入~/.bashrc中，每次编译前设置即可。

```shell
export ANDROID_VERSION=11
```

### 编译客户端

```shell
cd ~/ARMNative/vmi
./build_video.sh video_client
```

命令执行成功后，将在output目录生成CloudPhoneApk.tar.gz和已解压好的CloudPhone.apk

### 编译服务端

```shell
cd ~/ARMNative/vmi
./build_video.sh video_server
```

命令执行成功后，将在output目录生成DemoVideoEngine.tar.gz

### 编译二进制

```shell
cd ~/ARMNative/VMIEngine
./build.sh build VideoEngine
```

命令执行后，将在output/native/release_imgs/下生成VideoEngine.tar.gz