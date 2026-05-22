# 版本说明书<a name="ZH-CN_TOPIC_0000002552848875"></a>

## 1 版本配套说明<a name="ZH-CN_TOPIC_0000002518186576"></a>

### 1.1 产品版本信息<a name="ZH-CN_TOPIC_0000002518346470"></a>

<a name="table229mcpsimp"></a>
<table><tbody><tr id="row234mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.1.1"><p id="p236mcpsimp"><a name="p236mcpsimp"></a><a name="p236mcpsimp"></a>产品名称</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.1.1 "><p id="p6308719427"><a name="p6308719427"></a><a name="p6308719427"></a>Kunpeng BoostKit</p>
</td>
</tr>
<tr id="row239mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.2.1"><p id="p241mcpsimp"><a name="p241mcpsimp"></a><a name="p241mcpsimp"></a>产品版本</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.2.1 "><p id="p243mcpsimp"><a name="p243mcpsimp"></a><a name="p243mcpsimp"></a><span id="text10760143210428"><a name="text10760143210428"></a><a name="text10760143210428"></a>26.0.RC1</span></p>
</td>
</tr>
<tr id="row244mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.3.1"><p id="p246mcpsimp"><a name="p246mcpsimp"></a><a name="p246mcpsimp"></a>软件名称</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.3.1 "><p id="p248mcpsimp"><a name="p248mcpsimp"></a><a name="p248mcpsimp"></a>视频流引擎</p>
</td>
</tr>
<tr id="row143655718466"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.4.1"><p id="p173652078469"><a name="p173652078469"></a><a name="p173652078469"></a>软件包版本</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.4.1 "><p id="p936514714612"><a name="p936514714612"></a><a name="p936514714612"></a>8.0.RC1_11</p>
</td>
</tr>
</tbody>
</table>

### 1.2 软件版本配套说明<a name="ZH-CN_TOPIC_0000002549826339"></a>

|软件类型|版本|备注|
|--|--|--|
|Kunpeng BoostKit|Kunpeng BoostKit 26.0.RC1|-|
|OS|openEuler-22.03-LTS-SP4-aarch64 （内核5.10.0-216.0.0）|-|
|ExaGear|ExaGear ARM32-ARM64 V2.5|转码软件|

### 1.3 硬件版本配套说明<a name="ZH-CN_TOPIC_0000002549826331"></a>

|服务器类型|处理器型号|BIOS版本|CPLD版本|BMC版本|
|--|--|--|--|--|
|鲲鹏服务器|鲲鹏920 7260处理器|6.56|5.09|5.96|
|鲲鹏服务器|鲲鹏920 7280Z处理器|20.55|5.08|5.05.12.15|
|鲲鹏服务器|鲲鹏920 7260W处理器|21.52|7.08|5.05.12.31|


### 1.4 病毒扫描结果<a name="ZH-CN_TOPIC_0000002518186560"></a>

本软件包及相关文档经过防病毒软件扫描，没有发现病毒。

|防病毒软件名称|防病毒软件版本|病毒库版本|扫描时间|扫描结果|
|--|--|--|--|--|
|QiAnXin|8.0.5.5260|2026-03-16 08:00:00.0|2026-03-17 17:34:43|OK|
|Bitdefender|7.5.1.200224|7.100367|2026-03-17 17:34:56|OK|
|Kaspersky|12.0.0.6672|2026-03-17 10:04:00|2026-03-17 17:34:39|OK|

## 2 版本使用注意事项<a name="ZH-CN_TOPIC_0000002549706329"></a>

版本使用注意事项详见《视频流引擎 特性指南》中“[约束与限制](feature.md)”章节。


## 3 版本配套表<a name="ZH-CN_TOPIC_0000002549706328"></a>

### 3.1 内嵌平台版本配套关系<a name="ZH-CN_TOPIC_0000002518346481"></a>

|本软件名称             |详细版本号                         |配套硬件平台   |平台软件名称     |平台软件版本号                                                                     |处理器类型*操作系统                                                                                                                  |备注           |
|------------------|------------------------------|---------|-----------|----------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------|-------------|
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | 鲲鹏服务器   | iBMC      | S920X20-S920X21-iBMC_5.05.12.15                                            | Kunpeng 920 V200 7280Z 2.9GHz<br>Host OS：openEuler-22.03-LTS-SP4-aarch64(内核5.10.0-216.0.0)；<br>Guest OS：android-11.0.0_r48 | openEuler系统 |
|                  |                              |         | CPLD      | S920X20-BCU(BC83AMDA-BC83AMDB)-CPLD1-CPLD2_5.08                            |                                                                                                                            |             |
|                  |                              |         | BIOS      | S920X20-BIOS_20.55                                                         |                                                                                                                            |             |
|                  |                              |         | Exagear软件 | ExaGear_ARM32-ARM64_V2.5                                                   | Host OS：openEuler-22.03-LTS-SP4-aarch64<br>Guest OS：android-11.0.0_r48                                                     | v2.5        |
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | 鲲鹏服务器   | iBMC      | TS200-1280_2180_2280_5280_2180K_2280K_5280K-iBMC-V596<br>                  | KunPeng920 7260 2.6GHz<br>Host OS：openEuler-22.03-LTS-SP4-aarch64(内核5.10.0-216.0.0)<br>Guest OS：android-11.0.0_r48         | openEuler系统 |
|                  |                              |         | CPLD      | TS200-2280-2180-5280-2180K-2280K-5280K_Mainboard(BC2AMDD01)_CPLD_5.0.9<br> |                                                                                                                            |             |
|                  |                              |         | BIOS      | TS200-2180_2280_5180_5280-BIOS-V656<br>                                    |                                                                                                                            |             |
|                  |                              |         | Exagear软件 | ExaGear_ARM32-ARM64_V2.5                                                   | Host OS：openEuler-22.03-LTS-SP4-aarch64<br>Guest OS：android-11.0.0_r48                                                     | v2.5        |


### 3.2 产品内配套关系<a name="ZH-CN_TOPIC_0000002518346482"></a>

|版本信息                               |                              |       |                                                       |       |
|-----------------------------------|------------------------------|-------|-------------------------------------------------------|-------|
| 产品名称                              | Kunpeng BoostKit             |       |                                                       |       |
| 产品版本                              | Kunpeng BoostKit 25.3.0      |       |                                                       |       |
| 详细版本号                             | Kunpeng BoostKit 25.3.0.B016 |       |                                                       |       |
| 2          软件版本配套表                |                              |       |                                                       |       |
| 表1 软件版本配套表                        |                              |       |                                                       |       |
| 软件名称                              | 版本号                          | 硬件平台  | 操作系统                                                  | 数据库系统 |
| BoostKit-videoengine_7.3.0_11.zip | B016                         | 鲲鹏服务器 | openEuler-22.03-LTS-SP4-aarch64<br>android-11.0.0_r48 | /     |
| Kbox-AOSP11.zip                   | B016                         |       |                                                       |       |


### 3.3 应用兼容性列表<a name="ZH-CN_TOPIC_0000002518346483"></a>

| 应用兼容性测试条件说明：<br>1.测试环境应为2280服务器的标准硬件环境<br>2.测云手机规格(8核1路/16g内存/64g存储)<br>3.兼容性测试场景，单个容器在测试期间只运行一款游戏，不同时运行多款游戏<br>4.当前版本暂不支持WIFI设置和WIFI搜索等操作；<br>5.当前版本不支持蓝牙设备；<br> |      |    |         |      |                                                                     |                                |      | 
|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|------|----|---------|------|---------------------------------------------------------------------|--------------------------------|------|
| AOSP11应用兼容性列表                                                                                                                                                     |      |    |         |      |                                                                     |                                |      |
| 序号                                                                                                                                                                | 名称   | 类型 | 游戏版本号   | 下载来源 | 问题                                                                  | 详细描述                           | 原因分析 |
| 1                                                                                                                                                                 | 王者荣耀 | 游戏 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 2                                                                                                                                                                 | 原神   | 游戏 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 3                                                                                                                                                                 | 星穹铁道 | 游戏 | 跟随应用宝更新 | 应用宝  | 无                                                                   | 当前对应用禁用vulkan api，驱动修复后用户可自行放开 |      | 
| 4                                                                                                                                                                 | 抖音   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 5                                                                                                                                                                 | 快手   | 应用 | 跟随应用宝更新 | 应用宝  | 同城：定位在北京                                                            |                                |      |
| 6                                                                                                                                                                 | 微信   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 7                                                                                                                                                                 | QQ   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 8                                                                                                                                                                 | WPS  | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 9                                                                                                                                                                 | 微博   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 10                                                                                                                                                                | 腾讯视频 | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 11                                                                                                                                                                | 爱奇艺  | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                   |                                |      | 
| 12                                                                                                                                                                | 蛋仔派对 | 应用 | 跟随应用宝更新 | 应用宝  | 在AMD环境上，配置为软解时，登录界面背景视频无法正常播放，游戏内直播无法正常播放，会发生黑屏（游戏UI正常）。配置为硬解可解决此问题 |


## 4 8.0.RC1_11<a name="ZH-CN_TOPIC_0000002549706331"></a>

### 4.1 更新说明<a name="ZH-CN_TOPIC_0000002518346480"></a>

**新增特性<a name="section402mcpsimp"></a>**

无

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 4.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518346476"></a>

无

### 4.3 遗留问题<a name="ZH-CN_TOPIC_0000002518346478"></a>

无

## 5 V7.3.0_11<a name="ZH-CN_TOPIC_0000002549706331"></a>

### 5.1 更新说明<a name="ZH-CN_TOPIC_0000002518346480"></a>

**新增特性<a name="section402mcpsimp"></a>**

无

**修改特性<a name="section451mcpsimp"></a>**

合成优化功能在配套瀚博VAGPU-25.03.01.01-RC20版本驱动时不可生效

**删除特性<a name="section454mcpsimp"></a>**

无

### 5.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518346476"></a>

<a name="zh-cn_topic_0000001498002964_table1077520124617"></a>
<table><tbody><tr id="zh-cn_topic_0000001498002964_row07751817464"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000001498002964_p177751174618"><a name="zh-cn_topic_0000001498002964_p177751174618"></a><a name="zh-cn_topic_0000001498002964_p177751174618"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.1.1 "><p id="p12376102249"><a name="p12376102249"></a><a name="p12376102249"></a>DTS2025090442627</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row157751511464"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000001498002964_p167751810462"><a name="zh-cn_topic_0000001498002964_p167751810462"></a><a name="zh-cn_topic_0000001498002964_p167751810462"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.2.1 "><p id="p5376142147"><a name="p5376142147"></a><a name="p5376142147"></a>一般</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row11775191144616"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000001498002964_p20775919467"><a name="zh-cn_topic_0000001498002964_p20775919467"></a><a name="zh-cn_topic_0000001498002964_p20775919467"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.3.1 "><p id="p37315810478"><a name="p37315810478"></a><a name="p37315810478"></a>鲲鹏服务器搭载瀚博GPU搭配驱动版本VAGPU-25.03.01.01-RC6时，视频流云手机使用xplayer播放特定ts格式视频文件，跳转时间点会必然出现播放卡顿问题，仅有一个特定视频文件会触发该问题现象。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row12775151134619"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000001498002964_p197756111466"><a name="zh-cn_topic_0000001498002964_p197756111466"></a><a name="zh-cn_topic_0000001498002964_p197756111466"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.4.1 "><p id="p856219144476"><a name="p856219144476"></a><a name="p856219144476"></a>使用xplayer播放问题视频文件，在跳转时间点时xplayer重启解码器，由于该视频只在视频开头存了一个sps/pps，导致跳转后解码器无法读取到sps/pps，解码失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row1677518118466"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000001498002964_p97756164616"><a name="zh-cn_topic_0000001498002964_p97756164616"></a><a name="zh-cn_topic_0000001498002964_p97756164616"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.5.1 "><p id="p53778219413"><a name="p53778219413"></a><a name="p53778219413"></a>仅使用瀚博GPU搭配驱动VAGPU-25.03.01.01-RC6，使用跳转时触发解码器reset的播放器（如xplayer）播放仅开头存有一个sps/pps的视频时存在该问题。该问题仅会导致播放停止，可通过重启xplayer应用恢复，不会导致其他功能/性能问题。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row1177581134617"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.6.1"><p id="zh-cn_topic_0000001498002964_p677517114617"><a name="zh-cn_topic_0000001498002964_p677517114617"></a><a name="zh-cn_topic_0000001498002964_p677517114617"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.6.1 "><p id="p53771212415"><a name="p53771212415"></a><a name="p53771212415"></a>播放其他视频，或通过其他视频播放器（如系统自带的图库）播放该视频，均可正常播放。使用AMD GPU配置的服务器，也可正常播放该视频。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row1777511154617"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.7.1"><p id="p168758055115"><a name="p168758055115"></a><a name="p168758055115"></a>解决方案</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.7.1 "><p id="p83771424412"><a name="p83771424412"></a><a name="p83771424412"></a>瀚博驱动VAGPU-25.03.01.01-RC20 已解决</p>
</td>
</tr>
</tbody>
</table>

### 5.3 遗留问题<a name="ZH-CN_TOPIC_0000002518346478"></a>

无

## 6 V7.2.RC1<a name="ZH-CN_TOPIC_0000002518186570"></a>

### 6.1 更新说明<a name="ZH-CN_TOPIC_0000002518186562"></a>

**新增特性<a name="section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|新增4K分辨率适配|视频流新增支持4K分辨率，提升云手机用户体验。|

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 6.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518346494"></a>

无

### 6.3 遗留问题<a name="ZH-CN_TOPIC_0000002518186572"></a>

<a name="zh-cn_topic_0000001498002964_table1077520124617"></a>
<table><tbody><tr id="zh-cn_topic_0000001498002964_row07751817464"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000001498002964_p177751174618"><a name="zh-cn_topic_0000001498002964_p177751174618"></a><a name="zh-cn_topic_0000001498002964_p177751174618"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.1.1 "><p id="p12376102249"><a name="p12376102249"></a><a name="p12376102249"></a>DTS2025090442627</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row157751511464"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000001498002964_p167751810462"><a name="zh-cn_topic_0000001498002964_p167751810462"></a><a name="zh-cn_topic_0000001498002964_p167751810462"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.2.1 "><p id="p5376142147"><a name="p5376142147"></a><a name="p5376142147"></a>一般</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row11775191144616"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000001498002964_p20775919467"><a name="zh-cn_topic_0000001498002964_p20775919467"></a><a name="zh-cn_topic_0000001498002964_p20775919467"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.3.1 "><p id="p37315810478"><a name="p37315810478"></a><a name="p37315810478"></a>鲲鹏服务器搭载瀚博GPU搭配驱动版本VAGPU-25.03.01.01-RC6时，视频流云手机使用xplayer播放特定ts格式视频文件，跳转时间点会必然出现播放卡顿问题，仅有一个特定视频文件会触发该问题现象。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row12775151134619"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000001498002964_p197756111466"><a name="zh-cn_topic_0000001498002964_p197756111466"></a><a name="zh-cn_topic_0000001498002964_p197756111466"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.4.1 "><p id="p856219144476"><a name="p856219144476"></a><a name="p856219144476"></a>使用xplayer播放问题视频文件，在跳转时间点时xplayer重启解码器，由于该视频只在视频开头存了一个sps/pps，导致跳转后解码器无法读取到sps/pps，解码失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row1677518118466"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000001498002964_p97756164616"><a name="zh-cn_topic_0000001498002964_p97756164616"></a><a name="zh-cn_topic_0000001498002964_p97756164616"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.5.1 "><p id="p53778219413"><a name="p53778219413"></a><a name="p53778219413"></a>仅使用瀚博GPU搭配驱动VAGPU-25.03.01.01-RC6，使用跳转时触发解码器reset的播放器（如xplayer）播放仅开头存有一个sps/pps的视频时存在该问题。该问题仅会导致播放停止，可通过重启xplayer应用恢复，不会导致其他功能/性能问题。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row1177581134617"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.6.1"><p id="zh-cn_topic_0000001498002964_p677517114617"><a name="zh-cn_topic_0000001498002964_p677517114617"></a><a name="zh-cn_topic_0000001498002964_p677517114617"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.6.1 "><p id="p53771212415"><a name="p53771212415"></a><a name="p53771212415"></a>播放其他视频，或通过其他视频播放器（如系统自带的图库）播放该视频，均可正常播放。使用AMD GPU配置的服务器，也可正常播放该视频。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001498002964_row1777511154617"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.7.1"><p id="zh-cn_topic_0000001498002964_p87762154616"><a name="zh-cn_topic_0000001498002964_p87762154616"></a><a name="zh-cn_topic_0000001498002964_p87762154616"></a>解决计划</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.7.1 "><p id="p83771424412"><a name="p83771424412"></a><a name="p83771424412"></a>瀚博厂商提供新驱动用于修复问题，计划1030解决。</p>
</td>
</tr>
</tbody>
</table>

## 7 V7.1.RC1<a name="ZH-CN_TOPIC_0000002518346486"></a>

### 7.1 更新说明<a name="ZH-CN_TOPIC_0000002549826345"></a>

**新增特性<a name="section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|视频流支持360P/480P分辨率|视频流支持将DPI最低设置为120，以支持360P和480P分辨率。|
|2|实现Android composer优化|实现Android composer优化，提升游戏性能10%。|
|3|实现K8s NUMA亲和绑定|提供K8s管理脚本，支持CPU、GPU、内存NUMA亲和绑定。|
|4|支持虚拟机和容器下部署视频流云手机|提供虚拟机+容器参考方案，性能损失<10%。|

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 7.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518346488"></a>

无

### 7.3 遗留问题<a name="ZH-CN_TOPIC_0000002518346474"></a>

无

## 8 V7.0.RC1<a name="ZH-CN_TOPIC_0000002518346472"></a>

### 8.1 更新说明<a name="ZH-CN_TOPIC_0000002549706351"></a>

**新增特性<a name="section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|支持动态帧率调整|在挂机场景下，云手机与客户端断开连接时，动态向下调整帧率以减少渲染性能开销。检测到云手机客户端断开连接，动态向下调整帧率。检测到云手机客户端连接，恢复到正常帧率。|
|2|提供GPU显存、内存等资源监测能力|提供实时的GPU显存和内存资源监测能力，便于ISV根据资源的使用情况进行相应的处理。|

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 8.2 已解决的问题<a name="ZH-CN_TOPIC_0000002549706349"></a>

无

### 8.3 遗留问题<a name="ZH-CN_TOPIC_0000002549706337"></a>

无

## 9 V6.0.0<a name="ZH-CN_TOPIC_0000002518346496"></a>

### 9.1 更新说明<a name="ZH-CN_TOPIC_0000002549826335"></a>

**新增特性<a name="section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|支持动态设置编码分辨率|在网络不稳定的情况下，如1080p，2k等高分辨率云手机，渲染分辨率和编码分辨率相同时，可能会出现卡顿等影响用户体验的情况，因此，需要支持动态降低编码分辨率的功能，在网络不稳定的情况下，通过降低编码分辨率，从而降低对网络带宽的消耗，缓解卡顿，提升用户体验。|

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 9.2 已解决的问题<a name="ZH-CN_TOPIC_0000002549826353"></a>

无

### 9.3 遗留问题<a name="ZH-CN_TOPIC_0000002549826351"></a>

无

## 10 V6.0.RC2<a name="ZH-CN_TOPIC_0000002549826349"></a>

### 10.1 更新说明<a name="ZH-CN_TOPIC_0000002518186558"></a>

**新增特性<a name="section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|支持设置视频相关参数|在弱网情况下使用云手机，会出现卡顿等情况影响体验，因此需要开放灵活的编码参数、码控方式、码率限制等配置，便于在弱网情况下进行调节，优化用户体验。|
|2|提供传感器和GPS直通功能|提供传感器和GPS直通功能。|
|3|支持WebRTC流化传输|采用WebRTC实现视频流云手机流传输方案。|
|4|基于Containerd运行云手机，并实现K8s集群管理|基于Containerd运行云手机，并实现K8s集群管理，便于ISV集成。|
|5|基于国产化GPU协同实现渲染流化一体方案|基于鲲鹏920处理器+道客GPU实现渲染流化一体方案，提升整机密度10%。|

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 10.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518186582"></a>

无

### 10.3 遗留问题<a name="ZH-CN_TOPIC_0000002549826337"></a>

无

## 11 V6.0.RC1<a name="ZH-CN_TOPIC_0000002549826357"></a>

### 11.1 更新说明<a name="ZH-CN_TOPIC_0000002549706339"></a>

**新增特性<a name="section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|提供屏幕分辨率接口|提供屏幕分辨率接口，由客户端调用，实现从终端手机获取手机的分辨率和屏幕尺寸，通过网络传入服务器端云手机，实现服务端按照终端屏幕比例出流。|
|2|提供视频接口|根据架构解耦，对视频相关的接口进行整改，并提供对外API，便于ISV集成和二次开发。|
|3|提供音频播放接口|根据架构解耦，对音频播放相关的接口进行整改，并提供对外API，便于ISV集成和二次开发。|
|4|提供麦克风输入接口|提供云手机上可注入麦克风输入的数据的API接口，便于ISV集成和二次开发。|
|5|提供触控相关接口|根据架构解耦，对触控相关的接口进行整改，并提供对外API，便于ISV集成和二次开发。|
|6|提供对外接口框架|整改VmiAgent与网络模块等解耦，实现对外接口的框架。|

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 11.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518186564"></a>

无

### 11.3 遗留问题<a name="ZH-CN_TOPIC_0000002518346492"></a>

无

## 12 V5.0.0<a name="ZH-CN_TOPIC_0000002518186554"></a>

### 12.1 更新说明<a name="ZH-CN_TOPIC_0000002549706343"></a>

**新增特性<a name="zh-cn_topic_0000001691765529_section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|视频流云手机支持1080p30帧显示|视频流云手机支持1080p30帧显示。|

**修改特性<a name="zh-cn_topic_0000001691765529_section451mcpsimp"></a>**

无

**删除特性<a name="zh-cn_topic_0000001691765529_section454mcpsimp"></a>**

视频流引擎支持AMD GPU编码。

### 12.2 已解决的问题<a name="ZH-CN_TOPIC_0000002549706335"></a>

<a name="zh-cn_topic_0000001691765525_table5817144218195"></a>
<table><tbody><tr id="zh-cn_topic_0000001691765525_row586944271918"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000001691765525_p118691042181914"><a name="zh-cn_topic_0000001691765525_p118691042181914"></a><a name="zh-cn_topic_0000001691765525_p118691042181914"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000001691765525_p138690421191"><a name="zh-cn_topic_0000001691765525_p138690421191"></a><a name="zh-cn_topic_0000001691765525_p138690421191"></a>DTS2023021615441</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001691765525_row0869142131918"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000001691765525_p786910421194"><a name="zh-cn_topic_0000001691765525_p786910421194"></a><a name="zh-cn_topic_0000001691765525_p786910421194"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000001691765525_p4869174231912"><a name="zh-cn_topic_0000001691765525_p4869174231912"></a><a name="zh-cn_topic_0000001691765525_p4869174231912"></a>提示</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001691765525_row586904221912"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000001691765525_p186944218191"><a name="zh-cn_topic_0000001691765525_p186944218191"></a><a name="zh-cn_topic_0000001691765525_p186944218191"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000001691765525_p128706421198"><a name="zh-cn_topic_0000001691765525_p128706421198"></a><a name="zh-cn_topic_0000001691765525_p128706421198"></a>使用Kbox11当客户端出流视频流云手机，运行10分钟后画面卡住，接收帧率解码帧率均为0，重连可恢复，影响视频流长稳的可测试性。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001691765525_row16870164218197"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000001691765525_p887084291920"><a name="zh-cn_topic_0000001691765525_p887084291920"></a><a name="zh-cn_topic_0000001691765525_p887084291920"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000001691765525_p1587004210193"><a name="zh-cn_topic_0000001691765525_p1587004210193"></a><a name="zh-cn_topic_0000001691765525_p1587004210193"></a>Kbox11仿真机软解码性能不足，存在解码时延冲高的问题。</p>
<p id="zh-cn_topic_0000001691765525_p1487084216194"><a name="zh-cn_topic_0000001691765525_p1487084216194"></a><a name="zh-cn_topic_0000001691765525_p1487084216194"></a>当系统软解码接口卡顿时，视频流客户端解码线程无法获取新buffer用于接收服务端码流包。</p>
<p id="zh-cn_topic_0000001691765525_p38701142191913"><a name="zh-cn_topic_0000001691765525_p38701142191913"></a><a name="zh-cn_topic_0000001691765525_p38701142191913"></a>在多次重试超时后，视频流客户端解码功能进入错误处理流程，解码线程退出。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001691765525_row387094291917"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000001691765525_p6870142121916"><a name="zh-cn_topic_0000001691765525_p6870142121916"></a><a name="zh-cn_topic_0000001691765525_p6870142121916"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000001691765525_p1187014271913"><a name="zh-cn_topic_0000001691765525_p1187014271913"></a><a name="zh-cn_topic_0000001691765525_p1187014271913"></a>当前Kbox11自带软解只有本身图库会使用，主流视频播放器xplayer、腾讯视频、抖音、爱奇艺等在Kbox中运行均使用自身集成的FFmpeg软解或自研的软解，不会使用Kbox自带的软解，软解性能差的影响范围有限。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001691765525_row17870642191919"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.6.1"><p id="zh-cn_topic_0000001691765525_p1870184210193"><a name="zh-cn_topic_0000001691765525_p1870184210193"></a><a name="zh-cn_topic_0000001691765525_p1870184210193"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.6.1 "><p id="zh-cn_topic_0000001691765525_p68701842121911"><a name="zh-cn_topic_0000001691765525_p68701842121911"></a><a name="zh-cn_topic_0000001691765525_p68701842121911"></a>使用Kbox9仿真机做客户端。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001691765525_row12870144216195"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.7.1"><p id="zh-cn_topic_0000001691765525_p2087013427198"><a name="zh-cn_topic_0000001691765525_p2087013427198"></a><a name="zh-cn_topic_0000001691765525_p2087013427198"></a>解决方案</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.7.1 "><p id="zh-cn_topic_0000001691765525_p11870142181919"><a name="zh-cn_topic_0000001691765525_p11870142181919"></a><a name="zh-cn_topic_0000001691765525_p11870142181919"></a>客户端解码线程取消超时机制，避免因为系统解码接口卡顿退出。</p>
</td>
</tr>
</tbody>
</table>

### 12.3 遗留问题<a name="ZH-CN_TOPIC_0000002549706333"></a>

无

## 13 V5.0.RC2<a name="ZH-CN_TOPIC_0000002549826329"></a>

### 13.1 更新说明<a name="ZH-CN_TOPIC_0000002518346484"></a>

**新增特性<a name="zh-cn_topic_0000001498482656_section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|支持视频流引擎组件版本号查询和显示|支持视频流和基础引擎组件查询方式及回显规范。|
|2|视频流适配Mesa 22.1.7版本|视频流引擎适配Mesa 22.1.7版本。|
|3|视频流云手机适配W6600卡|基于鲲鹏920 7260服务器+W6600硬件平台，完成视频流云手机密度验证要求。|
|4|服务端和客户端校验版本号并在版本不一致时提示客户并退出|视频流云手机支持服务端和客户端都检验版本信息，防止版本不匹配引发问题。|

**修改特性<a name="zh-cn_topic_0000001498482656_section451mcpsimp"></a>**

无

**删除特性<a name="zh-cn_topic_0000001498482656_section454mcpsimp"></a>**

视频流引擎支持AMD GPU编码。

### 13.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518186556"></a>

无

### 13.3 遗留问题<a name="ZH-CN_TOPIC_0000002549826333"></a>

<a name="zh-cn_topic_0000001549442493_table5817144218195"></a>
<table><tbody><tr id="zh-cn_topic_0000001549442493_row586944271918"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000001549442493_p118691042181914"><a name="zh-cn_topic_0000001549442493_p118691042181914"></a><a name="zh-cn_topic_0000001549442493_p118691042181914"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000001549442493_p138690421191"><a name="zh-cn_topic_0000001549442493_p138690421191"></a><a name="zh-cn_topic_0000001549442493_p138690421191"></a>DTS2023021615441</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001549442493_row0869142131918"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000001549442493_p786910421194"><a name="zh-cn_topic_0000001549442493_p786910421194"></a><a name="zh-cn_topic_0000001549442493_p786910421194"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000001549442493_p4869174231912"><a name="zh-cn_topic_0000001549442493_p4869174231912"></a><a name="zh-cn_topic_0000001549442493_p4869174231912"></a>提示</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001549442493_row586904221912"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000001549442493_p186944218191"><a name="zh-cn_topic_0000001549442493_p186944218191"></a><a name="zh-cn_topic_0000001549442493_p186944218191"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000001549442493_p128706421198"><a name="zh-cn_topic_0000001549442493_p128706421198"></a><a name="zh-cn_topic_0000001549442493_p128706421198"></a>使用Kbox11当客户端出流视频流云手机，运行10分钟后画面卡住，接收帧率解码帧率均为0，重连可恢复，影响视频流长稳的可测试性。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001549442493_row16870164218197"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000001549442493_p887084291920"><a name="zh-cn_topic_0000001549442493_p887084291920"></a><a name="zh-cn_topic_0000001549442493_p887084291920"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000001549442493_p1587004210193"><a name="zh-cn_topic_0000001549442493_p1587004210193"></a><a name="zh-cn_topic_0000001549442493_p1587004210193"></a>Kbox11仿真机软解码性能不足，存在解码时延冲高的问题。</p>
<p id="zh-cn_topic_0000001549442493_p1487084216194"><a name="zh-cn_topic_0000001549442493_p1487084216194"></a><a name="zh-cn_topic_0000001549442493_p1487084216194"></a>当系统软解码接口卡顿时，视频流客户端解码线程无法获取新buffer用于接收服务端码流包。</p>
<p id="zh-cn_topic_0000001549442493_p38701142191913"><a name="zh-cn_topic_0000001549442493_p38701142191913"></a><a name="zh-cn_topic_0000001549442493_p38701142191913"></a>在多次重试超时后，视频流客户端解码功能进入错误处理流程，解码线程退出。</p>
<p id="zh-cn_topic_0000001549442493_p128701427195"><a name="zh-cn_topic_0000001549442493_p128701427195"></a><a name="zh-cn_topic_0000001549442493_p128701427195"></a>Kbox11自带Android解码器性能差问题需进一步分析。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001549442493_row387094291917"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000001549442493_p6870142121916"><a name="zh-cn_topic_0000001549442493_p6870142121916"></a><a name="zh-cn_topic_0000001549442493_p6870142121916"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000001549442493_p1187014271913"><a name="zh-cn_topic_0000001549442493_p1187014271913"></a><a name="zh-cn_topic_0000001549442493_p1187014271913"></a>当前Kbox11自带软解只有本身图库会使用，主流视频播放器xplayer、腾讯视频、抖音、爱奇艺等在Kbox中运行均使用自身集成的<span id="zh-cn_topic_0000001549442493_ph122933341588"><a name="zh-cn_topic_0000001549442493_ph122933341588"></a><a name="zh-cn_topic_0000001549442493_ph122933341588"></a>FF</span>mpeg软解或自研的软解，不会使用Kbox自带的软解，软解性能差的影响范围有限。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001549442493_row17870642191919"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.6.1"><p id="zh-cn_topic_0000001549442493_p1870184210193"><a name="zh-cn_topic_0000001549442493_p1870184210193"></a><a name="zh-cn_topic_0000001549442493_p1870184210193"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.6.1 "><p id="zh-cn_topic_0000001549442493_p68701842121911"><a name="zh-cn_topic_0000001549442493_p68701842121911"></a><a name="zh-cn_topic_0000001549442493_p68701842121911"></a>使用Kbox9仿真机做客户端。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001549442493_row12870144216195"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.7.1"><p id="zh-cn_topic_0000001549442493_p2087013427198"><a name="zh-cn_topic_0000001549442493_p2087013427198"></a><a name="zh-cn_topic_0000001549442493_p2087013427198"></a>解决计划</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.7.1 "><p id="zh-cn_topic_0000001549442493_p11870142181919"><a name="zh-cn_topic_0000001549442493_p11870142181919"></a><a name="zh-cn_topic_0000001549442493_p11870142181919"></a>继续分析<span id="zh-cn_topic_0000001549442493_ph13334226407"><a name="zh-cn_topic_0000001549442493_ph13334226407"></a><a name="zh-cn_topic_0000001549442493_ph13334226407"></a>开源</span>软件解码性能问题，定位到根因，若为<span id="zh-cn_topic_0000001549442493_ph10919955406"><a name="zh-cn_topic_0000001549442493_ph10919955406"></a><a name="zh-cn_topic_0000001549442493_ph10919955406"></a>开源软件</span>问题可不解决，若为集成问题需要继续解决。</p>
</td>
</tr>
</tbody>
</table>

## 14 V5.0.RC3<a name="ZH-CN_TOPIC_0000002549706341"></a>

### 14.1 更新说明<a name="ZH-CN_TOPIC_0000002518186578"></a>

**新增特性<a name="zh-cn_topic_0000001525082429_section402mcpsimp"></a>**

视频流引擎适配鲲鹏920 7265F处理器。

**修改特性<a name="zh-cn_topic_0000001525082429_section451mcpsimp"></a>**

无

**删除特性<a name="zh-cn_topic_0000001525082429_section454mcpsimp"></a>**

本版本不支持Android 9，故删除Android 9内容。

### 14.2 已解决的问题<a name="ZH-CN_TOPIC_0000002549706345"></a>

无

### 14.3 遗留问题<a name="ZH-CN_TOPIC_0000002518346482"></a>

<a name="zh-cn_topic_0000001524642921_table5817144218195"></a>
<table><tbody><tr id="zh-cn_topic_0000001524642921_row586944271918"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000001524642921_p118691042181914"><a name="zh-cn_topic_0000001524642921_p118691042181914"></a><a name="zh-cn_topic_0000001524642921_p118691042181914"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000001524642921_p138690421191"><a name="zh-cn_topic_0000001524642921_p138690421191"></a><a name="zh-cn_topic_0000001524642921_p138690421191"></a>DTS2023021615441</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row0869142131918"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000001524642921_p786910421194"><a name="zh-cn_topic_0000001524642921_p786910421194"></a><a name="zh-cn_topic_0000001524642921_p786910421194"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000001524642921_p4869174231912"><a name="zh-cn_topic_0000001524642921_p4869174231912"></a><a name="zh-cn_topic_0000001524642921_p4869174231912"></a>提示</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row586904221912"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000001524642921_p186944218191"><a name="zh-cn_topic_0000001524642921_p186944218191"></a><a name="zh-cn_topic_0000001524642921_p186944218191"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000001524642921_p128706421198"><a name="zh-cn_topic_0000001524642921_p128706421198"></a><a name="zh-cn_topic_0000001524642921_p128706421198"></a>使用Kbox11当客户端出流视频流云手机，运行10分钟后画面卡住，接收帧率解码帧率均为0，重连可恢复，影响视频流长稳的可测试性。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row16870164218197"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000001524642921_p887084291920"><a name="zh-cn_topic_0000001524642921_p887084291920"></a><a name="zh-cn_topic_0000001524642921_p887084291920"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000001524642921_p1587004210193"><a name="zh-cn_topic_0000001524642921_p1587004210193"></a><a name="zh-cn_topic_0000001524642921_p1587004210193"></a>Kbox11仿真机软解码性能不足，存在解码时延冲高的问题。</p>
<p id="zh-cn_topic_0000001524642921_p1487084216194"><a name="zh-cn_topic_0000001524642921_p1487084216194"></a><a name="zh-cn_topic_0000001524642921_p1487084216194"></a>当系统软解码接口卡顿时，视频流客户端解码线程无法获取新buffer用于接收服务端码流包。</p>
<p id="zh-cn_topic_0000001524642921_p38701142191913"><a name="zh-cn_topic_0000001524642921_p38701142191913"></a><a name="zh-cn_topic_0000001524642921_p38701142191913"></a>在多次重试超时后，视频流客户端解码功能进入错误处理流程，解码线程退出。</p>
<p id="zh-cn_topic_0000001524642921_p128701427195"><a name="zh-cn_topic_0000001524642921_p128701427195"></a><a name="zh-cn_topic_0000001524642921_p128701427195"></a>Kbox11自带Android解码器性能差问题需进一步分析。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row387094291917"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000001524642921_p6870142121916"><a name="zh-cn_topic_0000001524642921_p6870142121916"></a><a name="zh-cn_topic_0000001524642921_p6870142121916"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000001524642921_p1187014271913"><a name="zh-cn_topic_0000001524642921_p1187014271913"></a><a name="zh-cn_topic_0000001524642921_p1187014271913"></a>当前Kbox11自带软解只有本身图库会使用，主流视频播放器xplayer、腾讯视频、抖音、爱奇艺等在Kbox中运行均使用自身集成的<span id="zh-cn_topic_0000001524642921_ph189012531588"><a name="zh-cn_topic_0000001524642921_ph189012531588"></a><a name="zh-cn_topic_0000001524642921_ph189012531588"></a>FF</span>mpeg软解或自研的软解，不会使用Kbox自带的软解，软解性能差的影响范围有限。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row17870642191919"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.6.1"><p id="zh-cn_topic_0000001524642921_p1870184210193"><a name="zh-cn_topic_0000001524642921_p1870184210193"></a><a name="zh-cn_topic_0000001524642921_p1870184210193"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.6.1 "><p id="zh-cn_topic_0000001524642921_p68701842121911"><a name="zh-cn_topic_0000001524642921_p68701842121911"></a><a name="zh-cn_topic_0000001524642921_p68701842121911"></a>使用Kbox9仿真机做客户端。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row12870144216195"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.7.1"><p id="zh-cn_topic_0000001524642921_p2087013427198"><a name="zh-cn_topic_0000001524642921_p2087013427198"></a><a name="zh-cn_topic_0000001524642921_p2087013427198"></a>解决计划</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.7.1 "><p id="zh-cn_topic_0000001524642921_p11870142181919"><a name="zh-cn_topic_0000001524642921_p11870142181919"></a><a name="zh-cn_topic_0000001524642921_p11870142181919"></a>继续分析<span id="zh-cn_topic_0000001524642921_ph137419569408"><a name="zh-cn_topic_0000001524642921_ph137419569408"></a><a name="zh-cn_topic_0000001524642921_ph137419569408"></a>开源</span>软件解码性能问题，定位到根因，若为<span id="zh-cn_topic_0000001524642921_ph98144144120"><a name="zh-cn_topic_0000001524642921_ph98144144120"></a><a name="zh-cn_topic_0000001524642921_ph98144144120"></a>开源软件</span>问题可不解决，若为集成问题需要继续解决。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000001524642921_table11710428153217"></a>
<table><tbody><tr id="zh-cn_topic_0000001524642921_row671052815323"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000001524642921_p8710928153218"><a name="zh-cn_topic_0000001524642921_p8710928153218"></a><a name="zh-cn_topic_0000001524642921_p8710928153218"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000001524642921_p15710182816328"><a name="zh-cn_topic_0000001524642921_p15710182816328"></a><a name="zh-cn_topic_0000001524642921_p15710182816328"></a>DTS2023030902437</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row117101828163215"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000001524642921_p47108284329"><a name="zh-cn_topic_0000001524642921_p47108284329"></a><a name="zh-cn_topic_0000001524642921_p47108284329"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000001524642921_p771017282324"><a name="zh-cn_topic_0000001524642921_p771017282324"></a><a name="zh-cn_topic_0000001524642921_p771017282324"></a>一般</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row20711202819324"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000001524642921_p9711132820321"><a name="zh-cn_topic_0000001524642921_p9711132820321"></a><a name="zh-cn_topic_0000001524642921_p9711132820321"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000001524642921_p156111710113515"><a name="zh-cn_topic_0000001524642921_p156111710113515"></a><a name="zh-cn_topic_0000001524642921_p156111710113515"></a>构建工具gradle 7.5.1版本存在漏洞（CVE-2023-26053）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row67111928143210"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000001524642921_p1711328143210"><a name="zh-cn_topic_0000001524642921_p1711328143210"></a><a name="zh-cn_topic_0000001524642921_p1711328143210"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000001524642921_p77111828133214"><a name="zh-cn_topic_0000001524642921_p77111828133214"></a><a name="zh-cn_topic_0000001524642921_p77111828133214"></a>3月9号，报告构建工具gradle 7.5.1存在漏洞（CVE-2023-26053），本次版本交付中视频流引擎使用了构建工具gradle 7.5.1。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row1571112284322"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000001524642921_p7711182810328"><a name="zh-cn_topic_0000001524642921_p7711182810328"></a><a name="zh-cn_topic_0000001524642921_p7711182810328"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.5.1 "><a name="zh-cn_topic_0000001524642921_ol1752507192215"></a><a name="zh-cn_topic_0000001524642921_ol1752507192215"></a><ol id="zh-cn_topic_0000001524642921_ol1752507192215"><li>构建工具的漏洞不影响最终二进制，故现网不存在风险。</li><li>经过分析漏洞，本次交付不受此漏洞影响。</li></ol>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row18711172863212"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.6.1"><p id="zh-cn_topic_0000001524642921_p19711102819325"><a name="zh-cn_topic_0000001524642921_p19711102819325"></a><a name="zh-cn_topic_0000001524642921_p19711102819325"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.6.1 "><p id="zh-cn_topic_0000001524642921_p5711728153210"><a name="zh-cn_topic_0000001524642921_p5711728153210"></a><a name="zh-cn_topic_0000001524642921_p5711728153210"></a>本次交付不受此漏洞的影响，无需消减。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001524642921_row771132811325"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.7.1"><p id="zh-cn_topic_0000001524642921_p1071232843217"><a name="zh-cn_topic_0000001524642921_p1071232843217"></a><a name="zh-cn_topic_0000001524642921_p1071232843217"></a>解决计划</p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.7.1 "><p id="zh-cn_topic_0000001524642921_p187122289325"><a name="zh-cn_topic_0000001524642921_p187122289325"></a><a name="zh-cn_topic_0000001524642921_p187122289325"></a>因当前没有可用gradle版本，同时此漏洞无影响，待gradle 8.0.1入库后，构建工具版本将由7.5.1升级至8.0.1版本以解决此漏洞。</p>
</td>
</tr>
</tbody>
</table>

## 15 V3.0.0<a name="ZH-CN_TOPIC_0000002549826343"></a>

### 15.1 更新说明<a name="ZH-CN_TOPIC_0000002549706327"></a>

**新增特性<a name="zh-cn_topic_0000001518929401_section402mcpsimp"></a>**

- 视频流引擎支持主副屏渲染优化。
- 视频流引擎支持H.265硬编码。
- 视频流引擎支持GPU编码。
- 提供视频流引擎相关资料文档。
- 视频流引擎基于鲲鹏920 7260服务器和Android 11的性能规格。
- 视频流引擎基于鲲鹏920 7260/5220服务器适配Android 11。
- 视频流引擎的应用兼容性测试。

**修改特性<a name="zh-cn_topic_0000001518929401_section451mcpsimp"></a>**

无

**删除特性<a name="zh-cn_topic_0000001518929401_section454mcpsimp"></a>**

无

### 15.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518186566"></a>

无

### 15.3 遗留问题<a name="ZH-CN_TOPIC_0000002518186574"></a>

<a name="zh-cn_topic_0000001468008732_table476315617294"></a>
<table><tbody><tr id="zh-cn_topic_0000001468008732_row68081665299"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000001468008732_p880915602910"><a name="zh-cn_topic_0000001468008732_p880915602910"></a><a name="zh-cn_topic_0000001468008732_p880915602910"></a><strong id="zh-cn_topic_0000001468008732_b1480966122914"><a name="zh-cn_topic_0000001468008732_b1480966122914"></a><a name="zh-cn_topic_0000001468008732_b1480966122914"></a>问题单号</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000001468008732_p4809136152917"><a name="zh-cn_topic_0000001468008732_p4809136152917"></a><a name="zh-cn_topic_0000001468008732_p4809136152917"></a>DTS2022120612498</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001468008732_row2080915617299"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000001468008732_p118091861298"><a name="zh-cn_topic_0000001468008732_p118091861298"></a><a name="zh-cn_topic_0000001468008732_p118091861298"></a><strong id="zh-cn_topic_0000001468008732_b1380915619299"><a name="zh-cn_topic_0000001468008732_b1380915619299"></a><a name="zh-cn_topic_0000001468008732_b1380915619299"></a>严重级别</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000001468008732_p380912618293"><a name="zh-cn_topic_0000001468008732_p380912618293"></a><a name="zh-cn_topic_0000001468008732_p380912618293"></a>一般</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001468008732_row1680926162916"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000001468008732_p6809461297"><a name="zh-cn_topic_0000001468008732_p6809461297"></a><a name="zh-cn_topic_0000001468008732_p6809461297"></a><strong id="zh-cn_topic_0000001468008732_b148099617293"><a name="zh-cn_topic_0000001468008732_b148099617293"></a><a name="zh-cn_topic_0000001468008732_b148099617293"></a>责任领域</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000001468008732_p1280936142919"><a name="zh-cn_topic_0000001468008732_p1280936142919"></a><a name="zh-cn_topic_0000001468008732_p1280936142919"></a>视频流引擎原型</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001468008732_row1180906142911"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000001468008732_p880917642916"><a name="zh-cn_topic_0000001468008732_p880917642916"></a><a name="zh-cn_topic_0000001468008732_p880917642916"></a><strong id="zh-cn_topic_0000001468008732_b780912692913"><a name="zh-cn_topic_0000001468008732_b780912692913"></a><a name="zh-cn_topic_0000001468008732_b780912692913"></a>问题描述</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000001468008732_p0861111143019"><a name="zh-cn_topic_0000001468008732_p0861111143019"></a><a name="zh-cn_topic_0000001468008732_p0861111143019"></a>【Kunpeng BoostKit 22.0.0.B095】【视频流功能】【CI日构建问题，环境部署完成后，服务端网络异常一直连不上】</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001468008732_row178092612298"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000001468008732_p10809116122919"><a name="zh-cn_topic_0000001468008732_p10809116122919"></a><a name="zh-cn_topic_0000001468008732_p10809116122919"></a><strong id="zh-cn_topic_0000001468008732_b98091692913"><a name="zh-cn_topic_0000001468008732_b98091692913"></a><a name="zh-cn_topic_0000001468008732_b98091692913"></a>根因分析</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000001468008732_p178094682915"><a name="zh-cn_topic_0000001468008732_p178094682915"></a><a name="zh-cn_topic_0000001468008732_p178094682915"></a>CI服务器部分网络端口问题。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001468008732_row180911642915"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.6.1"><p id="zh-cn_topic_0000001468008732_p1280910672914"><a name="zh-cn_topic_0000001468008732_p1280910672914"></a><a name="zh-cn_topic_0000001468008732_p1280910672914"></a><strong id="zh-cn_topic_0000001468008732_b1680919622915"><a name="zh-cn_topic_0000001468008732_b1680919622915"></a><a name="zh-cn_topic_0000001468008732_b1680919622915"></a>影响评估</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.6.1 "><p id="zh-cn_topic_0000001468008732_p1280906192915"><a name="zh-cn_topic_0000001468008732_p1280906192915"></a><a name="zh-cn_topic_0000001468008732_p1280906192915"></a>单点端口问题，重新部署容器可解决。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001468008732_row480916614295"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.7.1"><p id="zh-cn_topic_0000001468008732_p38090614291"><a name="zh-cn_topic_0000001468008732_p38090614291"></a><a name="zh-cn_topic_0000001468008732_p38090614291"></a><strong id="zh-cn_topic_0000001468008732_b2080918619295"><a name="zh-cn_topic_0000001468008732_b2080918619295"></a><a name="zh-cn_topic_0000001468008732_b2080918619295"></a>规避和应急措施</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.7.1 "><p id="zh-cn_topic_0000001468008732_p188091967299"><a name="zh-cn_topic_0000001468008732_p188091967299"></a><a name="zh-cn_topic_0000001468008732_p188091967299"></a>单点端口问题，重新部署容器可解决。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001468008732_row18809569291"><th class="firstcol" valign="top" width="23%" id="mcps1.1.3.8.1"><p id="zh-cn_topic_0000001468008732_p1080917619295"><a name="zh-cn_topic_0000001468008732_p1080917619295"></a><a name="zh-cn_topic_0000001468008732_p1080917619295"></a><strong id="zh-cn_topic_0000001468008732_b680920612298"><a name="zh-cn_topic_0000001468008732_b680920612298"></a><a name="zh-cn_topic_0000001468008732_b680920612298"></a>解决计划</strong></p>
</th>
<td class="cellrowborder" valign="top" width="77%" headers="mcps1.1.3.8.1 "><p id="zh-cn_topic_0000001468008732_p2809563291"><a name="zh-cn_topic_0000001468008732_p2809563291"></a><a name="zh-cn_topic_0000001468008732_p2809563291"></a>初步确认为第三方网络问题，复现后修复CI机器端口问题解决。</p>
</td>
</tr>
</tbody>
</table>

## 16 V2.0.0<a name="ZH-CN_TOPIC_0000002549826355"></a>

### 16.1 更新说明<a name="ZH-CN_TOPIC_0000002518186568"></a>

**新增特性<a name="zh-cn_topic_0000001470294425_section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
|1|开源软件可信整改|将开源软件定制补丁迁移至开源源码中心仓的产品版本分支。支持开源软件“源代码”必须从开源中心仓下载的整改。|

**修改特性<a name="zh-cn_topic_0000001470294425_section451mcpsimp"></a>**

无

**删除特性<a name="zh-cn_topic_0000001470294425_section454mcpsimp"></a>**

无

### 16.2 已解决的问题<a name="ZH-CN_TOPIC_0000002518346490"></a>

无

### 16.3 遗留问题<a name="ZH-CN_TOPIC_0000002549706353"></a>

无
