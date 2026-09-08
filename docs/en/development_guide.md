# Developer Guide<a name="ZH-CN_TOPIC_0000002521735642"></a>

## 1 Introduction<a name="ZH-CN_TOPIC_0000002549826163"></a>

A cloud phone is a cloud server with an Android Open Source Project (AOSP) and functions as a virtual phone. It extends the functions of a physical phone and can be used in various scenarios such as cloud mobile gaming and mobile office.

The device-cloud engine consists of the device side and the cloud side. The cloud side runs on a server; the device side is generally a cloud phone APK, which can be installed on your Android mobile phone to interact with the cloud side and operate the container on the cloud.

This document describes the integrated development of the device and cloud engines used in the video stream cloud phone.

## 2 Obtaining Software Packages<a name="obtaining-software-packages"></a>

**Table 1** Software requirements<a id="software-requirements"></a>

|No.|Software|Description|How to Obtain|
|--|--|--|--|
|1|AOSP source code|Version: android-15.0.0_r17|[Link](https://android.googlesource.com/platform/manifest)|
|2|Android NDK|Version: r25b|[Link](https://dl.google.com/android/repository/android-ndk-r25b-linux.zip)|
|3|SDK platform|Version: 33_r02|[Link](https://dl.google.com/android/repository/platform-33_r02.zip)|
|4|SDK platform tools|Version: r33.0.3|[Link](https://dl.google.com/android/repository/platform-tools_r33.0.3-linux.zip)|
|5|SDK build tools|Version: r33.0.1|[Link](https://dl.google.com/android/repository/build-tools_r33.0.1-linux.zip)|
|6|BoostKit-boostcph-videoengine_*_15.zip|Android 15 video stream engine development kit|Please submit an ISSUE feedback.|

**Verifying Software Package Integrity<a name="section16873181764512"></a>**

To prevent software packages from being maliciously tampered with during transfer or storage, download also the corresponding digital signature files for integrity verification while obtaining the software packages from the Kunpeng community.

1. Obtain the software packages by referring to [**Table 1** Software requirements](#software-requirements).
2. <a name="li1273482318125"></a>Obtain the verification tool and guide from the [Huawei enterprise website](https://support.huawei.com/enterprise/en/tool/pgp-verify-TL1000000054) or [Huawei carrier website](http://support.huawei.com/carrier/digitalSignatureAction).
3. Based on the *OpenPGP Signature Verification Guide* obtained in [2](#li1273482318125), verify the PGP digital signatures of the software packages.

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>If the verification fails, do not use the software package. Please submit an ISSUE feedback.
>Before a software package is used for installation or upgrade, its digital signature also needs to be verified to ensure that the software package is not tampered with.
>Before using the software package, read and agree to [Kunpeng BoostKit User License Agreement 2.0](https://www.hikunpeng.com/en/legal/developer/boostkit/software/protocol).

## 3 Configuring the Development Environment<a name="ZH-CN_TOPIC_0000002518186398"></a>

### 3.1 Decompressing the Software Package<a name="ZH-CN_TOPIC_0000002518346310"></a>

The software package of the video stream engine is `BoostKit-boostcph-videoengine_*_15.zip`. To obtain it, see [2-Obtaining Software Packages](#obtaining-software-packages). After obtaining the .zip package, decompress it to obtain the .tar.gz package.

|Software|Description|
|--|--|
| VideoEngine.tar.gz | Development package of the video stream engine server.|

### 3.2 Deploying the Server Compilation Environment<a name="ZH-CN_TOPIC_0000002518346308"></a>

**Environment Requirements<a name="section183915472144"></a>**

This document describes how to compile the server software based on the x86_64 server running Ubuntu 22.04 LTS. Before the compilation, ensure that your hardware environment meets the requirements.

[**Table 1**](#hardware-environment) lists the hardware environment requirements for compiling and building the server software.

**Table 1** Hardware environment<a id="hardware-environment"></a>

|Device Model|Function|Server OS|
|--|--|--|
|x86_64 server|Used to compile and build the server software.|Recommended: Ubuntu 22.04 LTS ([ubuntu-22.04-live-server-amd64.iso](https://old-releases.ubuntu.com/releases/22.04/ubuntu-22.04-live-server-amd64.iso))|

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>- In this document, the server model is 2288H V5.
>- Ensure that the server can access the Internet to download the OS image.

**Deployment Procedure<a name="section039914210159"></a>**

1. Install the dependencies.

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

2. Download the VMI code and go to the `vmi` directory.

    ```shell
    git clone https://gitee.com/kunpengcompute/vmi.git
    cd vmi
    ```

3. Use the automatic deployment script to deploy the compilation tool.

    ```shell
    chmod +x scripts/auto_install_tools.sh
    ./scripts/auto_install_tools.sh ${installation_directory}
    ```

    *${installation_directory}* indicates the directory where the compilation tool is downloaded and decompressed. This directory can be customized. If you do not specify an installation directory, the script uses the default directory `~/NativeCompileToolsDir`.

    After the script is executed successfully, import environment variables.

    ```shell
    source ~/.bashrc
    ```

### 3.3 Deploying the Server Debugging Environment<a name="ZH-CN_TOPIC_0000002549826179"></a>

>![](public_sys-resources/icon-notice.gif) **NOTICE**
>
>- You can customize a directory for storing the package.
>- For details about the hardware requirements and deployment process of the server debugging environment, refer to [Video Stream Engine installation Guide (Android 15)](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/videostreamengine_ad15/docs/en/install_guide.md).

1. Save the `VideoEngine.tar.gz` package to the `/home/VideoEngine/Cloud` directory.
2. Decompress the development package of the video stream engine server.

    ```shell
    tar xzvf VideoEngine.tar.gz
    ```

3. Copy the extracted binary files to a cloud phone (for example, `cloudphone_1`).

    ```shell
    docker cp system/bin cloudphone_1:/system/
    docker cp system/etc cloudphone_1:/system/
    docker cp system/lib cloudphone_1:/system/
    docker cp system/lib64 cloudphone_1:/system/
    docker cp vendor/lib cloudphone_1:/vendor/
    docker cp vendor/lib64 cloudphone_1:/vendor/
    docker cp vendor/etc cloudphone_1:/vendor/
    ```

4. Copy the external dependency libraries to a cloud phone (`cloudphone_1` for example). [**Table 1**](#external-dependency-libraries-of-the-video-stream-cloud-phone) lists the required dependency libraries.

    **Table 1** External dependency libraries of the video stream cloud phone<a id="external-dependency-libraries-of-the-video-stream-cloud-phone"></a>

    |Item|Library Name|Paths|
    |--|--|--|
    |Encoding dependency library|libVideoCodec.so|/vendor/lib, /vendor/lib64|
    |Decoding dependency library|libVideoDecoder.so|/vendor/lib, /vendor/lib64|
    |GPU acceleration dependency library|libVmiEncTurbo.so|/vendor/lib, /vendor/lib64|
    |GPU acceleration dependency library (system permission)|libVmiEncTurboSys.so|/system/lib, /system/lib64|
    |Opus encoding and decoding library|libVmiOpus.so|/system/lib, /system/lib64|

5. Restart the cloud phone, and then the video stream cloud phone can run. You can debug the functions by calling the corresponding APIs according to the subsequent development process.

## 4 External APIs<a name="ZH-CN_TOPIC_0000002549706167"></a>

### 4.1 Conventions<a name="ZH-CN_TOPIC_0000002549706165"></a>

The dynamic libraries provided by external interfaces are compiled using the AOSP 15.0.0_r17 source code and must be used in the AOSP 15.0.0_r17 environment.

External interfaces are public for all modules to exchange data. Data type definitions are provided in this document. For example, you can refer to [5-Video Output Development](#video-output-development) to learn about video output data.

### 4.2 Basic Data Types<a name="ZH-CN_TOPIC_0000002518186412"></a>

#### 4.2.1 VmiDataType (Module Data Type)<a name="ZH-CN_TOPIC_0000002549826161"></a>

`VmiDataType` defines the supported data types. Each data type corresponds to a module. Available modules include: video output, audio output, microphone input, touch input, sensor, and GPS. The data types are defined as follows:

```c++
enum VmiDataType : uint8_t {
    DATA_VIDEO = 0,                                  // Stream output module
    DATA_AUDIO,                                      // Audio playback module
    DATA_TOUCH,                                      // Touch and keyboard module
    DATA_MIC,                                        // Microphone module
    DATA_SENSOR,                                     // Sensor module
    DATA_GPS,                                        // GPS module
    DATA_TYPE_MAX
};
```

#### 4.2.2 VmiModuleStatus (Module Status)<a name="ZH-CN_TOPIC_0000002518186404"></a>

Enumeration of `VmiModuleStatus`:

```c++
enum VmiModuleStatus : uint32_t {
    MODULE_NOT_INIT = 0,                             // The module is not initialized.
    MODULE_INITED,                                   // The module is initialized.
    MODULE_STARTED,                                  // The module is started.
    MODULE_STOPED,                                   // The module is stopped.
};
```

#### 4.2.3 VmiVersion (Module Version)<a name="vmiversion-module-version"></a>

The version number of a module consists of the module type and the minor version number of the module. This API can be used to support module-level version control. The definition is as follows:

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

#### 4.2.4 VmiErrCode (Error Code)<a name="ZH-CN_TOPIC_0000002549706147"></a>

Enumeration of `VmiErrCode`:

```c++
enum VmiErrCode : int32_t {
    OK = 0,
    ERR_VERSION_MISMATCH = 0x1,                      // Incompatible version
    ERR_ILLEGAL_INPUT,                               // Invalid input parameter
    ERR_INVALID_CMD,                                 // Unsupported command
    ERR_INVALID_STATUS,                              // Unsupported operation in the current status
    ERR_INVALID_MODULE,                              // Unsupported module
    ERR_NOT_ENOUGH_RESOURCE,                         // Insufficient resource for completing the operation
    ERR_INTERNAL,                                    // Internal error
    ERR_REPEAT_INITIALIZATION,                       // Repeated initialization
    ERR_MODULE_NOT_INIT,                             // Uninitialized module
    ERR_MODULE_NOT_START,                            // Module not started
    ERR_MODULE_REPEAT_START,                         // Module started repeatedly
    ERR_MODULE_REPEAT_STOP,                          // Module stopped repeatedly
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

#### 4.2.5 VmiCmd (Command Word)<a name="ZH-CN_TOPIC_0000002518346276"></a>

Command words need to be specified as interface input parameters for functions such as module data input, data output, and parameter setting. `VmiCmd` is an enumeration type of uint32_t. Each 32-bit enumeration value consists of three parts that are sequentially concatenated as follows: `VmiDataType` (module data type, uint8_t), `VmiCmdType` (command word type, uint8_t), and specific command word (such as `VmiVideoCmdId` and `VmiAudioCmdId`, uint16_t). The definition is as follows:

```c++
enum VmiCmdType : uint8_t {
    CMD_SET_PARAM = 0,                               // Parameter setting
    CMD_GET_PARAM,                                   // Parameter obtaining
    CMD_TRANS_DATA,                                  // Data transmitting
};
enum VmiVideoCmdId : uint16_t {
    SET_ENCODER_PARAM = 0,                           // The client sets encoding parameters to the server.
    RETURN_VIDEO_DATA,                               // The server returns video data to the client.
    GET_ENCODER_PARAM,                               // Obtains the encoding parameters of the video module.
};
enum VmiAudioCmdId : uint16_t {
    SET_AUDIOPLAY_PARAM = 0,                         // The client sets audio playback parameters to the server.
    RETURN_AUDIO_PLAY_DATA,                          // The server sends audio playback data to the client.
    SET_CLIENT_VOLUME,                               // The server sends the volume size to the client.
    GET_AUDIOPLAY_PARAM,                             // The client obtains audio playback parameters from the server.
};
enum VmiMicCmdId : uint16_t {
    SEND_MIC_DATA = 0,                               // The client sends microphone data to the server.
    RETURN_OPEN_CLIENT_MIC,                          // The server notifies the client that the microphone is opened.
    RETURN_CLOSE_CLIENT_MIC,                         // The server notifies the client that the microphone is closed.
};
enum VmiTouchCmdId : uint16_t {
    SEND_TOUCH_EVENT = 0,                            // The client sends touch data to the server.
    SEND_KEY_EVENT,                                  // The client sends keyboard data to the server.
};
enum VmiSensorCmdId : uint16_t {
    SEND_SENSOR_DATA = 0,                            // The client sends sensor data to the server.
    RETURN_REGISTER_CLIENT_SENSOR,                   // The server notifies the client to register sensor listening.
    RETURN_UNREGISTER_CLIENT_SENSOR,                 // The server notifies the client to unregister sensor listening.
    RETURN_UPDATE_CLIENT_SENSOR_RATE,                // The server notifies the client to update the sensor sampling rate.
};
enum VmiGpsCmdId : uint16_t {
    SEND_LOCATION_DATA = 0,                            // The client sends GPS location data to the server.
    SEND_NMEA_DATA,                                    // The client sends NMEA data to the server.
    RETURN_START_GPS,                                  // The server notifies the client to start to send GPS data.
    RETURN_STOP_GPS,                                   // The server notifies the client to stop sending GPS data.
};
#define MAKE_CMD(dataType, cmdType, cmdId)  ((dataType) << 24 | (cmdType) << 16 | (cmdId))
enum VmiCmd : uint32_t {
    // CMD definition of the video module
    VIDEO_SET_ENCODER_PARAM = MAKE_CMD(DATA_VIDEO, CMD_SET_PARAM, SET_ENCODER_PARAM),      // Refer to EncodeParams for parameter formats.
    VIDEO_RETURN_VIDEO_DATA = MAKE_CMD(DATA_VIDEO, CMD_TRANS_DATA, RETURN_VIDEO_DATA),     // Refer to VideoData for data formats.
    VIDEO_GET_ENCODER_PARAM = MAKE_CMD(DATA_VIDEO, CMD_GET_PARAM, GET_ENCODER_PARAM),      // Refer to EncodeParams for parameter formats.
    // CMD definition of the audio module
    AUDIO_SET_AUDIOPLAY_PARAM = MAKE_CMD(DATA_AUDIO, CMD_SET_PARAM, SET_AUDIOPLAY_PARAM),  // Refer to AudioPlayParams for parameter formats.
    AUDIO_GET_AUDIOPLAY_PARAM = MAKE_CMD(DATA_AUDIO, CMD_GET_PARAM, GET_AUDIOPLAY_PARAM),
    AUDIO_RETURN_AUDIO_PLAY_DATA = MAKE_CMD(DATA_AUDIO, CMD_TRANS_DATA, RETURN_AUDIO_PLAY_DATA),  // Refer to AudioData for data formats.
    AUDIO_SET_CLIENT_VOLUME = MAKE_CMD(DATA_AUDIO, CMD_SET_PARAM, SET_CLIENT_VOLUME),
    // CMD definition of the microphone module
    MIC_SEND_MIC_DATA = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, SEND_MIC_DATA),
    MIC_RETURN_OPEN_CLIENT_MIC = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, RETURN_OPEN_CLIENT_MIC),
    MIC_RETURN_CLOSE_CLIENT_MIC = MAKE_CMD(DATA_MIC, CMD_TRANS_DATA, RETURN_CLOSE_CLIENT_MIC),
    // CMD definition of the touch module
    TOUCH_SEND_TOUCH_EVENT = MAKE_CMD(DATA_TOUCH, CMD_TRANS_DATA, SEND_TOUCH_EVENT),    // Refer to VmiTouchInputData for data formats.
    TOUCH_SEND_KEY_EVENT = MAKE_CMD(DATA_TOUCH, CMD_TRANS_DATA, SEND_KEY_EVENT),        // Refer to VmiKeyInputData for data formats.
    // CMD definition of the sensor module
    SENSOR_SEND_SENSOR_DATA = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, SEND_SENSOR_DATA),  // Refer to SensorData for data formats.
    SENSOR_RETURN_REGISTER_CLIENT_SENSOR = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, RETURN_REGISTER_CLIENT_SENSOR),
    SENSOR_RETURN_UNREGISTER_CLIENT_SENSOR = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, RETURN_UNREGISTER_CLIENT_SENSOR),
    SENSOR_RETURN_UPDATE_CLIENT_SENSOR_RATE = MAKE_CMD(DATA_SENSOR, CMD_TRANS_DATA, RETURN_UPDATE_CLIENT_SENSOR_RATE),
    // CMD definition of the GPS module
    GPS_SEND_LOCATION_DATA = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, SEND_LOCATION_DATA), // Refer to VmiGPSLocationData for data formats.
    GPS_SEND_NMEA_DATA = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, SEND_NMEA_DATA),         // Refer to VmiGPSLocationData for data formats.
    GPS_RETURN_START_GPS = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, RETURN_START_GPS),
    GPS_RETURN_STOP_GPS = MAKE_CMD(DATA_GPS, CMD_TRANS_DATA, RETURN_STOP_GPS),
};
```

### 4.3 Function APIs<a name="ZH-CN_TOPIC_0000002518346322"></a>

#### 4.3.1 GetVersion<a name="ZH-CN_TOPIC_0000002518186356"></a>

**Function Usage<a name="section713mcpsimp"></a>**

Obtains the product version number.

**Prototype<a name="section719mcpsimp"></a>**

const char \*GetVersion\(\)

**Return Value Description<a name="section725mcpsimp"></a>**

Data type: char \*

A return value example is provided in the "Querying Component Version Information" section in [Video Stream Engine User Guide (Android 15)](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/videostreamengine_ad15/docs/en/user_guide.md#d12-querying-component-version-information).

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>The version number in the example command output is irrelevant to the `VmiVersion` module version number.

#### 4.3.2 InitVmiEngine<a name="ZH-CN_TOPIC_0000002518346282"></a>

**Function Usage<a name="section730mcpsimp"></a>**

Initializes the cloud phone server.

**Restrictions<a name="section733mcpsimp"></a>**

- This API cannot be called repeatedly. That is, it cannot be called after the initialization is complete.
- The data output callback cannot be blocked for a long time. It is recommended that the callback return within 1 ms.

**Prototype<a name="section736mcpsimp"></a>**

VmiErrCode InitVmiEngine\(VmiConfigEngine \*config\);

**Parameters<a name="section739mcpsimp"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| config | Input| VmiConfigEngine | This structure consists of the `DataCallback` and `DataTypeConfig` arrays.|

The `VmiConfigEngine` structure is required for starting the engine. It includes `DataCallback` and the list of modules to be started.

```c++
struct VmiConfigEngine {
    DataCallback dataCallback = nullptr;        // Callback for sending server data.
    DataTypeConfig dataTypeConfig[DATA_TYPE_MAX];
} __attribute__((packed));
```

No function interface is provided for data output. Instead, `DataCallback` is used. `DataCallback` definition:

```c++
using DataCallback = int(*)(VmiDataType module, VmiCmd cmd, uint8_t *data, uint32_t size);
```

The `DataTypeConfig` array specifies whether each module needs to be initialized and the size of the memory space to be reserved before the data packet header during initialization, making it easier for developers to fill in required data.

```c++
struct DataTypeConfig {
    bool shouldInit = false;
    uint32_t sendDataOffset = 0;                // Space (in bytes) reserved before the data packet header when the callback is used to send data. The value range is 0 to 1024.
} __attribute__((packed));
```

If `sendDataOffset` is not `0`, the memory layout of the `data` pointer (the third parameter) of the `dataCallback` API is as follows: *N* bytes of reserved data (specified by `sendDataOffset`) + *N* bytes of valid data (specified by the fourth parameter `size`).

**Return Value Description<a name="section791mcpsimp"></a>**

Data type: enum VmiErrCode: int32_t

The value can be any of the following:

- `OK (0)`: The initialization is successful.
- Other values: The initialization fails, and an error code is returned.

#### 4.3.3 DeInitVmiEngine<a name="ZH-CN_TOPIC_0000002549826123"></a>

**Function Usage<a name="section799mcpsimp"></a>**

Deinitializes the cloud phone server.

**Restrictions<a name="section802mcpsimp"></a>**

This API cannot be called repeatedly.

**Prototype<a name="section805mcpsimp"></a>**

VmiErrCode DeInitVmiEngine\(\);

**Return Value Description<a name="section811mcpsimp"></a>**

Data type: enum VmiErrCode: int32_t

The value can be any of the following:

- `OK (0)`: The deinitialization is successful.
- Other values: The deinitialization fails, and an error code is returned.

#### 4.3.4 GetStatus<a name="ZH-CN_TOPIC_0000002518346306"></a>

**Function Usage<a name="section819mcpsimp"></a>**

Obtains module status.

**Prototype<a name="section825mcpsimp"></a>**

VmiModuleStatus GetStatus\(VmiDataType module\)

**Parameters<a name="section828mcpsimp"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| module | Input| VmiDataType | Module data type.|

**Return Value Description<a name="section856mcpsimp"></a>**

Data type: enum VmiModuleStatus: uint32_t

#### 4.3.5 StartModule<a name="ZH-CN_TOPIC_0000002549706125"></a>

**Function Usage<a name="section869mcpsimp"></a>**

Starts the module.

**Prototype<a name="section875mcpsimp"></a>**

VmiErrCode StartModule\(VmiDataType module, uint8_t\*config, uint32_t size\)

**Parameters<a name="section878mcpsimp"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| module | Input| VmiDataType | Module data type.|
| config | Input| uint8_t* | Configuration used for module startup.|
| size | Input| uint32_t | Length of the memory pointed to by `config`.|

`config` varies with `module` and is inherited from `VmiConfig`. `VmiConfigVideo` corresponds to the video module, `VmiConfigAudio` to the audio module, `VmiConfigTouch` to the touch module, `VmiConfigMic` to the microphone module, and `VmiConfig` to the sensor and GPS modules.

```c++
struct VmiConfig {
    uint32_t version;                           // Set this parameter to the current version number of each module. For details, see the VmiVersion structure.
} __attribute__((packed));
```

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>`VmiConfig` needs to transfer the module version number mentioned in [4.2.3-VmiVersion (Module Version)](#vmiversion-module-version). The version number is specific to each module.

**Return Value Description<a name="section916mcpsimp"></a>**

Data type: enum VmiErrCode: int32_t

The value can be any of the following:

- `OK (0)`: The module is successfully started.
- Other values: The module fails to be started, and an error code is returned.

#### 4.3.6 StopModule<a name="ZH-CN_TOPIC_0000002518186350"></a>

**Function Usage<a name="section923mcpsimp"></a>**

Stops the module.

**Prototype<a name="section929mcpsimp"></a>**

VmiErrCode StopModule\(VmiDataType module\)

**Parameters<a name="section932mcpsimp"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| module | Input| VmiDataType | Module data type.|

**Return Value Description<a name="section960mcpsimp"></a>**

Data type: enum VmiErrCode: int32_t

The value can be any of the following:

- `OK (0)`: The module is successfully stopped.
- Other values: The module fails to be stopped, and an error code is returned.

#### 4.3.7 InjectData<a name="ZH-CN_TOPIC_0000002518346324"></a>

**Function Usage<a name="section968mcpsimp"></a>**

Injects data.

**Prototype<a name="section974mcpsimp"></a>**

VmiErrCode InjectData\(VmiDataType module, VmiCmd cmd, uint8_t \*data, uint32_t size\)

**Parameters<a name="section977mcpsimp"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| module | Input| VmiDataType | Module data type.|
| cmd | Input| VmiCmd | Operation instruction during data injection.|
| data | Input| uint8_t* | A pointer pointing to data to be sent.|
| size | Input| uint32_t | Size of the data to be injected.|

**Return Value Description<a name="section1031mcpsimp"></a>**

Data type: enum VmiErrCode: int32_t

The value can be any of the following:

- `OK (0)`: The data injection is successful.
- Other values: The data injection fails, and an error code is returned.

#### 4.3.8 SetParam<a name="ZH-CN_TOPIC_0000002518186368"></a>

**Function Usage<a name="section1039mcpsimp"></a>**

Sets parameters.

**Prototype<a name="section1045mcpsimp"></a>**

VmiErrCode SetParam\(VmiDataType module, VmiCmd cmd, uint8_t \*param, uint32_t size\)

**Parameters<a name="section1048mcpsimp"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| module | Input| VmiDataType | Module data type.|
| cmd | Input| VmiCmd | Operation instruction for setting the parameter values.|
| param | Input| uint8_t* | Pointer to the parameters to be set.|
| size | Input| uint32_t | Size of the data to be set.|

**Return Value Description<a name="section1103mcpsimp"></a>**

Data type: enum VmiErrCode: int32_t

The value can be any of the following:

- `OK (0)`: The parameters are set successfully.
- Other values: The parameters fail to be set, and an error code is returned.

#### 4.3.9 GetParam<a name="ZH-CN_TOPIC_0000002518186372"></a>

**Function Usage<a name="section1039mcpsimp"></a>**

Obtains the parameter values of a module.

**Prototype<a name="section1045mcpsimp"></a>**

VmiErrCode GetParam\(VmiDataType module, VmiCmd cmd, uint8_t \*param, uint32_t size\)

**Parameters<a name="section1048mcpsimp"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| module | Input| VmiDataType | Module data type.|
| cmd | Input| VmiCmd | Operation instruction for obtaining the parameter values.|
| param | Input| uint8_t* | Pointer to the output parameter data.|
| size | Input| uint32_t | Size of the output data.|

**Return Value Description<a name="section1103mcpsimp"></a>**

Data type: enum VmiErrCode: int32_t

The value can be any of the following:

- `OK (0)`: The parameters are obtained successfully.
- Other values: The parameters fail to be obtained, and an error code is returned.

## 5 Video Output Development<a name="video-output-development"></a>

### 5.1 Conventions<a name="ZH-CN_TOPIC_0000002518346312"></a>

The video output (VO) configuration structure `VmiConfigVideo` is inherited from the `VmiConfig` structure. The structure is as follows:

```c++
struct VmiConfigVideo : public VmiConfig {
    EncoderType encoderType = CPU;              // Encoder type
    VideoFrameType videoFrameType = H264;       // VO format
    FrameSize resolution;                       // Snapshot resolution
    uint32_t density = 320;                     // Screen density
    bool renderOptimize = true;                 // Rendering optimization (enabled by default and cannot be disabled currently)
    EncodeParams encodeParams;                  // Initial encoding parameters
} __attribute__((packed));
```

### 5.2 Configuration Parameters<a name="video-configuration-parameters"></a>

#### 5.2.1 encoderType<a name="ZH-CN_TOPIC_0000002518186370"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

Encoder type.

**Variable Data Definition<a name="section1137mcpsimp"></a>**

```c++
enum EncoderType : uint32_t {
    CPU,                                        // Software encoding
    VPU,                                        // Hardware-based acceleration via encoding cards
    GPU,                                        // Hardware-based acceleration via GPUs
    ENCODE_TYPE_MAX
};
```

**Restrictions<a name="section2011449181"></a>**

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
|encoderType|Encoder type.|`0`: CPU (software encoding via CPU); `1`: VPU (hardware encoding via external hardware); `2`: GPU (available only when DC1000 is used)|0|

#### 5.2.2 videoFrameType<a name="ZH-CN_TOPIC_0000002518346298"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

VO type.

**Variable Data Definition<a name="section1137mcpsimp"></a>**

```c++
enum VideoFrameType {
    H264,                                             // H.264 (default)
    YUV,                       // YV12
    RGB,                       // RGBA8888 (not supported currently)
    H265,
    FRAME_TYPE_MAX
};
```

**Restrictions<a name="section686412814243"></a>**

RGBA8888 in RGB is not supported currently.

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
| videoFrameType | Video data output format.| `0`: H.264; `1`: YUV (supported only when `encoderType` is set to `0`); `2`: RGB (not supported currently); `3`: H.265 (not supported when `encoderType` is set to `0`)| 0|

#### 5.2.3 resolution<a name="ZH-CN_TOPIC_0000002549826169"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

Snapshot resolution.

**Variable Data Definition<a name="section1510552312316"></a>**

```c++
struct FrameSize {
    uint32_t width = 720;               // 720 (defaults to 720p)
    uint32_t height = 1280;             // 1280 (defaults to 720p)
    uint32_t widthAligned = 720;        // 720 (not aligned by default)
    uint32_t heightAligned = 1280;      // 1280 (not aligned by default)
} __attribute__((packed));
```

**Restrictions<a name="section1134mcpsimp"></a>**

The value ranges of `width` and `height` can be exchanged.

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
| width | Width of the adaptive resolution. The value must be a multiple of 8.| 360 to 2160| 720 |
| height | Height of the adaptive resolution. The value must be a multiple of 8.| 360 to 3840| 1280 |
| widthAligned | Aligned width of the resolution (not configurable currently).| 360 to 2160| 720 |
| heightAligned | Aligned height of the resolution (not configurable currently).| 360 to 3840| 1280 |

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>When adaptive resolution is enabled, after you change the video output resolution (to a resolution different from the configuration at the last startup), the rendering resolution of the AOSP system and apps is changed. In this case, a compatibility issue or rendering problem may occur in some apps. Generally, this problem can be solved by restarting apps. It is recommended that you return to the home screen and clear background apps before changing the resolution.

#### 5.2.4 density<a name="ZH-CN_TOPIC_0000002549706153"></a>

**Variable Description<a name="section111994488367"></a>**

Screen density.

**Variable Data Definition<a name="section11711586375"></a>**

The value is of the uint32_t type and defaults to `320`.

**Restrictions<a name="section2021618526388"></a>**

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
| density | Screen density.| [120, 960] `0`: The adaptive resolution function is disabled.| 320 |

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>1. When the `density` property is set to `0`, adaptive resolution is disabled. That is, when the video module is started, the existing rendering resolution and screen density of the cloud phone are used for rendering and stream output.
>2. When `ro.vmi.video.wmcmd` (an Android property) is set to `0`, adaptive resolution is also disabled. That is, if either this property or `density` is set to `0`, adaptive resolution is disabled.

#### 5.2.5 renderOptimize<a name="ZH-CN_TOPIC_0000002549706119"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

Rendering optimization flag. If enabled, the system rendering load can be reduced.

**Restrictions<a name="section1134mcpsimp"></a>**

Currently, this parameter cannot be disabled.

**Variable Data Definition<a name="section1137mcpsimp"></a>**

The value is of the Boolean type and defaults to `true`, indicating that rendering optimization is performed by default.

#### 5.2.6 encodeParams<a name="ZH-CN_TOPIC_0000002549706179"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

Encoding parameters.

**Variable Data Definition<a name="section1582613465553"></a>**

```c++
struct EncodeParams {
    uint32_t bitrate = 3000000;             // Bit rate
    uint32_t gopSize = 30;                  // I-frame interval
    ProfileType profile = BASELINE;         // Encoding complexity
    RCMode rcMode = CBR;                    // Rate control mode
    uint32_t forceKeyFrame = 0;             // Forcibly generate an I-frame in frame N after the setting. 0 indicates that the setting does not take effect.
    bool interpolation = false;             // Frame interpolation
    uint32_t crf = 34;                      // CRF bit rate control level
    uint32_t maxCrfRate = 20000000;         // Maximum CRF bit rate
    int32_t vbvBufferSize = 1000;           // Size of the CRF bit rate buffer
    uint32_t streamWidth = 720;             // Width of the output stream resolution
    uint32_t streamHeight = 1280;           // Height of the output stream resolution
} __attribute__((packed));

enum ProfileType : uint32_t {
    BASELINE,
    MAIN,
    HIGH
};

enum RCMode : uint32_t  {
    ABR,                       // Average bit rate, not supported currently
    CRF,                       // Constant rate factor, not supported currently
    CBR,                       // Constant bit rate
    CAPPED_CRF,                // Capped CRF
    RC_MODE_MAX
};
```

**Restrictions<a name="section1134mcpsimp"></a>**

Only the constant bit rate (CBR) and CAPPED_CRF modes are supported in rate control.

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
| bitrate | Encoding bit rate.| For AMD (usually W6800): 500000 to 50000000; for DC1000: 500000 to 30000000. Unit: bit/s| 3000000 |
| gopSize | Encoding GOP size.| 30 to 3000| 30 |
| profile | Encoding profile. (Only `main` can be used for H.265 encoding.)| `0`: baseline (supported only in H.264 encoding); `1`: main; `2`: high (supported only in H.264 encoding)| 0|
| rcMode | Bit rate control mode.| `0`: average bit rate (ABR) (not supported currently); `1`: constant rate factor (CRF) (not supported currently); `2`: constant bit rate (CBR); `3`: capped CRF| 2|
| forceKeyFrame | Forced I-frame encoding.| `0`: Disable forced I-frame encoding. `1`: Forcibly generate an I-frame as the next frame.| 0|
| interpolation | Frame interpolation.| `false`: Disable frame interpolation. `true`: Enable frame interpolation.| false|
| crf | CRF bit rate control level.| 0 to 51| 34 |
| maxCrfRate | Maximum CRF bit rate.| 500000 to 100000000| 20000000 |
| vbvBufferSize | Size of the CRF bit rate buffer.| `-1`: auto mode. `0`: Disable the maximum bit rate restriction. [*min_vbv_size*, 3000]: *min_vbv_size* = ceil(1000/fps) + 1 and *min_vbv_size* ≥ 10| 1000 |
| streamWidth | Output frame width.| [240, *cloud phone rendering resolution*]. The output frame width must be a multiple of 8.| 720 |
| streamHeight | Output frame height.| [240, *cloud phone rendering resolution*]. The output frame height must be a multiple of 8.| 1080 |

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>1. Currently, two options are valid for the `rcMode` parameter: CBR mode and capped CRF mode. Quadra supports both modes, and DC1000 supports only the CBR mode.
>2. If `rcMode` is set to CBR, you can use the `bitrate` variable to specify the constant bit rate. If `rcMode` is set to capped CRF, use the `crf`, `maxCrfRate`, and `vbvBufferSize` variables to control the bit rate when Quadra is used, or use the `crf` and `maxCrfRate` variables to control the bit rate when DC1000 is used.

### 5.3 Data Definitions<a name="ZH-CN_TOPIC_0000002518346266"></a>

The output data structure of the VO module consists of the extra data packet information (`extData`), data packet size, and actual data. The data is specified by the `VIDEO_RETURN_VIDEO_DATA` command word in the `VmiCmd` enumeration and returned to developers through callbacks. Example:

```c++
VmiDataType module = DATA_VIDEO;
VmiCmd cmd = VIDEO_RETURN_VIDEO_DATA;
VideoData videoData;
uint8_t* data = &videoData;
DataCallback(module, cmd, data, sizeof(VideoData));
```

**Definition of VideoData<a name="section1643611241814"></a>**

```c++
struct VideoData {
    ExtDataVideo extData;               // Extra information about the stream packet
    uint32_t size;                      // Stream packet size
    uint8_t data[0];                    // Stream data
} __attribute__((packed));
```

**Definition of ExtDataVideo<a name="section379912203186"></a>**

```c++
struct FrameSize {
    uint32_t width = 720;               // 720 (defaults to 720p)
    uint32_t height = 1280;             // 1280 (defaults to 720p)
    uint32_t widthAligned = 720;        // 720 (not aligned by default)
    uint32_t heightAligned = 1280;      // 1280 (not aligned by default)
} __attribute__((packed));

struct VideoLatency {
    uint64_t startCaptureTimestamp;     // Snapshot capturing start time
    uint64_t endCaptureTimestamp;       // Snapshot capturing end time
    uint64_t startEncodeTimestamp;      // Encoding start time
    uint64_t endEncodeTimestamp;        // Encoding end time
} __attribute__((packed));

struct ExtDataVideo {
    uint64_t timestamp;                  // Timestamp when the frame rendering is complete
    uint8_t orientation;                // Orientation of the current frame
    FrameSize size;                     // Current frame size
    VideoLatency latencyInfo;           // Snapshotting and encoding time points
    uint32_t frameRate;                 // Current screen refresh rate
    uint32_t transform;                 // Image rotation, which is 0 if CompositionBypass is disabled and may not be 0 if CompositionBypass is enabled
} __attribute__((packed));
```

### 5.4 Parameter Obtaining<a name="ZH-CN_TOPIC_0000002518186388"></a>

The VO module provides the feature of obtaining parameters through the GetParam API.

**VIDEO_GET_ENCODER_PARAM<a name="section14422058191914"></a>**

Set `module` to `DATA_VIDEO` and `cmd` to `VIDEO_GET_ENCODER_PARAM`, and input data of the `EncodeParams` type to obtain encoding parameters. Example:

```c++
VmiDataType module = DATA_VIDEO;
VmiCmd cmd = VIDEO_GET_ENCODER_PARAM;
EncodeParams encodeParams;
uint8_t* param = &encodeParams;
GetParam(module, cmd, param, sizeof(EncodeParams));
```

### 5.5 Parameter Setting<a name="ZH-CN_TOPIC_0000002549826121"></a>

The VO module provides the feature of setting parameters through the SetParam API.

**VIDEO_SET_ENCODER_PARAM<a name="section59471253221"></a>**

Set `module` to `DATA_VIDEO` and `cmd` to `VIDEO_SET_ENCODER_PARAM`, and input data of the `EncodeParams` type to set encoding parameters. Example:

```c++
VmiDataType module = DATA_VIDEO;
VmiCmd cmd = VIDEO_SET_ENCODER_PARAM;
EncodeParams encodeParams;
uint8_t* param = &encodeParams;
SetParam(module, cmd, param, sizeof(EncodeParams));
```

### 5.6 Recommended Configurations<a name="ZH-CN_TOPIC_0000002518186374"></a>

#### 5.6.1 Recommended Resolution and Screen Density<a name="ZH-CN_TOPIC_0000002518186384"></a>

|Specification|Resolution Width|Resolution Height|Screen Density|
|--|--|--|--|
|360P|360|640|120|
|480P|480|856|160|
|720P|720|1280|320|
|1080P|1080|1920|480|
|2K|1440|2560|640|
|4K|2160|3840|960|

#### 5.6.2 Recommended Encoding Parameters<a name="ZH-CN_TOPIC_0000002549706129"></a>

The encoding parameters are related to the hardware environment of the video stream cloud phone. For details about the hardware environment, see "Software Deployment > Environment Requirements > "Hardware Environment" in [Video Stream Engine installation Guide (Android 15)](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/videostreamengine_ad15/docs/en/install_guide.md).

**Hardware Configuration Scheme 1<a name="section1175422174614"></a>**

**Table 1** Encoding parameters in CBR mode<a id="cbr-encoding-parameters"></a>

|Parameter|Recommended Value|
|--|--|
|bitrate|8000000|
|gopsize|60|
|profile|main|

**Table 2** Encoding parameters in CAPPED_CRF mode<a id="capped-crf-encoding-parameters"></a>

|Parameter|Recommended Value|
|--|--|
|gopsize|60|
|profile|main|
|crf|21|
|maxCrfRate|10000000|
|vbvBufferSize|1000|

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>In hardware configuration scheme 1 (W6800 + Quadra), you are advised to use VPU for encoding. During H.265 encoding, only `main` is supported as the encoding profile.

**Hardware Configuration Scheme 2<a name="section1827916308460"></a>**

**Table 3** Encoding parameters in CBR mode<a id="cbr-encoding-parameters-1"></a>

|Parameter|Recommended Value|
|--|--|
|bitrate|8000000|
|gopsize|60|
|profile|main|

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>In hardware configuration scheme 2 (DaoCloud DC1000), you are advised to use GPU for encoding. During H.265 encoding, only `main` is supported as the encoding profile.

## 6 Audio Output Development<a name="ZH-CN_TOPIC_0000002518346272"></a>

### 6.1 Conventions<a name="ZH-CN_TOPIC_0000002518346280"></a>

The audio output (AO) configuration structure `VmiConfigAudio` is inherited from the `VmiConfig` structure. The structure is as follows:

```c++
struct VmiConfigAudio : public VmiConfig  {
    AudioType audioType = OPUS;
    AudioPlayParams params;                     // Default audio playback parameters
} __attribute__((packed));
```

### 6.2 Configuration Parameters<a name="ZH-CN_TOPIC_0000002549706173"></a>

#### 6.2.1 AudioType<a name="ZH-CN_TOPIC_0000002518186358"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

Audio type.

**Restrictions<a name="section1134mcpsimp"></a>**

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
| audioType | Audio output format.| `0`: OPUS; `1`: PCM| 0|

**Variable Data Definition<a name="section1137mcpsimp"></a>**

```c++
enum AudioType : uint32_t {
    OPUS,                     // Opus is used to compress audio by default.
    PCM                       // Raw audio data
};
```

#### 6.2.2 AudioPlayParams<a name="ZH-CN_TOPIC_0000002518346278"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

Audio playback parameters.

**Restrictions<a name="section1134mcpsimp"></a>**

The `bitrate` parameter is valid only in OPUS format, and the `sampleInterval` parameter is valid in both OPUS and PCM formats.

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
| sampleInterval | Audio output sampling interval.| `5`: 5 ms (not supported currently); `10`: 10 ms; `20`: 20 ms (not supported currently)| 10|
| bitrate | Audio OPUS encoding bit rate (bit/s).| 13200 to 512000| 192000 |

**Variable Data Definition<a name="section1137mcpsimp"></a>**

```c++
struct AudioPlayParams {
    uint32_t sampleInterval = 10;               // ms
    uint32_t bitrate = 192000;                  // The unit is bit/s. This parameter is valid only in OPUS format.
} __attribute__((packed));
```

### 6.3 Data Definitions<a name="ZH-CN_TOPIC_0000002518186408"></a>

The output data structure of the AO module consists of the extra stream packet information (`extData`), packet size, and actual data. The data is specified by the `AUDIO_RETURN_AUDIO_PLAY_DATA` command word in the `VmiCmd` enumeration and returned to developers through callbacks.

Example:

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

**Definition of AudioData<a name="section425686182512"></a>**

```c++
struct AudioData {
    ExtDataAudio extData;               // Extra information
    uint32_t size;                      // Size
    uint8_t data[0];                    // Data
} __attribute__((packed));
```

**Definition of ExtDataAudio<a name="section541061872519"></a>**

```c++
struct ExtDataAudio {
    uint32_t audioType = OPUS;              // Encoding type: OPUS/PCM
    uint8_t channels = 2;                   // Number of audio channels. 1: MONO/2: STEREO (default)
    uint8_t audioBitDepth = 16;             // Sampling depth in bits: 8 or 16 (default)
    uint32_t sampleRate = 48000;            // Sampling rate in Hz: 48000 (default)
    uint32_t sampleInterval = 10;           // Sampling interval in ms: 10 (default)
    int64_t timestamp;                      // Audio data timestamp in ms (UTC time)
} __attribute__((packed));
```

**Definition of AudioVolume<a name="section1613302917121"></a>**

```c++
struct AudioVolume {
    uint32_t leftVolume = 100;                  // Volume of the left channel
    uint32_t rightVolume = 100;                 // Volume of the right channel
} __attribute__((packed));
```

### 6.4 Parameter Obtaining<a name="ZH-CN_TOPIC_0000002549826129"></a>

The AO module provides the feature of obtaining parameters through the GetParam API.

**AUDIO_GET_AUDIOPLAY_PARAM<a name="section11153139172510"></a>**

Set `module` to `DATA_AUDIO` and `cmd` to `AUDIO_GET_AUDIOPLAY_PARAM`, and input data of the `AudioPlayParams` type to obtain audio playback parameters. Example:

```c++
VmiDataType module = DATA_AUDIO;
VmiCmd cmd = AUDIO_GET_AUDIOPLAY_PARAM;
AudioPlayParams audioPlayParams;
uint8_t* param = &audioPlayParams;
GetParam(module, cmd, param, sizeof(AudioPlayParams));
```

### 6.5 Parameter Setting<a name="ZH-CN_TOPIC_0000002549826165"></a>

The AO module provides the feature of setting parameters through the SetParam API.

**AUDIO_SET_AUDIOPLAY_PARAM<a name="section108503742617"></a>**

Set `module` to `DATA_AUDIO` and `cmd` to `AUDIO_SET_AUDIOPLAY_PARAM`, and input data of the `AudioPlayParams` type to set audio playback parameters. Example:

```c++
VmiDataType module = DATA_AUDIO;
VmiCmd cmd = AUDIO_SET_AUDIOPLAY_PARAM;
AudioPlayParams audioPlayParams;
uint8_t* param = &audioPlayParams;
SetParam(module, cmd, param, sizeof(AudioPlayParams));
```

## 7 Microphone Input Development<a name="ZH-CN_TOPIC_0000002518346270"></a>

### 7.1 Conventions<a name="ZH-CN_TOPIC_0000002549706137"></a>

The microphone input (MI) configuration structure `VmiConfigMic` is inherited from the `VmiConfig` structure. The structure is as follows:

```c++
struct VmiConfigMic : public VmiConfig {
    AudioType audioType = OPUS;
} __attribute__((packed));
```

### 7.2 Configuration Parameters<a name="ZH-CN_TOPIC_0000002549826147"></a>

#### 7.2.1 AudioType<a name="ZH-CN_TOPIC_0000002549826149"></a>

**Variable Description<a name="section1131mcpsimp"></a>**

Audio type.

**Restrictions<a name="section1134mcpsimp"></a>**

|Property Field Name|Description|Value Range|Default Value|
|--|--|--|--|
| audioType | Microphone input format.| `0`: OPUS; `1`: PCM| 0|

**Variable Data Definition<a name="section1137mcpsimp"></a>**

```c++
enum AudioType : uint32_t {
    OPUS,                     // Opus is used to compress audio by default.
    PCM                       // Raw audio data
};
```

### 7.3 Data Definitions<a name="ZH-CN_TOPIC_0000002549826119"></a>

The MI data structure is the same as the AO data structure, that is, `AudioData`, which consists of the extra stream packet information (`extData`), packet size, and actual data.

**Definition of AudioData<a name="section9322642132713"></a>**

```c++
struct AudioData {
    ExtDataAudio extData;               // Extra information
    uint32_t size;                      // Size
    uint8_t data[0];                    // Data
} __attribute__((packed));
```

**Definition of ExtDataAudio<a name="section1386435174014"></a>**

```c++
struct ExtDataAudio {
    uint32_t audioType = OPUS;              // Encoding type: OPUS/PCM
    uint8_t channels = 2;                   // Number of audio channels. 1: MONO/2: STEREO (default)
    uint8_t audioBitDepth = 16;             // Sampling depth in bits: 8 or 16 (default)
    uint32_t sampleRate = 48000;            // Sampling rate in Hz: 48000 (default)
    uint32_t sampleInterval = 10;           // Sampling interval in ms: 10 (default)
    int64_t timestamp;                      // Audio data timestamp in ms (UTC time)
} __attribute__((packed));
```

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>The timestamp must be the timestamp of the client instead of the timestamp after the server receives audio data packets transmitted through the network. Otherwise, the packets that are severely delayed due to network fluctuation may fail to be filtered out, resulting in audio lagging.

### 7.4 Data Injection<a name="ZH-CN_TOPIC_0000002518186410"></a>

Different from data output, the InjectData API needs to be invoked for data input. You can specify data using the `MIC_SEND_MIC_DATA` command word in the `VmiCmd` enumeration, and inject data to the server through the InjectData API. Example:

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

**Restrictions<a name="section1134mcpsimp"></a>**

Microphone frames need to be injected stably. Otherwise, the sound may be intermittent. The specific restrictions are subject to the value of `vmi.mic.cachefactor` in the `default.prop` configuration file. This configuration item indicates the buffer size of the microphone frame queue. The options are as follows:

- `0`: One frame needs to be injected every 10 ms, causing no extra delay.
- `1`: 16 frames need to be injected every 160 ms, causing a 160 ms delay.
- `2`: 32 frames need to be injected every 320 ms, causing a 320 ms delay.
- `3`: 48 frames need to be injected every 480 ms, causing a 480 ms delay.

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>- If microphone data is transmitted in PCM mode, it is recommended that the upstream bandwidth from the client to the server be greater than or equal to 2 Mbit/s.
>- Injected microphone frames cannot contain blank frames. Otherwise, the sound may be intermittent.

## 8 Touch Input Development<a name="ZH-CN_TOPIC_0000002549826137"></a>

### 8.1 Conventions<a name="ZH-CN_TOPIC_0000002549826157"></a>

The touch input (TI) configuration structure `VmiConfigTouch` is inherited from the `VmiConfig` structure. The structure is as follows:

```c++
struct VmiConfigTouch : public VmiConfig {} __attribute__((packed));
```

### 8.2 Data Definitions<a name="ZH-CN_TOPIC_0000002549706169"></a>

The TI data types include `VmiTouchInputData` (touchpad) and `VmiKeyInputData` (keyboard).

**VmiTouchInputData<a name="section266218297293"></a>**

Data types of the touchpad input are defined as follows:

```c++
// Touch event
struct VmiTouchEvent {
    uint8_t pointerID;
    uint8_t action;
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
} __attribute__((packed));
// Touchpad input data
struct VmiTouchInputData {
    uint8_t orientation;                        // Current orientation of the client
    uint16_t uiWidth;                           // Width of the client UI resolution
    uint16_t uiHeight;                          // Height of the client UI resolution
    VmiTouchEvent touchEvent;
} __attribute__((packed));
```

**VmiKeyInputData<a name="section157041843162911"></a>**

Data types of the keyboard input are defined as follows:

```c++
// Keyboard input data
struct VmiKeyInputData {
    uint16_t keyCode;
    uint16_t action;
} __attribute__((packed));
```

### 8.3 Data Injection<a name="ZH-CN_TOPIC_0000002549826125"></a>

Different from data output, the InjectData API needs to be invoked for data input. You can specify data using the `SEND_TOUCH_EVENT` and `SEND_KEY_EVENT` command words in the `VmiCmdTouch` enumeration and inject data to the server through the InjectData API. Example:

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

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>- The server performs limited verification on touch data, for example, determining whether a touch point in the touch data is within an effective area. However, the server cannot distinguish abnormal data caused by abnormal behavior. For example, a finger is lifted from the client screen but the client sends a Down event (which should be an UP event). Therefore, pay attention to this issue when writing client code.
>- On some mobile phone models (vivo phones detected), if three or more fingers press the screen simultaneously or sequentially within a short interval, the client sends an EVENT_CANCEL event. As a result, the server resets the status of all touch points, causing a touch control failure. This issue does not occur if the interval between finger pressing is longer.

## 9 Sensor Input Development<a name="ZH-CN_TOPIC_0000002518346262"></a>

### 9.1 Conventions<a name="ZH-CN_TOPIC_0000002518186380"></a>

Use the `VmiConfig` structure to configure the sensor input. The structure is as follows:

```c++
struct VmiConfig {
    uint32_t version;
} __attribute__((packed));
```

### 9.2 Data Definitions<a name="ZH-CN_TOPIC_0000002549826145"></a>

The input data structure of the sensor input module consists of four parts: `x`, `y`, `z`, and `type`. The data is provided for developers through callbacks.

Example:

```c++
struct SensorData {
    float x;
    float y;
    float z;
    int32_t type;
} __attribute__((packed));
```

The following sensors are available:

```c++
enum VmiSensorType : uint32_t {
    HANDLE_ACCELERATION = 0,                // Acceleration sensor
    HANDLE_GYROSCOPE,                       // Gyroscope sensor
    HANDLE_MAGNETIC_FIELD,                  // Magnetic field sensor
    HANDLE_ORIENTATION,                     // Orientation sensor
    HANDLE_TEMPERATURE,                     // Temperature sensor
    HANDLE_PROXIMITY,                       // Proximity sensor
    HANDLE_LIGHT,                           // Light sensor
    HANDLE_PRESSURE,                        // Pressure sensor
    HANDLE_HUMIDITY,                        // Humidity sensor
    HANDLE_MAGNETIC_FIELD_UNCALIBRATED,     // Uncalibrated magnetic field sensor
    HANDLE_MAX = 10
};
```

The output data structure of the sensor output module consists of the sensor type handle and the sensor sampling rate. The data is provided for developers through callbacks.

Example:

```c++
struct SensorActivateData {
    int handle;             // handle corresponds to the value of VmiSensorType.
    int64_t samplingPeriod; // When a sensor is deactivated, this field can be ignored or set to the default value.
} __attribute__((packed));
```

### 9.3 Data Injection<a name="ZH-CN_TOPIC_0000002518186406"></a>

The InjectData API needs to be invoked for data input. You can specify data using the `SENSOR_SEND_SENSOR_DATA` command word in the `VmiCmd` enumeration and inject data to the server through the InjectData API. Example:

```c++
VmiDataType module = DATA_SENSOR;
VmiCmd cmd = SENSOR_SEND_SENSOR_DATA;
SensorData sensorData;
uint8_t* data = &sensorData;
InjectData(module, cmd, data, sizeof(SensorData));
```

### 9.4 Data Output<a name="ZH-CN_TOPIC_0000002549826117"></a>

The SetParam API needs to be invoked for data output. You can specify data using the `SENSOR_RETURN_REGISTER_CLIENT_SENSOR`, `SENSOR_RETURN_UNREGISTER_CLIENT_SENSOR`, and `SENSOR_RETURN_UPDATE_CLIENT_SENSOR_RATE` command words in the `VmiCmd` enumeration and output data to the client through the SetParam API. Example:

```c++
VmiDataType module = SENSOR;
VmiCmd cmd = SENSOR_RETURN_REGISTER_CLIENT_SENSOR;
VmiCmd cmd = SENSOR_RETURN_UNREGISTER_CLIENT_SENSOR;
VmiCmd cmd = SENSOR_RETURN_UPDATE_CLIENT_SENSOR_RATE;
SensorActivateData sensorActivateData;
uint8_t* data = &sensorActivateData;
SetParam(module, cmd, param, sizeof(SensorActivateData));
```

## 10 GPS Input Development<a name="ZH-CN_TOPIC_0000002518346296"></a>

### 10.1 Conventions<a name="ZH-CN_TOPIC_0000002549706159"></a>

Use the `VmiConfig` structure to configure the GPS input. The structure is as follows:

```c++
struct VmiConfig {
    uint32_t version;
} __attribute__((packed));
```

### 10.2 Data Definitions<a name="ZH-CN_TOPIC_0000002549706171"></a>

The GPS input data types include the longitude and latitude data in the `VmiGpsLocationData` structure.

**VmiGpsLocationData<a name="section266218297293"></a>**

The input data types of the longitude and latitude are defined as follows:

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

### 10.3 Data Injection<a name="ZH-CN_TOPIC_0000002549706139"></a>

The InjectData API needs to be invoked for data input. You can specify data using the `GPS_SEND_LOCATION_DATA` command word in the `VmiCmd` enumeration and inject data to the server through the InjectData API. Example:

```c++
VmiDataType module = DATA_GPS;
VmiCmd cmd = GPS_SEND_LOCATION_DATA;
VmiGpsLocationData locationData;
uint8_t* data = &locationData;
InjectData(module, cmd, data, sizeof(VmiGpsLocationData));
```

## 11 APIs for Non-commercial Implementation<a name="ZH-CN_TOPIC_0000002549826171"></a>

### 11.1 Encoding APIs (C and C++)<a name="ZH-CN_TOPIC_0000002549706127"></a>

#### 11.1.1 API Description<a name="ZH-CN_TOPIC_0000002518186382"></a>

The video encoding module provides external video encoding APIs required for the normal running of the video stream engine server. These APIs are implemented by secondary developers and provided as a DLL. The name of the DLL is `libVideoCodec.so`.

Developers can inherit the VideoEncoder class and implement the APIs according to the description in this section. In addition, [11.1.2-CreateVideoEncoder](#CreateVideoEncoder) and [11.1.3-DestroyVideoEncoder](#DestroyVideoEncoder) are provided for creating specific instances of this implementation class.

The return codes are defined as follows:

```c++
enum EncoderRetCode : uint32_t {
    VIDEO_ENCODER_SUCCESS                = 0x00,
    VIDEO_ENCODER_CREATE_FAIL            = 0x01,  // Failed to create an encoder.
    VIDEO_ENCODER_INIT_FAIL              = 0x02,  // Failed to initialize the encoder.
    VIDEO_ENCODER_START_FAIL             = 0x03,  // Failed to start the encoder.
    VIDEO_ENCODER_ENCODE_FAIL            = 0x04,  // Failed to encode data.
    VIDEO_ENCODER_STOP_FAIL              = 0x05,  // Failed to stop the encoder.
    VIDEO_ENCODER_DESTROY_FAIL           = 0x06,  // Failed to destroy the encoder.
    VIDEO_ENCODER_REGISTER_FAIL          = 0x07,  // Failed to register the function with the encoder.
    VIDEO_ENCODER_RESET_FAIL             = 0x08,  // Failed to reset the encoder.
    VIDEO_ENCODER_FORCE_KEY_FRAME_FAIL   = 0x09,  // Failed to set the forcible I-frame encoding mode.
    VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL = 0x0A   // Failed to set encoding parameters.
};
```

The encoder types are defined as follows:

```c++
enum EncoderFormat: uint32_t {
    ENCODER_TYPE_OPENH264 = 0,    // Open-source OpenH264 encoder
    ENCODER_TYPE_NETINTH264 = 1,  // NETINT H.264 hardware encoder
    ENCODER_TYPE_NETINTH265 = 2,  // NETINT H.265 hardware encoder
    ENCODER_TYPE_VASTAIH264 = 3,  // VASTAI H.264 hardware encoder
    ENCODER_TYPE_VASTAIH265 = 4,  // VASTAI H.265 hardware encoder
    ENCODER_TYPE_QUADRAH264 = 5,  // QUADRA H.264 hardware encoder
    ENCODER_TYPE_QUADRAH265 = 6,  // QUADRA H.265 hardware encoder
    ENCODER_TYPE_T432H264 = 7,    // T432 H.264 hardware encoder
    ENCODER_TYPE_T432H265 = 8,    // T432 H.265 hardware encoder
};
```

#### 11.1.2 CreateVideoEncoder<a name="CreateVideoEncoder"></a>

**Function Usage<a name="section20501062"></a>**

Creates an encoder instance.

**Prototype<a name="section49973353"></a>**

EncoderRetCode CreateVideoEncoder\(int32_t\*fd, EncoderFormat encoderFormat\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t* | Encoder descriptor.|
| encoderFormat | Input| `EncoderFormat` enumeration| Type of the encoder to be created.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: The encoder instance is successfully created.
- `VIDEO_ENCODER_CREATE_FAIL`: Failed to create an encoder instance.

#### 11.1.3 DestroyVideoEncoder<a name="DestroyVideoEncoder"></a>

**Function Usage<a name="section20501062"></a>**

Destroys an encoder instance.

**Prototype<a name="section49973353"></a>**

EncoderRetCode DestroyVideoEncoder\(int32_t fd\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: The encoder instance is successfully destroyed.
- `VIDEO_ENCODER_DESTROY_FAIL`: Failed to destroy the encoder instance.

**Involved APIs of the VideoEncoder Class<a name="section97621346152119"></a>**

For details about involved APIs of the VideoEncoder class, see [11.1.4-Config](#Config), [11.1.5-InitEncoder](#InitEncoder), [11.1.6-StartEncoder](#StartEncoder), [11.1.7-SetParams](#SetParams), [11.1.8-EncodeOneFrame], [11.1.9-StopEncoder](#StopEncoder), [11.1.10-DestroyEncoder](#DestroyEncoder), and [11.1.11-ResetEncoder](#ResetEncoder).

#### 11.1.4 Config<a name="Config"></a>

**Function Usage<a name="section20501062"></a>**

Inputs the initial configuration of the encoder. This API needs to be called before [11.1.5-InitEncoder](#InitEncoder).

**Restrictions<a name="section50291837"></a>**

This parameter passes through the parameters listed in [5.2-Configuration Parameters](#video-configuration-parameters) to `VmiEncoderConfig` and `VmiEncoderParams`. The restrictions on these parameters are still valid.

**Prototype<a name="section49973353"></a>**

EncoderRetCode Config\(int32_t fd, const VmiEncoderConfig& config\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|
| config | Input| struct VmiEncoderConfig | Encoder configuration.|

Struct declarations of `VmiEncoderConfig` and `VmiEncoderParams` are as follows:

```c++
struct VmiEncoderParams {
    uint32_t bitRate = 3000000;
    uint32_t gopSize = 30;
    uint32_t profile = 1;
    uint32_t keyFrame = 0;
    uint32_t rcMode = 2;                        // Rate control mode
    uint32_t crf = 34;                          // CRF bit rate control level
    uint32_t maxCrfRate = 20000000;             // Maximum CRF bit rate
    int32_t vbvBufferSize = 1000;               // Size of the CRF bit rate buffer
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

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

`VIDEO_ENCODER_SUCCESS`: The encoder is successfully configured.

#### 11.1.5 InitEncoder<a name="InitEncoder"></a>

**Function Usage<a name="section20501062"></a>**

Initializes the encoder.

**Restrictions<a name="section50291837"></a>**

The [11.1.4-Config](#Config) API must have been invoked to input valid configurations.

**Prototype<a name="section49973353"></a>**

EncoderRetCode InitEncoder\(int32_t fd\)

**Parameters<a name="section42241738122511"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: The encoder is successfully initialized.
- `VIDEO_ENCODER_INIT_FAIL`: The encoder fails to be initialized.

#### 11.1.6 StartEncoder<a name="StartEncoder"></a>

**Function Usage<a name="section20501062"></a>**

Starts the encoder.

**Prototype<a name="section49973353"></a>**

EncoderRetCode StartEncoder\(int32_t fd\)

**Parameters<a name="section42241738122511"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: The encoder is successfully started.
- `VIDEO_ENCODER_INIT_FAIL`: The encoder fails to be initialized.

#### 11.1.7 SetParams<a name="SetParams"></a>

**Function Usage<a name="section20501062"></a>**

Sets the encoding parameters of the encoder.

**Restrictions<a name="section50291837"></a>**

This parameter passes through the parameters listed in [5.2-Configuration Parameters](#video-configuration-parameters) to `VmiEncoderParams`. The restrictions on these parameters are still valid.

**Prototype<a name="section49973353"></a>**

EncoderRetCode SetParams\(int32_t fd, const VmiEncoderParams& params\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|
| params | Input| struct VmiEncoderParams | Encoding parameters to be set.|

The struct declaration of `VmiEncoderParams` is as follows:

```c++
struct VmiEncoderParams {
    uint32_t bitRate = 3000000;
    uint32_t gopSize = 30;
    uint32_t profile = 1;
    uint32_t keyFrame = 0;
    uint32_t rcMode = 2;                        // Rate control mode
    uint32_t crf = 34;                          // CRF bit rate control level
    uint32_t maxCrfRate = 20000000;             // Maximum CRF bit rate
    int32_t vbvBufferSize = 1000;               // Size of the CRF bit rate buffer
};
```

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

`VIDEO_ENCODER_SUCCESS`: The encoder is successfully set.

#### 11.1.8 EncodeOneFrame<a name="EncodeOneFrame"></a>

**Function Usage<a name="section20501062"></a>**

Encodes data of one frame.

**Prototype<a name="section49973353"></a>**

EncoderRetCode EncodeOneFrame\(int32_t fd, const uint8_t \*inputData, uint32_t inputSize, uint8_t \*\*outputData, uint32_t \*outputSize\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|
| inputData | Input| const uint8_t * | Start address of the buffer for storing data to be encoded. The data format is YUV420p.|
| inputSize | Input| uint32_t | Length of the data to be encoded, in bytes.|
| outputData | Output| uint8_t **| Output data.|
| outputSize | Output| uint32_t * | Length of the output data, in bytes.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: Data of one frame is successfully encoded.
- `VIDEO_ENCODER_ENCODE_FAIL`: Failed to encode data of one frame.

#### 11.1.9 StopEncoder<a name="StopEncoder"></a>

**Function Usage<a name="section20501062"></a>**

Stops the encoder.

**Prototype<a name="section49973353"></a>**

EncoderRetCode StopEncoder\(int32_t fd\)

**Parameters<a name="section42241738122511"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: The encoder is successfully stopped.
- `VIDEO_ENCODER_STOP_FAIL`: Failed to stop the encoder.

#### 11.1.10 DestroyEncoder<a name="DestroyEncoder"></a>

**Function Usage<a name="section20501062"></a>**

Destroys the encoder and releases encoding resources.

**Prototype<a name="section49973353"></a>**

EncoderRetCode DestoryEncoder\(int32_t fd\)

**Parameters<a name="section42241738122511"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: The encoder is successfully stopped.
- `VIDEO_ENCODER_DESTROY_FAIL`: Failed to destroy the encoder.

#### 11.1.11 ResetEncoder<a name="ResetEncoder"></a>

**Function Usage<a name="section20501062"></a>**

Resets the encoder.

**Prototype<a name="section49973353"></a>**

EncoderRetCode ResetEncoder\(int32_t fd\)

**Parameters<a name="section42241738122511"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| fd | Input| int32_t | Encoder descriptor.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: The encoder is successfully reset.
- `VIDEO_ENCODER_RESET_FAIL`: Failed to reset the encoder.

#### 11.1.12 FrameScaling<a name="ZH-CN_TOPIC_0000002518186362"></a>

**Function Usage<a name="section20501062"></a>**

Sets the expected encoding width and height.

**Restrictions<a name="section50291837"></a>**

This parameter passes through the parameters listed in [5.2-Configuration Parameters](#video-configuration-parameters) to `FrameScaling`. The restrictions on these parameters are still valid.

Secondary developers can directly call this API to implement YUV frame scaling.

**Prototype<a name="section49973353"></a>**

EncoderRetCode FrameScaling\(uint32_t width, uint32_t height\)

**Parameters<a name="section47106995"></a>**

`width`: expected encoding width. Its value type is uint32_t. This field is described as follows.

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| width | Input| uint32_t | Expected encoding width.|

`height`: expected encoding height. Its value type is uint32_t. This field is described as follows.

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| height | Input| uint32_t | Expected encoding height.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: EncoderRetCode

The value can be any of the following:

- `VIDEO_ENCODER_SUCCESS`: success
- `VIDEO_ENCODER_SET_ENCODE_PARAMS_FAIL`: failure

### 11.2 Decoding APIs (C and C++)<a name="ZH-CN_TOPIC_0000002549826141"></a>

#### 11.2.1 API Description<a name="ZH-CN_TOPIC_0000002518346288"></a>

The video decoding module provides external video decoding APIs required for the normal running of the video stream engine server. These APIs are implemented by secondary developers and provided as a DLL. The name of the DLL is `libVideoDecoder.so`.

Developers can inherit the `VideoDecoder` class and implement the APIs according to the description in this section. In addition, [11.2.2-CreateVideoDecoder] and [11.2.3-DestroyVideoDecoder](#DestroyVideoDecoder) are provided for creating specific instances of this implementation class.

This API is used with the internal OMX decoding component of the Android system and does not interfere with the video stream output component.

The return codes are defined as follows:

```c++
enum DecoderRetCode : uint32_t {
    VIDEO_DECODER_SUCCESS,                // Success.
    VIDEO_DECODER_CREATE_FAIL,            // Failed to create a decoder.
    VIDEO_DECODER_INIT_FAIL,              // Failed to initialize the decoder.
    VIDEO_DECODER_START_FAIL,             // Failed to start the decoder.
    VIDEO_DECODER_DECODE_FAIL,            // Decoding failure.
    VIDEO_DECODER_STOP_FAIL,              // Failed to stop the decoder.
    VIDEO_DECODER_DESTROY_FAIL,           // Failed to destroy the decoder.
    VIDEO_DECODER_RESET_FAIL,             // Failed to reset the decoder.
    VIDEO_DECODER_GET_DECODE_PARAMS_FAIL, // Failed to obtain decoding parameters.
    VIDEO_DECODER_SET_DECODE_PARAMS_FAIL, // Failed to set decoding parameters.
    VIDEO_DECODER_SET_FUNC_FAIL,          // Failed to set the callback.
    VIDEO_DECODER_WRITE_OVERFLOW,         // Write overflow.
    VIDEO_DECODER_READ_UNDERFLOW,         // Read underflow.
    VIDEO_DECODER_BAD_PIC_SIZE,           // The resolution after decoding is inconsistent with the configured resolution.
    VIDEO_DECODER_EOS
};
```

#### 11.2.2 CreateVideoDecoder<a name="CreateVideoDecoder"></a>

**Function Usage<a name="section20501062"></a>**

Creates a decoder instance.

**Prototype<a name="section49973353"></a>**

DecoderRetCode CreateVideoDecoder\(VideoDecoder \*\*decoder\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| decoder | Output| VideoDecoder **| Two-dimensional pointer to the decoder instance. It is used to store the decoder instance created in the API.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoder instance is successfully created.
- `VIDEO_DECODER_CREATE_FAIL`: Failed to create a decoder instance.

#### 11.2.3 DestroyVideoDecoder<a name="DestroyVideoDecoder"></a>

**Function Usage<a name="section20501062"></a>**

Destroys a decoder instance.

**Prototype<a name="section49973353"></a>**

DecoderRetCode DestroyVideoDecoder\(VideoDecoder \*decoder\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| decoder | Input| VideoDecoder * | Pointer to the decoder instance to be destroyed.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoder instance is successfully destroyed.
- `VIDEO_DECODER_DESTROY_FAIL`: Failed to destroy the decoder instance.

**Involved APIs of the VideoDecoder Class<a name="section97621346152119"></a>**

For details about involved APIs of the VideoDecoder class, see [11.2.4-CreateDecoder](#CreateDecoder), [11.2.5-InitDecoder](#InitDecoder), [11.2.6-SetDecodeParams](#SetDecodeParams), [11.2.7-GetDecodeParams](#GetDecodeParams), [11.2.8-SetCallbacks](#SetCallbacks), [11.2.9-SetCopyFrameFunc](#SetCopyFrameFunc), [11.2.10-SendStreamData](#SendStreamData), [11.2.11-Flush](#Flush), [11.2.12-StartDecoder](#StartDecoder), [11.2.13-StopDecoder](#StopDecoder), and [11.2.14-DestroyDecoder](#DestroyDecoder).

#### 11.2.4 CreateDecoder<a name="CreateDecoder"></a>

**Function Usage<a name="section20501062"></a>**

Creates a decoder.

**Prototype<a name="section49973353"></a>**

DecoderRetCode CreateDecoder\(MediaStreamFormat__decType\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| decType | Input| MediaStreamFormat | To-be-decoded stream type.|

Enumeration of `MediaStreamFormat`:

```c++
// Decoding input formats
enum MediaStreamFormat : uint32_t {
    STREAM_FORMAT_AVC,
    STREAM_FORMAT_HEVC,
    STREAM_FORMAT_NONE
};
```

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoder is successfully created.
- `VIDEO_DECODER_CREATE_FAIL`: Failed to create a decoder.

#### 11.2.5 InitDecoder<a name="InitDecoder"></a>

**Function Usage<a name="section20501062"></a>**

Initializes the decoder.

**Restrictions<a name="section50291837"></a>**

A decoder must have been created through the [11.2.4-CreateDecoder](#CreateDecoder) API before you invoke InitDecoder.

**Prototype<a name="section49973353"></a>**

DecoderRetCode InitDecoder\(\)

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoder is successfully initialized.
- `VIDEO_DECODER_INIT_FAIL`: Failed to initialize the decoder.

#### 11.2.6 SetDecodeParams<a name="SetDecodeParams"></a>

**Function Usage<a name="section20501062"></a>**

Sets decoder parameters.

**Prototype<a name="section49973353"></a>**

DecoderRetCode SetDecodeParams\(DecodeParamsIndex index, void \*decParams\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| index | Input| DecodeParamsIndex | Type of a parameter to be set.|
| decParams | Input| void * | Memory address of the parameter.|

Configurable parameters:

```c++
// Decoding parameters
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
// Decoding output formats
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
    uint32_t width = 0;        // Image width
    uint32_t height = 0;       // Image height
    int32_t stride = 0;        // Image height after horizontal alignment
    uint32_t scanLines = 0;    // Image height after vertical alignment
    uint32_t cropWidth = 0;    // Cropped width to the left of the image
    uint32_t cropHeight = 0;   // Cropped height to the top of the image
};

struct AlignInfoParams {
    uint32_t widthAlign = 0;
    uint32_t heightAlign = 0;
};
```

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoder is successfully configured.
- `VIDEO_DECODER_SET_DECODE_PARAMS_FAIL`: Failed to configure the decoder.

#### 11.2.7 GetDecodeParams<a name="GetDecodeParams"></a>

**Function Usage<a name="section20501062"></a>**

Obtains the decoding parameters of the decoder.

**Prototype<a name="section49973353"></a>**

DecoderRetCode GetDecodeParams\(DecodeParamsIndex index, void \*decParams\)

**Parameters<a name="section2630616193212"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| index | Input| DecodeParamsIndex | Type of a parameter to be got.|
| decParams | Input| void * | Memory address of the parameter.|

For details about the parameters, see [11.2.6-SetDecodeParams](#SetDecodeParams).

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoding parameters are successfully obtained.
- `VIDEO_DECODER_GET_DECODE_PARAMS_FAIL`: Failed to obtain the decoding parameters.

#### 11.2.8 SetCallbacks<a name="SetCallbacks"></a>

**Function Usage<a name="section20501062"></a>**

Sets the callback for notifying decoder events.

**Prototype<a name="section49973353"></a>**

DecoderRetCode SetCallbacks\(std::function<void\(DecodeEventIndex, uint32_t, void \*\)\> eventCallBack\)

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| eventCallBack | Input| std::function<void(DecodeEventIndex, uint32_t, void *)> | Callback for notifying decoder events.|

When an event in the decoder needs to be notified to the upper layer, the callback set in this API is invoked. `index` indicates the event type, and `uint32_t` and `void *` parameters are used to store corresponding data. Definition:

```c++
// Decoding events
enum DecodeEventIndex : uint32_t {
    INDEX_PIC_INFO_CHANGE,
    INDEX_EVENT_NONE
};
```

`INDEX_PIC_INFO_CHANGE` indicates that the size of decoded images changes. In this case, mount `struct PicInfoParams *` to the `void *` parameter. For details, see [11.2.6-SetDecodeParams](#SetDecodeParams).

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The callback is successfully set.
- `VIDEO_DECODER_SET_FUNC_FAIL`: Failed to set the callback.

#### 11.2.9 SetCopyFrameFunc<a name="SetCopyFrameFunc"></a>

**Function Usage<a name="section20501062"></a>**

Sets the hook function to reduce memory copies when frame data is output. This API is provided for the decoder.

**Prototype<a name="section49973353"></a>**

DecoderRetCode SetCopyFrameFunc\(std::function<uint32_t\(uint8_t\*, uint8_t\*, const PicInfoParams &, uint32_t\)\> copyFrame\)

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| copyFrame | Input| std::function<uint32_t(uint8_t*, uint8_t*, const PicInfoParams &, uint32_t)> | Sets the hook function for copying decoded data of one frame to the output buffer. This function is used to shield format differences of the output API.|

In the callback, the first `uint8_t*` indicates the address of decoded data and the second `uint8_t*` indicates the destination of the copied data, which needs to be the same as the buffer address entered in [11.2.11-RetrieveFrameData](#RetrieveFrameData). `const PicInfoParams &` indicates the image width and height (see [11.2.6-SetDecodeParams](#SetDecodeParams)). `uint32_t` indicates the maximum length of the output buffer, which needs to be the same as the maximum buffer size entered in [11.2.11-RetrieveFrameData](#RetrieveFrameData). The return value of this callback is the actual size of processed data.

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The callback is successfully set.
- `VIDEO_DECODER_SET_FUNC_FAIL`: Failed to set the callback.

#### 11.2.10 SendStreamData<a name="SendStreamData"></a>

**Function Usage<a name="section20501062"></a>**

Sends a copy of stream data to the decoder for decoding. This API is synchronous.

**Prototype<a name="section49973353"></a>**

DecoderRetCode SendStreamData\(uint8_t \*buffer, uint32_t filledLen\)

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| buffer | Input| uint8_t * | Buffer of the input stream data.|

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| filledLen | Input| uint32_t | Length of the input stream data.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: Stream data is sent successfully.
- `VIDEO_DECODER_DECODE_FAIL`: Failed to send stream data.
- `VIDEO_DECODER_WRITE_OVERFLOW`: Write overflow.

#### 11.2.11 RetrieveFrameData<a name="RetrieveFrameData"></a>

**Function Usage<a name="section20501062"></a>**

Retrieves decoded data of one frame. This API is synchronous. The CopyFrame function provided in [11.2.9-SetCopyFrameFunc](#SetCopyFrameFunc) needs to be used for copying data.

**Prototype<a name="section49973353"></a>**

DecoderRetCode RetrieveFrameData\(uint8_t \*buffer, uint32_t maxLen, uint32_t \*filledLen\)

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| buffer | Input| uint8_t * | Buffer for receiving output data.|

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| maxLen | Input| uint32_t | Maximum length of the output buffer.|

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| filledLen | Output| uint32_t * | Actual length of the output data.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoded data is successfully retrieved.
- `VIDEO_DECODER_DECODE_FAIL`: Failed to retrieve the decoded data.
- `VIDEO_DECODER_READ_UNDERFLOW`: Read underflow.

#### 11.2.12 Flush<a name="Flush"></a>

**Function Usage<a name="section20501062"></a>**

Flushes all the current decoding buffers. Generally, this API is invoked when the progress bar is dragged.

**Prototype<a name="section49973353"></a>**

DecoderRetCode Flush\(\)

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoding buffer status is successfully reset.
- `VIDEO_DECODER_RESET_FAIL`: Failed to reset the decoding buffer status.

#### 11.2.13 StartDecoder<a name="StartDecoder"></a>

**Function Usage<a name="section20501062"></a>**

Starts the decoder. After the decoder is started successfully, the decoding process starts.

**Prototype<a name="section49973353"></a>**

DecoderRetCode StartDecoder\(\)

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoder is successfully started.
- `VIDEO_DECODER_START_FAIL`: Failed to start the decoder.

#### 11.2.14 StopDecoder<a name="StopDecoder"></a>

**Function Usage<a name="section20501062"></a>**

Stops the decoder.

**Prototype<a name="section49973353"></a>**

DecoderRetCode StopDecoder\(\)

**Return Value Description<a name="section7911548131510"></a>**

Data type: DecoderRetCode

The value can be any of the following:

- `VIDEO_DECODER_SUCCESS`: The decoder is stopped successfully.
- `VIDEO_DECODER_STOP_FAIL`: Failed to stop the decoder.

#### 11.2.15 DestroyDecoder<a name="DestroyDecoder"></a>

**Function Usage<a name="section20501062"></a>**

Destroys the decoder.

**Prototype<a name="section49973353"></a>**

void DestroyDecoder\(\)

### 11.3 GPU Acceleration APIs (C and C++)<a name="ZH-CN_TOPIC_0000002549706149"></a>

#### 11.3.1 API Description<a name="ZH-CN_TOPIC_0000002518186354"></a>

The GPU acceleration module provides GPU acceleration APIs required for the normal running of the video stream engine server, including the RGB-to-YUV conversion and GPU encoding functions. These APIs are implemented by secondary developers or the GPU vendor and provided as a DLL. The DLLs are `libVmiEncTurbo.so` (vendor library) and `libVmiEncTurboSys.so` (system library).

#### 11.3.2 QueryModule<a name="QueryModule"></a>

**Function Usage<a name="section20501062"></a>**

Queries GPU models and acceleration capabilities supported by the current system.

**Prototype<a name="section49973353"></a>**

void QueryModule\(ModuleInfo \*\*moduleList, uint32_t \*listSize\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| moduleList | Output| ModuleInfo **| List of GPU acceleration classes supported by the system. Each item contains the ID and capability of a GPU acceleration class.|
| listSize | Output| uint32_t * | `moduleList` length.|

Definition of `moduleList`:

```c++
struct ModuleInfo {
    // Hardware ID
    uint32_t moduleId = 0;
    // Hardware capability list. For example, if H.264 encoding and H.265 encoding are supported, the value is (1 << CAP_VA_ENCODE_H264) | (1 << CAP_VA_ENCODE_HEVC).
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

Definition of `moduleId`:

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

#### 11.3.3 CreateModule<a name="CreateModule"></a>

**Function Usage<a name="section20501062"></a>**

Creates an instance of the specified GPU acceleration class.

**Restrictions<a name="section50291837"></a>**

The value of `moduleId` must be one of the values in `moduleList` returned by [11.3.2-QueryModule](#QueryModule).

**Prototype<a name="section49973353"></a>**

void \*CreateModule\(uint32_t moduleId\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| moduleId | Input| uint32_t | Specifies the GPU model to be used for GPU acceleration.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: void *, which points to the created instance of the specified GPU acceleration class. This instance can be used through APIs defined in GpuEncoderBase.

**Involved APIs of the GpuEncoderBase Class<a name="section97621346152119"></a>**

For details about involved APIs of the GpuEncoderBase class, see [11.3.5-Init](#Init), [11.3.6-Deinit](#Deinit), [11.3.7-Start](#Start), [11.3.8-Stop](#Stop), [11.3.9-CreateBuffer](#CreateBuffer), [11.3.10-ImportBuffer](#ImportBuffer), [11.3.11-ReleaseBuffer](#ReleaseBuffer), [11.3.12-MapBuffer](#MapBuffer), [11.3.13-UnmapBuffer](#UnmapBuffer), [11.3.14-RetriveBufferData](#RetriveBufferData), [11.3.15-Convert](#Convert), [11.3.16-Encode](#Encode), [11.3.17-SetEncodeParam](#SetEncodeParam), and [11.3.18-Reset](#Reset).

Return values of the GpuEncoderBase class:

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
    ERR_NEED_RESET, // Some encoding parameters take effect after the encoder is reset.
    ERR_UNKNOW
};
```

#### 11.3.4 DestroyModule<a name="ZH-CN_TOPIC_0000002549826151"></a>

**Function Usage<a name="section20501062"></a>**

Destroys a GPU acceleration instance.

**Restrictions<a name="section50291837"></a>**

The entered instance must be created by [11.3.3-CreateModule](#CreateModule).

**Prototype<a name="section49973353"></a>**

void DestroyModule\(void \*module\)

**Parameters<a name="section47106995"></a>**

|Parameter|Input/Output|Type|Description|
|--|--|--|--|
| module | Input| void * | Pointer to the instance to be destroyed.|

#### 11.3.5 Init<a name="Init"></a>

**Function Usage<a name="section20501062"></a>**

Initializes a GPU acceleration instance.

**Prototype<a name="section49973353"></a>**

int32_t Init\(EncoderConfig &config\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| config | Input| EncoderConfig & | A reference to the `EncoderConfig` structure, representing GPU encoding configuration items.|

The `EncoderConfig` structure and its enumeration are defined as follows:

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

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The GPU acceleration instance is successfully initialized.
- Other: Failed to initialize the GPU acceleration instance.

#### 11.3.6 Deinit<a name="Deinit"></a>

**Function Usage<a name="section20501062"></a>**

Destroys a GPU acceleration class instance.

**Prototype<a name="section49973353"></a>**

int32_t DeInit\(\) = 0

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The GPU acceleration class instance is successfully destroyed.
- Other: Failed to destroy the GPU acceleration class instance.

#### 11.3.7 Start<a name="Start"></a>

Starts a GPU acceleration instance.

**Restrictions<a name="section50291837"></a>**

Prerequisite: The Init API has been successfully called.

**Prototype<a name="section49973353"></a>**

int32_t Start\(\) = 0

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The GPU acceleration instance is started successfully.
- Other: Failed to start the GPU acceleration instance.

#### 11.3.8 Stop<a name="Stop"></a>

Stops a GPU acceleration instance.

**Restrictions<a name="section50291837"></a>**

Prerequisite: The Start API has been successfully called.

**Prototype<a name="section49973353"></a>**

int32_t Stop\(\) = 0

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The GPU acceleration instance is stopped successfully.
- Other: Failed to stop the GPU acceleration instance.

#### 11.3.9 CreateBuffer<a name="CreateBuffer"></a>

**Function Usage<a name="section20501062"></a>**

Creates a buffer.

**Prototype<a name="section49973353"></a>**

int32_t CreateBuffer\(FrameFormat format, MemType type, GpuEncoderBufferT &buffer\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| format | Input| FrameFormat | Format of the buffer to be created.|

Enumeration of `FrameFormat`:

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

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| type | Input| MemType | Location of space to be allocated to the buffer.|

Enumeration of `MemType`:

```c++
enum MemType : uint32_t {
    MEM_TYPE_NONE,
    MEM_TYPE_HOST, // Host memory
    MEM_TYPE_DEVICE, // Device memory
    MEM_TYPE_COUNT
};
```

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| buffer | Output| GpuEncoderBufferT  & | Pointer used to mount the buffer after allocation.|

The definition of `GpuEncoderBufferT` is as follows. When implementing this API, developers or the vendor need to use `new` to allocate memory for `GpuEncoderBuffer` and release the memory in the ReleaseBuffer API.

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

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The buffer is successfully created.
- Other: Failed to create a buffer.

#### 11.3.10 ImportBuffer<a name="ImportBuffer"></a>

**Function Usage<a name="section20501062"></a>**

Imports a buffer from an external source.

**Prototype<a name="section49973353"></a>**

int32_t ImportBuffer\(FrameFormat format, uint64_t handle, GpuEncoderBufferT &buffer\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| format | Input| FrameFormat | Format of the buffer to be imported.|
| handle | Input| uint64_t | Handle or video RAM address corresponding to the buffer to be imported.|
| buffer | Output| GpuEncoderBufferT & | Pointer used to mount the buffer after the import.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The buffer is successfully imported.
- Other: Failed to import the buffer.

#### 11.3.11 ReleaseBuffer<a name="ReleaseBuffer"></a>

**Function Usage<a name="section20501062"></a>**

Releases a buffer.

**Prototype<a name="section49973353"></a>**

int32_t ReleaseBuffer\(GpuEncoderBufferT &buffer\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| buffer | Input| GpuEncoderBufferT & | Pointer to the buffer to be released.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The buffer is successfully released.
- Other: Failed to release the buffer.

#### 11.3.12 MapBuffer<a name="MapBuffer"></a>

**Function Usage<a name="section20501062"></a>**

Maps data in a buffer to the memory.

- If data in the buffer can be mapped to contiguous memory in user space, fill in `data` and `dataLen` in the `buffer` field for this API.
- If data in the buffer cannot be mapped to contiguous memory, leave `data` in the `buffer` field empty and fill in the `dataLen` field.

**Prototype<a name="section49973353"></a>**

int32_t MapBuffer\(GpuEncoderBufferT &buffer, uint32_t flag\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| buffer | Output| GpuEncoderBufferT & | Pointer to the buffer whose data needs to be mapped.|
| flag | Input| uint32_t | Configures the access type of the mapping data, including read and write.|

The value of `flag` is `FLAG_READ | FLAG_WRITE` when both read and write are enabled.

```c++
enum MapFlag : uint32_t {
    FLAG_READ = 1,
    FLAG_WRITE = 1 << 1,
};
```

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: Data in the buffer is successfully mapped.
- Other: Failed to map data in the buffer.

#### 11.3.13 UnmapBuffer<a name="UnmapBuffer"></a>

**Function Usage<a name="section20501062"></a>**

Unmaps data in a buffer from the memory.

**Prototype<a name="section49973353"></a>**

int32_t UnmapBuffer\(GpuEncoderBufferT &buffer\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| buffer | Input| GpuEncoderBufferT & | Pointer to the buffer whose data needs to be unmapped.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The data in the buffer is successfully unmapped.
- Other: Failed to unmap data in the buffer.

#### 11.3.14 RetriveBufferData<a name="RetriveBufferData"></a>

**Function Usage<a name="section20501062"></a>**

Retrieves data in a buffer.

This API copies data in the buffer to the memory provided by the caller. It is usually called when the MapBuffer API cannot map data in a buffer to contiguous memory.

**Restrictions<a name="section50291837"></a>**

Data in the buffer must have been mapped through the MapBuffer API.

**Prototype<a name="section49973353"></a>**

int32_t RetriveBufferData\(GpuEncoderBufferT &buffer, uint8_t \*data, uint32_t memLen, uint32_t &dataLen\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| buffer | Input| GpuEncoderBufferT & | Pointer to the buffer whose data needs to be copied.|
| data | Input| uint8_t * | Address of the memory space for receiving data in the buffer.|
| memLen | Input| uint32_t | Maximum length of the memory space for receiving buffer data.|
| dataLen | Output| uint32_t & | Actual size of the copied data.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The buffer data is successfully retrieved.
- Other: Failed to retrieve data in the buffer.

#### 11.3.15 Convert<a name="Convert"></a>

**Function Usage<a name="section20501062"></a>**

Calls the GPU to convert the format. It converts data in the input buffer and writes it to the output buffer.

**Prototype<a name="section49973353"></a>**

int32_t Convert\(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| inBuffer | Input| GpuEncoderBufferT & | Input buffer whose data format is to be converted.|
| outBuffer | Output| GpuEncoderBufferT & | Output buffer for receiving data after format conversion.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The buffer format is successfully converted.
- Other: Failed to convert the data format.

#### 11.3.16 Encode<a name="Encode"></a>

**Function Usage<a name="section20501062"></a>**

Calls the GPU for video encoding. It encodes data in the input buffer and writes it to the output buffer.

**Prototype<a name="section49973353"></a>**

int32_t Encode\(GpuEncoderBufferT &inBuffer, GpuEncoderBufferT &outBuffer\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| inBuffer | Input| GpuEncoderBufferT & | Input buffer whose data is to be encoded.|
| outBuffer | Output| GpuEncoderBufferT & | Output buffer for receiving encoded data.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The encoding is successful.
- Other: The encoding fails.

#### 11.3.17 SetEncodeParam<a name="SetEncodeParam"></a>

**Function Usage<a name="section20501062"></a>**

Sets encoding parameters.

**Prototype<a name="section49973353"></a>**

int32_t SetEncodeParam\(EncodeParamT params\[\], uint32_t num\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
| params | Input| EncodeParamT[] | List of the encoding parameters to be configured.|
| num | Input| uint32_t | Number of the encoding parameters to be configured.|

Enumeration of `EncodeParamT`:

```c++
enum EncodeParamIndex : uint32_t {
    ENCODE_PARAM_NONE,
    ENCODE_PARAM_RATE_CONTROL, // Sets the bit rate control mode.
    ENCODE_PARAM_BITRATE, // Sets the bit rate.
    ENCODE_PARAM_FRAMERATE, // Sets the frame rate.
    ENCODE_PARAM_GOPSIZE, // Set the GOP size.
    ENCODE_PARAM_PROFILE, // Set the profile.
    ENCODE_PARAM_KEYFRAME, // Sets the key frame.
    ENCODE_PARAM_STREAM_WIDTH,// Sets the output stream width.
    ENCODE_PARAM_STREAM_HEIGHT,// Sets the output stream height.
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

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The encoding parameters are set successfully.
- `ERR_NEED_RESET`: The encoding parameters take effect only after the Reset API is called.
- Other: Failed to set the encoding parameters.

#### 11.3.18 Reset<a name="Reset"></a>

Resets the encoder. Some encoding parameter configurations take effect only after this API is called.

**Restrictions<a name="section50291837"></a>**

None

**Prototype<a name="section49973353"></a>**

int32_t Reset\(\) = 0

**Return Value Description<a name="section7911548131510"></a>**

Data type: GpuEncoderErrorCode

The value can be any of the following:

- `OK`: The encoder is successfully reset.
- Other: Failed to reset the encoder.

#### 11.3.19 ResetImgSize<a name="ZH-CN_TOPIC_0000002549706143"></a>

**Function Usage<a name="section20501062"></a>**

Sets the size of the saved image according to the snapshot size. The setting takes effect after GPU Turbo is restarted.

**Restrictions<a name="section50291837"></a>**

The input parameters `width` and `height` must be even numbers.

**Prototype<a name="section49973353"></a>**

int32_t ResetImgSize\(uint32_t width, uint32_t height\) = 0

**Parameters<a name="section47106995"></a>**

|Field Name|Input/Output|Field Type|Description|
|--|--|--|--|
|width|Input|uint32_t|Image width in pixels, which must be an even number.|
|height|Input|uint32_t|Image height in pixels, which must be an even number.|

**Return Value Description<a name="section7911548131510"></a>**

Data type: int32_t

The value can be any of the following:

- `0`: The image size is successfully set.
- Other: Failed to set the image size.

### 11.4 Opus Encoding and Decoding APIs (C and C++)<a name="ZH-CN_TOPIC_0000002518186360"></a>

#### 11.4.1 API Description<a name="ZH-CN_TOPIC_0000002549706123"></a>

The audio playback and microphone input functions of the video stream engine support the input and output of Opus streams. This requires the Opus encoding and decoding library to provide encoding and decoding capabilities. This library is implemented by secondary developers and is provided as a DLL. The name of the DLL is `libVmiOpus.so`.

These APIs are similar to those of open-source software [Opus 1.3.1](https://github.com/xiph/opus/tree/v1.3.1). You can refer to the APIs and data interface definitions of Opus.

#### 11.4.2 OpusEncoderCreateApi<a name="ZH-CN_TOPIC_0000002518346286"></a>

**Function Usage<a name="section20501062"></a>**

Creates an Opus encoder. This API is similar to the opus_encoder_create interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

OpusEncoder \*OpusEncoderCreateApi\(opus_int32 Fs, int channels, int application, int \*error\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_encoder_create interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_encoder_create interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.3 OpusEncodeApi<a name="ZH-CN_TOPIC_0000002549826177"></a>

**Function Usage<a name="section20501062"></a>**

Performs Opus encoding. This API is similar to the opus_encode interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

opus_int32 OpusEncodeApi\(OpusEncoder \*st, const opus_int16 \*pcm, int frame_size, unsigned char \*data, opus_int32 max_data_bytes\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_encode interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_encode interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.4 OpusEncoderCtlApi<a name="ZH-CN_TOPIC_0000002549706181"></a>

**Function Usage<a name="section20501062"></a>**

Controls Opus encoding. This API is similar to the opus_encoder_ctl interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

int OpusEncoderCtlApi\(OpusEncoder \*st, int request, opus_int32 par\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_encoder_ctl interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_encoder_ctl interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.5 OpusEncoderDestroyApi<a name="ZH-CN_TOPIC_0000002549826133"></a>

**Function Usage<a name="section20501062"></a>**

Destroys the Opus encoder. This API is similar to the opus_encoder_destroy interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

void OpusEncoderDestroyApi\(OpusEncoder \*st\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_encoder_destroy interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_encoder_destroy interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.6 OpusDecoderCreateApi<a name="ZH-CN_TOPIC_0000002549826127"></a>

**Function Usage<a name="section20501062"></a>**

Creates an Opus decoder. This API is similar to the opus_decoder_create interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

OpusDecoder \*OpusDecoderCreateApi\(opus_int32 Fs, int channels, int \*error\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_decoder_create interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_decoder_create interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.7 OpusDecodeApi<a name="ZH-CN_TOPIC_0000002549706157"></a>

**Function Usage<a name="section20501062"></a>**

Performs Opus decoding. This API is similar to the opus_decode interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

int OpusDecodeApi\(OpusDecoder \*st, const unsigned char \*data, opus_int32 len, opus_int16 \*pcm, int frame_size, int decode_fec\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_decode interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_decode interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.8 OpusDecoderCtlApi<a name="ZH-CN_TOPIC_0000002518346284"></a>

**Function Usage<a name="section20501062"></a>**

Controls Opus decoding. This API is similar to the opus_decoder_ctl interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

int OpusDecoderCtlApi\(OpusDecoder \*st, int request, opus_int32 par\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_decoder_ctl interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_decoder_ctl interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.9 OpusDecoderDestroyApi<a name="ZH-CN_TOPIC_0000002549826131"></a>

**Function Usage<a name="section20501062"></a>**

Destroys the Opus decoder. This API is similar to the opus_decoder_destroy interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

void OpusDecoderDestroyApi\(OpusDecoder \*st\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_decoder_destroy interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_decoder_destroy interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

#### 11.4.10 OpusStrerrorApi<a name="ZH-CN_TOPIC_0000002549706133"></a>

**Function Usage<a name="section20501062"></a>**

Converts an Opus error code into a human-readable string. This API is similar to the opus_strerror interface in Opus 1.3.1.

**Prototype<a name="section49973353"></a>**

const char \*OpusStrerrorApi\(int error\)

**Parameters<a name="section47106995"></a>**

This API is similar to the opus_strerror interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.

**Return Value Description<a name="section7911548131510"></a>**

This API is similar to the opus_strerror interface in Opus 1.3.1. For details, refer to the Opus 1.3.1 documentation.
