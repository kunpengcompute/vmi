# Test Guide<a name="ZH-CN_TOPIC_0000002521895632"></a>

## Introduction<a name="ZH-CN_TOPIC_0000002549706223"></a>

### Acceptance Criteria<a name="ZH-CN_TOPIC_0000002549826213"></a>

This document provides guidance for accepting cloud phone products. Before the acceptance, ensure that the physical environment, system environment, and software version are correct. The test cases in this document are designed by the cloud phone product test team, covering the basic functions, performance, and reliability.

### Important Notes<a name="ZH-CN_TOPIC_0000002549826223"></a>

1. Before the acceptance, ensure that the physical environment, system environment, and software version are correct and matched.
2. Before executing the acceptance test cases, deploy the end-to-end environment of the video stream cloud phone. For details, see [Installation Guide](install_guide.md).
3. Ensure that the acceptance items are confirmed by Huawei and the customer.
4. During the product acceptance and preliminary acceptance tests, both parties should strictly observe applicable test criteria. Because some items have been tested before delivery, you can omit or sample such items if site conditions are limited.

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>Perform the acceptance in accordance with the contract and agreement reached by both parties. This document serves as a reference only.

## Test Preparations<a name="ZH-CN_TOPIC_0000002549706221"></a>

For details about the server hardware and software package information, environment deployment before test case acceptance, and the density test method, see [Installation Guide](install_guide.md). For details about the BIOS, iBMC, and CPLD versions, see [Version Mappings](release_notes.md#ZH-CN_TOPIC_0000002518346261).

## Test Conventions<a name="ZH-CN_TOPIC_0000002549826217"></a>

**Result Description<a name="zh-cn_topic_0000001214689902_section62693428"></a>**

The test results are defined as follows:

- **PASS**: The test result is consistent with the expected result after a test is performed based on the prerequisites and preset procedure.
- **FAIL**: The test result is inconsistent with the expected result after a test is performed based on the prerequisites and preset procedure.
- **NT**: The test is not implemented because the requirements have changed or the test environment does not meet the requirements.

## Test Cases and Test Records<a name="ZH-CN_TOPIC_0000002549706217"></a>

### Basic Functional Tests<a name="ZH-CN_TOPIC_0000002549826221"></a>

#### Video Stream Cloud Phone Codec Test<a name="ZH-CN_TOPIC_0000002518346358"></a>

<a name="table35101782"></a>

| Item| Content|
| --- | --- |
| Case No.| 4.1.1 |
| Test Objective| Verify the video codec function of the video stream cloud phone.|
| Test Networking| None|
| Prerequisites| 1. The video stream cloud phone environment has been deployed.<br>2. The client has been connected to the video stream cloud phone.<br>3. An app has been installed on the video stream cloud phone. (You are advised to select an app from the compatibility list, such as Subway Surfers and KuGou.)|
| Test Procedure| Run the app on the client for 10 minutes and observe the screen display of each scenario.|
| Expected Result| The encoding and decoding modules of the video stream cloud phone run properly. During the running of the app, the image rendering, interface switching, and functions are normal. No artifact, black screen, crash, serious frame freezing, or breakdown occurs. The screen orientation switchover is normal. The video stream cloud phone container exhibits no anomalies, such as system crashes or unresponsiveness.|
| Test Result|  |
| Remarks|  |

#### Video Stream Cloud Phone Touch Test<a name="ZH-CN_TOPIC_0000002549706219"></a>

<a name="table35101782"></a>

| Item| Content|
| --- | --- |
| Case No.| 4.1.2 |
| Test Objective| Verify the touch control function of the video stream cloud phone.|
| Test Networking| None|
| Prerequisites| 1. The video stream cloud phone environment has been deployed.<br>2. The client has been connected to the video stream cloud phone.<br>3. An app has been installed on the video stream cloud phone. (You are advised to select an app from the compatibility list, such as Subway Surfers and KuGou.)|
| Test Procedure| On the client, perform click or swipe operations for 10 minutes on the cloud phone or the app screen.|
| Expected Result| The touch control function is normal and smooth without frame freezing.|
| Test Result|  |
| Remarks|  |

#### Video Stream Cloud Phone Audio Test<a name="ZH-CN_TOPIC_0000002518186456"></a>

<a name="table35101782"></a>

| Item| Content|
| --- | --- |
| Case No.| 4.1.3 |
| Test Objective| Verify the audio function of the video stream cloud phone.|
| Test Networking| None|
| Prerequisites| 1. The video stream cloud phone environment has been deployed.<br>2. The client has been connected to the video stream cloud phone.<br>3. KuGou has been installed on the video stream cloud phone.|
| Test Procedure| Start KuGou, play a song, and adjust the volume.|
| Expected Result| The audio is played properly, and the volume can be adjusted.|
| Test Result|  |
| Remarks|  |

#### Video Stream Cloud Phone Microphone Test<a name="ZH-CN_TOPIC_0000002549826215"></a>

<a name="table35101782"></a>

| Item| Content|
| --- | --- |
| Case No.| 4.1.4 |
| Test Objective| Verify the microphone function of the video stream cloud phone.|
| Test Networking| None|
| Prerequisites| 1. The video stream cloud phone environment has been deployed.<br>2. The client has been connected to the video stream cloud phone.<br>3. A voice recording app has been installed on the video stream cloud phone.|
| Test Procedure| Start the voice recording app and record voices.|
| Expected Result| The recording is successful, and the recording file is played properly.|
| Test Result|  |
| Remarks|  |

#### Video Stream Cloud Phone High Resolution Test<a name="ZH-CN_TOPIC_0000002549826219"></a>

<a name="table35101782"></a>

| Item| Content|
| --- | --- |
| Case No.| 4.1.7 |
| Test Objective| Verify that the video stream cloud phone supports high resolution.|
| Test Networking| None|
| Prerequisites| 1. The video stream cloud phone environment has been deployed.<br>2. Change the resolution to 1080p by referring to "Software Deployment" in [Installation Guide](install_guide.md), and then start the video stream cloud phone.<br>3. An app has been installed on the video stream cloud phone. (You are advised to select an app from the compatibility list, such as Subway Surfers and KuGou.)|
| Test Procedure| 1. Connect a physical phone to the cloud phone client and run the installed app.<br>2. Start the app and try it out for 10 minutes.|
| Expected Result| 1. The video stream cloud phone and app are running properly without frame freezing or black screen.<br>2. Run the `docker exec -it android_x sh` command to log in to the cloud phone, and then run the `wm size` command to check the resolution. The 1080p resolution is set successfully.|
| Test Result|  |
| Remarks| In the test procedure, `x` in `android_x` indicates the ID of the created video stream cloud phone container.|

#### Video Stream Cloud Phone High Frame Rate Test<a name="ZH-CN_TOPIC_0000002518346368"></a>

<a name="table35101782"></a>

| Item| Content|
| --- | --- |
| Case No.| 4.1.8 |
| Test Objective| Verify that the video stream cloud phone supports a high frame rate.|
| Test Networking| None|
| Prerequisites| 1. The video stream cloud phone environment has been deployed.<br>2. Change the frame rate to 60 fps by referring to "Software Deployment" in [Installation Guide](install_guide.md), and then start the video stream cloud phone.<br>3. An app has been installed on the video stream cloud phone. (You are advised to select an app from the compatibility list, such as Subway Surfers and KuGou.)|
| Test Procedure| 1. Connect a physical phone to the cloud phone client and run the installed app.<br>2. Start the app and try it out for 10 minutes.|
| Expected Result| The video stream cloud phone and app are running properly. The frame rate displayed in the floating window in the upper left corner on the video stream cloud phone client APK is stable at 55 fps to 60 fps.|
| Test Result|  |
| Remarks|  |

#### Video Stream Component Version Query<a name="ZH-CN_TOPIC_0000002549826225"></a>

<a name="table35101782"></a>

| Item| Content|
| --- | --- |
| Case No.| 4.1.9 |
| Test Objective| Verify the function of querying the video stream component version.|
| Test Networking| None|
| Prerequisites| 1. The video stream cloud phone environment has been deployed.<br>2. A video stream cloud phone container has been created and connected to the ADB.|
| Test Procedure| 1. Run the `sudo docker exec -it android_x` sh command to access the video stream cloud phone container. Expected result 1 is displayed.<br>2. Run the `cat /vendor/etc/videoengine_version.txt` command to query the version. Expected result 2 is displayed.|
| Expected Result| 1. The container can be accessed.<br>2. The correct version information of the video stream component is displayed as follows. (The actual version number prevails.)<br>Product Name: Kunpeng BoostKit<br>Product Version: xxx<br>Component Name: BoostKit-boostcph-videoengine<br>Component Version: xxx<br>Component AppendInfo: 15.0.0_r17 |
| Test Result|  |
| Remarks| In the test procedure, `x` in `android_x` indicates the ID of the created video stream cloud phone container.|

## Test Result Analysis<a name="ZH-CN_TOPIC_0000002518186450"></a>

### Basic Test Information<a name="ZH-CN_TOPIC_0000002549706211"></a>

<a name="table56604068"></a>

| Item| Content|
| --- | --- |
| Device Manufacturer|  |
| Device Model|  |
| Test Location|  |
| Test Personnel|  |
| Test Time|  |
| Other Information|  |

### Test Result List<a name="ZH-CN_TOPIC_0000002518186446"></a>

|Test Type|Case ID.|Case Name|Test Result (PASS/FAIL/NT)|
|--|--|--|--|
|Basic functional tests|4.1.1|Video Stream Cloud Phone Codec Test|
|Basic functional tests|4.1.2|Video Stream Cloud Phone Touch Test|
|Basic functional tests|4.1.3|Video Stream Cloud Phone Audio Test|
|Basic functional tests|4.1.4|Video Stream Cloud Phone Microphone Test|
|Basic functional tests|4.1.5|Video Stream Cloud Phone GPS Passthrough Test|
|Basic functional tests|4.1.6|Video Stream Cloud Phone Sensor Passthrough Test|
|Basic functional tests|4.1.7|Video Stream Cloud Phone High Resolution Test|
|Basic functional tests|4.1.8|Video Stream Cloud Phone High Frame Rate Test|
|Basic functional tests|4.1.9|Video Stream Component Version Query|
|Performance Test|4.2.1|Video Stream Cloud Phone Density Test|

## Customer Suggestions and Result Confirmation<a name="ZH-CN_TOPIC_0000002518186448"></a>

### Customer Suggestions<a name="ZH-CN_TOPIC_0000002549706213"></a>

### Result Confirmation<a name="ZH-CN_TOPIC_0000002518346362"></a>

|Tested Party: Huawei Technologies Co., Ltd.|Testing Party:|
|--|--|
|Test Personnel Signature:|Test Personnel Signature:|
|Time:|Time:|
