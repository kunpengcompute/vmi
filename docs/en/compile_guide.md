# Compilation Guide

<!-- md-trans-meta sourceCommit=5bf1870ee699806369ea9a362c9b91057ff285be translatedAt=2026-09-08T02:25:29.742Z pushedAt=2026-09-08T03:28:25.049Z -->

This document describes how to set up the compilation environment for the cloud phone project, as well as the compilation process, helping developers complete project compilation quickly.

## Environment Preparation

### Compilation Server

System version: Ubuntu 22.04.3 LTS

System architecture: x86_64

Use your own user account.

### Network Requirement

Ensure that the server is properly connected to the network so that Gradle compilation and open-source software download can be performed properly.

### Setting Up the AOSP Environment

Download the [AOSP source code](https://link.gitcode.com/?target=https%3A%2F%2Fandroid.googlesource.com%2Fplatform%2Fmanifest&from=https%3A%2F%2Fgitcode.com%2Ffuaniu%2FKbox%2Fblob%2FAOSP11%2Fdocs%2Fzh%2Fcompile_guide.md&lang=zh&theme=white), upload it to the `~/ARMNative` directory, and decompress it.

```bash
cd ~/ARMNative
tar -xvf aosp11r48.tar
```

### Downloading Project Code

Download this repository.

```bash
git clone https://gitcode.com/boostkit/vmi.git
```

Switch to the `CloudPhone15` branch.

```bash
cd ~/ARMNative/vmi
git checkout CloudPhone15
```

### Installing the Compilation Environment

Go to the VMI source code directory and run the `auto_install_tools.sh` script. For details about the implementation of each script, see [Introduction to Compilation Scripts](compile_scripts_introduction.md).

```bash
cd ~/ARMNative/vmi
./scripts/auto_install_tools.sh ${installation_directory}
source ~/.bashrc 
```

You can specify ${installation_directory}. If you do not specify it, the script uses the default directory `~/NativeCompileToolsDir`.

Add the following environment variables to the `~/.bashrc` file:

```bash
# android
export AN_AOSPDIR=/home/newdisk/XXX/ARMNative/aosp11 # Change it to the absolute path to the AOSP 11 source code.
export AN_AOSPOUT=${AN_AOSPDIR}/out
```

Run the `source` command to make the update take effect.

```bash
source ~/.bashrc
```

### Configuring ccache

You can configure ccache to accelerate the compilation. Check the local ccache installation location.

```bash
sudo apt install ccache
which ccache
```

Configure environment variables in `.bashrc`.

```bash
export NDK_CCACHE=ccache
export CCACHE_DIR=~/.ccache
export PATH={ccache_installation_location}:$PATH # For example, export PATH=/usr/bin:$PATH
export USR_CCACHE=1
```

Run the `source` command to make the update take effect.

```bash
source ~/.bashrc
```

Create soft links for all GCC and G++ executables to ccache.

```bash
cd /usr/bin # ccache installation location
ln -s ccache /usr/bin/gcc
ln -s ccache /usr/bin/g++
ln -s ccache /usr/bin/cc
ln -s ccache /usr/bin/c++
```

## AOSP Source Code Compilation

### Installing Dependencies

Download necessary dependencies. If they have been installed, skip this step.

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

### Compiling Source Code

Go to the Android source code directory and run the following build commands.

```bash
cd ~/ARMNative/aosp11
source build/envsetup.sh
lunch aosp_arm64-eng
make -j [Number_of_threads]
```

## Video Stream Compilation

Before compilation, set the following environment variable. You are not advised to write it into `~/.bashrc`. Instead, set it upon each compilation.

```bash
# The Android 15 version of the video stream engine can be compiled using AOSP 11.
export ANDROID_VERSION=11
```

### Compiling the Client

```bash
cd ~/ARMNative/vmi
./build_video.sh video_client
```

After the command execution succeeds, `CloudPhoneApk.tar.gz` and the extracted `CloudPhone.apk` are generated in the `output` directory.

### Compiling the Server

```bash
cd ~/ARMNative/vmi
./build_video.sh video_server
```

After the command execution succeeds, the `DemoVideoEngine.tar.gz` package is generated in the `output` directory.
