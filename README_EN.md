# Video Stream Introduction<a name="ZH-CN_TOPIC_0000002550291729"></a>

<!-- md-trans-meta sourceCommit=7a9be47c8b0d73507834023b6671a1277c7f0640 translatedAt=2026-10-15T11:54:42.537Z pushedAt=2026-10-16T02:10:21.963Z -->

[简体中文](./README.md) | English

## What's New

\[2026-09-30\]: Added support for the shared data volume feature.

\[2026-06-30\]: Enhanced emulation for the CPU/F2FS file system/system partition/init process.

\[2025-12-30\]: Supports Android 15.

## Project Description<a name="ZH-CN_TOPIC_0000002518611972"></a>

### Overview<a name="ZH-CN_TOPIC_0000002518771870"></a>

The video stream engine is mainly applied to cloud phones. The cloud phone solution implemented based on the video stream engine technology is called video stream cloud phone. This document describes the basic concepts of the video stream engine and provides guidance for setting up the environment and using the video stream engine.

The cloud phone solution is a virtual phone service virtualized based on the Arm server and runs the Android Open Source Project (AOSP). In short, cloud phones are Arm servers that run the Android OS and function as virtual phones. You can remotely control the cloud phone in real time to run Android applications on the cloud. Based on the basic computing power of cloud phones, you can also efficiently build applications for scenarios like cloud gaming, mobile office, and live streaming interaction.

The device-cloud synergy engine consists of the device side and the cloud side. The cloud side runs on a server; the device side is generally a cloud phone APK, which can be installed on your Android mobile phone to interact with the cloud side and operate the Kbox container.

The device-cloud synergy engine consists of the video stream engine and instruction stream engine. This document describes the video stream engine.

### Software Architecture<a name="ZH-CN_TOPIC_0000002518611974"></a>

This section describes the context logical structure and modules (including module functions) of the video stream cloud phone.

**Figure 1** Video stream cloud phone architecture<a name="fig0420851397"></a><a id="video-stream-cloud-phone-architecture"></a>
![](docs/en/figures/video-stream-cloud-phone-architecture.png "Video stream cloud phone architecture")

The video stream engine consists of the server and client. The server provides functions such as image capture and encoding, and the client decodes and plays video data. In some scenarios, functions such as obtaining and injecting user touch data, and obtaining and playing audio data are also supported.

|Module|Description|
|--|--|
|Capture module|Obtains image data. The output format is the RGBA video RAM address or RGBA memory address.|
|Encoding module|Encodes YUV data into H.264/H.265 streams and sends them through external APIs of the video stream engine.|
|GPU acceleration module|Converts the RGBA data obtained by the capture module into YUV data or video streams by utilizing GPU capabilities.|
|Audio data collection|Obtains audio data, outputs audio data in OPUS or PCM format, and sends the data through external APIs of the video stream engine.|
|Microphone injection|Obtains OPUS or PCM data from external APIs of the video stream engine and injects the data to the Android system.|
|Touch data distribution|Injects touch data into the Android cloud phone on the server.|
|External APIs|External APIs of the video stream engine server.|

### Specifications<a name="ZH-CN_TOPIC_0000002518771874"></a>

[**Table 1**](#video-stream-cloud-phone-specifications) lists the specifications of the video stream cloud phone on Kunpeng servers.

**Table 1** Video stream cloud phone specifications<a id="video-stream-cloud-phone-specifications"></a>

|Item|Configuration|
|--|--|
|Scenario|Hardware encoding for a mid-core game (login page of Honor of Kings)|
|CPU core binding policy|Bind containers to NUMA nodes. Reserve the first two cores of each NUMA node.|
|Memory|3 GB|
|Storage|16 GB|
|Resolution/Frame rate|720 x 1280/30 fps|
|Quantity|120 channels|

>![](docs/en/public_sys-resources/icon-note.gif) **NOTE**
>
>The memory and drives can be flexibly configured based on the device specifications.

## Directory Structure<a name="ZH-CN_TOPIC_0000002518771876"></a>

```text
├── docs                                           # Project documentation
│   └── zh                                         # Chinese documentation
│       ├── figures                                # Figure resources
│       ├── public_sys-resources                   # Public icons
│       ├── compile_guide.md                       # Video stream cloud phone compilation guide
│       ├── compile_scripts_introduction.md        # Introduction to video stream cloud phone compilation scripts
│       ├── development_guide.md                   # Video stream cloud phone developer guide
│       ├── feature_guide.md                       # Video stream cloud phone feature guide
│       ├── install_guide.md                       # Video stream cloud phone installation guide
│       ├── menu.md                                # Video stream cloud phone document menu
│       ├── release_notes.md                       # Video stream cloud phone release notes
│       ├── routine_maintenance.md                 # Video stream cloud phone routine maintenance guide
│       ├── security_statement.md                  # Video stream cloud phone security statement
│       ├── test_guide.md                          # Video stream cloud phone test guide
│       ├── troubleshooting.md                     # Video stream cloud phone troubleshooting cases
│       ├── user_guide.md                          # Video stream cloud phone user guide
├── CloudPhoneClient                              # Build directory for the video stream client APK
│   ├── AudioPlay                                # Audio module
│   ├── TouchCapture                             # Touch module
│   ├── VideoDecoder                             # Client decoder control and adaptation layer
│   └── VideoEngine 
│       ├── CloudPhoneUI                         # UI component of the client app
│       └── NdkDecoder                           # Android native NDK decoder
├── CloudPhoneService                             # Build directory for the video stream server
│   ├── VideoEngine 
│   │   ├── GpuEncTurbo                         # Integrates and adapts the GPU encoding module, covering integrated rendering-streaming and GPU encoding solutions. (The hardware decoding module is implemented by vendors at the AOSP OMX layer.)
│   │   └── Media                               # Adaptation layer for software and hardware encoders (encoding cards)
│   ├── VideoScripts                             # Scripts for creating video stream server images
│   └── VmiAgent                                 # Main logic of the video stream server, running continuously after connection to control server module status, initialize the network module, and manage data transmission operations of each module.
├── Common                                        # Common files directory
│   ├── Communication                            # Manages data exchange between the client and the server.
│   │   ├── Connection                          # Client and server implementations of VmiSocket
│   │   ├── Heartbeat                           # Heartbeat module; sends a heartbeat packet at regular intervals (currently 100 seconds) to the peer and waits for a response. It evaluates whether the network connection is slow or disconnected based on the response wait time. If the connection is slow or disconnected, it sends a signal to the video stream engine to actively terminate the connection.
│   │   ├── MsgFragment                         # Message consolidation and transmission
│   │   ├── NetComm                             # Network communication module
│   │   ├── PacketHandle                        # Packet handler for packet queuing and reassembly functions
│   │   ├── PacketManager                       # Packet manager; creates packet queues to store reassembled packets awaiting further processing.
│   │   ├── Socket                              # Encapsulates sockets into VmiSockets for engine use.
│   │   └── StreamParse                         # Message data decapsulation
│   ├── Connection                               # Communication library implemented based on the TCP protocol
│   ├── Log                                      # Log module
│   └── Utils                                    # Other common utilities, including unified engine event reporting, packet queue implementation, and version validation
├── open_source_download                          # Directory for downloaded open-source software packages
├── scripts                                       # Directory for build scripts
└── unpack_open_source                            # Directory for unpacked open-source software
├── hantro                                        # DaoCloud GPU encoding module
├── libdrm                                        # Provides kernel subsystems interacting with the GPU.
├── libva                                         # Provides hardware acceleration for video processing.
├── openH264                                      # Supports H.264 video format encoding and decoding.
└── opus                                          # Audio encoding and decoding software
```

## Version Description<a name="ZH-CN_TOPIC_0000002518611968"></a>

The video stream engine has two branch versions: Android 11 and Android 15. This section describes the differences and feature changes between the two versions.

The video stream engine is developed based on AOSP and currently supports AOSP 11 and AOSP 15. Due to differences in Android versions, the video stream engine has two code branches to support different Android code.

**Table 2** Code branch differences<a id="code-branch-differences"></a>

|Code Branch|AOSP11|AOSP15|
|--|--|--|
|Supported kernel version|5.10|6.6|
|Supported Docker version|18.0|24.0|
|Corresponding AOSP version|11|15|

**Change Description<a name="section4408930144513"></a>**

For detailed information about feature changes in each release, see the *Release Notes*.

## Environment Deployment<a name="ZH-CN_TOPIC_0000002518611970"></a>

For details about the hardware environment and OSs supported by the video stream cloud phone and the software packages required for environment setup, see "Environment Requirements" in the *Deployment Guide*.

The video stream cloud phone supports bare metal servers and VMs. For details, see the *Deployment Guide*.

## Related Documents<a name="ZH-CN_TOPIC_0000002550291725"></a>

|Resource Type|Resource Name|Description|
|--|--|--|
|Document| [Release Notes](./docs/en/release_notes.md)|Provides the basic information and feature update information of each release of the video stream cloud phone.|
|Document| [Feature Guide](./docs/en/feature_guide.md)|Provides an introduction to the functions and features supported by the video stream cloud phone.|
|Document| [Compilation Guide](./docs/en/compile_guide.md)|Provides the compilation and build process and operation instructions for the video stream cloud phone.|
|Document| [Installation Guide](./docs/en/install_guide.md)|Provides detailed instructions for deploying the video stream cloud phone in both bare metal and virtual machine environments.|
|Document| [User Guide](./docs/en/user_guide.md)|Provides operation instructions for starting and uninstalling video stream cloud phone instances.|
|Document| [Developer Guide](./docs/en/development_guide.md)|Provides guidance for developing the video stream cloud phone.|
|Document| [Routine Maintenance](./docs/en/routine_maintenance.md)|Provides routine maintenance instructions for the video stream cloud phone container.|
|Document| [Troubleshooting](./docs/en/troubleshooting.md)|Provides troubleshooting methods for the video stream cloud phone.|
|Document| [Test Guide](./docs/en/test_guide.md)|Provides guidance for product acceptance of the video stream cloud phone.|
|Document| [Security Statement](./docs/en/security_statement.md)|Provides security statement for using the video stream cloud phone.|

## Disclaimer<a name="ZH-CN_TOPIC_0000002550251723"></a>

This repository contains only feature demonstrations and development sample code, intended to showcase the usage and integration methods of specific features. Do not use the code in this repository directly in production environments. All code is provided solely as technical reference and does not inherit or guarantee any security design or protection mechanisms of upstream or downstream software. The sample code in this repository may contain security defects, vulnerabilities, or incomplete implementations. The Kunpeng Computing Community assumes no responsibility for the security, stability, or compliance of the code. Users should assess risks on their own and perform security hardening based on actual scenarios. Any security issues arising from the use of the code in this repository shall be borne solely by the user. It is recommended to keep track of security advisories and version updates from upstream open-source projects.

**To Users of This Project**

- This project is intended solely for debugging and development. You are responsible for any risks and should carefully review the following information:
    - Data processing and deletion: Users are responsible for managing and deleting any data generated while using this project. You are advised to promptly delete any related data after use to prevent information leaks.
    - Data confidentiality and transmission: Users understand and agree not to share or transmit any data generated by this project. Neither the project nor its developers are responsible for any information leaks, data breaches, or other negative consequences.
    - User input security: Users are responsible for the security of any commands they enter and for any risks or losses resulting from improper input. The project and its developers are not liable for issues caused by incorrect command usage.

- Disclaimer scope: This disclaimer applies to all individuals and entities using this project. By using the project, you acknowledge and accept this statement and assume all risks and responsibilities arising from its use. If you do not agree, please stop using the project immediately.
- Before using this project, **please read and understand the preceding disclaimer**. If you have any questions, contact the developer.

**To Data Owners**

If you do not want your model or dataset to be mentioned in this project, or if you wish to update its description, please submit an issue on GitCode. We will delete or update your description according to your request. Thank you for your understanding and contribution to this project.

## License<a name="ZH-CN_TOPIC_0000002518771872"></a>

This project is licensed under Mulan PSL v2. For details, see [LICENSE](LICENSE).
The documents of this project are licensed under CC-BY 4.0. For details, see [LICENSE](docs/LICENSE).

## Contribution Statement<a name="ZH-CN_TOPIC_0000002550251729"></a>

We welcome your contributions to the community. If you have any questions/suggestions or want to provide feedback on feature requirements and bug reports, you can submit [issues](https://gitcode.com/boostkit/community/blob/master/docs/contributor/issue-submit.md). For details, see the [contribution guideline](https://gitcode.com/boostkit/community/blob/master/docs/contributor/contributing.md). You are also welcome to share insights in [Discussions](https://gitcode.com/boostkit/community/discussions). Thank you for your support.

## Acknowledgement<a name="ZH-CN_TOPIC_0000002550251725"></a>

Thank you to everyone in the community for your PRs. We warmly welcome contributions to Kbox!
