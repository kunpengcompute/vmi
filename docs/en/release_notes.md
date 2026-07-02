# Release Notes<a name="ZH-CN_TOPIC_0000002552895601"></a>

## Version Requirements<a name="ZH-CN_TOPIC_0000002549826111"></a>

### Product Version<a name="ZH-CN_TOPIC_0000002518186340"></a>

<a name="table229mcpsimp"></a>
<table><tbody><tr id="row234mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.1.1"><p id="p236mcpsimp"><a name="p236mcpsimp"></a><a name="p236mcpsimp"></a>Product Name</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.1.1 "><p id="p6308719427"><a name="p6308719427"></a><a name="p6308719427"></a>Kunpeng BoostKit</p>
</td>
</tr>
<tr id="row239mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.2.1"><p id="p241mcpsimp"><a name="p241mcpsimp"></a><a name="p241mcpsimp"></a>Product Version</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.2.1 "><p id="p243mcpsimp"><a name="p243mcpsimp"></a><a name="p243mcpsimp"></a><span id="text10612828441"><a name="text10612828441"></a><a name="text10612828441"></a>26.0.RC1</span></p>
</td>
</tr>
<tr id="row244mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.3.1"><p id="p246mcpsimp"><a name="p246mcpsimp"></a><a name="p246mcpsimp"></a>Software Name</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.3.1 "><p id="p248mcpsimp"><a name="p248mcpsimp"></a><a name="p248mcpsimp"></a>Video stream engine</p>
</td>
</tr>
<tr id="row1691834411463"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.4.1"><p id="p1491974414614"><a name="p1491974414614"></a><a name="p1491974414614"></a>Software Package Version</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.4.1 "><p id="p091914413462"><a name="p091914413462"></a><a name="p091914413462"></a>8.0.RC1_15</p>
</td>
</tr>
</tbody>
</table>

### Software Versions<a name="ZH-CN_TOPIC_0000002549826109"></a>

|Software|Version|Remarks|
|--|--|--|
|Kunpeng BoostKit|Kunpeng BoostKit 26.0.RC1|-|
|OS|openEuler-24.03-LTS-SP1-AArch64 (kernel 6.6.0-72.0.0)|-|
|ExaGear|ExaGear ARM32-ARM64|Transcoding software|

### Hardware Versions<a name="ZH-CN_TOPIC_0000002549826113"></a>

|Server|Processor|BIOS|CPLD|BMC|
|--|--|--|--|--|
|Kunpeng server|Kunpeng 920 processor|6.56|5.09|5.96|
|Kunpeng server|New Kunpeng 920 processor model|20.55|5.08|5.05.12.15|

### Virus Scan Results<a name="ZH-CN_TOPIC_0000002549706115"></a>

The software package and related documents have been scanned by antivirus software and no risks have been found.

|Antivirus Software|Antivirus Software Version|Virus Library Version|Scan Time|Scan Result|
|--|--|--|--|--|
|QiAnXin|8.0.5.5260|2026-03-16 08:00:00.0|2026-03-17 17:34:38|OK|
|Bitdefender|7.5.1.200224|7.100367|2026-03-17 17:34:46|OK|
|Kaspersky|12.0.0.6672|2026-03-17 10:04:00|2026-03-17 17:34:35|OK|

## 2 Important Notes<a name="ZH-CN_TOPIC_0000002518346260"></a>

For details, see "Constraints" in [Video Stream Engine Feature Guide (Android 15)](feature_guide.md).

## 3 Version Mappings<a name="ZH-CN_TOPIC_0000002518346261"></a>

### 3.1 Embedded Platform Mapping<a name="ZH-CN_TOPIC_0000002518346262"></a>

| Software Name| Detailed Version Number| Hardware Platform| Platform Software Name| Platform Software Version| Processor Type/OS| Remarks|
|------------------|------------------------------|---------|-----------|----------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------|-------------|
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | Kunpeng server  | iBMC      | S920X20-S920X21-iBMC_5.05.12.15                                            | New Kunpeng 920 processor model, 2.9 GHz<br>Host OS: openEuler-24.03-LTS-SP1-AArch64 (kernel 6.6.0-72.0.0)<br>Guest OS: Android-15.0.0_r17| openEuler|
|                  |                              |         | CPLD      | S920X20-BCU(BC83AMDA-BC83AMDB)-CPLD1-CPLD2_5.08                            |                                                                                                                          |             |
|                  |                              |         | BIOS      | S920X20-BIOS_20.55                                                         |                                                                                                                          |             |
|                  |                              |         | ExaGear| ExaGear_ARM32-ARM64                                                        | Host OS: openEuler-24.03-LTS-SP1-AArch64<br>Guest OS: Android-15.0.0_r17                                                  |             |
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | Kunpeng server  | iBMC      | TS200-1280_2180_2280_5280_2180K_2280K_5280K-iBMC-V596<br>                 | Kunpeng 920, 2.6 GHz<br>Host OS: openEuler-24.03-LTS-SP1-AArch64 (kernel 6.6.0-72.0.0)<br>Guest OS: Android-15.0.0_r17        | openEuler|
|                  |                              |         | CPLD      | TS200-2280-2180-5280-2180K-2280K-5280K_Mainboard(BC2AMDD01)_CPLD_5.0.9<br>|                                                                                                                          |             |
|                  |                              |         | BIOS      | TS200-2180_2280_5180_5280-BIOS-V656<br>                                   |                                                                                                                          |             |
|                  |                              |         | ExaGear| ExaGear_ARM32-ARM64                                                        | Host OS: openEuler-24.03-LTS-SP1-AArch64<br>Guest OS: Android-15.0.0_r17                                                  |             |

### 3.2 Product Version Mapping<a name="ZH-CN_TOPIC_0000002518346263"></a>

| 1 Version Information                  |                              |       |                                                       |       |
|-----------------------------------|------------------------------|-------|-------------------------------------------------------|-------|
| Product Name                             | Kunpeng BoostKit             |       |                                                       |       |
| Product Version                             | Kunpeng BoostKit 25.3.0      |       |                                                       |       |
| Detailed Version Number                            | Kunpeng BoostKit 25.3.0.B016 |       |                                                       |       |
| 2 Software Version Mapping               |                              |       |                                                       |       |
| Table 1 Software version mapping                       |                              |       |                                                       |       |
| Software Name                             | Version                         | Hardware Platform | OS                                                 | Database System|
| BoostKit-videoengine_7.3.0_15.zip | B016                         | Kunpeng server| openEuler-24.03-LTS-SP1-AArch64<br>Android-15.0.0_r17| /     |
| Kbox-AOSP15.zip                   | B016                         |       |                                                       |       |

### 3.3 Application Compatibility List<a name="ZH-CN_TOPIC_0000002518346264"></a>

| Application compatibility test conditions:<br>1. The test environment is the standard hardware environment on the 2280 server.<br>2. Cloud phone specifications: 8 cores and 1 channel, 16 GB memory, and 64 GB storage<br>3. A single container runs only one game app during the test.<br>4. The current version does not support Wi-Fi settings and Wi-Fi search.<br>5. The current version does not support Bluetooth devices.<br>|      |    |         |      |                                                                                  |                                      |      |
|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|------|----|---------|------|----------------------------------------------------------------------------------|--------------------------------------|------|
| AOSP 15 App Compatibility List                                                                                                                                                    |      |    |         |      |                                                                                  |                                      |      |
| No.                                                                                                                                                               | Name  | Type| Version  | Download Source| Issue                                                                              | Description                                | Cause Analysis|
| 1                                                                                                                                                                 | Honor of Kings| Game| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 2                                                                                                                                                                 | Genshin Impact  | Game| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 3                                                                                                                                                                 | Star Rail| Game| Updated with the Appstore| Appstore | None                                                                               | Currently, the Vulkan API is disabled for the application to avoid rendering problems. After the driver is fixed, users can manually enable this API.|      |
| 4                                                                                                                                                                 | Douyin  | Application| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 5                                                                                                                                                                 | Kuaishou  | Application| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 6                                                                                                                                                                 | WeChat  | Application| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 7                                                                                                                                                                 | QQ   | Application| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 8                                                                                                                                                                 | WPS  | Application| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 9                                                                                                                                                                 | Weibo  | Application| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 10                                                                                                                                                                | Tencent Video| Application| Updated with the Appstore| Appstore | None                                                                               |                                      |      |
| 11 | iQIYI| Application| Updated with the Appstore| Appstore| 1. Small-window playback may result in application not responding or frame freezing.<br>2. Under the light mode, the background color of the short video comment section exhibits an anomaly, resulting in insufficient contrast between the comment text and the background. The display remains normal under the dark mode.|  |  |
| 12 | Eggy Party| Application| Updated with the Appstore| Appstore| In the AMD environment, when software decoding is configured, the background video on the login page cannot be played properly. Additionally, in-game live streams encounter black screen issues (the game UI remains normal). This problem can be solved by configuring hardware decoding.|  |  |

## 4 8.0.RC1<a name="ZH-CN_TOPIC_0000002518186342"></a>

### 4.1 Change Description<a name="ZH-CN_TOPIC_0000002549706113"></a>

**New Features<a name="section402mcpsimp"></a>**

None

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### 4.2 Resolved Issues<a name="ZH-CN_TOPIC_0000002549706111"></a>

None

### 4.3 Known Issues<a name="ZH-CN_TOPIC_0000002518186338"></a>

<a name="table202271648124311"></a>
<table><tbody><tr id="row822764816434"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.1.1"><p id="p1622724854314"><a name="p1622724854314"></a><a name="p1622724854314"></a>Trouble Ticket No.</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.1.1 "><p id="p422712488431"><a name="p422712488431"></a><a name="p422712488431"></a>DTS2025120110758</p>
</td>
</tr>
<tr id="row722774812437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.2.1"><p id="p1222734884319"><a name="p1222734884319"></a><a name="p1222734884319"></a>Severity</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.2.1 "><p id="p4227164816433"><a name="p4227164816433"></a><a name="p4227164816433"></a>Minor</p>
</td>
</tr>
<tr id="row1122744819437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.3.1"><p id="p12228114814317"><a name="p12228114814317"></a><a name="p12228114814317"></a>Symptom</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.3.1 "><p id="p92282484433"><a name="p92282484433"></a><a name="p92282484433"></a>In the AMD environment of Android 15, after the system has run for four days under high load, the CPU load abnormally decreases and the container cannot be deleted.</p>
</td>
</tr>
<tr id="row8228144824319"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.4.1"><p id="p62289484438"><a name="p62289484438"></a><a name="p62289484438"></a>Cause Analysis</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.4.1 "><p id="p12282048104312"><a name="p12282048104312"></a><a name="p12282048104312"></a>There is a low probability that the amdgpu driver in kernel 6.6 encounters a page fault, causing the GPU to restart. After the application exits, the CPU load decreases. There is a low probability that the restart fails. In this case, the Android system processes that use GPU resources are suspended. As a result, the container cannot clear the processes and fails to be deleted.</p>
</td>
</tr>
<tr id="row82281448174313"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.5.1"><p id="p02281848114310"><a name="p02281848114310"></a><a name="p02281848114310"></a>Impact Assessment</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.5.1 "><p id="p13228948104315"><a name="p13228948104315"></a><a name="p13228948104315"></a>This problem is caused by the amdgpu driver in kernel 6.6. According to issues in the upstream community, this is a common problem. AMD has not fixed this issue or provided a fix plan.</p>
</td>
</tr>
<tr id="row422874814316"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.6.1"><p id="p722844820438"><a name="p722844820438"></a><a name="p722844820438"></a>Workaround</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.6.1 "><p id="p2247115164414"><a name="p2247115164414"></a><a name="p2247115164414"></a>This issue does not occur after the Mesa driver is replaced with the Mesa version built in Android 15.</p>
</td>
</tr>
<tr id="row1922804814432"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.7.1"><p id="p722844834313"><a name="p722844834313"></a><a name="p722844834313"></a>Progress</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.7.1 "><p id="p192281848174311"><a name="p192281848174311"></a><a name="p192281848174311"></a>Future efforts will focus on analyzing the differences between these two Mesa driver versions, with results to be submitted to the upstream community to facilitate a fix.</p>
</td>
</tr>
</tbody>
</table>

## 5 V7.3.0_15<a name="ZH-CN_TOPIC_0000002518186342"></a>

### 5.1 Change Description<a name="ZH-CN_TOPIC_0000002549706113"></a>

**New Features<a name="section402mcpsimp"></a>**

|No.|Description|Purpose|
|--|--|--|
| 1 | Video stream engine adaptation| Adapted the video stream engine to Android 15 and completed stream output.|

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### 5.2 Resolved Issues<a name="ZH-CN_TOPIC_0000002549706111"></a>

None

### 5.3 Known Issues<a name="ZH-CN_TOPIC_0000002518186338"></a>

<a name="table202271648124311"></a>
<table><tbody><tr id="row822764816434"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.1.1"><p id="p1622724854314"><a name="p1622724854314"></a><a name="p1622724854314"></a>Trouble Ticket No.</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.1.1 "><p id="p422712488431"><a name="p422712488431"></a><a name="p422712488431"></a>DTS2025120110758</p>
</td>
</tr>
<tr id="row722774812437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.2.1"><p id="p1222734884319"><a name="p1222734884319"></a><a name="p1222734884319"></a>Severity</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.2.1 "><p id="p4227164816433"><a name="p4227164816433"></a><a name="p4227164816433"></a>Minor</p>
</td>
</tr>
<tr id="row1122744819437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.3.1"><p id="p12228114814317"><a name="p12228114814317"></a><a name="p12228114814317"></a>Symptom</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.3.1 "><p id="p92282484433"><a name="p92282484433"></a><a name="p92282484433"></a>In the AMD environment of Android 15, after the system has run for four days under high load, the CPU load abnormally decreases and the container cannot be deleted.</p>
</td>
</tr>
<tr id="row8228144824319"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.4.1"><p id="p62289484438"><a name="p62289484438"></a><a name="p62289484438"></a>Cause Analysis</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.4.1 "><p id="p12282048104312"><a name="p12282048104312"></a><a name="p12282048104312"></a>There is a low probability that the amdgpu driver in kernel 6.6 encounters a page fault, causing the GPU to restart. After the application exits, the CPU load decreases. There is a low probability that the restart fails. In this case, the Android system processes that use GPU resources are suspended. As a result, the container cannot clear the processes and fails to be deleted.</p>
</td>
</tr>
<tr id="row82281448174313"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.5.1"><p id="p02281848114310"><a name="p02281848114310"></a><a name="p02281848114310"></a>Impact Assessment</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.5.1 "><p id="p13228948104315"><a name="p13228948104315"></a><a name="p13228948104315"></a>This problem is caused by the amdgpu driver in kernel 6.6. According to issues in the upstream community, this is a common problem. AMD has not fixed this issue or provided a fix plan.</p>
</td>
</tr>
<tr id="row422874814316"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.6.1"><p id="p722844820438"><a name="p722844820438"></a><a name="p722844820438"></a>Workaround</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.6.1 "><p id="p2247115164414"><a name="p2247115164414"></a><a name="p2247115164414"></a>This issue does not occur after the Mesa driver is replaced with the Mesa version built in Android 15.</p>
</td>
</tr>
<tr id="row1922804814432"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.7.1"><p id="p722844834313"><a name="p722844834313"></a><a name="p722844834313"></a>Progress</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.7.1 "><p id="p192281848174311"><a name="p192281848174311"></a><a name="p192281848174311"></a>Future efforts will focus on analyzing the differences between these two Mesa driver versions, with results to be submitted to the upstream community to facilitate a fix.</p>
</td>
</tr>
</tbody>
</table>

## 6 Related Documentation<a name="ZH-CN_TOPIC_0000002518346256"></a>

### V7.3.0_15 Documentation<a name="ZH-CN_TOPIC_0000002549706109"></a>

|No.|Document|Description|How to Obtain|
|--|--|--|--|
| 1 | Kunpeng BoostKit 26.0.RC1 Developer Guide| Describes the APIs of the video stream engine and provides development examples.| Kunpeng community|
| 2 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Feature Guide (Android 15)| Describes the features of the device- and cloud-side engines.| Kunpeng community|
| 3 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine VM Environment Deployment Guide (Android 15)| Describes how to deploy video stream cloud phone containers on KVM-based VMs in terms of VM deployment and configuration.| Kunpeng community|
| 4 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Release Notes (Android 15)| Provides the release notes of the video stream engine.| Kunpeng community|
| 5 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Version Mapping (Android 15)| Describes the version mapping of the video stream engine.| Kunpeng community|
| 6 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Personal Data Description| Describes the personal data involved in the video stream engine.| Kunpeng community|
| 7 | Kunpeng BoostKit 26.0.RC1 Video Stream Engine Routine Maintenance (Android 15)| Describes how to perform routine maintenance on the device- and cloud-side engines.| Kunpeng community|

### Obtaining Documentation<a name="ZH-CN_TOPIC_0000002549826107"></a>

Visit the [Kunpeng community](https://www.hikunpeng.com/document/detail/en/kunpengcps/overview/kunpengcps.html) to view or download related documents.
