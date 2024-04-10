# 介绍
华为VMI引擎云手机开源项目

# 软件架构
请参考 [目录结构.md](/doc/目录结构.md) 文档

# 工程编译
## 编译机推荐配置
系统版本：Ubuntu 22.04.3 LTS

系统架构：x86_64

## 网络要求
保证服务器正常联网，以确保可以正常gradle编译和下载开源软件。

## 安装依赖库
```
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

## 执行脚本
如果想了解各脚本的具体实现细节，请参考 [编译脚本介绍.md](/doc/编译脚本介绍.md) 文档

### 安装编译环境
```
./scripts/auto_install_tools.sh ${安装目录}
source ~/.bashrc
```
其中，\${安装目录}可以自己指定，若不输入\${安装目录}，则脚本使用默认目录~/NativeCompileToolsDir

此脚本执行成功后，之后不需要再重复执行。

<a id="buildclient"></a>
### 编译客户端
```
./build_video.sh video_client
```
命令执行成功后，将在output目录生成CloudPhoneApk.tar.gz和已解压好的CloudPhone.apk

<a id="buildserver"></a>
### 编译服务端
```
./build_video.sh video_server
```
命令执行成功后，将在output目录生成DemoVideoEngine.tar.gz

# 使用视频流
请确认已完成 [编译客户端](#buildclient) 和 [编译服务端](#buildserver) 章节，并获取到CloudPhone.apk、CloudPhoneApk.tar.gz和DemoVideoEngine.tar.gz文件。

之后，请参考 《Kunpeng BoostKit 24.0.RC1 ARM原生视频流引擎 特性指南.docx》 2.2.2.2 制作视频流云手机镜像 以及之后的章节，使用云手机。