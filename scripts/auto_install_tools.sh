#!/bin/bash
if [ $# -ne 1 ]; then
    echo "Usage:$0 (deploy directory)"
fi

INSTALL_DIR=$1
DEF_INSTALL_DIR=~/NativeCompileToolsDir
ENV_BASHRC=~/.bashrc

:<<!
    函数功能: warnning级别日志输出
    入参：无
    返回值：
!
LogWarn()
{
    echo -e "\033[35m$@\033[0m"
}

:<<!
    函数功能: info级别日志输出
    入参：无
    返回值：
!
LogInfo()
{
    echo -e "\033[32m$@\033[0m"
}

:<<!
    函数功能: error级别日志输出
    入参：无
    返回值：
!
LogError()
{
    echo -e "\033[31m[ERROR] $@\033[0m"
}

function DownloadToolPkg()
{
    LogInfo "Start download packages"
    if [ ! -f "${INSTALL_DIR}/android-ndk-r25b-linux.zip" ]; then
        wget https://dl.google.com/android/repository/android-ndk-r25b-linux.zip
    fi
    if [ ! -f "${INSTALL_DIR}/build-tools_r33.0.1-linux.zip" ]; then
        wget https://dl.google.com/android/repository/build-tools_r33.0.1-linux.zip
    fi
    if [ ! -f "${INSTALL_DIR}/gradle-8.4-bin.zip" ]; then
        wget https://mirrors.cloud.tencent.com/gradle/gradle-8.4-bin.zip
    fi
    if [ ! -f "${INSTALL_DIR}/cmake-3.25.3-linux-x86_64.tar.gz" ]; then
        wget https://cmake.org/files/v3.25/cmake-3.25.3-linux-x86_64.tar.gz
    fi
    if [ ! -f "${INSTALL_DIR}/openjdk-11+28_linux-x64_bin.tar.gz" ]; then
        wget https://d6.injdk.cn/openjdk/openjdk/11/openjdk-11+28_linux-x64_bin.tar.gz
    fi
    if [ ! -f "${INSTALL_DIR}/ninja-linux.zip" ]; then
        wget https://github.com/ninja-build/ninja/releases/download/v1.11.0/ninja-linux.zip
    fi
    if [ ! -f "${INSTALL_DIR}/platform-33_r02.zip" ]; then
        wget https://dl.google.com/android/repository/platform-33_r02.zip
    fi
    if [ ! -f "${INSTALL_DIR}/platform-tools_r33.0.3-linux.zip" ]; then
        wget https://dl.google.com/android/repository/platform-tools_r33.0.3-linux.zip
    fi
    LogInfo "Finish download packages"
}

function UnpackTools()
{
    LogInfo "unpack Android NDK"
    mkdir -p ${INSTALL_DIR}/android-sdk-linux/ndk/25.1.8937393/
    unzip android-ndk-r25b-linux.zip
    mv android-ndk-r25b/* ${INSTALL_DIR}/android-sdk-linux/ndk/25.1.8937393/
    rm -r android-ndk-r25b/

    LogInfo "unpack Android SDK Build Tools"
    mkdir -p ${INSTALL_DIR}/android-sdk-linux/build-tools/33.0.1/
    unzip build-tools_r33.0.1-linux.zip
    mv android-13/* ${INSTALL_DIR}/android-sdk-linux/build-tools/33.0.1/
    rm -r android-13/

    LogInfo "unpack gradle"
    unzip gradle-8.4-bin.zip
    mv gradle-8.4/ ${INSTALL_DIR}/android-sdk-linux/

    LogInfo "unpack cmake"
    mkdir -p ${INSTALL_DIR}/android-sdk-linux/cmake/
    tar -zxvf cmake-3.25.3-linux-x86_64.tar.gz -C ${INSTALL_DIR}/android-sdk-linux/cmake/

    LogInfo "unpack AdoptOpenJDK"
    tar -zxvf openjdk-11+28_linux-x64_bin.tar.gz -C ${INSTALL_DIR}/android-sdk-linux/

    LogInfo "unpack ninja"
    unzip ninja-linux.zip
    mv ninja ${INSTALL_DIR}/android-sdk-linux/cmake/cmake-3.25.3-linux-x86_64/bin/

    LogInfo "unpack SDK platform"
    mkdir -p ${INSTALL_DIR}/android-sdk-linux/platforms/
    unzip platform-33_r02.zip
    mv android-13/ ${INSTALL_DIR}/android-sdk-linux/platforms/android-33/

    LogInfo "unpack SDK platform tools"
    unzip platform-tools_r33.0.3-linux.zip
    mv platform-tools/ ${INSTALL_DIR}/android-sdk-linux/
}

function DelOldEnv()
{
    if [ ! -f "${ENV_BASHRC}" ]; then
        Logwarn "找不到${ENV_BASHRC}"
        return 1
    fi
    
    local lineSt=$(sed -n "/#tools_env_start:/=" ${ENV_BASHRC})
    local lineEnd=$(sed -n "/#tools_env_end:/=" ${ENV_BASHRC})
    if [[ "x${lineSt}" == "x" ]] || [[ "x${lineEnd}" == "x" ]]; then
        return 0
    else
        if [ ${lineSt} -gt ${lineEnd} ]; then
            Logwarn "自动部署工具起始行在结束行前面，请解决冲突"
	        return 1
        else
            sed -i "${lineSt},${lineEnd}d" ${ENV_BASHRC}
        fi
    fi
    return 0
}

function CheckEnvConflict()
{
    local lineSt=$(sed -n "/#tools_env_start:/=" ${ENV_BASHRC})
    local lineEnd=$(sed -n "/#tools_env_end:/=" ${ENV_BASHRC})
    local envList="USER_LOCAL_PATH
        JAVA_HOME
        AN_JDKPATH
        AN_JAVADIR
        CLASSPATH
        AN_SDKDIR
        AN_NDKDIR
        GRADLE_HOME
        AN_GRADLEDIR
        CMAKE_PATH
        ANDROID_NDK_HOME
        ANDROID_NDK
    "

    if [ ! -f "${ENV_BASHRC}" ]; then
        Logwarn "找不到${ENV_BASHRC}"
        return 1
    fi
    
    intlineSt=$(expr ${lineSt} + 0)
    intlineEnd=$(expr ${lineEnd} + 0)
    for envEle in ${envList[*]}
    do
        xStr=$(sed -n "/${envEle}=/=" ${ENV_BASHRC})
        if [ "x${xStr}" != "x" ]; then
            for xstrSub in ${xStr}
            do
                intxStr=$(expr ${xstrSub} + 0)
                if [ ${intxStr} -lt ${intlineEnd} ]; then
                    if [ ${intxStr} -gt ${intlineSt} ]; then
                        continue
                    fi
                fi
                LogInfo "环境变量冲突: ${intxStr}行 ${envEle} 已被设置，请解决冲突"
                return 1
            done
        fi
    done
    return 0
}

function SetEnvConfig()
{
    local toolsPath=${INSTALL_DIR}
        toolsPath=$(realpath ${toolsPath})
cat >> ${ENV_BASHRC}  <<EOF
#tools_env_start:
export USER_LOCAL_PATH=${toolsPath}
export JAVA_HOME=\${USER_LOCAL_PATH}/android-sdk-linux/jdk-11
export AN_JDKPATH=\${JAVA_HOME}
export AN_JAVADIR=\${AN_JDKPATH}
export CLASSPATH=\${JAVA_HOME}/lib:\$CLASSPATH
export AN_SDKDIR=\${USER_LOCAL_PATH}/android-sdk-linux
export AN_NDKDIR=\${AN_SDKDIR}/ndk/25.1.8937393
export GRADLE_HOME=\${AN_SDKDIR}/gradle-8.4
export AN_GRADLEDIR=\${GRADLE_HOME}
export CMAKE_PATH=\${USER_LOCAL_PATH}/android-sdk-linux/cmake/cmake-3.25.3-linux-x86_64/bin
export ANDROID_NDK_HOME=\${AN_NDKDIR}
export ANDROID_NDK=\${AN_NDKDIR}
export PATH=\${JAVA_HOME}/bin:\${AN_NDKDIR}:\${AN_SDKDIR}/platform-tools:\${GRADLE_HOME}/bin:\${CMAKE_PATH}:\$PATH
#tools_env_end:
EOF
}

function main()
{
    if [ -z "${INSTALL_DIR}" ];then
        LogInfo "未输入安装路径，使用默认路径${DEF_INSTALL_DIR}"
        INSTALL_DIR=${DEF_INSTALL_DIR}
    fi

    mkdir -p ${INSTALL_DIR}
    cd ${INSTALL_DIR}
    DownloadToolPkg
    UnpackTools
    CheckEnvConflict
    [ $? != 0 ] && exit 0
    DelOldEnv
    [ $? != 0 ] && exit 0
    SetEnvConfig
    cd -
}

main
