# 版本说明书<a name="ZH-CN_TOPIC_0000002552895601"></a>

## 版本配套说明<a name="ZH-CN_TOPIC_0000002549826111"></a>

### 产品版本信息<a name="ZH-CN_TOPIC_0000002518186340"></a>

<a name="table229mcpsimp"></a>
<table><tbody><tr id="row234mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.1.1"><p id="p236mcpsimp"><a name="p236mcpsimp"></a><a name="p236mcpsimp"></a>产品名称</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.1.1 "><p id="p6308719427"><a name="p6308719427"></a><a name="p6308719427"></a>Kunpeng BoostKit</p>
</td>
</tr>
<tr id="row239mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.2.1"><p id="p241mcpsimp"><a name="p241mcpsimp"></a><a name="p241mcpsimp"></a>产品版本</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.2.1 "><p id="p243mcpsimp"><a name="p243mcpsimp"></a><a name="p243mcpsimp"></a><span id="text10612828441"><a name="text10612828441"></a><a name="text10612828441"></a>26.0.RC1</span></p>
</td>
</tr>
<tr id="row244mcpsimp"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.3.1"><p id="p246mcpsimp"><a name="p246mcpsimp"></a><a name="p246mcpsimp"></a>软件名称</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.3.1 "><p id="p248mcpsimp"><a name="p248mcpsimp"></a><a name="p248mcpsimp"></a>视频流引擎</p>
</td>
</tr>
<tr id="row1691834411463"><th class="firstcol" valign="top" width="14.000000000000002%" id="mcps1.1.3.4.1"><p id="p1491974414614"><a name="p1491974414614"></a><a name="p1491974414614"></a>软件包版本</p>
</th>
<td class="cellrowborder" valign="top" width="86%" headers="mcps1.1.3.4.1 "><p id="p091914413462"><a name="p091914413462"></a><a name="p091914413462"></a>8.0.RC1_15</p>
</td>
</tr>
</tbody>
</table>

### 软件版本配套说明<a name="ZH-CN_TOPIC_0000002549826109"></a>

|软件类型|版本|备注|
|--|--|--|
|Kunpeng BoostKit|Kunpeng BoostKit 26.0.RC1|-|
|OS|openEuler-24.03-LTS-SP1-aarch64 （内核6.6.0-72.0.0）|-|
|ExaGear|ExaGear ARM32-ARM64|转码软件|

### 硬件版本配套说明<a name="ZH-CN_TOPIC_0000002549826113"></a>

|服务器类型|处理器型号|BIOS版本|CPLD版本|BMC版本|
|--|--|--|--|--|
|鲲鹏服务器|鲲鹏920 7260处理器|6.56|5.09|5.96|
|鲲鹏服务器|鲲鹏920 7280Z处理器|20.55|5.08|5.05.12.15|

### 病毒扫描结果<a name="ZH-CN_TOPIC_0000002549706115"></a>

本软件包及相关文档经过防病毒软件扫描，没有发现病毒。

|防病毒软件名称|防病毒软件版本|病毒库版本|扫描时间|扫描结果|
|--|--|--|--|--|
|QiAnXin|8.0.5.5260|2026-03-16 08:00:00.0|2026-03-17 17:34:38|OK|
|Bitdefender|7.5.1.200224|7.100367|2026-03-17 17:34:46|OK|
|Kaspersky|12.0.0.6672|2026-03-17 10:04:00|2026-03-17 17:34:35|OK|

## 2 版本使用注意事项<a name="ZH-CN_TOPIC_0000002518346260"></a>

版本使用注意事项详见《视频流引擎 特性指南（Android 15）》中“[约束与限制](feature_guide.md)”章节。

## 3 版本配套表<a name="ZH-CN_TOPIC_0000002518346261"></a>

### 3.1 内嵌平台版本配套关系<a name="ZH-CN_TOPIC_0000002518346262"></a>

| 本软件名称 | 详细版本号 | 配套硬件平台 | 平台软件名称 | 平台软件版本号 | 处理器类型操作系统 | 备注 |
|------------------|------------------------------|---------|-----------|----------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------|-------------|
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | 鲲鹏服务器   | iBMC      | S920X20-S920X21-iBMC_5.05.12.15                                            | Kunpeng 920 V200 7280Z 2.9GHz<br>Host OS：openEuler-24.03-LTS-SP1-aarch64(内核6.6.0-72.0.0)；<br>Guest OS：android-15.0.0_r17 | openEuler系统 |
|                  |                              |         | CPLD      | S920X20-BCU(BC83AMDA-BC83AMDB)-CPLD1-CPLD2_5.08                            |                                                                                                                          |             |
|                  |                              |         | BIOS      | S920X20-BIOS_20.55                                                         |                                                                                                                          |             |
|                  |                              |         | Exagear软件 | ExaGear_ARM32-ARM64                                                        | Host OS：openEuler-24.03-LTS-SP1-aarch64<br>Guest OS：android-15.0.0_r17                                                   |             |
| Kunpeng BoostKit | Kunpeng BoostKit 25.3.0.B016 | 鲲鹏服务器   | iBMC      | TS200-1280_2180_2280_5280_2180K_2280K_5280K-iBMC-V596<br>                  | KunPeng920 7260 2.6GHz<br>Host OS：openEuler-24.03-LTS-SP1-aarch64(内核6.6.0-72.0.0)<br>Guest OS：android-15.0.0_r17         | openEuler系统 |
|                  |                              |         | CPLD      | TS200-2280-2180-5280-2180K-2280K-5280K_Mainboard(BC2AMDD01)_CPLD_5.0.9<br> |                                                                                                                          |             |
|                  |                              |         | BIOS      | TS200-2180_2280_5180_5280-BIOS-V656<br>                                    |                                                                                                                          |             |
|                  |                              |         | Exagear软件 | ExaGear_ARM32-ARM64                                                        | Host OS：openEuler-24.03-LTS-SP1-aarch64<br>Guest OS：android-15.0.0_r17                                                   |             |

### 3.2 产品内配套关系<a name="ZH-CN_TOPIC_0000002518346263"></a>

| 1          版本信息                   |                              |       |                                                       |       |
|-----------------------------------|------------------------------|-------|-------------------------------------------------------|-------|
| 产品名称                              | Kunpeng BoostKit             |       |                                                       |       |
| 产品版本                              | Kunpeng BoostKit 25.3.0      |       |                                                       |       |
| 详细版本号                             | Kunpeng BoostKit 25.3.0.B016 |       |                                                       |       |

| 2          软件版本配套表                |                              |       |                                                       |       |
|-----------------------------------|------------------------------|-------|-------------------------------------------------------|-------|
| 表1 软件版本配套表                        |                              |       |                                                       |       |
| 软件名称                              | 版本号                          | 硬件平台  | 操作系统                                                  | 数据库系统 |
| BoostKit-videoengine_7.3.0_15.zip | B016                         | 鲲鹏服务器 | openEuler-24.03-LTS-SP1-aarch64<br>android-15.0.0_r17 | /     |
| Kbox-AOSP15.zip                   | B016                         |       |                                                       |       |

### 3.3 应用兼容性列表<a name="ZH-CN_TOPIC_0000002518346264"></a>

| AOSP15应用兼容性列表 |      |    |         |      |                                                                                  |                                      |      |
|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|------|----|---------|------|----------------------------------------------------------------------------------|--------------------------------------|------|
| 序号                                                                                                                                                                | 名称   | 类型 | 游戏版本号   | 下载来源 | 问题                                                                               | 详细描述                                 | 原因分析 |
| 1                                                                                                                                                                 | 王者荣耀 | 游戏 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 2                                                                                                                                                                 | 原神   | 游戏 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 3                                                                                                                                                                 | 星穹铁道 | 游戏 | 跟随应用宝更新 | 应用宝  | 无                                                                                | 当前对应用禁用vulkan api规避渲染问题，驱动修复后用户可自行放开 |      |
| 4                                                                                                                                                                 | 抖音   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 5                                                                                                                                                                 | 快手   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 6                                                                                                                                                                 | 微信   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 7                                                                                                                                                                 | QQ   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 8                                                                                                                                                                 | WPS  | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 9                                                                                                                                                                 | 微博   | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 10                                                                                                                                                                | 腾讯视频 | 应用 | 跟随应用宝更新 | 应用宝  | 无                                                                                |                                      |      |
| 11 | 爱奇艺 | 应用 | 跟随应用宝更新 | 应用宝 | 1、小窗播放可能会导致应用未响应或画面卡死。<br>2、在配置为浅色模式时，短视频页面中评论区的背景颜色显示异常，导致评论文字与背景对比不明显。配置深色模式正常 |  |  |
| 12 | 蛋仔派对 | 应用 | 跟随应用宝更新 | 应用宝 | 在AMD环境上，配置为软解时，登录界面背景视频无法正常播放，游戏内直播无法正常播放，会发生黑屏（游戏UI正常）。配置为硬解可解决此问题 |  |  |

>![](public_sys-resources/icon-note.gif) **说明：** 
>
> 应用兼容性测试条件说明：<br>1.测试环境应为2280服务器的标准硬件环境<br>2.测云手机规格(8核1路/16g内存/64g存储)<br>3.兼容性测试场景，单个容器在测试期间只运行一款游戏，不同时运行多款游戏<br>4.当前版本暂不支持WIFI设置和WIFI搜索等操作；<br>5.当前版本不支持蓝牙设备；<br> 
>

## 4 8.0.RC1<a name="ZH-CN_TOPIC_0000002518186342"></a>

### 4.1 更新说明<a name="ZH-CN_TOPIC_0000002549706113"></a>

**新增特性<a name="section402mcpsimp"></a>**

无

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 4.2 已解决的问题<a name="ZH-CN_TOPIC_0000002549706111"></a>

无

### 4.3 遗留问题<a name="ZH-CN_TOPIC_0000002518186338"></a>

<a name="table202271648124311"></a>
<table><tbody><tr id="row822764816434"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.1.1"><p id="p1622724854314"><a name="p1622724854314"></a><a name="p1622724854314"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.1.1 "><p id="p422712488431"><a name="p422712488431"></a><a name="p422712488431"></a>DTS2025120110758</p>
</td>
</tr>
<tr id="row722774812437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.2.1"><p id="p1222734884319"><a name="p1222734884319"></a><a name="p1222734884319"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.2.1 "><p id="p4227164816433"><a name="p4227164816433"></a><a name="p4227164816433"></a>一般</p>
</td>
</tr>
<tr id="row1122744819437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.3.1"><p id="p12228114814317"><a name="p12228114814317"></a><a name="p12228114814317"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.3.1 "><p id="p92282484433"><a name="p92282484433"></a><a name="p92282484433"></a>安卓15的AMD环境上，长稳高负载运行4天后，机器CPU负载异常下降，容器无法正常删除</p>
</td>
</tr>
<tr id="row8228144824319"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.4.1"><p id="p62289484438"><a name="p62289484438"></a><a name="p62289484438"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.4.1 "><p id="p12282048104312"><a name="p12282048104312"></a><a name="p12282048104312"></a>6.6版本内核的amdgpu驱动小概率发生page fault，引发GPU重启，应用退出后CPU负载下降。其中小概率重启失败时，使用GPU相关资源的安卓系统进程被挂起，导致容器无法正常将其清理，容器删除失败。</p>
</td>
</tr>
<tr id="row82281448174313"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.5.1"><p id="p02281848114310"><a name="p02281848114310"></a><a name="p02281848114310"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.5.1 "><p id="p13228948104315"><a name="p13228948104315"></a><a name="p13228948104315"></a>此问题为6.6内核中，amdgpu驱动问题，从上流社区的issue来看，是一个普遍存在的现象，AMD当前并未修复或给出修复计划。</p>
</td>
</tr>
<tr id="row422874814316"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.6.1"><p id="p722844820438"><a name="p722844820438"></a><a name="p722844820438"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.6.1 "><p id="p2247115164414"><a name="p2247115164414"></a><a name="p2247115164414"></a>将mesa驱动替换为安卓15自带的mesa版本无此问题</p>
</td>
</tr>
<tr id="row1922804814432"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.7.1"><p id="p722844834313"><a name="p722844834313"></a><a name="p722844834313"></a>解决计划</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.7.1 "><p id="p192281848174311"><a name="p192281848174311"></a><a name="p192281848174311"></a>继续分析mesa发布版本驱动与安卓15自带mesa版本之间的差异，并反馈上流社区解决</p>
</td>
</tr>
</tbody>
</table>

## 5 V7.3.0_15<a name="ZH-CN_TOPIC_0000002518186342"></a>

### 5.1 更新说明<a name="ZH-CN_TOPIC_0000002549706113"></a>

**新增特性<a name="section402mcpsimp"></a>**

|编号|描述|目的|
|--|--|--|
| 1 | 视频流引擎适配Android 15 | 视频流引擎适配Android 15，完成出流 |

**修改特性<a name="section451mcpsimp"></a>**

无

**删除特性<a name="section454mcpsimp"></a>**

无

### 5.2 已解决的问题<a name="ZH-CN_TOPIC_0000002549706111"></a>

无

### 5.3 遗留问题<a name="ZH-CN_TOPIC_0000002518186338"></a>

<a name="table202271648124311"></a>
<table><tbody><tr id="row822764816434"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.1.1"><p id="p1622724854314"><a name="p1622724854314"></a><a name="p1622724854314"></a>问题单号</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.1.1 "><p id="p422712488431"><a name="p422712488431"></a><a name="p422712488431"></a>DTS2025120110758</p>
</td>
</tr>
<tr id="row722774812437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.2.1"><p id="p1222734884319"><a name="p1222734884319"></a><a name="p1222734884319"></a>严重级别</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.2.1 "><p id="p4227164816433"><a name="p4227164816433"></a><a name="p4227164816433"></a>一般</p>
</td>
</tr>
<tr id="row1122744819437"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.3.1"><p id="p12228114814317"><a name="p12228114814317"></a><a name="p12228114814317"></a>问题描述</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.3.1 "><p id="p92282484433"><a name="p92282484433"></a><a name="p92282484433"></a>安卓15的AMD环境上，长稳高负载运行4天后，机器CPU负载异常下降，容器无法正常删除</p>
</td>
</tr>
<tr id="row8228144824319"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.4.1"><p id="p62289484438"><a name="p62289484438"></a><a name="p62289484438"></a>根因分析</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.4.1 "><p id="p12282048104312"><a name="p12282048104312"></a><a name="p12282048104312"></a>6.6版本内核的amdgpu驱动小概率发生page fault，引发GPU重启，应用退出后CPU负载下降。其中小概率重启失败时，使用GPU相关资源的安卓系统进程被挂起，导致容器无法正常将其清理，容器删除失败。</p>
</td>
</tr>
<tr id="row82281448174313"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.5.1"><p id="p02281848114310"><a name="p02281848114310"></a><a name="p02281848114310"></a>影响评估</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.5.1 "><p id="p13228948104315"><a name="p13228948104315"></a><a name="p13228948104315"></a>此问题为6.6内核中，amdgpu驱动问题，从上流社区的issue来看，是一个普遍存在的现象，AMD当前并未修复或给出修复计划。</p>
</td>
</tr>
<tr id="row422874814316"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.6.1"><p id="p722844820438"><a name="p722844820438"></a><a name="p722844820438"></a>规避和应急措施</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.6.1 "><p id="p2247115164414"><a name="p2247115164414"></a><a name="p2247115164414"></a>将mesa驱动替换为安卓15自带的mesa版本无此问题</p>
</td>
</tr>
<tr id="row1922804814432"><th class="firstcol" valign="top" width="20.830000000000002%" id="mcps1.1.3.7.1"><p id="p722844834313"><a name="p722844834313"></a><a name="p722844834313"></a>解决计划</p>
</th>
<td class="cellrowborder" valign="top" width="79.17%" headers="mcps1.1.3.7.1 "><p id="p192281848174311"><a name="p192281848174311"></a><a name="p192281848174311"></a>继续分析mesa发布版本驱动与安卓15自带mesa版本之间的差异，并反馈上流社区解决</p>
</td>
</tr>
</tbody>
</table>

## 6 版本配套文档<a name="ZH-CN_TOPIC_0000002518346256"></a>

### V7.3.0_15配套文档<a name="ZH-CN_TOPIC_0000002549706109"></a>

|序号|文档名称|内容简介|获取方法|
|--|--|--|--|
| 1 | Kunpeng BoostKit 26.0.RC1 开发指南 | 本文档向用户介绍视频流引擎API接口说明及开发指南 | 鲲鹏社区 |
| 2 | Kunpeng BoostKit 26.0.RC1 视频流引擎 特性指南（Android 15） | 本文档主要介绍端侧和云侧引擎的特性说明 | 鲲鹏社区 |
| 3 | Kunpeng BoostKit 26.0.RC1 视频流引擎虚拟机环境 部署指南（Android 15） | 本文档提供基于KVM的虚拟机部署视频流云手机容器的方案，涵盖KVM虚拟机的部署及相关配置流程 | 鲲鹏社区 |
| 4 | Kunpeng BoostKit 26.0.RC1 视频流引擎 版本说明书（Android 15） | 本文档向用户介绍视频流引擎的版本说明 | 鲲鹏社区 |
| 5 | Kunpeng BoostKit 26.0.RC1 视频流引擎 版本配套表（Android 15） | 本文档向用户介绍视频流引擎的版本配套说明 | 鲲鹏社区 |
| 6 | Kunpeng BoostKit 26.0.RC1 视频流引擎 个人数据说明 | 本文档向用户介绍视频流引擎的个人数据说明 | 鲲鹏社区 |
| 7 | Kunpeng BoostKit 26.0.RC1 视频流引擎 例行维护（Android 15） | 本文档主要介绍端侧和云侧引擎的例行维护说明 | 鲲鹏社区 |

### 获取文档方式<a name="ZH-CN_TOPIC_0000002549826107"></a>

您可以通过访问[鲲鹏社区](https://www.hikunpeng.com/document/detail/zh/kunpengcps/overview/kunpengcps.html)浏览和获取相关文档。
