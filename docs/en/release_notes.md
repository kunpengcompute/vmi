# Release Notes

## Version Requirements

### Product Version

| Item | Details |
| ---- | ------- |
| Product Name | Kunpeng BoostKit |
| Product Version | 26.0.RC1 |
| Software Name | Video stream engine |
| Software Package Version | 8.0.RC1_15 |

### Software Versions

| Software | Version | Remarks |
| -------- | ------- | ------- |
| Kunpeng BoostKit | Kunpeng BoostKit 26.0.RC1 | - |
| OS | openEuler-24.03-LTS-SP1-AArch64 (kernel 6.6.0-72.0.0) | - |
| ExaGear | ExaGear ARM32-ARM64 | Transcoding software |

### Hardware Versions

| Server | Processor | BIOS | CPLD | BMC |
| ------ | --------- | ---- | ---- | --- |
| Kunpeng server | Kunpeng 920 processor | 6.56 | 5.09 | 5.96 |
| Kunpeng server | New Kunpeng 920 processor model | 20.55 | 5.08 | 5.05.12.15 |

### Virus Scan Results

The software package and related documents have been scanned by antivirus software and no risks have been found.

| Antivirus Software | Antivirus Software Version | Virus Library Version | Scan Time | Scan Result |
| ------------------ | -------------------------- | --------------------- | --------- | ----------- |
| QiAnXin | 8.0.5.5260 | 2026-03-16 08:00:00.0 | 2026-03-17 17:34:38 | OK |
| Bitdefender | 7.5.1.200224 | 7.100367 | 2026-03-17 17:34:46 | OK |
| Kaspersky | 12.0.0.6672 | 2026-03-17 10:04:00 | 2026-03-17 17:34:35 | OK |

## Important Notes

For details, see "Constraints" in [Video Stream Engine Feature Guide (Android 15)](feature_guide.md).

## Version Mappings

### Embedded Platform Mapping

| Software Name | Detailed Version Number | Hardware Platform | Platform Software Name | Platform Software Version | Processor Type/OS | Remarks |
| ------------- | ----------------------- | ----------------- | ---------------------- | ------------------------- | ----------------- | ------- |
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | Kunpeng server | iBMC | S920X20-S920X21-iBMC_5.05.12.15 | New Kunpeng 920 processor model, 2.9 GHz<br>Host OS: openEuler-24.03-LTS-SP1-AArch64 (kernel 6.6.0-72.0.0)<br>Guest OS: Android-15.0.0_r17 | openEuler |
|  |  |  | CPLD | S920X20-BCU(BC83AMDA-BC83AMDB)-CPLD1-CPLD2_5.08 |  |  |
|  |  |  | BIOS | S920X20-BIOS_20.55 |  |  |
|  |  |  | ExaGear | ExaGear_ARM32-ARM64 | Host OS: openEuler-24.03-LTS-SP1-AArch64<br>Guest OS: Android-15.0.0_r17 |  |
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | Kunpeng server | iBMC | TS200-1280_2180_2280_5280_2180K_2280K_5280K-iBMC-V596 | Kunpeng 920, 2.6 GHz<br>Host OS: openEuler-24.03-LTS-SP1-AArch64 (kernel 6.6.0-72.0.0)<br>Guest OS: Android-15.0.0_r17 | openEuler |
|  |  |  | CPLD | TS200-2280-2180-5280-2180K-2280K-5280K_Mainboard(BC2AMDD01)_CPLD_5.0.9 |  |  |
|  |  |  | BIOS | TS200-2180_2280_5180_5280-BIOS-V656 |  |  |
|  |  |  | ExaGear | ExaGear_ARM32-ARM64 | Host OS: openEuler-24.03-LTS-SP1-AArch64<br>Guest OS: Android-15.0.0_r17 |  |

### Product Version Mapping

**Version Information**

| Item | Details |
| ---- | ------- |
| Product Name | Kunpeng BoostKit |
| Product Version | Kunpeng BoostKit 25.3.0 |
| Detailed Version Number | Kunpeng BoostKit 25.3.0.B016 |

**Software Version Mapping**

| Software Name | Version | Hardware Platform | OS | Database System |
| ------------- | ------- | ----------------- | -- | --------------- |
| BoostKit-videoengine_7.3.0_15.zip | B016 | Kunpeng server | openEuler-24.03-LTS-SP1-AArch64<br>Android-15.0.0_r17 | / |
| Kbox-AOSP15.zip | B016 |  |  |  |

### Application Compatibility List

**AOSP 15 App Compatibility List**

| No. | Name | Type | Version | Download Source | Issue | Description | Cause Analysis |
| --- | ---- | ---- | ------- | --------------- | ----- | ----------- | -------------- |
| 1 | Honor of Kings | Game | Updated with the Appstore | Appstore | None |  |  |
| 2 | Genshin Impact | Game | Updated with the Appstore | Appstore | None |  |  |
| 3 | Star Rail | Game | Updated with the Appstore | Appstore | None | Currently, the Vulkan API is disabled for the application to avoid rendering problems. After the driver is fixed, users can manually enable this API. |  |
| 4 | Douyin | Application | Updated with the Appstore | Appstore | None |  |  |
| 5 | Kuaishou | Application | Updated with the Appstore | Appstore | None |  |  |
| 6 | WeChat | Application | Updated with the Appstore | Appstore | None |  |  |
| 7 | QQ | Application | Updated with the Appstore | Appstore | None |  |  |
| 8 | WPS | Application | Updated with the Appstore | Appstore | None |  |  |
| 9 | Weibo | Application | Updated with the Appstore | Appstore | None |  |  |
| 10 | Tencent Video | Application | Updated with the Appstore | Appstore | None |  |  |
| 11 | iQIYI | Application | Updated with the Appstore | Appstore | 1. Small-window playback may result in application not responding or frame freezing.<br>2. Under the light mode, the background color of the short video comment section exhibits an anomaly, resulting in insufficient contrast between the comment text and the background. The display remains normal under the dark mode. |  |  |
| 12 | Eggy Party | Application | Updated with the Appstore | Appstore | In the AMD environment, when software decoding is configured, the background video on the login page cannot be played properly. Additionally, in-game live streams encounter black screen issues (the game UI remains normal). This problem can be solved by configuring hardware decoding. |  |  |

> [!NOTE]
> Application compatibility test conditions:
>
> 1. The test environment is the standard hardware environment on the 2280 server.
> 2. Cloud phone specifications: 8 cores and 1 channel, 16 GB memory, and 64 GB storage.
> 3. A single container runs only one game app during the test.
> 4. The current version does not support Wi-Fi settings and Wi-Fi search.
> 5. The current version does not support Bluetooth devices.

## 8.0.RC1

### Change Description

**New Features**

None

**Modified Features**

None

**Removed Features**

None

### Resolved Issues

None

### Known Issues

| Item | Details |
| ---- | ------- |
| Trouble Ticket No. | DTS2025120110758 |
| Severity | Minor |
| Symptom | In the AMD environment of Android 15, after the system has run for four days under high load, the CPU load abnormally decreases and the container cannot be deleted. |
| Cause Analysis | There is a low probability that the amdgpu driver in kernel 6.6 encounters a page fault, causing the GPU to restart. After the application exits, the CPU load decreases. There is a low probability that the restart fails. In this case, the Android system processes that use GPU resources are suspended. As a result, the container cannot clear the processes and fails to be deleted. |
| Impact Assessment | This problem is caused by the amdgpu driver in kernel 6.6. According to issues in the upstream community, this is a common problem. AMD has not fixed this issue or provided a fix plan. |
| Workaround | This issue does not occur after the Mesa driver is replaced with the Mesa version built in Android 15. |
| Progress | Future efforts will focus on analyzing the differences between these two Mesa driver versions, with results to be submitted to the upstream community to facilitate a fix. |

## V7.3.0_15

### Change Description

**New Features**

| No. | Description | Purpose |
| --- | ----------- | ------- |
| 1 | Video stream engine adaptation | Adapted the video stream engine to Android 15 and completed stream output. |

**Modified Features**

None

**Removed Features**

None

### Resolved Issues

None

### Known Issues

| Item | Details |
| ---- | ------- |
| Trouble Ticket No. | DTS2025120110758 |
| Severity | Minor |
| Symptom | In the AMD environment of Android 15, after the system has run for four days under high load, the CPU load abnormally decreases and the container cannot be deleted. |
| Cause Analysis | There is a low probability that the amdgpu driver in kernel 6.6 encounters a page fault, causing the GPU to restart. After the application exits, the CPU load decreases. There is a low probability that the restart fails. In this case, the Android system processes that use GPU resources are suspended. As a result, the container cannot clear the processes and fails to be deleted. |
| Impact Assessment | This problem is caused by the amdgpu driver in kernel 6.6. According to issues in the upstream community, this is a common problem. AMD has not fixed this issue or provided a fix plan. |
| Workaround | This issue does not occur after the Mesa driver is replaced with the Mesa version built in Android 15. |
| Progress | Future efforts will focus on analyzing the differences between these two Mesa driver versions, with results to be submitted to the upstream community to facilitate a fix. |

## Related Documentation

### V7.3.0_15 Documentation

| No. | Document | Description | How to Obtain |
| --- | -------- | ----------- | ------------- |
| 1 | Kunpeng BoostKit 26.0.RC1 Developer Guide | Describes the APIs of the video stream engine and provides development examples. | Kunpeng community |
| 2 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Feature Guide (Android 15) | Describes the features of the device- and cloud-side engines. | Kunpeng community |
| 3 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine VM Environment Deployment Guide (Android 15) | Describes how to deploy video stream cloud phone containers on KVM-based VMs in terms of VM deployment and configuration. | Kunpeng community |
| 4 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Release Notes (Android 15) | Provides the release notes of the video stream engine. | Kunpeng community |
| 5 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Version Mapping (Android 15) | Describes the version mapping of the video stream engine. | Kunpeng community |
| 6 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Personal Data Description | Describes the personal data involved in the video stream engine. | Kunpeng community |
| 7 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Routine Maintenance (Android 15) | Describes how to perform routine maintenance on the device- and cloud-side engines. | Kunpeng community |

### Obtaining Documentation

Visit the [Kunpeng community](https://www.hikunpeng.com/document/detail/en/kunpengcps/overview/kunpengcps.html) to view or download related documents.
