# 开发指南

## 介绍

云手机是指虚拟出带有AOSP （Android Open Source Project）系统，具有虚拟手机功能的云服务器。作为一种新型应用，它对物理手机起到了有效的延伸和扩展作用，可以用在云手游、移动办公等诸多场景。

端云引擎顾名思义可以分为端侧和云侧两个部分：云侧运行于服务器上；端侧一般为云手机APK，可以被安装在用户的Android手机上，用于和云侧进行交互，进而对云侧容器进行正常的操作。

本文档主要用于描述视频流云手机端侧和云侧引擎的集成开发指南。

## 获取软件包

### 软件要求

|序号|软件|说明|获取地址|
|--|--|--|--|
|1|AOSP源码|版本：android-15.0.0_r17|[获取链接](https://android.googlesource.com/platform/manifest)|
|2|Android NDK|版本：r25b|[获取链接](https://dl.google.com/android/repository/android-ndk-r25b-linux.zip)|
|3|SDK platform|版本：33_r02|[获取链接](https://dl.google.com/android/repository/platform-33_r02.zip)|
|4|SDK platform tools|版本：r33.0.3|[获取链接](https://dl.google.com/android/repository/platform-tools_r33.0.3-linux.zip)|
|5|SDK build tools|版本：r33.0.1|[获取链接](https://dl.google.com/android/repository/build-tools_r33.0.1-linux.zip)|
|6|BoostKit-boostcph-videoengine_*_15.zip|Android 15视频流引擎开发包|[获取链接](https://www.hikunpeng.com/boostkit/arm-native?application=%E8%A7%86%E9%A2%91%E6%B5%81%E5%BC%95%E6%93%8E#application-soft)|

### 软件包完整性校验

为了防止软件包在传递过程或存储期间被恶意篡改，从鲲鹏社区获取软件包时需下载对应的数字签名文件用于完整性验证。

1. 请参见[软件要求](#软件要求)获取软件包。
2. 从[华为企业业务网站](https://support.huawei.com/enterprise/zh/tool/pgp-verify-TL1000000054)或[运营商网站](http://support.huawei.com/carrier/digitalSignatureAction)获取校验工具和校验方法。
3. 使用第2步获取的签名验证指南文档对下载的软件包进行 PGP 数字签名校验。

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 如果校验失败，请不要使用该软件包，先联系华为技术支持工程师解决。
> 使用软件包安装/升级之前，也需要按上述过程先验证软件包的数字签名，确保软件包未被篡改。
> 使用软件包前请先阅读《[鲲鹏应用使能套件 BoostKit 用户许可协议 2.0](https://www.hikunpeng.com/zh/legal/developer/boostkit/software/protocol)》，如确认继续使用，则默认同意协议的条款和条件。

## 配置开发环境

### 软件包分解

视频流引擎的软件包名称为 BoostKit-boostcph-videoengine_*_15.zip，获取方式请参见[获取软件包](#获取软件包)，获取到软件 zip 包后，将 zip 包解压获得 .tar.gz 开发包。

|软件|说明|
|--|--|
| VideoEngine.tar.gz | 视频流引擎服务端开发包 |

## 服务端编译环境部署

### 环境要求

本文基于 x86_64 服务器和 Ubuntu 22.04 LTS 操作系统进行服务端的编译，编译前请确保您的硬件环境满足要求。

服务端编译构建的硬件环境要求如[服务端编译构建硬件环境要求](#服务端编译构建硬件环境要求)所示。

#### 服务端编译构建硬件环境要求

|设备型号|用途|服务器 OS 版本|
|--|--|--|
|x86_64 服务器|服务端编译制作|Ubuntu 22.04 LTS 推荐：[ubuntu-22.04-live-server-amd64.iso](https://old-releases.ubuntu.com/releases/22.04/ubuntu-22.04-live-server-amd64.iso)|

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> - 本文档测试服务器型号为 2288H V5。
> - 服务器需有访问外网权限，以方便下载 OS 镜像。

**部署步骤**

1. 安装依赖库。

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

2. 下载 vmi 代码，进入 vmi 代码目录。

    ```bash
    git clone https://gitee.com/kunpengcompute/vmi.git
    cd vmi
    ```

3. 使用自动部署脚本，部署编译工具。

    ```bash
    chmod +x scripts/auto_install_tools.sh
    ./scripts/auto_install_tools.sh ${安装目录}
    ```

    其中，{安装目录} 为下载和解压编译工具的目录，可由用户自己指定。如果不输入 {安装目录}，则脚本会使用默认目录："~/NativeCompileToolsDir"。

    脚本执行成功后，使用以下命令引入环境变量。

    ```bash
    source ~/.bashrc
    ```

### 服务端调试环境部署

> [!WARNING]注意
>
> - 用户可以根据需求选择文件存放目录。
> - 服务端调试环境的硬件要求及部署过程可以请参见《[视频流引擎 安装指南（Android 15）](https://www.hikunpeng.com/document/detail/zh/kunpengcps/boostcph/videostreamengine_ad15/docs/zh/install_guide.md)》。

1. 将 VideoEngine.tar.gz 放至 "/home/VideoEngine/Cloud" 目录下。
2. 解压视频流引擎服务端开发包。

    ```bash
    tar xzvf VideoEngine.tar.gz
    ```

3. 再将解压出来的二进制文件复制至云手机（以_cloudphone_1_云手机为例）内。

    ```bash
    docker cp system/bin cloudphone_1:/system/
    docker cp system/etc cloudphone_1:/system/
    docker cp system/lib cloudphone_1:/system/
    docker cp system/lib64 cloudphone_1:/system/
    docker cp vendor/lib cloudphone_1:/vendor/
    docker cp vendor/lib64 cloudphone_1:/vendor/
    docker cp vendor/etc cloudphone_1:/vendor/
    ```

4. 复制视频流云手机对外依赖库到云手机（以_cloudphone_1_云手机为例）内，需要提供的依赖库列表如[视频流云手机对外依赖库](#视频流云手机对外依赖库)所示。

#### 视频流云手机对外依赖库

    |描述|库名|路径|
    |--|--|--|
    |对外依赖的编码库|libVideoCodec.so|/vendor/lib、/vendor/lib64|
    |对外依赖的解码库|libVideoDecoder.so|/vendor/lib、/vendor/lib64|
    |对外依赖的GPU加速库|libVmiEncTurbo.so|/vendor/lib、/vendor/lib64|
    |对外依赖的GPU加速库（system权限）|libVmiEncTurboSys.so|/system/lib、/system/lib64|
    |Opus编解码库|libVmiOpus.so|/system/lib、/system/lib64|

5. 重启云手机，此时视频流云手机可运行，请参见后续开发流程调用对应的接口即可调试对应功能。

## 对外接口

### 使用约定

对外接口提供的动态链接库是使用 AOSP 15.0.0_r17 源码编译，这些库需在 AOSP 15.0.0_r17 环境中使用。

对外接口是通用接口，即所有的组件模块都使用相同接口交换数据，各种数据类型的定义参照本文对应章节目录描述。如：视频输出请参见[视频输出开发](#视频输出开发)。

### 基础数据类型

#### VmiDataType 模块数据类型

VmiDataType 定义支持的数据类型，各个数据类型对应一个组件模块，当前支持的组件模块包括：视频输出、音频输出、麦克风输入、触控输入、传感器、GPS，数据类型具体定义如下：

```c++
enum VmiDataType : uint8_t {
    DATA_VIDEO = 0,                                  // 码流出流组件
    DATA_AUDIO,                                      // 音频播放组件
    DATA_TOUCH,                                      // 触控和按键组件
    DATA_MIC,                                        // 麦克风组件
    DATA_SENSOR,                                     // 传感器组件
    DATA_GPS,                                        // GPS组件
    DATA_TYPE_MAX
};
```

#### VmiModuleStatus 模块状态

组件模块状态定义如下：

```c++
enum VmiModuleStatus : uint32_t {
    MODULE_NOT_INIT = 0,                             // 模块未初始化
    MODULE_INITED,                                   // 模块已初始化
    MODULE_STARTED,                                  // 模块已启动
    MODULE_STOPED,                                   // 模块已停止
};
```

#### VmiVersion 模块版本号

组件模块的版本号由模块数据类型和当前模块小版本号两部分组成，因此通过版本号可以支持模块级别的版本控制。具体定义如下：

```c++
enum VmiVideoVersion : uint16_t {
    CUR_VIDEO_VERSION = 7,
};
enum VmiAudioVersion : uint16_t {
    CUR_AUDIO_VERSION = 7,
};
enum VmiTouchVersion : uint16_t {
    CUR_TOUCH_VERSION = 7,
};
enum VmiMicVersion : uint16_t {
    CUR_MIC_VERSION = 7,
};
enum VmiSensorVersion : uint16_t {
    CUR_SENSOR_VERSION = 7,
};
enum VmiGpsVersion : uint16_t {
    CUR_GPS_VERSION = 7,
};
#define MAKE_VERSION(dataType, version) ((dataType) << 24 | (version))
enum VmiVersion : uint32_t {
    VIDEO_CUR_VERSION = MAKE_VERSION(DATA_VIDEO, CUR_VIDEO_VERSION),
    AUDIO_CUR_VERSION = MAKE_VERSION(DATA_AUDIO, CUR_AUDIO_VERSION),
    TOUCH_CUR_VERSION = MAKE_VERSION(DATA_TOUCH, CUR_TOUCH_VERSION),
    MIC_CUR_VERSION = MAKE_VERSION(DATA_MIC, CUR_MIC_VERSION),
    SENSOR_CUR_VERSION = MAKE_VERSION(DATA_SENSOR, CUR_SENSOR_VERSION),
    GPS_CUR_VERSION = MAKE_VERSION(DATA_GPS, CUR_GPS_VERSION),
};
```

#### VmiErrCode 错误码定义

接口返回的错误码定义如下：

```c++
enum VmiErrCode : int32_t {
    OK = 0,
    ERR_VERSION_MISMATCH = 0x1,                      // 版本不兼容
    ERR_ILLEGAL_INPUT,                               // 输入参数不合法
    ERR_INVALID_CMD,                                 // 不支持的命令
    ERR_INVALID_STATUS,                              // 当前组件状态不支持该操作
    ERR_INVALID_MODULE,                              // 不支持的组件类型
    ERR_NOT_ENOUGH_RESOURCE,                         // 资源不足，无法完成操作
    ERR_INTERNAL,                                    // 组件内部错误
    ERR_REPEAT_INITIALIZATION,                       // 重复初始化
    ERR_MODULE_NOT_INIT,                             // 模块未初始化
    ERR_MODULE_NOT_START,                            // 模块未启动
    ERR_MODULE_REPEAT_START,                         // 模块重复启动
    ERR_MODULE_REPEAT_STOP,                          // 模块重复停止
    ERR_COMMON_MAX = 0x100,
    ERR_VIDEO_UNSUPPORT_OPERATION = 0x101,
    ERR_VIDEO_PARAMS_SETTING = 0x102,
    ERR_VIDEO_MAX = 0x200,
    ERR_AUDIO_MAX = 0x300,
    ERR_MIC_MAX = 0x400,
    ERR_TOUCH_MAX = 0x500,
    ERR_SENSOR_MAX = 0x600
};
```

#### VmiCmd 命令字定义

模块数据输入、数据输出、参数设置等功能均需要指定命令字作为接口的入参。命令字是一个枚举值为 uint32_t 类型的枚举类型。每个 32 位的命令字枚举值由三部分按顺序拼接而成，分别为 VmiDataType 模块数据类型 uint8_t 枚举值、VmiCmdType 命令字类型 uint8_t 枚举值、模块具体命令字（VmiVideoCmdId、VmiAudioCmdId 等）uint16_t 枚举值。具体定义如下：

```c++
enum VmiCmdType : uint8_t {
    CMD_SET_PARAM = 0,                               // 设置参数类型命令
    CMD_GET_PARAM,                                   // 获取参数类型命令
    CMD_TRANS_DATA,                                  // 传输数据类型命令
};
enum VmiVideoCmdId : uint16_t {
    SET_ENCODER_PARAM = 0,                           // 客户端设置编码参数到服务端
    RETURN_VIDEO_DATA,                               // 服务端返回视频数据到客户端
    GET_ENCODER_PARAM,                               // 获取视频模块的编码参数
};
enum VmiAudioCmdId : uint16_t {
    SET_AUDIOPLAY_PARAM = 0,                         // 客户端设置音频播放参数到服务端
    RETURN_AUDIO_PLAY_DATA,                          // 服务端发送音频播放数据到客户端
    SET_CLIENT_VOLUME,                               // 服务端发送扬声器大小到客户端
    GET_AUDIOPLAY_PARAM,                             // 客户端从服务端获取音频播放参数
};
enum VmiMicCmdId : uint16_t {
    SEND_MIC_DATA = 0,                               // 客户端发送麦克风数据到服务端
    RETURN_OPEN_CLIENT_MIC,                          // 服务端通知客户端麦克风启动
    RETURN_CLOSE_CLIENT_MIC,                         // 服务端客户端麦克风关闭
};
enum VmiTouchCmdId : uint16_t {
    SEND_TOUCH_EVENT = 0,                            // 客户端发送触控数据到服务端
    SEND_KEY_EVENT,                                  // 客户端发送按键数据到服务端
};
enum VmiSensorCmdId : uint16_t {
    SEND_SENSOR_DATA = 0,                            // 客户端发送传感器数据到服务端
    RETURN_REGISTER_CLIENT_SENSOR,                   // 服务端通知客户端注册传感器侦听
    RETURN_UNREGISTER_CLIENT_SENSOR,                 // 服务端通知客户端取消传感器侦听
    RETURN_UPDATE_CLIENT_SENSOR_RATE,                // 服务端通知客户端更新传感器采集率
};
enum VmiGpsCmdId : uint16_t {
    SEND_LOCATION_DATA = 0,                            // 客户端发送gps location到服务端
    SEND_NMEA_DATA,                                    // 客户端发送nmea到服务端
    RETURN_START_GPS,                                  // 服务端通知客户端启动GPS数据发送
    RETURN_STOP_GPS,                                   // 服务端通知客户端停止GPS数据发送
};
#define MAKE_CMD(dataType, cmdType, cmdId)  ((dataType) << 24 | (cmdType) << 16 | (cmdId))
enum VmiCmd : uint32_t {
    // 视频模块的cmd定义
    VIDEO_SET_ENCODER_PARAM = MAKE_CMD(DATA_VIDEO, CMD_SET_PARAM, SET_ENCODER_PARAM),      // 参数数据格式参见：EncodeParams
    VIDEO_RETURN_VIDEO_DATA = MAKE_CMD(DATA_VIDEO, CMD_TRANS_DATA, RETURN_VIDEO_DATA),     // 传输数据格式参见：VideoData
    VIDEO_GET_ENCODER_PARAM = MAKE_CMD(DATA_VIDEO, CMD_GET_PARAM, GET_ENCODER_PARAM),      // 参数数据格式参见：EncodeParams
    // 音频模块的cmd定义
    AUDIO_SET_AUDIOPLAY_PARAM = MAKE_CMD(DATA_AUDIO, CMD_SET_PARAM, SET_AUDIOPLAY_PARAM),  // 参数数据格式参见：AudioPlayParams
    AUDIO_GET_AUDIOPLAY_PARAM = MAKE_CMD(DATA_AUDIO, CMD_GET_PARAM, GET_AUDIOPLAY_PARAM),
    AUDIO_RETURN_AUDIO_PLAY_DATA = MAKE_CMD(DATA_AUDIO, CMD_TRANS_DATA, RETURN_AUDIO_PLAY_DATA),  // 传输数据格式参见：AudioData
    AUDIO_SET_CLIENT_VOLUME = MAKE_CMD(DATA_AUDIO, CMD_SET_PARAM, SET_CLIENT_VOLUME),
    // 麦克风模块的cmd定义
    MIC_SEND_MIC_DATA = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, SEND_MIC_DATA),
    MIC_RETURN_OPEN_CLIENT_MIC = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, RETURN_OPEN_CLIENT_MIC),
    MIC_RETURN_CLOSE_CLIENT_MIC = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, RETURN_CLOSE_CLIENT_MIC),
    // 触控模块的cmd定义
    TOUCH_SEND_TOUCH_EVENT = MAKE_CMD(DATA_TOUCH, CMD_TRANS_DATA, SEND_TOUCH_EVENT),    // 传输数据格式参见：VmiTouchInputData
    TOUCH_SEND_KEY_EVENT = MAKE_CMD(DATA_TOUCH, CMD_TRANS_DATA, SEND_KEY_EVENT),        // 传输数据格式参见：VmiKeyInputData
    // 传感器模块的cmd定义
    SENSOR_SEND_SENSOR_DATA = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, SEND_SENSOR_DATA),  // 传输数据格式参见：SensorData
    SENSOR_RETURN_REGISTER_CLIENT_SENSOR = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, RETURN_REGISTER_CLIENT_SENSOR),
    SENSOR_RETURN_UNREGISTER_CLIENT_SENSOR = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, RETURN_UNREGISTER_CLIENT_SENSOR),
    SENSOR_RETURN_UPDATE_CLIENT_SENSOR_RATE = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, RETURN_UPDATE_CLIENT_SENSOR_RATE),
    // GPS模块的cmd定义
    GPS_SEND_LOCATION_DATA = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, SEND_LOCATION_DATA), // 传输数据格式参见：VmiGPSLocationData
    GPS_SEND_NMEA_DATA = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, SEND_NMEA_DATA),         // 传输数据格式参见：VmiGPSLocationData
    GPS_RETURN_START_GPS = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, RETURN_START_GPS),
    GPS_RETURN_STOP_GPS = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, RETURN_STOP_GPS),
};
```

### 函数接口

#### GetVersion

**函数功能**

获取产品版本号。

**函数原型**

`const char *GetVersion()`

**返回值说明**

数据类型：`char *`

返回值示例请参见《[视频流引擎 用户指南（Android 15）](https://www.hikunpeng.com/document/detail/zh/kunpengcps/boostcph/videostreamengine_ad15/docs/zh/user_guide.md#d12-%E6%9F%A5%E8%AF%A2%E7%BB%84%E4%BB%B6%E7%89%88%E6%9C%AC%E5%8F%B7%E4%BF%A1%E6%81%AF)》中的"查询组件版本号信息"章节。

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 回显示例中的版本号与 VmiVersion 模块版本号无关。

#### InitVmiEngine

**函数功能**

初始化云手机服务端。

**约束说明**

- 由于不支持反复调用，因此初始化成功后不允许再次调用。
- 不允许长时间阻塞数据输出的回调函数，建议回调函数需在 1ms 内返回。

**函数原型**

`VmiErrCode InitVmiEngine(VmiConfigEngine *config);`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| config | 输入 | VmiConfigEngine | 该结构体由DataCallback和DataTypeConfig数组构成 |

启动引擎需要的配置项为 VmiConfigEngine，包含 DataCallback 数据回调和需要启动的模块列表。

```c++
struct VmiConfigEngine {
    DataCallback dataCallback = nullptr;        // 用于发送服务端数据的回调
    DataTypeConfig dataTypeConfig[DATA_TYPE_MAX];
} __attribute__((packed));
```

数据输出统一使用回调函数实现，不提供数据输出的函数接口。回调函数定义如下：

```c++
using DataCallback = int(*)(VmiDataType module, VmiCmd cmd, uint8_t *data, uint32_t size);
```

DataTypeConfig 数组指明每个模块是否需要初始化，以及初始化时数据包头前需要预留的内存空间大小，便于开发者填充需要的数据。

```c++
struct DataTypeConfig {
    bool shouldInit = false;
    uint32_t sendDataOffset = 0;                // 通过回调函数发送数据时，在数据头预留的空间，最小值是0，最大值是1024，单位为Byte
} __attribute__((packed));
```

若 sendDataOffset 不为 0，则引擎调用 dataCallback 接口时，接口的第三个参数 data 指针的内存排布为：sendDataOffset 字节的预留数据+size 字节的有效数据，第四个参数 size 指示有效数据的字节数。

**返回值说明**

数据类型：`enum VmiErrCode : int32_t`

取值如下：

- OK（0）：表示初始化成功。
- 非 0 值：表示初始化失败，返回值指示错误码。

#### DeInitVmiEngine

**函数功能**

反初始化云手机服务端。

**约束说明**

不支持反复调用。

**函数原型**

`VmiErrCode DeInitVmiEngine();`

**返回值说明**

数据类型：`enum VmiErrCode : int32_t`

取值如下：

- OK（0）：表示反初始化成功。
- 非 0 值：表示反初始化失败，返回值指示错误码。

#### GetStatus

**函数功能**

获取模块状态。

**函数原型**

`VmiModuleStatus GetStatus(VmiDataType module)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| module | 输入 | VmiDataType | 模块数据类型名 |

**返回值说明**

数据类型：`enum VmiModuleStatus : uint32_t`

#### StartModule

**函数功能**

启动模块。

**函数原型**

`VmiErrCode StartModule(VmiDataType module, uint8_t* config, uint32_t size)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| module | 输入 | VmiDataType | 模块数据类型名 |
| config | 输入 | uint8_t* | 启动模块时使用的配置 |
| size | 输入 | uint32_t | config 所指向的内存长度 |

不同 module 的 config 不同，并且都需要继承自 VmiConfig，其中，视频参考 VmiConfigVideo，音频播放参考 VmiConfigAudio，触控参考 VmiConfigTouch，麦克风参考 VmiConfigMic，sensor 参考 VmiConfig，GPS 参考 VmiConfig。

```c++
struct VmiConfig {
    uint32_t version;                           // 设置为各个组件的当前的版本号，参见：VmiVersion结构体
} __attribute__((packed));
```

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> VmiConfig 需要传入[VmiVersion 模块版本号](#vmiversion-模块版本号)中提到的模块版本号，该版本号每个模块都不同。

**返回值说明**

数据类型：`enum VmiErrCode : int32_t`

取值如下：

- OK（0）：表示启动模块成功。
- 非 0 值：表示启动模块失败，返回值指示错误码。

#### StopModule

**函数功能**

停止模块。

**函数原型**

`VmiErrCode StopModule(VmiDataType module)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| module | 输入 | VmiDataType | 模块数据类型名 |

**返回值说明**

数据类型：`enum VmiErrCode : int32_t`

取值如下：

- OK（0）：表示停止模块成功。
- 非 0 值：表示停止模块失败，返回值指示错误码。

#### InjectData

**函数功能**

注入数据。

**函数原型**

`VmiErrCode InjectData(VmiDataType module, VmiCmd cmd, uint8_t *data, uint32_t size)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| module | 输入 | VmiDataType | 模块数据类型名 |
| cmd | 输入 | VmiCmd | 注入数据时的操作指令 |
| data | 输入 | uint8_t* | 指向待发送数据的指针 |
| size | 输入 | uint32_t | 注入数据的大小 |

**返回值说明**

数据类型：`enum VmiErrCode : int32_t`

取值如下：

- OK（0）：表示注入数据成功。
- 非 0 值：表示注入数据失败，返回值指示错误码。

#### SetParam

**函数功能**

设置参数。

**函数原型**

`VmiErrCode SetParam(VmiDataType module, VmiCmd cmd, uint8_t *param, uint32_t size)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| module | 输入 | VmiDataType | 模块数据类型名 |
| cmd | 输入 | VmiCmd | 设置参数的操作指令 |
| param | 输入 | uint8_t* | 设置参数的数据指针 |
| size | 输入 | uint32_t | 设置数据的大小 |

**返回值说明**

数据类型：`enum VmiErrCode : int32_t`

取值如下：

- OK（0）：表示设置成功。
- 非 0 值：表示设置参数失败，返回值指示错误码。

#### GetParam

**函数功能**

获取模块的参数值。

**函数原型**

`VmiErrCode GetParam(VmiDataType module, VmiCmd cmd, uint8_t *param, uint32_t size)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| module | 输入 | VmiDataType | 模块数据类型名 |
| cmd | 输入 | VmiCmd | 获取参数的操作指令 |
| param | 输入 | uint8_t* | 输出参数的数据指针 |
| size | 输入 | uint32_t | 输出数据的大小 |

**返回值说明**

数据类型：`enum VmiErrCode : int32_t`

取值如下：

- OK（0）：获取参数成功。
- 非 0 值：表示获取参数失败，返回值指示错误码。

## 视频输出开发

### 使用约定

视频输出配置 VmiConfigVideo 继承自 VmiConfig 结构体。具体结构如下：

```c++
struct VmiConfigVideo : public VmiConfig {
    EncoderType encoderType = CPU;              // 编码器类型
    VideoFrameType videoFrameType = H264;       // 视频输出格式
    FrameSize resolution;                       // 抓图分辨率
    uint32_t density = 320;                     // 屏幕密度
    bool renderOptimize = true;                 // 出流优化，默认开启，暂不支持关闭
    EncodeParams encodeParams;                  // 初始编码参数
} __attribute__((packed));
```

### 配置参数

#### encoderType

**变量说明**

编码器类型。

**变量数据定义**

```c++
enum EncoderType : uint32_t {
    CPU,                                        // 软编
    VPU,                                        // 编码卡硬件加速
    GPU,                                        // GPU硬件加速
    ENCODE_TYPE_MAX
};
```

**约束说明**

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
|encoderType|编码器类型|0：CPU（CPU 软编码器编码）1：VPU（外置硬件编码器编码）2：GPU（仅 DC1000/DC1000C 支持）|0，CPU 编码|

#### videoFrameType

**变量说明**

视频输出格式。

**变量数据定义**

```c++
enum VideoFrameType {
    H264,　　　　　　　　　　　// 默认输出H264
    YUV,                       // YV12
    RGB,                       // RGBA8888，暂不支持
    H265,
    FRAME_TYPE_MAX
};
```

**约束说明**

RGB 中 RGBA8888 暂不支持。

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
| videoFrameType | 视频数据输出格式 | 0：H264 1：YUV（encoderType 取值为 0 时才支持）2：RGB（暂不支持）3：H265（encoderType 取值为 0 时不支持） | 0：H264 |

#### resolution

**变量说明**

抓图分辨率。

**变量数据定义**

```c++
struct FrameSize {
    uint32_t width = 720;               // 720: 默认720P
    uint32_t height = 1280;             // 1280: 默认720P
    uint32_t widthAligned = 720;        // 720: 默认不做对齐
    uint32_t heightAligned = 1280;      // 1280: 默认不做对齐
} __attribute__((packed));
```

**约束说明**

width 和 height 的取值范围也支持互换。

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
| width | 自适应分辨率宽度（必须是 8 的倍数） | 360~2160 | 720 |
| height | 自适应分辨率高度（必须是 8 的倍数） | 360~3840 | 1280 |
| widthAligned | 对齐后分辨率宽度（暂不支持配置） | 360~2160 | 720 |
| heightAligned | 对齐后分辨率高度（暂不支持配置） | 360~3840 | 1280 |

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 在启用自适应分辨率功能时，改变视频输出分辨率（与上次启动时配置不同）时，会改变 AOSP 系统和应用的渲染分辨率，可能会导致部分应用出现兼容性问题或渲染问题。一般此类问题可以通过重新启动应用解决，因此建议在修改分辨率前返回桌面，同时清空后台应用，以提升用户使用体验。

#### density

**变量说明**

屏幕密度。

**变量数据定义**

uint32_t 类型，默认值为 320。

**约束说明**

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
| density | 屏幕密度 | [120~960]：包括 120 与 960 边界值。0：表示不启用自适应分辨率功能 | 320 |

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 1. 当 density 属性设置为 0 时，意味着不启用自适应分辨率功能，既启动视频模块时会使用云手机的现有渲染分辨率和屏幕密度进行渲染与出流。
> 2. 当 Android 属性（ro.vmi.video.wmcmd）设置为 0 时，同样不会启用自适应分辨率功能；该属性与 density 变量任意一个设置为 0，都将不启用自适应分辨率功能。

#### renderOptimize

**变量说明**

渲染优化标志。开启后可降低系统渲染负载。

**约束说明**

暂不支持关闭。

**变量数据定义**

bool 类型，默认置 true，默认进行渲染优化。

#### encodeParams

**变量说明**

编码参数。

**变量数据定义**

```c++
struct EncodeParams {
    uint32_t bitrate = 3000000;             // 码率
    uint32_t gopSize = 30;                  // I帧间隔
    ProfileType profile = BASELINE;         // 编码复杂度
    RCMode rcMode = CBR;                    // 流控模式
    uint32_t forceKeyFrame = 0;             // 在设置后第N帧强制生成I帧，0表示不生效
    bool interpolation = false;             // 补帧开关
    uint32_t crf = 34;                      // crf码控级别
    uint32_t maxCrfRate = 20000000;         // crf码率峰值
    int32_t vbvBufferSize = 1000;           // crf码率缓冲区大小
    uint32_t streamWidth = 720;             // 出流分辨率宽
    uint32_t streamHeight = 1280;           // 出流分辨率高
} __attribute__((packed));

enum ProfileType : uint32_t {
    BASELINE,
    MAIN,
    HIGH
};

enum RCMode : uint32_t  {
    ABR,                       // 平均码率，暂不支持
    CRF,                       // 画质优先，暂不支持
    CBR,                       // 恒定码率
    CAPPED_CRF,                // 画质优先，但限制码率
    RC_MODE_MAX
};
```

**约束说明**

流控模式暂时只支持 CBR 恒定码率模式与 CAPPED_CRF（画质优先，但限制码率）模式。

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
| bitrate | 编码码率 | AMD，一般为 W6800：500000~50000000 DC1000/DC1000C：500000~30000000 单位 bps | 3000000 |
| gopSize | 编码 GOP 大小配置项 | 30~3000 | 30 |
| profile | 编码 profile 配置项（H.265 编码仅支持配置 main） | 0：BASELINE（仅 H264 支持）1：MAIN 2：HIGH（仅 H264 支持） | 0：BASELINE（仅 H264 支持） |
| rcMode | 码控模式配置项 | 0：ABR 平均码率模式（暂不支持）1：CRF 画质优先模式（暂不支持）2：CBR 恒定码率模式 3：CAPPED_CRF 画质优先并限制最大码率模式 | 2：CBR 恒定码率模式 |
| forceKeyFrame | 编码强制 I 帧配置项 | 0：不触发编码强制 I 帧 1：在下一帧强制生成 I 帧 | 0：默认不触发编码强制 I 帧 |
| interpolation | 补帧参数项 | false：关闭补帧 true：开启补帧 | false：关闭补帧 |
| crf | crf 码控级别 | 0-51 | 34 |
| maxCrfRate | crf 码率峰值 | 500000~100000000 | 20000000 |
| vbvBufferSize | crf 码率缓冲区大小 | -1：自动模式 0：禁用峰值比特率限制 [min_vbv_size ~ 3000]：min_vbv_size = ceil(1000 / fps) +1 且 min_vbv_size >= 10 | 1000 |
| streamWidth | 出流帧宽度 | [240, 云手机渲染分辨率]，并且出流帧宽度需要为 8 的倍数 | 720 |
| streamHeight | 出流帧高度 | [240, 云手机渲染分辨率]，并且出流帧高度需要为 8 的倍数 | 1080 |

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 1. 码控模式（RCMode）仅支持固定码率模式（CBR）与画质优先并限制最大码率（CAPPED_CRF），Quadra 编码卡支持两种码控模式，DC1000/DC1000C 暂时仅支持固定码率模式（CBR）；
> 2. 码控模式（RCMode）设置为固定码率模式（CBR），可通过 bitrate 变量来指定固定码率的数值；而 Quadra 编码卡码控模式（RCMode）设置为画质优先并限制最大码率（CAPPED_CRF）时，会使用 crf、maxCrfRate 和 vbvBufferSize 变量来进行码率控制；DC1000/DC1000C 控模式（RCMode）会使用 crf、maxCrfRate 变量来进行码率控制

### 数据定义

视频输出模块输出数据结构由数据包额外信息 extData、数据包大小和实际数据三部分组成。该数据通过回调函数提供给开发者，并通过 VmiCmd 枚举中的 VIDEO_RETURN_VIDEO_DATA 命令字指定。使用示例如下：

```c++
VmiDataType module = DATA_VIDEO;
VmiCmd cmd = VIDEO_RETURN_VIDEO_DATA;
VideoData videoData;
uint8_t* data = &videoData;
DataCallback(module, cmd, data, sizeof(VideoData));
```

**VideoData 定义**

```c++
struct VideoData {
    ExtDataVideo extData;               // 码流包额外信息
    uint32_t size;                      // 码流包大小
    uint8_t data[0];                    // 码流数据
} __attribute__((packed));
```

**ExtDataVideo 定义**

```c++
struct FrameSize {
    uint32_t width = 720;               // 720: 默认720P
    uint32_t height = 1280;             // 1280: 默认720P
    uint32_t widthAligned = 720;        // 720: 默认不做对齐
    uint32_t heightAligned = 1280;      // 1280: 默认不做对齐
} __attribute__((packed));

struct VideoLatency {
    uint64_t startCaptureTimestamp;     // 抓图开始时间点
    uint64_t endCaptureTimestamp;       // 抓图完成时间点
    uint64_t startEncodeTimestamp;      // 编码开始时间点
    uint64_t endEncodeTimestamp;        // 编码完成时间点
} __attribute__((packed));

struct ExtDataVideo {
    uint64_t timestamp;                  // 帧渲染完成的时间戳
    uint8_t orientation;                // 当前帧的旋转方向
    FrameSize size;                     // 当前图像大小
    VideoLatency latencyInfo;           // 抓图和编码时间点
    uint32_t frameRate;                 // 当前屏幕刷新率
    uint32_t transform;                 // 当前图像的旋转。CompositionBypass特性开启时，transform可能不为0；CompositionBypass特性关闭时，transform=0。
} __attribute__((packed));
```

### 参数获取

视频输出提供参数获取特性，该特性通过对外接口 GetParam 进行获取。

**VIDEO_GET_ENCODER_PARAM**

通过指定 module 为 DATA_VIDEO，cmd 为 VIDEO_GET_ENCODER_PARAM，data 传入 EncodeParams 类型数据，可进行编码参数获取。示例如下：

```c++
VmiDataType module = DATA_VIDEO;
VmiCmd cmd = VIDEO_GET_ENCODER_PARAM;
EncodeParams encodeParams;
uint8_t* param = &encodeParams;
GetParam(module, cmd, param, sizeof(EncodeParams));
```

### 参数设置

视频输出提供参数设置特性，该特性通过对外接口 SetParam 进行设置。

**VIDEO_SET_ENCODER_PARAM**

通过指定 module 为 DATA_VIDEO，cmd 为 VIDEO_SET_ENCODER_PARAM，data 传入 EncodeParams 类型数据，可进行编码参数设置。示例如下：

```c++
VmiDataType module = DATA_VIDEO;
VmiCmd cmd = VIDEO_SET_ENCODER_PARAM;
EncodeParams encodeParams;
uint8_t* param = &encodeParams;
SetParam(module, cmd, param, sizeof(EncodeParams));
```

### 配置推荐

#### 分辨率与屏幕密度推荐搭配

|规格|分辨率宽度（width）|分辨率长度（height）|屏幕密度（density）|
|--|--|--|--|
|360P|360|640|120|
|480P|480|856|160|
|720P|720|1280|320|
|1080P|1080|1920|480|
|2K|1440|2560|640|
|4K|2160|3840|960|

#### 编码参数推荐搭配

编码参数与视频流云手机运行的硬件环境相关，硬件环境请参见《[视频流引擎 安装指南（Android 15）](https://www.hikunpeng.com/document/detail/zh/kunpengcps/boostcph/videostreamengine_ad15/docs/zh/install_guide.md#d1-%E8%BD%AF%E4%BB%B6%E9%83%A8%E7%BD%B2)》"软件部署"中的"硬件环境"章节。

**硬件配置方案一**

**CBR 编码模式编码参数**

|编码参数|推荐值|
|--|--|
|bitrate|8000000|
|gopSize|60|
|profile|main|

**CAPPED_CRF 编码模式编码参数**

|编码参数|推荐值|
|--|--|
|gopSize|60|
|profile|main|
|crf|21|
|maxCrfRate|10000000|
|vbvBufferSize|1000|

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 硬件配置方案一（W6800+Quadra）建议选择使用"VPU"进行编码，并且在 H.265 编码时 profile 仅支持 main。

**硬件配置方案二**

**CBR 编码模式编码参数**

|编码参数|推荐值|
|--|--|
|bitrate|8000000|
|gopSize|60|
|profile|main|

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 硬件配置方案二（道客 DC1000/DC1000C）建议选择使用"GPU"进行编码，并且在 H.265 编码时 profile 仅支持 main。

## 音频输出开发

### 使用约定

音频输出配置 VmiConfigAudio 继承自 VmiConfig 结构体。具体结构如下：

```c++
struct VmiConfigAudio : public VmiConfig  {
    AudioType audioType = OPUS;
    AudioPlayParams params;                     // 默认的音频播放参数
} __attribute__((packed));
```

### 配置参数

#### AudioType

**变量说明**

音频类型。

**约束说明**

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
| audioType | 音频输出格式 | 0：OPUS 1：PCM | 0：OPUS |

**变量数据定义**

```c++
enum AudioType : uint32_t {
    OPUS,                     // 默认使用OPUS压缩音频
    PCM                       // 原始音频数据
};
```

#### AudioPlayParams

**变量说明**

音频播放参数。

**约束说明**

bitrate 参数只在 OPUS 格式下有效，sampleInterval 参数在 OPUS 和 PCM 模式均有效。

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
| sampleInterval | 音频输出采样间隔 | 5：5ms（暂不支持）10：10ms 20：20ms（暂不支持） | 10：10ms |
| bitrate | 音频 OPUS 编码码率（bps） | 13200~512000 | 192000 |

**变量数据定义**

```c++
struct AudioPlayParams {
    uint32_t sampleInterval = 10;               // ms
    uint32_t bitrate = 192000;                  // bps，只有在OPUS格式有效
} __attribute__((packed));
```

### 数据定义

音频输出模块输出数据结构由码流包额外信息 extData、数据包大小和实际数据三部分组成。该数据通过回调函数提供给开发者，并通过 VmiCmd 枚举中的 AUDIO_RETURN_AUDIO_PLAY_DATA 命令字指定。

使用示例如下：

```c++
VmiDataType module = DATA_AUDIO;
VmiCmd cmd = AUDIO_RETURN_AUDIO_PLAY_DATA;
uint8_t audioPlayData[240];
AudioData audioData;
audioData.size = sizeof(audioPlayData);
uint8_t *data = new uint8_t[sizeof(AudioData) + sizeof(audioPlayData)];
memcpy(data, &audioData, sizeof(AudioData));
memcpy(data + sizeof(AudioData), audioPlayData, sizeof(audioPlayData))
DataCallback(module, cmd, data, sizeof(AudioData) + audioData.size);
```

**AudioData 定义**

```c++
struct AudioData {
    ExtDataAudio extData;               // 额外信息
    uint32_t size;                      // 大小
    uint8_t data[0];                    // 数据
} __attribute__((packed));
```

**ExtDataAudio 定义**

```c++
struct ExtDataAudio {
    uint32_t audioType = OPUS;              // 编码模式：OPUS/PCM
    uint8_t channels = 2;                   // 声道数 1：MONO/2:STEREO(默认)
    uint8_t audioBitDepth = 16;             // 采样深度 8bit/16bit(默认)
    uint32_t sampleRate = 48000;            // 采样率 48000Hz(默认)
    uint32_t sampleInterval = 10;           // 采样间隔 10ms(默认)
    int64_t timestamp;                      // 音频数据时间戳 毫秒，UTC时间
} __attribute__((packed));
```

**AudioVolume 定义**

```c++
struct AudioVolume {
    uint32_t leftVolume = 100;                  // 左声道音量
    uint32_t rightVolume = 100;                 // 右声道音量
} __attribute__((packed));
```

### 参数获取

音频输出提供参数获取特性，该特性通过对外接口 GetParam 进行获取。

**AUDIO_GET_AUDIOPLAY_PARAM**

通过指定 module 为 DATA_AUDIO，cmd 为 AUDIO_GET_AUDIOPLAY_PARAM，data 传入 AudioPlayParams 类型数据，可获取音频播放参数。示例如下：

```c++
VmiDataType module = DATA_AUDIO;
VmiCmd cmd = AUDIO_GET_AUDIOPLAY_PARAM;
AudioPlayParams audioPlayParams;
uint8_t* param = &audioPlayParams;
GetParam(module, cmd, param, sizeof(AudioPlayParams));
```

### 参数设置

音频输出提供参数设置特性，该特性通过对外接口 SetParam 进行设置。

**AUDIO_SET_AUDIOPLAY_PARAM**

通过指定 module 为 DATA_AUDIO，cmd 为 AUDIO_SET_AUDIOPLAY_PARAM，data 传入 AudioPlayParams 类型数据，可进行音频播放参数设置。示例如下：

```c++
VmiDataType module = DATA_AUDIO;
VmiCmd cmd = AUDIO_SET_AUDIOPLAY_PARAM;
AudioPlayParams audioPlayParams;
uint8_t* param = &audioPlayParams;
SetParam(module, cmd, param, sizeof(AudioPlayParams));
```

## 麦克风输入开发

### 使用约定

麦克风输入配置 VmiConfigMic 继承自 VmiConfig 结构体。具体结构如下：

```c++
struct VmiConfigMic : public VmiConfig {
    AudioType audioType = OPUS;
} __attribute__((packed));
```

### 配置参数

#### AudioType

**变量说明**

音频类型。

**约束说明**

|对应属性字段名称|字段描述|取值范围|默认值|
|--|--|--|--|
| audioType | 麦克风输入格式 | 0：OPUS 1：PCM | 0：OPUS |

**变量数据定义**

```c++
enum AudioType : uint32_t {
    OPUS,                     // 默认使用OPUS压缩音频
    PCM                       // 原始音频数据
};
```

### 数据定义

麦克风输入模块输入数据结构和音频输出数据结构均为 AudioData，由码流包额外信息 extData、数据包大小和实际数据三部分组成。

**AudioData 定义**

```c++
struct AudioData {
    ExtDataAudio extData;               // 额外信息
    uint32_t size;                      // 大小
    uint8_t data[0];                    // 数据
} __attribute__((packed));
```

**ExtDataAudio 定义**

```c++
struct ExtDataAudio {
    uint32_t audioType = OPUS;              // 编码模式：OPUS/PCM
    uint8_t channels = 2;                   // 声道数 1：MONO/2:STEREO(默认)
    uint8_t audioBitDepth = 16;             // 采样深度 8bit/16bit(默认)
    uint32_t sampleRate = 48000;            // 采样率 48000Hz(默认)
    uint32_t sampleInterval = 10;           // 采样间隔 10ms(默认)
    int64_t timestamp;                      // 音频数据时间戳 毫秒，UTC时间
} __attribute__((packed));
```

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> timestamp 必须是客户端的时间戳，不能是服务端通过网络收到音频数据包之后的时间戳，否则可能会导致无法过滤因网络波动而严重延迟的包，从而出现音频整体滞后的现象。

### 数据注入

输入与输出不同，所有的输入都需要使用 InjectData 接口进行数据注入。开发者通过 InjectData 将数据注入到服务端，并通过 VmiCmd 枚举中的 MIC_SEND_MIC_DATA 命令字指定。示例如下：

```c++
VmiDataType module = DATA_MIC;
VmiCmd cmd = MIC_SEND_MIC_DATA;
uint8_t micData[240];
AudioData audioData;
audioData.size = sizeof(micData);
uint8_t *data = new uint8_t[sizeof(AudioData) + sizeof(micData)];
memcpy(data, &audioData, sizeof(AudioData));
memcpy(data + sizeof(AudioData), micData, sizeof(micData))
InjectData(module, cmd, data, sizeof(AudioData) + sizeof(micData));
```

**约束说明**

麦克风帧需要按照稳定的节奏注入，否则会导致声音听起来不连续。具体约束与 default.prop 配置文件中 vmi.mic.cachefactor 的取值有关，该配置项表示麦克风帧队列的缓存大小，有以下 4 个取值：

- 0：要求每 10ms 注入 1 帧，不造成额外延迟。
- 1：要求每 160ms 注入 16 帧，造成 160ms 延迟。
- 2：要求每 320ms 注入 32 帧，造成 320ms 延迟。
- 3：要求每 480ms 注入 48 帧，造成 480ms 延迟。

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> - 如果使用 PCM 模式传输麦克风数据，建议客户端到服务端的上行网络带宽不低于 2Mbps。
> - 注入的麦克风帧之间不能混入空白帧，否则可能会导致声音听起来不连续。

## 触控输入开发

### 使用约定

触控输入配置 VmiConfigTouch 继承自 VmiConfig 结构体。具体结构如下：

```c++
struct VmiConfigTouch : public VmiConfig {} __attribute__((packed));
```

### 数据定义

触控输入数据类型包括触控板 VmiTouchInputData、键盘 VmiKeyInputData。

**VmiTouchInputData**

触控板输入数据类型定义如下：

```c++
// 触控事件
struct VmiTouchEvent {
    uint8_t pointerID;
    uint8_t action;
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
} __attribute__((packed));
// 触控输入数据
struct VmiTouchInputData {
    uint8_t orientation;                        // 客户端当前旋转方向
    uint16_t uiWidth;                           // 客户端UI分辨率的宽度
    uint16_t uiHeight;                          // 客户端UI分辨率的高度
    VmiTouchEvent touchEvent;
} __attribute__((packed));
```

**VmiKeyInputData**

键盘输入数据类型定义如下：

```c++
// 键盘输入数据
struct VmiKeyInputData {
    uint16_t keyCode;
    uint16_t action;
} __attribute__((packed));
```

### 数据注入

输入与输出不同，所有的输入都需要使用 InjectData 接口进行数据注入。该数据开发者通过 InjectData 注入到服务端，并通过 VmiCmdTouch 枚举中的 SEND_TOUCH_EVENT、SEND_KEY_EVENT 命令字指定。示例如下：

```c++
VmiDataType module = TOUCH;
VmiCmd cmd = SEND_TOUCH_EVENT;
VmiTouchInputData touchData;
uint8_t* data = &touchData;
InjectData(module, cmd, data, sizeof(VmiTouchInputData));

VmiDataType module = TOUCH;
VmiCmd cmd = SEND_KEY_EVENT;
VmiKeyInputData keyData;
uint8_t* data = &keyData;
InjectData(module, cmd, data, sizeof(VmiKeyInputData));
```

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> - 服务端会对触控数据进行有限的校验，例如判断触控数据的触点位置是否在有效范围内，但不能分辨因为异常行为带来的异常数据，例如手指从客户端所在的屏幕上抬起，但客户端却发送了一个 Down 事件（此处客户端应该发送 UP 事件），因此在编写客户端代码时，需要注意。
> - 在部分机型（目前发现的有 VIVO 手机）上进行多指操作（3 指及以上），当手指同时按下或手指依次按下的间隔时间很短的时候，客户端会发送 EVENT_CANCEL 事件，导致服务端重置所有触点状态导致触控失效，手指依次按下的间隔时间较长时则没有该问题。

## 传感器输入开发

### 使用约定

传感器输入配置 VmiConfig 结构体。具体结构如下：

```c++
struct VmiConfig {
    uint32_t version;
} __attribute__((packed));
```

### 数据定义

传感器输入模块输入数据结构由传感器数据 x，y，z 和 type 四部分组成。该数据通过回调函数提供给开发者。

使用示例如下：

```c++
struct SensorData {
    float x;
    float y;
    float z;
    int32_t type;
} __attribute__((packed));
```

传感器支持类型如下：

```c++
enum VmiSensorType : uint32_t {
    HANDLE_ACCELERATION = 0,                // 加速度传感器
    HANDLE_GYROSCOPE,                       // 陀螺仪传感器
    HANDLE_MAGNETIC_FIELD,                  // 磁场传感器
    HANDLE_ORIENTATION,                     // 方向传感器
    HANDLE_TEMPERATURE,                     // 温度传感器
    HANDLE_PROXIMITY,                       // 近程传感器
    HANDLE_LIGHT,                           // 光传感器
    HANDLE_PRESSURE,                        // 压力传感器
    HANDLE_HUMIDITY,                        // 湿度传感器
    HANDLE_MAGNETIC_FIELD_UNCALIBRATED,     // 未校准磁场传感器
    HANDLE_MAX = 10
};
```

传感器输出模块输出数据结构由传感器类型handle和传感器设置的采集率两部分组成。该数据通过回调函数提供给开发者。

使用示例如下：

```c++
struct SensorActivateData {
    int handle;             // handle对应VmiSensorType里的值
    int64_t samplingPeriod; // 在取消激活传感器时，这个字段可以忽略或设置为默认值
} __attribute__((packed));
```

### 数据注入

所有的输入都需要使用 InjectData 接口进行数据注入。开发者通过 InjectData 将该数据注入到服务端，并通过 VmiCmd 枚举中的 SENSOR_SEND_SENSOR_DATA 命令字指定。示例如下：

```c++
VmiDataType module = DATA_SENSOR;
VmiCmd cmd = SENSOR_SEND_SENSOR_DATA;
SensorData sensorData;
uint8_t* data = &sensorData;
InjectData(module, cmd, data, sizeof(SensorData));
```

### 数据输出

所有的输出都需要使用 SetParam 接口进行流出。开发者通过 SetParam 将该数据输出到客户端，并通过 VmiCmd 枚举中的 SENSOR_RETURN_REGISTER_CLIENT_SENSOR、SENSOR_RETURN_UNREGISTER_CLIENT_SENSOR、SENSOR_RETURN_UPDATE_CLIENT_SENSOR_RATE 命令字指定。示例如下：

```c++
VmiDataType module = SENSOR;
VmiCmd cmd = SENSOR_RETURN_REGISTER_CLIENT_SENSOR;
VmiCmd cmd = SENSOR_RETURN_UNREGISTER_CLIENT_SENSOR;
VmiCmd cmd = SENSOR_RETURN_UPDATE_CLIENT_SENSOR_RATE;
SensorActivateData sensorActivateData;
uint8_t* data = &sensorActivateData;
SetParam(module, cmd, param, sizeof(SensorActivateData));
```

## GPS输入开发

### 使用约定

GPS 输入配置 VmiConfig 结构体。具体结构如下：

```c++
struct VmiConfig {
    uint32_t version;
} __attribute__((packed));
```

### 数据定义

GPS 输入数据类型包括经纬度位置数据 VmiGpsLocationData。

**VmiGpsLocationData**

经纬度位置输入数据类型定义如下：

```c++
struct VmiGpsLocationData {
    double latitude;
    double longitude;
    double altitude;
    float speed;
    float bearing;
    float accuracy;
    int64_t timestamp;
} __attribute__((packed));
```

### 数据注入

所有的输入都需要使用 InjectData 接口进行数据注入。该数据开发者通过 InjectData 注入到服务端，并通过 VmiCmd 枚举中的 GPS_SEND_LOCATION_DATA 命令字指定。示例如下：

```c++
VmiDataType module = DATA_GPS;
VmiCmd cmd = GPS_SEND_LOCATION_DATA;
VmiGpsLocationData locationData;
uint8_t* data = &locationData;
InjectData(module, cmd, data, sizeof(VmiGpsLocationData));
```

## 视频流引擎非商用实现接口

### 编码接口（C&C++）

#### 接口说明

视频编码模块提供视频流服务端引擎正常运行所需要调用的外部视频编码接口，由二次开发者实现，并以动态链接库的形式提供。其动态库的名称为：libVideoCodec.so。

开发者继承 VideoEncoder 类并按照本章节中的描述实现对应接口，同时提供 [CreateVideoEncoder](#createvideoencoder)、[DestroyVideoEncoder](#destroyvideoencoder) 接口用于创建具体的实现类实例。

其中，接口调用的返回码定义如下：

```c++
enum EncoderRetCode : uint32_t {
    VIDEO_ENCODER_SUCCESS                = 0x00,
    VIDEO_ENCODER_CREATE_FAIL            = 0x01,  // 创建编码器失败
    VIDEO_ENCODER_INIT_FAIL              = 0x02,  // 初始化编码器失败
    VIDEO_ENCODER_START_FAIL             = 0x03,  // 启动编码器失败
    VIDEO_ENCODER_ENCODE_FAIL            = 0x04,  // 编码失败
    VIDEO_ENCODER_STOP_FAIL              = 0x05,  // 停止编码器失败
    VIDEO_ENCODER_DESTROY_FAIL           = 0x06,  // 销毁编码器失败
    VIDEO_ENCODER_REGISTER_FAIL          = 0x07,  // 注册函数失败
    VIDEO_ENCODER_RESET_FAIL             = 0x08,  // 重置编码器失败
    VIDEO_ENCODER_FORCE_KEY_FRAME_FAIL   = 0x09,  // 强制I帧失败
    VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL = 0x0A   // 设置编码参数失败
};
```

编码器类型定义如下：

```c++
enum EncoderFormat: uint32_t {
    ENCODER_TYPE_OPENH264 = 0,    // 开源OpenH264编码器
    ENCODER_TYPE_NETINTH264 = 1,  // NETINT h.264硬件编码器
    ENCODER_TYPE_NETINTH265 = 2,  // NETINT h.265硬件编码器
    ENCODER_TYPE_VASTAIH264 = 3,  // VASTAI h.264硬件编码器
    ENCODER_TYPE_VASTAIH265 = 4,  // VASTAI h.265硬件编码器
    ENCODER_TYPE_QUADRAH264 = 5,  // QUADRA h.264硬件编码器
    ENCODER_TYPE_QUADRAH265 = 6,  // QUADRA h.265硬件编码器
    ENCODER_TYPE_T432H264 = 7,    // T432 h.264硬件编码器
    ENCODER_TYPE_T432H265 = 8,    // T432 h.265硬件编码器
};
```

#### CreateVideoEncoder

**函数功能**

创建编码器实例。

**函数原型**

`EncoderRetCode CreateVideoEncoder(int32_t *fd, EncoderFormat encoderFormat)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t* | 编码器标记符 |
| encoderFormat | 输入 | EncoderFormat 枚举 | 要创建的编码器的类型 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：创建编码器实例成功。
- VIDEO_ENCODER_CREATE_FAIL：创建编码器实例失败。

#### DestroyVideoEncoder

**函数功能**

销毁编码器实例。

**函数原型**

`EncoderRetCode DestroyVideoEncoder(int32_t fd)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：销毁编码器实例成功。
- VIDEO_ENCODER_DESTROY_FAIL：销毁编码器实例失败。

**VideoEncoder 接口类涉及接口**

VideoEncoder 接口类涉及接口详细描述请参见 [Config](#config)、[InitEncoder](#initencoder)、[StartEncoder](#startencoder)、[SetParams](#setparams)、[EncodeOneFrame](#encodeoneframe)、[StopEncoder](#stopencoder)、[DestroyEncoder](#destroyencoder)、[ResetEncoder](#resetencoder)。

#### Config

**函数功能**

输入编码器的初始化配置，需要在 [InitEncoder](#initencoder) 之前调用。

**约束说明**

如 [配置参数](#配置参数) 所示的参数类型，会经由此参数透传至 VmiEncoderConfig 和 VmiEncoderParams 中；[配置参数](#配置参数) 中的配置参数的相关约束此处仍然存在。

**函数原型**

`EncoderRetCode Config(int32_t fd, const VmiEncoderConfig& config)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |
| config | 输入 | struct VmiEncoderConfig | 要配置的编码器配置 |

struct VmiEncoderConfig 和 struct VmiEncoderParams 的具体定义如下：

```c++
struct VmiEncoderParams {
    uint32_t bitRate = 3000000;
    uint32_t gopSize = 30;
    uint32_t profile = 1;
    uint32_t keyFrame = 0;
    uint32_t rcMode = 2;                        // 流控模式
    uint32_t crf = 34;                          // crf码控级别
    uint32_t maxCrfRate = 20000000;             // crf码率峰值
    int32_t vbvBufferSize = 1000;               // crf码率缓冲区大小
};
```

```c++
struct VmiEncoderConfig {
    uint32_t width = 720;
    uint32_t height = 1280;
    uint32_t frameRate = 30;
    VmiEncoderParams encodeParams;
};
```

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

VIDEO_ENCODER_SUCCESS：配置编码器成功。

#### InitEncoder

**函数功能**

初始化编码器。

**约束说明**

要求已经调用 [Config](#config) 接口输入合法的配置。

**函数原型**

`EncoderRetCode InitEncoder(int32_t fd)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：初始化编码器成功。
- VIDEO_ENCODER_INIT_FAIL：初始化编码器失败。

#### StartEncoder

**函数功能**

启动编码器。

**函数原型**

`EncoderRetCode StartEncoder(int32_t fd)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：启动编码器成功。
- VIDEO_ENCODER_INIT_FAIL：初始化编码器失败。

#### SetParams

**函数功能**

设置编码器的编码参数。

**约束说明**

如 [配置参数](#配置参数) 所示的参数类型，会经由此参数透传至 VmiEncoderParams 中；[配置参数](#配置参数) 中的配置参数的相关约束此处仍然存在。

**函数原型**

`EncoderRetCode SetParams(int32_t fd, const VmiEncoderParams& params)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |
| params | 输入 | struct VmiEncoderParams | 要设置的编码参数 |

struct VmiEncoderParams 的具体定义如下：

```c++
struct VmiEncoderParams {
    uint32_t bitRate = 3000000;
    uint32_t gopSize = 30;
    uint32_t profile = 1;
    uint32_t keyFrame = 0;
    uint32_t rcMode = 2;                        // 流控模式
    uint32_t crf = 34;                          // crf码控级别
    uint32_t maxCrfRate = 20000000;             // crf码率峰值
    int32_t vbvBufferSize = 1000;               // crf码率缓冲区大小
};
```

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

VIDEO_ENCODER_SUCCESS：设置编码器成功。

#### EncodeOneFrame

**函数功能**

编码一帧数据。

**函数原型**

`EncoderRetCode EncodeOneFrame(int32_t fd, const uint8_t *inputData, uint32_t inputSize, uint8_t **outputData, uint32_t *outputSize)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |
| inputData | 输入 | const uint8_t * | 待编码数据缓冲区首地址，数据格式为 yuv420p |
| inputSize | 输入 | uint32_t | 待编码数据长度，单位 Byte |
| outputData | 输出 | uint8_t ** | 编码后的数据 |
| outputSize | 输出 | uint32_t * | 编码后的数据长度，单位 Byte |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：编码一帧成功。
- VIDEO_ENCODER_ENCODE_FAIL：编码一帧失败。

#### StopEncoder

**函数功能**

停止编码器。

**函数原型**

`EncoderRetCode StopEncoder(int32_t fd)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：停止编码器成功。
- VIDEO_ENCODER_STOP_FAIL：停止编码器失败。

#### DestroyEncoder

**函数功能**

销毁编码器，释放编码资源。

**函数原型**

`EncoderRetCode DestroyEncoder(int32_t fd)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：销毁编码器成功。
- VIDEO_ENCODER_DESTROY_FAIL：销毁编码器失败。

#### ResetEncoder

**函数功能**

重置编码器。

**函数原型**

`EncoderRetCode ResetEncoder(int32_t fd)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| fd | 输入 | int32_t | 编码器标记符 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：重置编码器成功。
- VIDEO_ENCODER_RESET_FAIL：重置编码器失败。

#### FrameScaling

**函数功能**

设置期望编码宽高。

**约束说明**

如 [配置参数](#配置参数) 中所示的参数类型，会经由此参数透传至 FrameScaling 入参中；[配置参数](#配置参数) 中的配置参数的相关约束此处仍然存在。

二次开发者也可直接调用此接口，实现 YUV 帧缩放功能。

**函数原型**

`EncoderRetCode FrameScaling(uint32_t width, uint32_t height)`

**参数说明**

width：uint32_t 类型，期望编码宽度。该类字段如下：

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| width | 输入 | uint32_t | 期望编码宽度 |

height：uint32_t 类型，期望编码高度。该类字段如下：

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| height | 输入 | uint32_t | 期望编码高度 |

**返回值说明**

数据类型：`EncoderRetCode`

取值如下：

- VIDEO_ENCODER_SUCCESS：成功。
- VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL：失败。

### 解码接口（C&C++）

#### 接口说明

视频解码模块提供视频流服务端引擎正常运行所需要调用的外部视频解码接口，由二次开发者实现，并以动态链接库的形式提供。其动态库的名称为：libVideoDecoder.so。

开发者继承 VideoDecoder 类并按照本章节中的描述实现对应接口，同时提供 [CreateVideoDecoder](#createvideodecoder)、[DestroyVideoDecoder](#destroyvideodecoder) 接口用于创建具体的实现类实例。

此接口与 Android 内部 OMX 解码组件配套使用，与视频流出流组件不相干扰。

其中，接口调用的返回码定义如下：

```c++
enum DecoderRetCode : uint32_t {
    VIDEO_DECODER_SUCCESS,                // 成功
    VIDEO_DECODER_CREATE_FAIL,            // 创建解码器失败
    VIDEO_DECODER_INIT_FAIL,              // 初始化解码器失败
    VIDEO_DECODER_START_FAIL,             // 启动解码器失败
    VIDEO_DECODER_DECODE_FAIL,            // 解码失败
    VIDEO_DECODER_STOP_FAIL,              // 停止解码器失败
    VIDEO_DECODER_DESTROY_FAIL,           // 销毁解码器失败
    VIDEO_DECODER_RESET_FAIL,             // 重置解码器失败
    VIDEO_DECODER_GET_DECODE_PARAMS_FAIL, // 获取解码参数失败
    VIDEO_DECODER_SET_DECODE_PARAMS_FAIL, // 设置解码参数失败
    VIDEO_DECODER_SET_FUNC_FAIL,          // 设置回调函数失败
    VIDEO_DECODER_WRITE_OVERFLOW,         // 输入Buffer输送过快
    VIDEO_DECODER_READ_UNDERFLOW,         // 输出Buffer获取过快
    VIDEO_DECODER_BAD_PIC_SIZE,           // 解码后实际分辨率与配置分辨率不符
    VIDEO_DECODER_EOS
};
```

#### CreateVideoDecoder

**函数功能**

创建解码器实例。

**函数原型**

`DecoderRetCode CreateVideoDecoder(VideoDecoder **decoder)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| decoder | 输出 | VideoDecoder ** | 解码器实例二维指针，用于存放接口内创建的解码器实例 |

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：创建解码器实例成功。
- VIDEO_DECODER_CREATE_FAIL：创建解码器实例失败。

#### DestroyVideoDecoder

**函数功能**

销毁解码器实例。

**函数原型**

`DecoderRetCode DestroyVideoDecoder(VideoDecoder *decoder)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| decoder | 输入 | VideoDecoder * | 要销毁的解码器实例指针 |

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：销毁解码器实例成功。
- VIDEO_DECODER_DESTROY_FAIL：销毁解码器实例失败。

**VideoDecoder 接口类涉及接口**

VideoDecoder 接口类涉及接口详细描述请参见 [CreateDecoder](#createdecoder)、[InitDecoder](#initdecoder)、[SetDecodeParams](#setdecodeparams)、[GetDecodeParams](#getdecodeparams)、[SetCallbacks](#setcallbacks)、[SetCopyFrameFunc](#setcopyframefunc)、[SendStreamData](#sendstreamdata)、[Flush](#flush)、[StartDecoder](#startdecoder)、[StopDecoder](#stopdecoder)、[DestroyDecoder](#destroydecoder)。

#### CreateDecoder

**函数功能**

创建解码器。

**函数原型**

`DecoderRetCode CreateDecoder(MediaStreamFormat decType)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| decType | 输入 | MediaStreamFormat | 解码码流类型 |

MediaStreamFormat 的具体定义如下：

```c++
// 解码输入格式
enum MediaStreamFormat : uint32_t {
    STREAM_FORMAT_AVC,
    STREAM_FORMAT_HEVC,
    STREAM_FORMAT_NONE
};
```

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：创建解码器成功。
- VIDEO_DECODER_CREATE_FAIL：创建解码器失败。

#### InitDecoder

**函数功能**

初始化解码器。

**约束说明**

要求已经调用 [CreateDecoder](#createdecoder) 创建解码器成功。

**函数原型**

`DecoderRetCode InitDecoder()`

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：初始化解码器成功。
- VIDEO_DECODER_INIT_FAIL：初始化解码器失败。

#### SetDecodeParams

**函数功能**

设置解码器参数。

**函数原型**

`DecoderRetCode SetDecodeParams(DecodeParamsIndex index, void *decParams)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| index | 输入 | DecodeParamsIndex | 表示要设置的参数的类型 |
| decParams | 输入 | void * | 对应参数的内存地址 |

可设置的参数的定义如下：

```c++
// 解码参数
enum DecodeParamsIndex : uint32_t {
    INDEX_PIC_INFO,
    INDEX_PORT_FORMAT_INFO,
    INDEX_ALIGN_INFO,
    INDEX_PARAM_NONE
};
enum DecoderPort : uint32_t {
    IN_PORT,
    OUT_PORT
};
// 解码输出格式
enum MediaPixelFormat : uint32_t {
    PIXEL_FORMAT_RGBA_8888,
    PIXEL_FORMAT_YUV_420P,
    PIXEL_FORMAT_FLEX_YUV_420P,
    PIXEL_FORMAT_NV12,
    PIXEL_FORMAT_NV21,
    PIXEL_FORMAT_NONE
};
struct PortFormatParams {
    DecoderPort port {};
    int32_t format = 0;
};

struct PicInfoParams {
    uint32_t width = 0;        // 图像宽度
    uint32_t height = 0;       // 图像高度
    int32_t stride = 0;        // 图像横向对齐后高度
    uint32_t scanLines = 0;    // 图像纵向对齐后高度
    uint32_t cropWidth = 0;    // 图像左侧的空余像素数量
    uint32_t cropHeight = 0;   // 图像上侧的空余像素数量
};

struct AlignInfoParams {
    uint32_t widthAlign = 0;
    uint32_t heightAlign = 0;
};
```

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：配置解码器成功。
- VIDEO_DECODER_SET_DECODE_PARAMS_FAIL：配置解码器失败。

#### GetDecodeParams

**函数功能**

获取解码器解码参数。

**函数原型**

`DecoderRetCode GetDecodeParams(DecodeParamsIndex index, void *decParams)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| index | 输入 | DecodeParamsIndex | 表示要获取的参数类型 |
| decParams | 输入 | void * | 对应参数的内存地址 |

参数的详细定义请参见 [SetDecodeParams](#setdecodeparams)。

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：获取解码器参数成功。
- VIDEO_DECODER_GET_DECODE_PARAMS_FAIL：获取解码器参数失败。

#### SetCallbacks

**函数功能**

设置解码器事件通知回调函数。

**函数原型**

`DecoderRetCode SetCallbacks(std::function<void(DecodeEventIndex, uint32_t, void *)> eventCallBack)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| eventCallBack | 输入 | std::function<void(DecodeEventIndex, uint32_t, void *)> | 解码器事件通知回调函数 |

当解码器中有事件需要通知上层时，需要调用此接口中设置的回调函数，其中 index 表示事件的类型，uint32_t 和 void * 类型参数分别用于存放对应的数据，详细定义如下：

```c++
// 解码事件
enum DecodeEventIndex : uint32_t {
    INDEX_PIC_INFO_CHANGE,
    INDEX_EVENT_NONE
};
```

INDEX_PIC_INFO_CHANGE 表示解码图像大小变化，此时在 `void *` 参数处挂载 `struct PicInfoParams *`类型，具体定义请参见 [SetDecodeParams](#setdecodeparams)。

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：设置回调成功。
- VIDEO_DECODER_SET_FUNC_FAIL：设置回调失败。

#### SetCopyFrameFunc

**函数功能**

提供给解码器，在输出帧数据时调用该函数回调，用于减少内存拷贝。

**函数原型**

`DecoderRetCode SetCopyFrameFunc(std::function<uint32_t(uint8_t*, uint8_t*, const PicInfoParams &, uint32_t)> copyFrame)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| copyFrame | 输入 | std::function<uint32_t(uint8_t*, uint8_t*, const PicInfoParams &, uint32_t)> | 设置将解码好的一帧数据拷贝到输出 Buffer 的钩子函数（用于屏蔽输出接口格式差异） |

该回调中，第一个`uint8_t*`表示解码完成后数据地址，第二个 `uint8_t*`表示拷贝的目标地址，需要填入 [RetrieveFrameData](#retrieveframedata) 中输入的 Buffer 地址，第三个 const PicInfoParams & 参数表示图像宽高信息，具体定义请参见 [SetDecodeParams](#setdecodeparams)；第四个 uint32_t 参数表示输出缓冲区最大长度，需要填入 [RetrieveFrameData](#retrieveframedata) 中输入的最大缓冲区大小，该回调返回值为处理数据的实际大小。

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：设置回调成功。
- VIDEO_DECODER_SET_FUNC_FAIL：设置回调失败。

#### SendStreamData

**函数功能**

同步接口，提供一份码流数据供解码器解码。

**函数原型**

`DecoderRetCode SendStreamData(uint8_t *buffer, uint32_t filledLen)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| buffer | 输入 | uint8_t * | 输入码流数据缓存 |

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| filledLen | 输入 | uint32_t | 输入码流数据长度 |

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：发送成功。
- VIDEO_DECODER_DECODE_FAIL：发送失败。
- VIDEO_DECODER_WRITE_OVERFLOW：输入 Buffer 速度太快。

#### RetrieveFrameData

**函数功能**

该接口作为同步接口，用于获取一帧解码输出，拷贝数据时需要使用 [SetCopyFrameFunc](#setcopyframefunc) 提供的 CopyFrame 函数。

**函数原型**

`DecoderRetCode RetrieveFrameData(uint8_t *buffer, uint32_t maxLen, uint32_t *filledLen)`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| buffer | 输入 | uint8_t * | 输出数据接收地址 |

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| maxLen | 输入 | uint32_t | 输出缓冲区最大长度 |

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| filledLen | 输出 | uint32_t * | 输出数据的实际长度 |

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：获取解码数据成功。
- VIDEO_DECODER_DECODE_FAIL：获取解码数据失败。
- VIDEO_DECODER_READ_UNDERFLOW：请求输出速度太快。

#### Flush

**函数功能**

放弃当前所有的解码 Buffer，通常用于拖动进度条时使用。

**函数原型**

`DecoderRetCode Flush()`

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：重置解码缓存状态成功。
- VIDEO_DECODER_RESET_FAIL：重置解码缓存状态失败。

#### StartDecoder

**函数功能**

启动解码器，成功后可以开始解码流程。

**函数原型**

`DecoderRetCode StartDecoder()`

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：启动成功。
- VIDEO_DECODER_START_FAIL：启动失败。

#### StopDecoder

**函数功能**

停止解码流程。

**函数原型**

`DecoderRetCode StopDecoder()`

**返回值说明**

数据类型：`DecoderRetCode`

取值如下：

- VIDEO_DECODER_SUCCESS：停止成功。
- VIDEO_DECODER_STOP_FAIL：停止失败。

#### DestroyDecoder

**函数功能**

销毁解码流程。

**函数原型**

`void DestroyDecoder()`

### GPU加速接口（C&C++）

#### 接口说明

GPU 加速模块提供视频流服务端引擎正常运行所需要调用的 GPU 加速接口，包含 RGB 转 YUV 功能和 GPU 编码功能，由二次开发者实现或 GPU 厂商实现，并以动态链接库的形式提供。其动态库的名称为：libVmiEncTurbo.so、libVmiEncTurboSys.so，其中前者为 vendor 库，后者为 system 库。

#### QueryModule

**函数功能**

查询当前系统上支持的 GPU 型号及其加速能力。

**函数原型**

`void QueryModule(ModuleInfo **moduleList, uint32_t *listSize)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| moduleList | 输出 | ModuleInfo ** | 当前系统支持的 GPU 加速类的列表，每一项包括 GPU 加速类的 ID 以及其对应的能力 |
| listSize | 输出 | uint32_t * | moduleList 的长度 |

moduleList 的详细定义如下：

```c++
struct ModuleInfo {
    // 硬件对应ID
    uint32_t moduleId = 0;
    // 硬件具备的能力列表，例如支持H264和H265编码，该值为(1 << CAP_VA_ENCODE_H264) | (1 << CAP_VA_ENCODE_HEVC)
    uint32_t capabilities = 0;
};
```

```c++
enum EncoderCapability : uint32_t {
    CAP_NONE,
    CAP_FORMAT_CONVERT,
    CAP_VA_ENCODE_H264,
    CAP_VA_ENCODE_HEVC,
    CAP_MAX,
};
```

moduleId 的规定如下：

```c++
enum GpuType : uint32_t {
    GPU_NONE = 0,
    GPU_INNO_G1 = 0x1,
    GPU_INNO_G1P,
    GPU_INNO_MAX = 0x100,
    GPU_HANTRO_SG100 = 0x101,
    GPU_HANTRO_MAX = 0x200,
    GPU_A_W5100 = 0x201,
    GPU_A_W5500,
    GPU_A_W6600,
    GPU_A_W6800,
    GPU_A_MAX = 0x400,
    GPU_SOFT = 0x401,
    GPU_MAX = 0x10000,
    VPU_NETINT_T408 = 0x10001,
    VPU_NETINT_T432,
    VPU_NETINT_QUADRA_T2A,
    VPU_NETINT_MAX = 0x10200,
};
```

#### CreateModule

**函数功能**

创建 GPU 加速类实例。

**约束说明**

输入的 moduleId 需要从 [QueryModule](#querymodule) 返回的 moduleList 中选择。

**函数原型**

`void *CreateModule(uint32_t moduleId)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| moduleId | 输入 | uint32_t | 用于指定要用于 GPU 加速的 GPU 卡型号 |

**返回值说明**

数据类型：`void *`，实际指向一个创建好的 GPU 加速类实例，可以通过 GpuEncoderBase 中定义的接口使用。

**GpuEncoderBase 接口类涉及接口**

GpuEncoderBase 接口类涉及接口详细描述请参见 [Init](#init)、[Deinit](#deinit)、[Start](#start)、[Stop](#stop)、[CreateBuffer](#createbuffer)、[ImportBuffer](#importbuffer)、[ReleaseBuffer](#releasebuffer)、[MapBuffer](#mapbuffer)、[UnmapBuffer](#unmapbuffer)、[RetriveBufferData](#retrivebufferdata)、[Convert](#convert)、[Encode](#encode)、[SetEncodeParam](#setencodeparam)、[Reset](#reset)。

GpuEncoderBase 接口类返回值说明：

```c++
enum GpuEncoderErrorCode : uint32_t {
    OK,
    ERR_INVALID_STATUS,
    ERR_INVALID_DEVICE,
    ERR_INVALID_PARAM,
    ERR_OUT_OF_MEM,
    ERR_UNEXPECT_STATUS,
    ERR_UNSUPPORT_FORMAT,
    ERR_UNSUPPORT_OPERATION,
    ERR_INTERNAL_ERROR,
    ERR_NEED_RESET, // 部分编码参数设置后需要reset生效
    ERR_UNKNOW
};
```

#### DestroyModule

**函数功能**

销毁 GPU 加速类实例。

**约束说明**

输入的实例必须由 [CreateModule](#createmodule) 创建。

**函数原型**

`void DestroyModule(void *module)`

**参数说明**

|参数名称|输入/输出|参数类型|参数描述|
|--|--|--|--|
| module | 输入 | void * | 要销毁的 GPU 加速类实例指针 |

#### Init

**函数功能**

初始化 GPU 加速实例。

**函数原型**

`int32_t Init(EncoderConfig &config) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| config | 输入 | EncoderConfig & | EncoderConfig 结构体引用类型，GPU 编码配置项 |

EncoderConfig 结构体和相关枚举定义如下：

```c++
enum EncoderCapability : uint32_t {
    CAP_NONE,
    CAP_FORMAT_CONVERT,
    CAP_VA_ENCODE_H264,
    CAP_VA_ENCODE_HEVC,
    CAP_MAX,
};
```

```c++
enum DeviceNode : uint32_t {
    DEV_DRI_RENDERD128 = 128,
    DEV_DRI_RENDERD129 = 129,
    DEV_DRI_RENDERD130 = 130,
    DEV_DRI_RENDERD131 = 131,
    DEV_DRI_OTHER = 132
};
struct FrameSize {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t widthAligned = 0;
    uint32_t heightAligned = 0;
    bool operator==(const FrameSize &other)
    {
        return this->width == other.width &&
               this->height == other.height &&
               this->widthAligned == other.widthAligned &&
               this->heightAligned == other.heightAligned;
    }
    bool operator!=(const FrameSize &other)
    {
        return this->width != other.width ||
               this->height != other.height ||
               this->widthAligned != other.widthAligned ||
               this->heightAligned != other.heightAligned;
    }
};
```

```c++
struct EncoderConfig {
    EncoderCapability capability = CAP_NONE; 
    DeviceNode deviceNode = DEV_DRI_OTHER;
    FrameSize inSize = {};
    FrameSize outSize = {};
};
```

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：初始化 GPU 加速实例成功。
- 其他：初始化 GPU 加速实例失败。

#### Deinit

**函数功能**

销毁 GPU 加速实例。

**函数原型**

`int32_t DeInit() = 0`

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：销毁 GPU 加速实例成功。
- 其他：销毁 GPU 加速实例失败。

#### Start

**函数功能**

启动 GPU 加速实例。

**约束说明**

前置条件：已成功调用 Init 接口。

**函数原型**

`int32_t Start() = 0`

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：启动 GPU 加速实例成功。
- 其他：启动 GPU 加速实例失败。

#### Stop

**函数功能**

停止 GPU 加速实例。

**约束说明**

前置条件：已成功调用 Start 接口。

**函数原型**

`int32_t Stop() = 0`

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：停止 GPU 加速实例成功。
- 其他：停止 GPU 加速实例失败。

#### CreateBuffer

**函数功能**

创建一个 Buffer。

**函数原型**

`int32_t CreateBuffer(FrameFormat format, MemType type, GpuEncoderBufferT &buffer) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| format | 输入 | FrameFormat | 要创建的 Buffer 对应的格式 |

FrameFormat 定义如下。

```c++
enum FrameFormat : uint32_t {
    FRAME_FORMAT_NONE,
    FRAME_FORMAT_RGBA,
    FRAME_FORMAT_BGRA,
    FRAME_FORMAT_YUV,
    FRAME_FORMAT_NV12,
    FRAME_FORMAT_H264,
    FRAME_FORMAT_HEVC,
    FRAME_FORMAT_COUNT
};
```

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| type | 输入 | MemType | 需要为 Buffer 分配空间的位置 |

MemType 定义如下。

```c++
enum MemType : uint32_t {
    MEM_TYPE_NONE,
    MEM_TYPE_HOST, //分配在主机内存中
    MEM_TYPE_DEVICE, //分配在设备内存中
    MEM_TYPE_COUNT
};
```

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| buffer | 输出 | GpuEncoderBufferT & | 分配完成后用于挂载 Buffer 的指针 |

GpuEncoderBufferT 定义如下，开发者或厂商实现该接口时需要使用 new 为 GpuEncoderBuffer 分配内存，并在 ReleaseBuffer 接口中进行内存释放。

```c++
struct GpuEncoderBuffer {
    FrameFormat format = FRAME_FORMAT_NONE;
    MemType memType = MEM_TYPE_NONE;
    FrameSize size = {};
    GpuType gpuType = GPU_NONE;
    uint8_t *data  = nullptr;
    uint32_t dataLen = 0;
};
using GpuEncoderBufferT = GpuEncoderBuffer *;
```

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：创建 Buffer 成功。
- 其他：创建 Buffer 失败。

#### ImportBuffer

**函数功能**

从外部导入一个 Buffer。

**函数原型**

`int32_t ImportBuffer(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| format | 输入 | FrameFormat | 要导入的 Buffer 对应的格式 |
| handle | 输入 | uint64_t | 要导入的 Buffer 对应的句柄或显存地址 |
| buffer | 输出 | GpuEncoderBufferT & | 导入完成后用于挂载 Buffer 的指针 |

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：导入 Buffer 成功。
- 其他：导入 Buffer 失败。

#### ReleaseBuffer

**函数功能**

释放一个 Buffer。

**函数原型**

`int32_t ReleaseBuffer(GpuEncoderBufferT &buffer) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| buffer | 输入 | GpuEncoderBufferT & | 需要释放的 Buffer 的指针 |

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：释放 Buffer 成功。
- 其他：释放 Buffer 失败。

#### MapBuffer

**函数功能**

将一个 Buffer 的数据映射到内存中。

- 若 Buffer 中的数据能够映射为用户态的连续内存，则该接口应该填写 Buffer 字段中的 data 以及 dataLen 字段。
- 若无法将 Buffer 数据映射为连续内存，则需要将 Buffer 字段中的 data 指针置空，并填写 dataLen 字段。

**函数原型**

`int32_t MapBuffer(GpuEncoderBufferT &buffer, uint32_t flag) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| buffer | 输出 | GpuEncoderBufferT & | 需要进行数据映射 Buffer 的指针 |
| flag | 输入 | uint32_t | 控制映射数据的访问类型，包含可读和可写 |

flag 取值范围说明，同时使能读写时 flag 值为 FLAG_READ | FLAG_WRITE。

```c++
enum MapFlag : uint32_t {
    FLAG_READ = 1,
    FLAG_WRITE = 1 << 1,
};
```

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：映射 Buffer 数据成功。
- 其他：映射 Buffer 数据失败。

#### UnmapBuffer

**函数功能**

解除一个 Buffer 的数据到内存的映射。

**函数原型**

`int32_t UnmapBuffer(GpuEncoderBufferT &buffer) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| buffer | 输入 | GpuEncoderBufferT & | 需要解除数据映射的 Buffer 的指针 |

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：解除 Buffer 数据映射成功。
- 其他：解除 Buffer 数据映射失败。

#### RetriveBufferData

**函数功能**

获取一个 Buffer 中的数据。

通常在 MapBuffer 无法将 Buffer 数据映射为连续内存时调用，该接口需要将 Buffer 中的数据拷贝到调用者提供的内存中。

**约束说明**

要求 Buffer 已经通过 MapBuffer 接口进行了数据映射。

**函数原型**

`int32_t RetriveBufferData(GpuEncoderBufferT &buffer, uint8_t *data, uint32_t memLen, uint32_t &dataLen) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| buffer | 输入 | GpuEncoderBufferT & | 需要进行数据拷贝的 Buffer 的指针 |
| data | 输入 | uint8_t * | 用于接收 Buffer 数据的内存空间地址 |
| memLen | 输入 | uint32_t | 用于接收 Buffer 数据的内存空间的最大长度 |
| dataLen | 输出 | uint32_t & | 实际拷贝数据的大小 |

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：获取 Buffer 数据成功。
- 其他：获取 Buffer 数据失败。

#### Convert

**函数功能**

调用 GPU 进行格式转换，将输入 Buffer 中的数据转换后写入输出 Buffer 中。

**函数原型**

`int32_t Convert(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| inBuffer | 输入 | GpuEncoderBufferT & | 需要进行格式转换的输入 Buffer |
| outBuffer | 输出 | GpuEncoderBufferT & | 接收格式转换后数据的输出 Buffer |

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：转换 Buffer 格式成功。
- 其他：转换 Buffer 格式失败。

#### Encode

**函数功能**

调用 GPU 进行视频编码，将输入 Buffer 中的数据编码后写入输出 Buffer 中。

**函数原型**

`int32_t Encode(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| inBuffer | 输入 | GpuEncoderBufferT & | 需要进行编码的输入 Buffer |
| outBuffer | 输出 | GpuEncoderBufferT & | 接收编码后数据的输出 Buffer |

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：编码成功。
- 其他：编码失败。

#### SetEncodeParam

**函数功能**

设置编码参数。

**函数原型**

`int32_t SetEncodeParam(EncodeParamT params[], uint32_t num) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| params | 输入 | EncodeParamT[] | 要设置的编码参数的列表 |
| num | 输入 | uint32_t | 要设置编码参数的数量 |

EncodeParamT 的相关定义如下：

```c++
enum EncodeParamIndex : uint32_t {
    ENCODE_PARAM_NONE,
    ENCODE_PARAM_RATE_CONTROL, //设置码控方式
    ENCODE_PARAM_BITRATE, //设置码率
    ENCODE_PARAM_FRAMERATE, //设置帧率
    ENCODE_PARAM_GOPSIZE, //设置gopsize
    ENCODE_PARAM_PROFILE, //设置profile
    ENCODE_PARAM_KEYFRAME, //设置关键帧
    ENCODE_PARAM_STREAM_WIDTH,//设置出流宽度
    ENCODE_PARAM_STREAM_HEIGHT,//设置出流高度
    ENCODE_PARAM_MAX
};
struct EncodeParamBase {
    explicit EncodeParamBase(EncodeParamIndex index) : id(index) {}
    EncodeParamIndex id;
};
struct EncodeParamRateControl : public EncodeParamBase {
    EncodeParamRateControl() : EncodeParamBase(ENCODE_PARAM_RATE_CONTROL) {}
    uint32_t rateControl = 0;
};
struct EncodeParamBitRate : public EncodeParamBase {
    EncodeParamBitRate() : EncodeParamBase(ENCODE_PARAM_BITRATE) {}
    uint32_t bitRate = 0;
};
struct EncodeParamFrameRate : public EncodeParamBase {
    EncodeParamFrameRate() : EncodeParamBase(ENCODE_PARAM_FRAMERATE) {}
    uint32_t frameRate;
};
struct EncodeParamGopsize : public EncodeParamBase {
    EncodeParamGopsize() : EncodeParamBase(ENCODE_PARAM_GOPSIZE) {}
    uint32_t gopSize;
};
struct EncodeParamProfile : public EncodeParamBase {
    EncodeParamProfile() : EncodeParamBase(ENCODE_PARAM_PROFILE) {}
    uint32_t profile;
};
struct EncodeParamKeyframe : public EncodeParamBase {
    EncodeParamKeyframe() : EncodeParamBase(ENCODE_PARAM_KEYFRAME) {}
    uint32_t n;
};
struct EncodeParamStreamWidth : public EncodeParamBase {
    EncodeParamStreamWidth() : EncodeParamBase(ENCODE_PARAM_STREAM_WIDTH) {}
    uint32_t streamWidth;
};
struct EncodeParamStreamHeight : public EncodeParamBase {
    EncodeParamStreamHeight() : EncodeParamBase(ENCODE_PARAM_STREAM_HEIGHT) {}
    uint32_t streamHeight;
};
using EncodeParamT = EncodeParamBase *;
```

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：设置编码参数成功。
- ERR_NEED_RESET：需要调用 Reset 接口后编码参数才能生效。
- 其他：设置编码参数失败。

#### Reset

**函数功能**

重置编码器，部分编码参数设置需要在本接口调用后生效。

**约束说明**

无。

**函数原型**

`int32_t Reset() = 0`

**返回值说明**

数据类型：`GpuEncoderErrorCode`

取值如下：

- OK：重置编码器成功。
- 其他：重置编码器失败。

#### ResetImgSize

**函数功能**

根据抓图图层尺寸，设置保存的图像尺寸。设置后需重启 GPU Turbo 生效。

**约束说明**

入参 width/height 必须为偶数。

**函数原型**

`int32_t ResetImgSize(uint32_t width, uint32_t height) = 0`

**参数说明**

|字段名称|输入/输出|字段类型|字段描述|
|--|--|--|--|
| width | 输入 | uint32_t | 要设置的图像宽度（单位为像素，必须为偶数） |
| height | 输入 | uint32_t | 要设置的图像高度（单位为像素，必须为偶数） |

**返回值说明**

数据类型：`int32_t`

取值如下：

- 0：设置图像尺寸成功。
- 其他：设置图像尺寸失败。

### Opus编解码接口（C&C++）

#### 接口说明

视频流音频播放和麦克风输入功能支持 Opus 格式码流输入和输出，该功能需要对应的 Opus 编解码库提供编解码能力支持，该库由二次开发者实现，并以动态链接库的形式提供。其动态库的名称为：libVmiOpus.so。

该系列接口与开源软件 [Opus 1.3.1](https://github.com/xiph/opus/tree/v1.3.1) 版本类似，可以参考其 API 和数据接口定义。

#### OpusEncoderCreateApi

**函数功能**

创建 Opus 编码器，类似 Opus 1.3.1 版本的 opus_encoder_create 接口。

**函数原型**

`OpusEncoder *OpusEncoderCreateApi(opus_int32 Fs, int channels, int application, int *error)`

**参数说明**

与 Opus 1.3.1 中 opus_encoder_create 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_encoder_create 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusEncodeApi

**函数功能**

Opus 编码接口，类似 Opus 1.3.1 版本的 opus_encode 接口。

**函数原型**

`opus_int32 OpusEncodeApi(OpusEncoder *st, const opus_int16 *pcm, int frame_size, unsigned char *data, opus_int32 max_data_bytes)`

**参数说明**

与 Opus 1.3.1 中 opus_encode 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_encode 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusEncoderCtlApi

**函数功能**

Opus 编码控制接口，类似 Opus 1.3.1 版本的 opus_encoder_ctl 接口。

**函数原型**

`int OpusEncoderCtlApi(OpusEncoder *st, int request, opus_int32 par)`

**参数说明**

与 Opus 1.3.1 中 opus_encoder_ctl 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_encoder_ctl 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusEncoderDestroyApi

**函数功能**

Opus 编码器销毁接口，类似 Opus 1.3.1 版本的 opus_encoder_destroy 接口。

**函数原型**

`void OpusEncoderDestroyApi(OpusEncoder *st)`

**参数说明**

与 Opus 1.3.1 中 opus_encoder_destroy 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_encoder_destroy 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusDecoderCreateApi

**函数功能**

创建 Opus 解码器，类似 Opus 1.3.1 版本的 opus_decoder_create 接口。

**函数原型**

`OpusDecoder *OpusDecoderCreateApi(opus_int32 Fs, int channels, int *error)`

**参数说明**

与 Opus 1.3.1 中 opus_decoder_create 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_decoder_create 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusDecodeApi

**函数功能**

Opus 解码接口，类似 Opus 1.3.1 版本的 opus_decode 接口。

**函数原型**

`int OpusDecodeApi(OpusDecoder *st, const unsigned char *data, opus_int32 len, opus_int16 *pcm, int frame_size, int decode_fec)`

**参数说明**

与 Opus 1.3.1 中 opus_decode 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_decode 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusDecoderCtlApi

**函数功能**

Opus 解码控制接口，类似 Opus 1.3.1 版本的 opus_decoder_ctl 接口。

**函数原型**

`int OpusDecoderCtlApi(OpusDecoder *st, int request, opus_int32 par)`

**参数说明**

与 Opus 1.3.1 中 opus_decoder_ctl 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_decoder_ctl 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusDecoderDestroyApi

**函数功能**

Opus 解码器销毁接口，类似 Opus 1.3.1 版本的 opus_decoder_destroy 接口。

**函数原型**

`void OpusDecoderDestroyApi(OpusDecoder *st)`

**参数说明**

与 Opus 1.3.1 中 opus_decoder_destroy 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_decoder_destroy 接口一致，请参见 Opus 1.3.1 中定义。

#### OpusStrerrorApi

**函数功能**

通过错误码获取文字错误信息接口，类似 Opus 1.3.1 版本的 opus_strerror 接口。

**函数原型**

`const char *OpusStrerrorApi(int error)`

**参数说明**

与 Opus 1.3.1 中 opus_strerror 接口一致，请参见 Opus 1.3.1 中定义。

**返回值说明**

与 Opus 1.3.1 中 opus_strerror 接口一致，请参见 Opus 1.3.1 中定义。
