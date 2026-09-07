# Feature Guide

<!-- md-trans-meta sourceCommit=8137c5c6f22411c010ac7e5e01f7b05f199cf237 translatedAt=2026-08-29T08:15:02.634Z pushedAt=2026-09-01T06:06:52.021Z -->

## Introduction

The video stream engine is mainly applied to cloud phones. The cloud phone solution implemented based on the video stream engine technology is called video stream cloud phone. This document describes the basic concepts of the video stream engine and provides guidance for setting up the environment and using the video stream engine.

The cloud phone solution is a virtual phone service virtualized based on the Arm server, and runs the Android Open Source Project (AOSP). In short, cloud phones are Arm servers that run the Android OS and function as virtual phones. You can remotely control the cloud phone in real time to run Android applications on the cloud. Based on the basic computing power of cloud phones, you can also efficiently build applications for scenarios like cloud gaming, mobile office, and live streaming interaction.

The device-cloud synergy engine consists of the device side and the cloud side. The cloud side runs on a server; the device side is generally a cloud phone APK, which can be installed on your Android mobile phone to interact with the cloud side and operate the Kbox container.

The device-cloud synergy engine consists of the video stream engine and instruction stream engine. This document describes the video stream engine.

## Software Architecture

This section describes the context logical structure and modules (including module functions) of the video stream cloud phone.

**Figure 1 Video stream cloud phone architecture**

![](figures/video-stream-cloud-phone-architecture.png)

The video stream engine consists of the server and client. The server provides functions such as image capture and encoding, and the client decodes and plays video data. In some scenarios, functions such as obtaining and injecting user touch data, and obtaining and playing audio data are also supported.

|Module|Function|
| :---: | :---: |
|Capture module|Obtains image data. The output format is the RGBA video RAM address or RGBA memory address.|
|Encoding module|Encodes YUV data into H.264/H.265 streams and sends the streams through external APIs of the video stream engine.|
|GPU acceleration module|Converts the RGBA data obtained by the capture module into YUV data or video streams by utilizing GPU capabilities.|
|Audio data collection|Obtains audio data, outputs audio data in OPUS or PCM format, and sends the data through external APIs of the video stream engine.|
|Microphone injection|Obtains OPUS or PCM data from external APIs of the video stream engine and injects the data to the Android system.|
|Touch data distribution|Injects touch data into the Android cloud phone on the server.|
|External APIs|External APIs of the video stream engine server.|

## Specifications

[**Table 1**](#video-stream-cloud-phone-specifications) lists the specifications of the video stream cloud phone on Kunpeng servers.

**Table 1** Video stream cloud phone specifications<a id="video-stream-cloud-phone-specifications"></a>

| Item | Specification |
| :---: | :---: |
| Scenario | Hardware encoding for a mid-core game (login page of Honor of Kings) |
| Core binding policy | Bind containers to NUMA nodes. Reserve the first two cores of each NUMA node. |
| Memory | 3 GB |
| Storage | 16 GB |
| Resolution/Frame rate | 720 x 1280/30 fps |

>![](public_sys-resources/icon-note.gif) **NOTE**
>
> The memory and drives can be flexibly configured based on the device specifications.

## Reference Standards and Protocols

None

## Constraints

You need to fully understand the following constraints on the hardware, system, and applications when you deploy and use the video stream cloud phone.

- Software and hardware constraints

  - The video stream engine server runs in the Kbox container, and the OS is Android 11.0.0.
  - The video stream engine client is 64-bit and must run on 64-bit mobile phones running HarmonyOS or Android 7 or later.
  - The video encoding function of the video stream engine server depends on a third-party video encoding module. The module can be either a software or hardware implementation, for example, the NETINT Quadra T2A video encoding card. If the constant bit rate mode is used for encoding, the image may be blurry for a short time when the image changes rapidly. This is because the bit rate requirement in this scenario is too high. You can increase the bit rate to alleviate the problem.
  - The video decoding function of the video stream engine client depends on the hardware video decoding capability of the mobile phone.
  - The hardware-related performance and specifications described in this document need to be tested on specified hardware.

- Usage constraints

  - This project delivers the binary library of the video stream engine to third parties and promises the quality, performance, and security of the engine. Third parties integrate the video stream engine based on their service scenarios and define product specifications and performance. In addition, third parties are responsible for scenario definition, service development, network design, system security, and service O&M.
  - The unique one-click cleanup function of Huawei phones will not terminate active processes with data transmission in the background because the video stream engine client needs to maintain heartbeat data to ensure continuous network connection with the video stream engine server. Therefore, on Huawei phones, the video stream engine client will not be stopped during one-click cleanup.
  - The host OS is not within the delivery scope of this solution. Therefore, security hardening measures related to the OS need to be implemented by you or the independent software vendor (ISV). This document does not provide related descriptions.
  - In this document, the root permission is used by default to invoke scripts. The scripts are for reference only, and no commercial commitment is made. To minimize OS security risks, it is recommended that you or the ISV implement defense measures in commercial deployment scenarios, such as file access control and minimum authorization.
  - When performing operations based on this document, configure appropriate permissions on the uploaded files and directories. You are advised not to configure the write permission for other user groups.
  - To ensure the stable running and optimal performance of the video stream cloud phone, ensure that the bound GPU nodes and CPU cores are within the same socket when starting the cloud phone.

- Other constraints

  - The compatibility between applications and Android emulators, such as third-party login, advertisement, payment, customer feedback, extended games, and emulator detection, depends on the compatibility capability of the cloud phone image.

  - If you enable the dynamic frame rate adjustment function, set the **ro.hardware.downfps** environment variable to **12** or **24** to avoid unknown exceptions caused by insufficient frame rates.

  - When hardware configuration scheme 1 is used, if you enable the composition bypass function, the screen may rotate when you enter or exit a game or bring up the text input field. Evaluate the impact to determine whether to enable this function.

  - The video stream engine relies on the secure and reliable communication mechanism provided by a third party for session authentication and data transmission/encryption.

  - Commercial binary files comply with the minimum permission principle of Android. The file owner and owner group must be **root**.

  - When a video stream cloud phone runs for a long time, it occupies a large amount of cache on the host. Run the following command to periodically clear the cache:

    ```bash
    echo 3 > /proc/sys/vm/drop_caches
    ```

>![](public_sys-resources/icon-note.gif) **NOTE**
>
> Clearing the cache will temporarily affect the system performance. Therefore, you need to control the clearing period and granularity.
>
>- Clear the page cache.
>
> ```shell
> echo 1 > /proc/sys/vm/drop_caches
> ```
>
>- Clear kernel slab objects (including inode and dentry).
>
> ```shell
> echo 2 > /proc/sys/vm/drop_caches
> ```
>
>- Clear both the page cache and kernel slab objects.
>
> ```shell
> echo 3 > /proc/sys/vm/drop_caches
> ```

## Application Scenarios

The video stream engine is an important component of the cloud phone solution and one of the approaches to implementing device-cloud synergy on Android OSs. The video stream solution has advantages such as good compatibility with cloud applications, low requirements on device hardware, mature technologies, and stable network bandwidth. Therefore, the video stream cloud phone is the mainstream solution.

The main application scenarios of video stream cloud phones include cloud phones, cloud gaming, and mobile office.

## Principles

The design of the video stream cloud phone is to divide the video stream engine into the video stream engine server and the video stream engine client. The server obtains and encodes image data, and the client decodes and plays video data. In some scenarios, functions such as obtaining and injecting user touch data, and obtaining and playing audio data are also supported.

## Change History

|Document Version|Date|Description|
|--|--|--|
|01|2026-09-30|This is the first official release.|
