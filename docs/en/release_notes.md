# Release Notes<a name="ZH-CN_TOPIC_0000002552848875"></a>

<!-- md-trans-meta sourceCommit=f8f96e5f62319dc95ee2a6caf995177c9be932ff translatedAt=2026-08-29T08:17:47.238Z pushedAt=2026-09-04T09:51:40.268Z -->

## Version Requirements<a name="ZH-CN_TOPIC_0000002518186576"></a>

### Product Version<a name="ZH-CN_TOPIC_0000002518346470"></a>

|Item|Content|
| :---: | :---: |
|Product Name| Kunpeng BoostKit |
|Product Version| 26.0.RC1 |
|Software Name|Video stream engine|
|Software Package Version| 8.0.RC1_11 |

### Software Versions<a name="ZH-CN_TOPIC_0000002549826339"></a>

|Software|Version|Remarks|
| :---: | :---: | :---: |
|Kunpeng BoostKit|Kunpeng BoostKit 26.0.RC1|-|
|OS|openEuler-22.03-LTS-SP4-AArch64 (kernel 5.10.0-216.0.0)|-|
|ExaGear|ExaGear ARM32-ARM64 V2.5|Transcoding software|

### Hardware Versions<a name="ZH-CN_TOPIC_0000002549826331"></a>

|Server|Processor|BIOS|CPLD|BMC|
| :---: | :---: | :---: | :---: | :---: |
|Kunpeng server|Kunpeng 920 processor|6.56|5.09|5.96|
|Kunpeng server|New Kunpeng 920 processor model|20.55|5.08|5.05.12.15|
|Kunpeng server|New Kunpeng 920 processor model|21.52|7.08|5.05.12.31|

### Virus Scan Results<a name="ZH-CN_TOPIC_0000002518186560"></a>

The software package and related documents have been scanned by antivirus software and no  viruses have been found.

|Antivirus Software|Antivirus Software Version|Virus Database Version|Scan Time|Scan Result|
| :---: | :---: | :---: | :---: | :---: |
|QiAnXin|8.0.5.5260|2026-03-16 08:00:00.0|2026-03-17 17:34:43|OK|
|Bitdefender|7.5.1.200224|7.100367|2026-03-17 17:34:56|OK|
|Kaspersky|12.0.0.6672|2026-03-17 10:04:00|2026-03-17 17:34:39|OK|

## Important Notes<a name="ZH-CN_TOPIC_0000002549706329"></a>

For details, see "Constraints" in [Video Stream Engine Feature Guide](feature_guide.md).

## Version Mappings<a name="ZH-CN_TOPIC_0000002549706328"></a>

### Embedded Platform Mapping<a name="ZH-CN_TOPIC_0000002518346481"></a>

|Software Name|Detailed Version Number|Hardware Platform|Platform Software Name|Platform Software Version|Processor Type/OS|Remarks|
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | Kunpeng server | iBMC | S920X20-S920X21-iBMC_5.05.12.15 | New Kunpeng 920 processor model, 2.9 GHz<br>Host OS: openEuler-22.03-LTS-SP4-AArch64 (kernel 5.10.0-216.0.0)<br>Guest OS: Android-11.0.0_r48 | openEuler|
|  |  |  | CPLD | S920X20-BCU(BC83AMDA-BC83AMDB)-CPLD1-CPLD2_5.08 |  |  |
|  |  |  | BIOS | S920X20-BIOS_20.55 |  |  |
|  |  |  | ExaGear| ExaGear_ARM32-ARM64_V2.5 | Host OS: openEuler-22.03-LTS-SP4-AArch64<br>Guest OS: Android-11.0.0_r48 | v2.5 |
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | Kunpeng server | iBMC | TS200-1280_2180_2280_5280_2180K_2280K_5280K-iBMC-V596 | Kunpeng 920, 2.6 GHz<br>Host OS: openEuler-22.03-LTS-SP4-AArch64 (kernel 5.10.0-216.0.0)<br>Guest OS: Android-11.0.0_r48 | openEuler |
|  |  |  | CPLD | TS200-2280-2180-5280-2180K-2280K-5280K_Mainboard(BC2AMDD01)_CPLD_5.0.9 |  |  |
|  |  |  | BIOS | TS200-2180_2280_5180_5280-BIOS-V656 |  |  |
|  |  |  | ExaGear| ExaGear_ARM32-ARM64_V2.5 | Host OS: openEuler-22.03-LTS-SP4-AArch64<br>Guest OS: Android-11.0.0_r48 | v2.5 |

### Product Version Mapping<a name="ZH-CN_TOPIC_0000002518346482"></a>

| 1 Version Information |  |
| :---: | :---: |
|Product Name| Kunpeng BoostKit |
|Product Version| Kunpeng BoostKit 25.3.0 |
|Detailed Version Number| Kunpeng BoostKit 25.3.0.B016 |

| 2 Software Version Mapping |                              |       |                                                       |       |
| :---: | :---: | :---: | :---: | :---: |
|Software Name|Version|Hardware Platform|OS|Database System|
| BoostKit-videoengine_7.3.0_11.zip | B016 | Kunpeng server | openEuler-22.03-LTS-SP4-AArch64<br>Android-11.0.0_r48| / |
| Kbox-AOSP11.zip | B016 |  |  |  |

### Application Compatibility List<a name="ZH-CN_TOPIC_0000002518346483"></a>

| AOSP 11 App Compatibility List |  |  |  |  |  |  |  | 
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|No.|Name|Type|Game Version|Download Source|Issue|Detailed Description|Root Cause Analysis|
| 1 |Honor of Kings|Game|Updated with the Appstore|Appstore|None|  |  |
| 2 |Genshin Impact|Game|Updated with the Appstore|Appstore|None|  |  |
| 3 |Star Rail|Game|Updated with the Appstore|Appstore|None|Currently, the Vulkan API is disabled for the application to work around rendering issues. After the driver is fixed, users can manually enable this API.|  | 
| 4 |Douyin|Application|Updated with the Appstore|Appstore|None|  |  |
| 5 |Kuaishou|Application|Updated with the Appstore|Appstore|Local service: The location is fixed (Beijing).|  |  |
| 6 |WeChat|Application|Updated with the Appstore|Appstore|None|  |  | 
| 7 |QQ|Application|Updated with the Appstore|Appstore|None|  |  | 
| 8 |WPS|Application|Updated with the Appstore|Appstore|None|  |  | 
| 9 |Weibo|Application|Updated with the Appstore|Appstore|None|  |  | 
| 10 |Tencent Video|Application|Updated with the Appstore|Appstore|None|  |  |
| 11 |iQIYI|Application|Updated with the Appstore|Appstore|None|  |  | 
| 12 |Eggy Party|Application|Updated with the Appstore|Appstore|In the AMD environment, when software decoding is configured, the background video on the login page cannot be played properly. Additionally, in-game live streams encounter black screen issues (the game UI remains normal). This problem can be solved by configuring hardware decoding.|

> ![](public_sys-resources/icon-note.gif) **NOTE**
>
> Application compatibility test conditions:<br>1. The test environment is the standard hardware environment on the 2280 server.<br>2. Cloud phone specifications: 8 cores and 1 channel, 16 GB memory, and 64 GB storage<br>3. A single container runs only one game app during the compatibility test.<br>4. The current version does not support Wi-Fi settings and Wi-Fi search.<br>5. The current version does not support Bluetooth devices.<br> 
>

## 8.0.RC1_11<a name="ZH-CN_TOPIC_0000002549706331"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002518346480"></a>

**New Features<a name="section402mcpsimp"></a>**

None

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518346476"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002518346478"></a>

None

## V7.3.0_11<a name="ZH-CN_TOPIC_0000002549706331"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002518346480"></a>

**New Features<a name="section402mcpsimp"></a>**

None

**Modified Features<a name="section451mcpsimp"></a>**

The composition bypass function does not take effect when the DaoCloud **VAGPU-25.03.01.01-RC20** driver is used.

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518346476"></a>

|Item|Content|
| :---: | :---: |
|Trouble Ticket No.| DTS2025090442627 |
|Severity|Minor|
|Symptom|A Kunpeng server uses DaoCloud GPUs and the **VAGPU-25.03.01.01-RC6** driver. When a video stream cloud phone based on this server plays a .ts video file in XPlayer, the playback is suspended when jumping to another time point. This issue occurs only on a specific video file.|
|Cause Analysis|When XPlayer is used to play this video file, it restarts the decoder when jumping to another time point. Because the video only stores the sequence parameter set (SPS) and picture parameter set (PPS) at the beginning, the decoder is unable to read them after the jump, leading to decoding failure.|
|Impact Assessment|This issue occurs only when DaoCloud GPUs are used with the **VAGPU-25.03.01.01-RC6** driver and a video player (such as XPlayer) that triggers decoder reset upon a jump is used to play a video that stores the SPS/PPS at the beginning only. This issue only causes the video playback to suspend and can be recovered by restarting XPlayer. No other functional or performance issue is caused.|
|Workaround|Other videos can be played properly, and the video with this issue can be played using other video players (such as the pre-installed gallery). The video can also be properly played on a server configured with AMD GPUs.|
|Solution|This issue has been resolved in **VAGPU-25.03.01.01-RC20**.|

### Known Issues<a name="ZH-CN_TOPIC_0000002518346478"></a>

None

## V7.2.RC1<a name="ZH-CN_TOPIC_0000002518186570"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002518186562"></a>

**New Features<a name="section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Added support for 4K resolution.|4K resolution is supported for video streams, improving user experience of cloud phone users.|

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518346494"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002518186572"></a>

|Item|Content|
| :---: | :---: |
|Trouble Ticket No.| DTS2025090442627 |
|Severity|Minor|
|Symptom|A Kunpeng server uses DaoCloud GPUs and the **VAGPU-25.03.01.01-RC6** driver. When a video stream cloud phone based on this server plays a .ts video file in XPlayer, the playback is suspended when jumping to another time point. This issue occurs only on a specific video file.|
|Cause Analysis|When XPlayer is used to play this video file, it restarts the decoder when jumping to another time point. Because the video only stores the sequence parameter set (SPS) and picture parameter set (PPS) at the beginning, the decoder is unable to read them after the jump, leading to decoding failure.|
|Impact Assessment|This issue occurs only when DaoCloud GPUs are used with the **VAGPU-25.03.01.01-RC6** driver and a video player (such as XPlayer) that triggers decoder reset upon a jump is used to play a video that stores the SPS/PPS at the beginning only. This issue only causes the video playback to suspend and can be recovered by restarting XPlayer. No other functional or performance issue is caused.|
|Workaround|Other videos can be played properly, and the video with this issue can be played using other video players (such as the pre-installed gallery). The video can also be properly played on a server configured with AMD GPUs.|
|Solution|The GPU vendor will provide a new driver, and this issue is planned to be resolved on October 30.|

## V7.1.RC1<a name="ZH-CN_TOPIC_0000002518346486"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002549826345"></a>

**New Features<a name="section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Supports 360p and 480p resolutions.|The video stream engine allows a minimum DPI of 120 to support 360p and 480p.|
|2|Optimized the Android composer.|The Android composer is optimized to improve gaming performance by 10%.|
|3|Implemented NUMA affinity binding in the Kubernetes cluster.|A Kubernetes management script is provided for configuring NUMA affinity binding of CPUs, GPUs, and memory.|
|4|Supports VM- and container-based deployment of video stream cloud phones.|The VM+container reference solution is provided, and the performance loss is less than 10%.|

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518346488"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002518346474"></a>

None

## V7.0.RC1<a name="ZH-CN_TOPIC_0000002518346472"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002549706351"></a>

**New Features<a name="section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Dynamic frame rate adjustment|To dynamically decrease the frame rate for rendering overhead reduction when the client is disconnected from the cloud phone in the away from keyboard (AFK) scenario. When the client is disconnected from the cloud phone, the frame rate is decreased. When the client is reconnected to the cloud phone, the frame rate is restored to the normal value.|
|2|Resource monitoring|To monitor GPU memory and other memory resources so that ISVs can perform operations based on resource usage.|

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002549706349"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002549706337"></a>

None

## V6.0.0<a name="ZH-CN_TOPIC_0000002518346496"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002549826335"></a>

**New Features<a name="section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Dynamic encoding resolution adjustment|When the network is unstable, frame freezing may occur if the rendering resolution is the same as the encoding resolution of a high-resolution cloud phone such as 1080p or 2K, which impacts user experience. Dynamically reducing the encoding resolution can reduce bandwidth consumption, alleviate frame freezing, and improve user experience.|

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002549826353"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002549826351"></a>

None

## V6.0.RC2<a name="ZH-CN_TOPIC_0000002549826349"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002518186558"></a>

**New Features<a name="section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Setting video-related parameters|To allow flexible configuration of encoding parameters, bit rate control modes, and bit rate limits. This facilitates configuration adjustment to reduce frame freezing on cloud phones under poor network conditions.|
|2|Sensor and GPS passthrough|To provide the sensor and GPS passthrough function.|
|3|WebRTC stream transmission|To implement WebRTC stream transmission for the video stream cloud phone.|
|4|Kubernetes cluster management for cloud phones running with containerd|To run cloud phones with containerd and implement Kubernetes cluster management, facilitating integration by ISVs.|
|5|Integrated rendering and streaming solution|To implement an integrated rendering and streaming solution based on the Kunpeng 920 processor + DaoCloud GPU, improving the overall density by 10%.|

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518186582"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002549826337"></a>

None

## V6.0.RC1<a name="ZH-CN_TOPIC_0000002549826357"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002549706339"></a>

**New Features<a name="section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Screen resolution APIs|To provide screen resolution APIs, which are invoked by the client to obtain the resolution and screen size of the mobile phone and transfer the obtained data to the server-side cloud phone through the network. In this way, the server can output streams based on this data.|
|2|Video APIs|To optimize video APIs by decoupling them from the video stream engine architecture and provide external APIs to facilitate ISV integration and software re-engineering.|
|3|Audio playback APIs|To optimize audio playback APIs by decoupling them from the video stream engine architecture and provide external APIs to facilitate ISV integration and software re-engineering.|
|4|Microphone input APIs|To provide APIs for injecting microphone input data into cloud phones to facilitate ISV integration and software re-engineering.|
|5|Touch APIs|To optimize touch APIs by decoupling them from the video stream engine architecture and provide external APIs to facilitate ISV integration and software re-engineering.|
|6|External API framework|To decouple VmiAgent from network modules to implement the framework of external APIs.|

**Modified Features<a name="section451mcpsimp"></a>**

None

**Removed Features<a name="section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518186564"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002518346492"></a>

None

## V5.0.0<a name="ZH-CN_TOPIC_0000002518186554"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002549706343"></a>

**New Features<a name="zh-cn_topic_0000001691765529_section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|1080p@30 fps display|The video stream cloud phone supports 1080p@30 fps display.|

**Modified Features<a name="zh-cn_topic_0000001691765529_section451mcpsimp"></a>**

None

**Removed Features<a name="zh-cn_topic_0000001691765529_section454mcpsimp"></a>**

Support for AMD GPU encoding

### Resolved Issues<a name="ZH-CN_TOPIC_0000002549706335"></a>

|Item|Content|
| :---: | :---: |
|Trouble Ticket No.| DTS2023021615441 |
|Severity|Suggestion|
|Symptom|When Kbox 11 is used as the client for a cloud phone to stream video, the screen freezes after 10 minutes of operation. Both the receive frame rate and decoding frame rate drop to 0. The issue can be resolved after reconnection, but it affects the testability of the video stream engine's long-term stability.|
|Cause Analysis|The software decoding performance of the Kbox 11 emulated device is insufficient, and the decoding delay is high.<br>When the system software decoding interface freezes, the decoding thread of the video stream engine client cannot obtain a new buffer to receive stream packets from the server.<br>After multiple retries time out, the decoding function of the video stream engine client enters the error handling process and the decoding thread exits.|
|Impact Assessment|Currently, the software decoding interface of Kbox 11 is used only by its own gallery. Mainstream video players such as XPlayer, Tencent Video, Douyin, and iQIYI use their own FFmpeg software decoding interfaces or self-developed software decoding interfaces instead of the Kbox software decoding interface. Therefore, the impact scope of this issue is limited.|
|Workaround|Use the Kbox 9 emulated device as the client.|
|Solution|Cancel the timeout mechanism of the decoding thread on the client to avoid exit due to system decoding interface stalling.|

### Known Issues<a name="ZH-CN_TOPIC_0000002549706333"></a>

None

## V5.0.RC2<a name="ZH-CN_TOPIC_0000002549826329"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002518346484"></a>

**New Features<a name="zh-cn_topic_0000001498482656_section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Query and display of the version number of the video stream engine component|To support the query and standard display of version information about the video stream engine component and base engine components.|
|2|Adaptation to Mesa 22.1.7|To adapt the video stream engine to Mesa 22.1.7.|
|3|Adaptation of the video stream cloud phone to the W6600 graphics card|To verify the video stream cloud phone density based on the Kunpeng 920 server and W6600 hardware platform.|
|4|Verification of the server and client versions and client exit upon notification of a version mismatch|To support version information verification for both the server and client to prevent exceptions caused by version mismatch.|

**Modified Features<a name="zh-cn_topic_0000001498482656_section451mcpsimp"></a>**

None

**Removed Features<a name="zh-cn_topic_0000001498482656_section454mcpsimp"></a>**

Support for AMD GPU encoding

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518186556"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002549826333"></a>

|Item|Content|
| :---: | :---: |
|Trouble Ticket No.| DTS2023021615441 |
|Severity|Suggestion|
|Symptom|When Kbox 11 is used as the client for a cloud phone to stream video, the screen freezes after 10 minutes of operation. Both the receive frame rate and decoding frame rate drop to 0. The issue can be resolved after reconnection, but it affects the testability of the video stream engine's long-term stability.|
|Cause Analysis|The software decoding performance of the Kbox 11 emulated device is insufficient, and the decoding delay is high.<br>When the system software decoding interface freezes, the decoding thread of the video stream engine client cannot obtain a new buffer to receive stream packets from the server.<br>After multiple retries time out, the decoding function of the video stream engine client enters the error handling process and the decoding thread exits.<br>The issue of poor performance with the built-in Android decoder in Kbox 11 requires further analysis.|
|Impact Assessment|Currently, the software decoding interface of Kbox 11 is used only by its own gallery. Mainstream video players such as XPlayer, Tencent Video, Douyin, and iQIYI use their own FFmpeg software decoding interfaces or self-developed software decoding interfaces instead of the Kbox software decoding interface. Therefore, the impact scope of this issue is limited.|
|Workaround|Use the Kbox 9 emulated device as the client.|
|Solution|Continue to analyze the problem with the open-source software decoding performance and locate the root cause. If the problem lies in the open-source software, no solution will be provided. If the problem is related to integration, continue to solve the problem.|

## V5.0.RC3<a name="ZH-CN_TOPIC_0000002549706341"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002518186578"></a>

**New Features<a name="zh-cn_topic_0000001525082429_section402mcpsimp"></a>**

Adapted the video stream engine to servers powered by the new Kunpeng 920 processor model.

**Modified Features<a name="zh-cn_topic_0000001525082429_section451mcpsimp"></a>**

None

**Removed Features<a name="zh-cn_topic_0000001525082429_section454mcpsimp"></a>**

Deleted Android 9-related content because this version does not support Android 9.

### Resolved Issues<a name="ZH-CN_TOPIC_0000002549706345"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002518346482"></a>

|Item|Content|
| :---: | :---: |
|Trouble Ticket No.| DTS2023021615441 |
|Severity|Suggestion|
|Symptom|When Kbox 11 is used as the client for a cloud phone to stream video, the screen freezes after 10 minutes of operation. Both the receive frame rate and decoding frame rate drop to 0. The issue can be resolved after reconnection, but it affects the testability of the video stream engine's long-term stability.|
|Cause Analysis|The software decoding performance of the Kbox 11 emulated device is insufficient, and the decoding delay is high.<br>When the system software decoding interface freezes, the decoding thread of the video stream engine client cannot obtain a new buffer to receive stream packets from the server.<br>After multiple retries time out, the decoding function of the video stream engine client enters the error handling process and the decoding thread exits.<br>The issue of poor performance with the built-in Android decoder in Kbox 11 requires further analysis.|
|Impact Assessment|Currently, the software decoding interface of Kbox 11 is used only by its own gallery. Mainstream video players such as XPlayer, Tencent Video, Douyin, and iQIYI use their own FFmpeg software decoding interfaces or self-developed software decoding interfaces instead of the Kbox software decoding interface. Therefore, the impact scope of this issue is limited.|
|Workaround|Use the Kbox 9 emulated device as the client.|
|Solution|Continue to analyze the problem with the open-source software decoding performance and locate the root cause. If the problem lies in the open-source software, no solution will be provided. If the problem is related to integration, continue to solve the problem.|

|Item|Content|
| :---: | :---: |
|Trouble Ticket No.| DTS2023030902437 |
|Severity|Minor|
|Symptom|The build tool Gradle 7.5.1 has a vulnerability (CVE-2023-26053).|
|Cause Analysis|On March 9, it was reported that Gradle 7.5.1 had a vulnerability (CVE-2023-26053). The video stream engine in this delivery adopts Gradle 7.5.1.|
|Impact Assessment|1. The vulnerability of the build tool does not affect the final binary, thus imposing no risk on the live network.<br>2. After vulnerability analysis, it is confirmed that this delivery is not affected by this vulnerability.|
|Workaround|This delivery is not affected by the vulnerability and no workaround is required.|
|Solution|Currently, no other Gradle versions are available, and the vulnerability has no impact on this delivery. After Gradle 8.0.1 is available, the build tool will be upgraded from 7.5.1 to 8.0.1 to resolve this vulnerability.|

## V3.0.0<a name="ZH-CN_TOPIC_0000002549826343"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002549706327"></a>

**New Features<a name="zh-cn_topic_0000001518929401_section402mcpsimp"></a>**

- The video stream engine supports rendering optimization for the primary and secondary screens.

- The video stream engine supports H.265 hardware encoding.

- The video stream engine supports GPU encoding.

- New documents related to the video stream engine are added.

- Performance specifications of the video stream engine are provided based on the Kunpeng 920 processor and Android 11.

- The video stream engine adapts to Android 11 based on the Kunpeng 920 processor.

- The application compatibility test is added for the video stream engine.

**Modified Features<a name="zh-cn_topic_0000001518929401_section451mcpsimp"></a>**

None

**Removed Features<a name="zh-cn_topic_0000001518929401_section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518186566"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002518186574"></a>

|Item|Content|
| :---: | :---: |
|Trouble Ticket No.| DTS2022120612498 |
|Severity|Minor|
|Domain|Video stream engine prototype|
|Symptom|[Kunpeng BoostKit 22.0.0.B095] [Video stream function] [CI daily build issue. After the environment is deployed, the server network is abnormal.]|
|Cause Analysis|Some network ports of the CI server are faulty.|
|Impact Assessment|Only the container on the CI server is affected. This problem can be solved by redeploying the container.|
|Workaround|Redeploy the container.|
|Solution|It is preliminarily determined that the problem is caused by the third-party network. After the problem is reproduced and the CI server ports are fixed, the problem is resolved.|

## V2.0.0<a name="ZH-CN_TOPIC_0000002549826355"></a>

### Change Description<a name="ZH-CN_TOPIC_0000002518186568"></a>

**New Features<a name="zh-cn_topic_0000001470294425_section402mcpsimp"></a>**

|No.|Description|Purpose|
| :---: | :---: | :---: |
|1|Trustworthiness enhancement for open-source software|To migrate custom patches of open-source software to the corresponding product version branches in the open-source repository. To modify the URLs for downloading source code of open-source software to those in the open-source repository.|

**Modified Features<a name="zh-cn_topic_0000001470294425_section451mcpsimp"></a>**

None

**Removed Features<a name="zh-cn_topic_0000001470294425_section454mcpsimp"></a>**

None

### Resolved Issues<a name="ZH-CN_TOPIC_0000002518346490"></a>

None

### Known Issues<a name="ZH-CN_TOPIC_0000002549706353"></a>

None
