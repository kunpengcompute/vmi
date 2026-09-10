# 编译指南

本文介绍了云手机项目的编译环境搭建和编译流程，帮助开发者快速完成项目编译。

## 环境准备

### 编译服务器

系统版本：Ubuntu 22.04.3 LTS

系统架构：x86_64

使用自己的用户账号。

### 网络要求

保证服务器正常联网，以确保可以正常 gradle 编译和下载开源软件。

### 搭建 AOSP 环境

获取 [AOSP 源码](https://link.gitcode.com/?target=https%3A%2F%2Fandroid.googlesource.com%2Fplatform%2Fmanifest&from=https%3A%2F%2Fgitcode.com%2Ffuaniu%2FKbox%2Fblob%2FAOSP11%2Fdocs%2Fzh%2Fcompile_guide.md&lang=zh&theme=white)，上传至 `~/ARMNative` 目录下并解压。

```bash
cd ~/ARMNative
tar -xvf aosp11r48.tar
```

### 下载项目代码

下载本仓库。

```bash
git clone https://gitcode.com/boostkit/vmi.git
```

切换到 15 分支。

```bash
cd ~/ARMNative/vmi
git checkout CloudPhone15
```

### 安装编译环境

进入 vmi 源码目录下执行 auto_install_tools.sh，如果想了解各脚本的具体实现细节，请参考《[编译脚本介绍](compile_scripts_introduction.md)》文档。

```bash
cd ~/ARMNative/vmi
./scripts/auto_install_tools.sh ${安装目录}
source ~/.bashrc
```

其中，`${安装目录}` 可以自己指定，若不输入 `${安装目录}`，则脚本使用默认目录 `~/NativeCompileToolsDir`。

并在 `~/.bashrc` 中添加以下环境变量：

```bash
# android
export AN_AOSPDIR=/home/newdisk/XXX/ARMNative/aosp11 # 改成实际存放aosp11源码的绝对路径
export AN_AOSPOUT=${AN_AOSPDIR}/out
```

更新后 source 使其生效。

```bash
source ~/.bashrc
```

### 配置 ccache

配置 ccache 加快编译速度，查看本地 ccache 安装位置。

```bash
sudo apt install ccache
which ccache
```

在 `.bashrc` 中配置环境变量。

```bash
export NDK_CCACHE=ccache
export CCACHE_DIR=~/.ccache
export PATH={ccache安装位置}:$PATH # 例如export PATH=/usr/bin:$PATH
export USR_CCACHE=1
```

更新后 source 使其生效。

```bash
source ~/.bashrc
```

软链接所有的 gcc 和 g++ 到 ccache 上。

```bash
cd /usr/bin # ccache安装位置
ln -s ccache /usr/bin/gcc
ln -s ccache /usr/bin/g++
ln -s ccache /usr/bin/cc
ln -s ccache /usr/bin/c++
```

## 编译 AOSP 源码

### 安装依赖库

下载必要依赖，如果已安装，跳过即可。

```bash
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

进入安卓源码目录，执行如下编译指令。

```bash
cd ~/ARMNative/aosp11
source build/envsetup.sh
lunch aosp_arm64-eng
make -j【线程数】
```

## 编译视频流

编译前需设置如下环境变量，不建议写入 `~/.bashrc` 中，每次编译前设置即可。

```bash
# 视频流15的版本依然用aosp11去编译即可
export ANDROID_VERSION=11
```

### 编译客户端

```bash
cd ~/ARMNative/vmi
./build_video.sh video_client
```

命令执行成功后，将在 output 目录生成 `CloudPhoneApk.tar.gz` 和已解压好的 `CloudPhone.apk`。

### 编译服务端

```bash
cd ~/ARMNative/vmi
./build_video.sh video_server
```

命令执行成功后，将在 output 目录生成 `DemoVideoEngine.tar.gz`。

## 修订记录

|文档版本|发布日期|修改说明|
|--|--|--|
|01|2026-09-30|第一次正式发布|
