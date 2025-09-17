# 项目介绍<a name="ZH-CN_TOPIC_0000002442154444"></a>

视频流引擎主要应用于云手机，基于视频流引擎技术实现的云手机方案也称为视频流云手机。分为端侧和云侧两个部分：云侧运行于服务器上；端侧一般为云手机APK，可以被安装在用户的Android手机上，用于和云侧进行交互，进而对Kbox容器进行正常的操作。

# 版本说明<a name="ZH-CN_TOPIC_0000002442154440"></a>

<a name="table740710612324"></a>
<table><thead align="left"><tr id="row144075603214"><th class="cellrowborder" valign="top" width="49.96%" id="mcps1.1.3.1.1"><p id="p114078693216"><a name="p114078693216"></a><a name="p114078693216"></a>项目</p>
</th>
<th class="cellrowborder" valign="top" width="50.03999999999999%" id="mcps1.1.3.1.2"><p id="p4238382489"><a name="p4238382489"></a><a name="p4238382489"></a>版本</p>
</th>
</tr>
</thead>
<tbody><tr id="row64089653211"><td class="cellrowborder" valign="top" width="49.96%" headers="mcps1.1.3.1.1 "><p id="p24084615327"><a name="p24084615327"></a><a name="p24084615327"></a>OS</p>
</td>
<td class="cellrowborder" valign="top" width="50.03999999999999%" headers="mcps1.1.3.1.2 "><p id="p131779216275"><a name="p131779216275"></a><a name="p131779216275"></a>5.10.0-216.0.0</p>
</td>
</tr>
<tr id="row44089619323"><td class="cellrowborder" valign="top" width="49.96%" headers="mcps1.1.3.1.1 "><p id="p64089617323"><a name="p64089617323"></a><a name="p64089617323"></a><span>Android</span></p>
</td>
<td class="cellrowborder" valign="top" width="50.03999999999999%" headers="mcps1.1.3.1.2 "><p id="p540820673214"><a name="p540820673214"></a><a name="p540820673214"></a>11</p>
</td>
</tr>
</tbody>
</table>

# 环境部署<a name="ZH-CN_TOPIC_0000002442314316"></a>





## 编译机推荐配置<a name="ZH-CN_TOPIC_0000002442368342"></a>

除执行机外准备一个编译机，用于编译视频流容器启动所需要的包文件。

系统版本：Ubuntu 22.04.3 LTS

系统架构：x86\_64

## 网络要求<a name="ZH-CN_TOPIC_0000002475768317"></a>

确保服务器已连接网络，以便顺利完成Gradle编译和其他开源依赖包的下载。

## 安装依赖库<a name="ZH-CN_TOPIC_0000002442528242"></a>

```
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

## 执行脚本<a name="ZH-CN_TOPIC_0000002475728121"></a>

1.  git拉取代码，安装编译环境。

    ```
    ./scripts/auto_install_tools.sh ${安装目录} 
    source ~/.bashrc
    ```

    其中，安装目录可以自己指定，若不输入\{安装目录\}，则脚本使用默认目录\~/NativeCompileToolsDir

    此脚本执行成功后，之后不需要再重复执行。

2.  编译客户端。

    ```
    ./build_video.sh video_client
    ```

    命令执行成功后，将在output目录生成CloudPhoneApk.tar.gz和已解压好的CloudPhone.apk

3.  编译服务端。

    ```
    ./build_video.sh video_server
    ```

    命令执行成功后，将在output目录生成DemoVideoEngine.tar.gz

# 快速上手<a name="ZH-CN_TOPIC_0000002475594421"></a>

根据[环境部署](环境部署.md)章节获取到CloudPhone.apk、CloudPhoneApk.tar.gz和DemoVideoEngine.tar.gz文件。之后请依照[特性指南](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/videostreamengine/kunpengcpsvideo_20_0002.html)使用云手机。

# 贡献指南<a name="ZH-CN_TOPIC_0000002442314312"></a>

如果使用过程中有任何问题，或者需要反馈特性需求和bug报告，可以提交issue联系我们，具体贡献方法可参考[这里](https://gitcode.com/boostkit/community/blob/master/docs/contributor/contributing.md)。

# 免责声明<a name="ZH-CN_TOPIC_0000002475674237"></a>

此代码仓仅包含功能演示与开发示例代码，旨在展示特定功能的使用方式与集成方法，不用于生产环境。所有代码仅为技术参考，不继承或承诺任何上下游软件的安全设计与防护机制。本仓库中的示例代码可能存在安全缺陷、漏洞或不完整实现，鲲鹏计算社区不对代码的安全性、稳定性及合规性承担任何责任。使用者应自行评估风险，并根据实际场景进行安全加固。任何因使用本仓库代码所引发的安全问题，均由使用者自行承担。请勿将本仓库代码直接用于生产系统，建议持续关注上游开源项目的安全公告与版本更新。

# 许可证书<a name="ZH-CN_TOPIC_0000002475594429"></a>

本项目采用Apache License 2.0许可证。详见[LICENSE](https://gitcode.com/boostkit/cloud-virtual/blob/master/LICENSE)文件。

# 参考文档<a name="ZH-CN_TOPIC_0000002442368346"></a>

特性指南（[https://support.huawei.com/enterprise/zh/doc/EDOC1100494962/f2f40b4c?idPath=23710424|251364417|9856629|253662285](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/videostreamengine/kunpengcpsvideo_20_0002.html)）

