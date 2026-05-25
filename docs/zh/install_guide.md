# 安装指南<a name="ZH-CN_TOPIC_0000002521693392"></a>

## 1 软件部署<a name="ZH-CN_TOPIC_0000002549826277"></a>

### 1.1 环境要求<a name="ZH-CN_TOPIC_0000002549706275"></a>

#### 1.1.1 硬件环境<a name="ZH-CN_TOPIC_0000002550361171"></a>

部署Kbox云手机容器环境前请确保您的环境满足已验证的硬件环境要求。

Kbox云手机容器环境部署的硬件环境配置方案要求如[**表 1** Kbox云手机容器环境部署硬件配置方案要求](#Kbox云手机容器环境部署硬件配置方案要求)所示。

**表 1** Kbox云手机容器环境部署硬件配置方案要求<a id="Kbox云手机容器环境部署硬件配置方案要求"></a>

|配置项|硬件配置方案一|硬件配置方案二|硬件配置方案三|硬件配置方案四|
|--|--|--|--|--|
|服务器|鲲鹏服务器|鲲鹏服务器|鲲鹏服务器|鲲鹏服务器|
|CPU|2\*鲲鹏920 7260处理器，64 <Core@2.6GHz>|2\*鲲鹏920 7260处理器，64 <Core@2.6GHz>|2\*鲲鹏920 7280Z处理器，80 <Core@2.9GHz>|2\*鲲鹏920 7280W处理器，64 <Core@2.2GHz>|
|内存|16\*DDR4 RDIMM内存-32GB-2933MT/s|16\*DDR4 RDIMM内存-32GB-2933MT/s|16\*DDR5 DIMM内存-64GB-4800MT/s|16\*DDR5 DIMM内存-64GB-5200MT/s|
|硬盘|系统盘：2\*固态硬盘-480GB-SATA 6Gb/s-读取密集型<br>数据盘：2\*ES3521A V6固态硬盘-1920GB-SATA 6Gb/s-读取密集型|系统盘：2\*固态硬盘-480GB-SATA 6Gb/s-读取密集型<br>数据盘：2\*ES3521A V6固态硬盘-1920GB-SATA 6Gb/s-读取密集型|系统盘：1\*S3521A V6固态硬盘-1920GB-SATA 6Gb/s-读取密集型<br>数据盘：2\*S3521A V6固态硬盘-1920GB-SATA 6Gb/s-读取密集型|系统盘：1\*固态硬盘-480GB-SATA 6Gb/s-2.5 inch height-读密集型<br>1\*S4510 固态硬盘-960GB-SATA 6Gb/s-读取密集型<br>数据盘：1\*ES3600P V6固态硬盘-6400GB-NVMe 64Gb/s<br>1\*ES3500P V5固态硬盘-4000GB-NVMe 32Gb/s|
|网卡|板载：1\*（4\*GE接口卡）1\*TM280板载灵活网卡-25GE/10GE光口-4端口-SFP28（不含光模块）<br>外接：1\*Mellanox网卡|板载：1\*（4\*GE接口卡）1\*TM280板载灵活网卡-25GE/10GE光口-4端口-SFP28（不含光模块）<br>外接：1\*Mellanox网卡|板载：1\*（4\*GE接口卡）1\*TM280板载灵活网卡-225GE/10GE光口-4端口-SFP28（不含光模块）<br>外接：1\*Mellanox网卡|板载：1\*（4\*GE接口卡）1\*TM280板载灵活网卡-2\*25GE/10GE光口-4端口-SFP28（不含光模块）|
|Riser卡|Riser1与Riser2模组相同，均为：PCIe X16 + PCIe X8|Riser1与Riser2模组相同，均为：PCIe X8\*3|前置Riser（x8\*2）\*2+后置Riser（x8\*2）\*2+Riser3（x8\*2）\*1|后置Riser（x16+x8\*2）\*2+Riser3（x8\*2）\*1|
|编码卡|1\*NETINT Quadra T2A（X8）|无|无|无|
|GPU|2\*AMD W6800|4\*道客DC1000|8\*道客DC1000|8\*道客DC1000|
|操作系统|openEuler 22.03 LTS SP4|openEuler 22.03 LTS SP4|openEuler 22.03 LTS SP4|openEuler 22.03 LTS SP4|
|内核版本|5.10.0-216.0.0|5.10.0-216.0.0|5.10.0-216.0.0|5.10.0-216.0.0|

>![](public_sys-resources/icon-note.gif) **说明：** 
>
>- 选择鲲鹏服务器兼容的Mellanox网卡，通过[鲲鹏计算兼容性查询工具](https://info.support.huawei.com/computing/tools/compatibility-query/enterprise/kunpeng-computing/component-compatibility)可查询具体型号网卡。
>- NETINT Quadra是NETINT T432编码卡下一代演进，后续文档仅以Quadra为例进行说明；若需要使能T432编码卡，也可参考Quadra编码卡进行使能。

#### 1.1.2 软件环境<a name="ZH-CN_TOPIC_0000002549706295"></a>

##### 1.1.2.1 Kbox<a name="ZH-CN_TOPIC_0000002549826303"></a>

部署Kbox云手机容器的软件环境要求请参见《[Kbox云手机容器 特性指南](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/kboxcpc/kunpengcpskbox_20_0131.html)》中软件部署的“软件环境”章节。

##### 1.1.2.2 视频流引擎<a name="ZH-CN_TOPIC_0000002518346424" id="视频流引擎"></a>

在部署视频流云手机环境前，请参见本节提供的渠道获取相应的软件包并对华为提供的软件包进行完整性校验，以便进行后续的部署步骤。

**获取软件包<a name="section9155930113616" id="获取软件包"></a>**

**表 1** 部署视频流引擎的软件环境要求<a id="部署视频流引擎的软件环境要求"></a>

|序号|软件包|说明|获取地址|配置方案一|配置方案二|配置方案三|配置方案四|
|--|--|--|--|--|--|--|--|
|1|BoostKit-boostcph-videoengine_*.zip|视频流引擎二进制包。|[获取链接](https://www.hikunpeng.com/boostkit/arm-native?application=视频流引擎#base-soft)|√|√|√|√|
|2|DemoVideoEngine.tar.gz|视频流服务端tar包组件，负责获取Kbox容器音视频数据等。|请联系华为技术支持获取。|√|√|√|√|
|3|DemoVideoEngine_sha256.txt|视频流服务端tar包组件对应的SHA256文件，负责完整性校验。|请联系华为技术支持获取。|√|√|√|√|
|4|CloudPhoneApk.tar.gz|视频流客户端tar包组件，负责解码播放音视频数据。|请联系华为技术支持获取。|√|√|√|√|
|5|CloudPhoneApk_sha256.txt|视频流客户端tar包组件对应的SHA256文件，负责完整性校验。|请联系华为技术支持获取。|√|√|√|√|
|6|VideoClientEmulator.tar.gz|视频流云手机压测工具。|请联系华为技术支持获取。|√|√|√|√|
|7|NETINT-v*XXX*.tar.gz|视频流NETINT编码卡tar包组件，负责硬件编码，配套版本4.8.F-scale。|[获取链接](https://www.netint.cn/kunpeng-quadra-firmware-downloads)<br>下载密码：test123|√|-|-|-|
|8|Quadra_V*XXX*.zip|NETINT编码卡Quadra软固件及文档包。|[获取链接](https://www.netint.cn/kunpeng-quadra-firmware-downloads)<br/>下载密码：test123|√|-|-|-|
|9|WebClient.zip|Web客户端。|请联系华为技术支持获取。|√|√|√|√|
|10|topo-affinity-plugin-master.zip|K8s NUMA亲和插件。|[获取链接](https://gitee.com/kunpeng_compute/topo-affinity-plugin)|√|√|√|√|

>![](public_sys-resources/icon-note.gif) **说明：** 
>
>- √：是指使用对应硬件配置方案时需要安装该软件项目。
>- -：是指使用对应硬件配置方案时不需要安装该软件项目。

**软件包完整性校验<a name="section111981239163615"></a>**

为了防止软件包在传递过程或存储期间被恶意篡改，从鲲鹏社区获取软件包时需下载对应的数字签名文件用于完整性验证。

1. 请参见[**表 1** 部署视频流引擎的软件环境要求](#部署视频流引擎的软件环境要求)获取软件包。
2. <a name="li1273482318125"></a>从[华为企业业务网站](https://support.huawei.com/enterprise/zh/tool/pgp-verify-TL1000000054)或[运营商网站](http://support.huawei.com/carrier/digitalSignatureAction)获取校验工具和校验方法。
3. 使用[2](#li1273482318125)获取到的签名验证指南文档对下载的软件包进行PGP数字签名校验。

>![](public_sys-resources/icon-note.gif) **说明：** 
>如果校验失败，请不要使用该软件包，先联系华为技术支持工程师解决。
>使用软件包安装/升级之前，也需要按上述过程先验证软件包的数字签名，确保软件包未被篡改。
>使用软件包前请先阅读《[鲲鹏应用使能套件BoostKit用户许可协议 2.0](https://www.hikunpeng.com/zh/legal/developer/boostkit/software/protocol)》，如确认继续使用，则默认同意协议的条款和条件。

### 1.2 部署云手机<a name="ZH-CN_TOPIC_0000002549826291"></a>

#### 1.2.1 环境配置<a name="ZH-CN_TOPIC_0000002549826289"></a>

##### 1.2.1.1 配置BIOS<a name="ZH-CN_TOPIC_0000002518346454"></a>

环境部署指定的服务器BIOS版本对内存的插入格式有限制。在进行BIOS设置之前，请确保内存插入格式正确，然后根据硬件配置方案需求完成MISC、Performance和Memory等相关选项的配置，用以提高服务器性能。

具体配置步骤请参见《[Kbox云手机容器 特性指南](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/kboxcpc/kunpengcpskbox_20_0167.html)》中“软件部署”的“配置BIOS”章节内容。

环境部署指定的服务器BIOS版本对内存的插入格式有限制。在进行BIOS设置之前，请确保内存插入格式正确，然后根据硬件配置方案需求完成MISC、Performance和Memory等相关选项的配置，用以提高服务器性能。

- **[（硬件配置方案一，可选）升级NVMe固件版本](https://gitcode.com/boostkit/Kbox/blob/AOSP11/docs/zh/install_guide.md#升级NVMe固件版本)**

- **[（硬件配置方案一）配置GPU工作模式和CPU绑定](https://gitcode.com/boostkit/Kbox/blob/AOSP11/docs/zh/install_guide.md#配置GPU工作模式和CPU绑定)**  

##### 1.2.1.2 部署Kbox容器基础环境<a name="ZH-CN_TOPIC_0000002518186516" id="部署Kbox容器基础环境"></a>

Kbox云手机容器部署的详细操作请参见《[Kbox云手机容器 特性指南](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/kboxcpc/kunpengcpskbox_20_0130.html)》中的“软件部署”章节。按照步骤完成该章节中“启动和卸载云手机实例”小节前的所有部署。

##### 1.2.1.3 （可选）部署Containerd环境<a name="ZH-CN_TOPIC_0000002549826283" id="部署Containerd环境"></a>

视频流云手机除支持以基于Docker的容器运行时启动外，还支持以Containerd容器运行时启动。若选择以Containerd运行时运行视频流云手机，可参考本章节进行Containerd相关软件包的部署。

**获取软件包<a name="section11783175614511"></a>**

部署Containerd环境需要使用的软件包如[**表 1** Containerd相关软件包](#Containerd相关软件包)所示。

**表 1** Containerd相关软件包<a id="Containerd相关软件包"></a>

|软件名|版本号|软件描述|获取方式|
|--|--|--|--|
|Containerd|v1.7.14|Containerd是一个容器运行时。|Containerd二进制软件包：containerd-1.7.14-linux-arm64.tar.gzContainerd Service文件：[获取链接](https://raw.githubusercontent.com/containerd/containerd/main/containerd.service)|
|runc|v1.1.12|runc是一个符合开放容器标准OCI（Open Container Initiative）规范的轻量级容器运行时，是Containerd的一个依赖组件。|[获取链接]( https://github.com/opencontainers/runc/releases/download/v1.1.12/runc.arm64)|
|/rCNI Plugin|v1.4.1|容器网络接口CNI（Container Network Interface）是一个规范和库，用于在Linux容器中配置网络接口。|[获取链接]( https://github.com/containernetworking/plugins/releases/download/v1.4.1/cni-plugins-linux-arm64-v1.4.1.tgz)|
|nerdctl|v1.7.5|nerdctl是一个兼容Docker CLI的命令行工具，用于管理Containerd容器和镜像。|[获取链接](https://github.com/containerd/nerdctl/releases/download/v1.7.5/nerdctl-1.7.5-linux-arm64.tar.gz)|

**部署Containerd环境<a name="section343716111874"></a>**

1. <a id="部署Containerd环境1"></a>下载并解压Containerd二进制软件包到“/usr/local”目录下。

    ```shell
    mkdir -p /root/containerdenv/downloads
    cd /root/containerdenv/downloads
    wget https://github.com/containerd/containerd/releases/download/v1.7.14/containerd-1.7.14-linux-arm64.tar.gz --no-check-certificate
    tar Cxzvf /usr/local containerd-1.7.14-linux-arm64.tar.gz
    ```

    确认Containerd版本号为v1.7.14。

    ```shell
    containerd --version
    ```

2. <a id="部署Containerd环境2"></a>下载Containerd Service文件，配置成系统服务。

    ```shell
    cd /root/containerdenv/downloads
    wget https://raw.githubusercontent.com/containerd/containerd/main/containerd.service --no-check-certificate
    mkdir -p /usr/local/lib/systemd/system/
    cp containerd.service /usr/local/lib/systemd/system/
    systemctl daemon-reload
    systemctl enable --now containerd
    ```

    确认Containerd服务是否已正常启动。

    ```shell
    systemctl status containerd
    ```

    ![](figures/zh-cn_image_0000002549706313.png)

    回显信息如上图所示表示Containerd服务已经正常启动。

3. <a id="部署Containerd环境3"></a>下载并安装runc。

    ```shell
    cd /root/containerdenv/downloads
    wget https://github.com/opencontainers/runc/releases/download/v1.1.12/runc.arm64 --no-check-certificate
    install -m 755 runc.arm64 /usr/local/sbin/runc
    ```

    确认runc版本号为1.1.12。

    ```shell
    runc --version
    ```

4. <a id="部署Containerd环境4"></a>下载并安装CNI plugin。

    ```shell
    cd /root/containerdenv/downloads
    mkdir -p /opt/cni/bin
    wget https://github.com/containernetworking/plugins/releases/download/v1.4.1/cni-plugins-linux-arm64-v1.4.1.tgz --no-check-certificate
    tar Cxzvf /opt/cni/bin cni-plugins-linux-arm64-v1.4.1.tgz
    ```

5. <a id="部署Containerd环境5"></a>下载并安装nerdctl工具。

    ```shell
    cd /root/containerdenv/downloads
    wget https://github.com/containerd/nerdctl/releases/download/v1.7.5/nerdctl-1.7.5-linux-arm64.tar.gz --no-check-certificate
    tar Cxzvf /usr/local/bin nerdctl-1.7.5-linux-arm64.tar.gz
    ```

    确认nerdctl版本号为1.7.5。

    ```shell
    nerdctl --version
    ```

6. <a id="部署Containerd环境6"></a>重新启动Docker服务，并重新启动一个新的终端以使新的容器运行时生效。

    ```shell
    systemctl restart docker
    ```

    若要切换至Docker版本的容器运行时，须将[1](#部署Containerd环境1)~[5](#部署Containerd环境5)已安装的相关软件二进制从对应目录中移除。移除完成后，参考以上命令重启Docker服务并重新启动一个新的终端。

- **[（硬件配置方案二、三、四）安装显卡驱动](https://gitcode.com/boostkit/Kbox/blob/AOSP11/docs/zh/install_guide.md#安装显卡驱动)**  

#### 1.2.2 制作镜像<a name="ZH-CN_TOPIC_0000002549826281"></a>

##### 1.2.2.1 制作Kbox镜像<a name="ZH-CN_TOPIC_0000002549826313"></a>

制作视频流云手机镜像前需要根据本章节内容完成Kbox镜像的制作。

1. 请参见[部署Kbox容器基础环境](#部署Kbox容器基础环境)获取Kbox容器启动依赖组件android.tar和Kbox-patches-AOSP11.zip，并上传至服务器的“/home/kbox_video”目录（本文以此目录作为示例，用户也可自行设置目录）。
2. 解压Kbox-patches-AOSP11.zip，获取“deploy_scripts”路径下的组件base_box.sh，并将其拷贝到“/home/kbox_video”目录，赋予文件权限，使文件拥有者有读、写、执行权限而属组用户和其他用户只有读和执行权限。

    ```shell
    unzip Kbox-patches-AOSP11.zip
    cp Kbox-patches-AOSP11/deploy_scripts/base_box.sh /home/kbox_video/
    chmod 755 /home/kbox_video/base_box.sh
    ```

3. 制作Kbox镜像，镜像名称通常命名为kbox:origin。
    1. 上传Kbox Demo镜像包android.tar至“~/dependency”目录（本文以此目录作为示例，用户可自行设置目录），并挂载。

        镜像的名称和tag可以自行定义，格式为“{名称}:{tag}”，此处设置镜像名为kbox:demo。

        ```shell
        cd ~/dependency
        docker import android.tar kbox:demo
        ```

    2. 将Kbox-patches-AOSP11文件夹中的deploy_scripts目录上传至服务器的“~/dependency”目录。
    3. 上传Android Kbox二进制文件包BoostKit-boostcph-kbox_\*.zip到“~/dependency/deploy_scripts”目录。
    4. （硬件配置方案二、三、四）使用硬件配置方案二、三、四时需要解压显卡驱动压缩包VAGPU-25.03.01.01-RC20.tgz，获取va_driver.tgz，上传到服务器的“~/dependency/deploy_scripts”目录。
    5. 制作包含Android Kbox二进制的Kbox镜像，其中kbox:demo为导入的官方Kbox Demo镜像，kbox:origin为包含Android Kbox二进制的新镜像。
        - 硬件配置方案一：

            ```shell
            cd ~/dependency/deploy_scripts
            chmod +x make_image.sh
            ./make_image.sh kbox:demo kbox:origin
            ```

        - 硬件配置方案二、三、四：

            ```shell
            cd ~/dependency/deploy_scripts
            chmod +x make_image.sh
            ./make_image.sh kbox:demo kbox:origin va_driver.tgz
            ```

4. 查看Kbox镜像（kbox:origin）是否制作成功。

    ```shell
    docker images
    ```

    回显如下，表示镜像制作成功。

    ```shell
    REPOSITORY    TAG       IMAGE ID        CREATED          SIZE
    kbox          origin    d1f5cfd2e722    6 seconds ago    2.09GB
    ```

##### 1.2.2.2 制作视频流云手机镜像<a name="ZH-CN_TOPIC_0000002549706277"></a>

获取视频流客户端tar包组件、视频流服务端tar包组件、视频流引擎二进制软件包和视频流NETINT编码卡tar包组件进行视频流云手机镜像的制作。若使用Containerd容器运行时启动视频流云手机，则需要使用Docker工具导出符合OCI格式的镜像，并通过nerdctl工具导入以供Containerd使用。

**校验软件包完整性<a name="section1286473717216"></a>**

1. 请参见[视频流引擎](#视频流引擎)获取CloudPhoneApk.tar.gz、DemoVideoEngine.tar.gz和BoostKit-boostcph-videoengine_\*.zip软件包，获取后将软件包上传至服务器的“/home/kbox_video”目录。
2. 通过以下命令获取如下组件的SHA256校验码。

    ```shell
    sha256sum DemoVideoEngine.tar.gz
    sha256sum CloudPhoneApk.tar.gz
    ```

3. 将校验码分别与DemoVideoEngine_sha256.txt和CloudPhoneApk_sha256.txt进行对比。

    如果一致，说明获取的软件包完整，可以继续下一步操作，否则应该暂停部署，重新获取完整的软件包。

4. （硬件配置方案一）使用硬件配置方案一时请参见[视频流引擎](#视频流引擎)获取NETINT-vXXX.tar.gz软件包，获取后将软件包上传至服务器的“/home/kbox_video”目录，并重命名为NETINT.tar.gz。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >- Quadra编码卡和T432配套的NETINT.tar.gz不同，请选择对应的NETINT.tar.gz。
    >- NETINT Quadra是NETINT T432编码卡的下一代演进，后续文档仅以Quadra为例进行说明；若需要使能T432编码卡，可参考Quadra编码卡进行使能。

**制作镜像<a name="section118652371219" id="制作镜像"></a>**

1. 解压DemoVideoEngine.tar.gz软件包获取制作镜像的脚本，并赋予可执行权限。
    - 配置方案一：

        ```shell
        tar -xvf DemoVideoEngine.tar.gz Dockerfile_NoVPU Dockerfile_T432 Dockerfile_QuadraT2A make_image.sh
        chmod +x Dockerfile_NoVPU Dockerfile_T432 Dockerfile_QuadraT2A make_image.sh
        ```

    - 配置方案二、三、四：

        ```shell
        tar -xvf DemoVideoEngine.tar.gz Dockerfile_NoVPU  make_image.sh
        chmod +x Dockerfile_NoVPU  make_image.sh
        ```

2. 制作视频流云手机镜像。镜像名称可以使用默认镜像名也可以自己指定。
    - 使用默认镜像名称，执行命令如下。Kbox基础云手机和视频流云手机默认镜像名称分别为kbox:latest和video:latest。

        ```shell
        ./make_image.sh
        ```

    - 如果镜像名称需要自定义，执行命令如下例所示。通过参数指定Kbox基础云手机和视频流云手机镜像名称，格式均为“{镜像名}:{tag}”，如下例所示kbox、video为镜像名，origin、latest为tag。

        ```shell
        ./make_image.sh kbox:origin video:latest
        ```

        >![](public_sys-resources/icon-note.gif) **说明：** 
        >镜像名只可包含数字与小写字母，首字符应为小写字母，tag名只可包含数字与字母。若自定义修改了视频流云手机的镜像名称，请参见[制作基础数据卷](#制作基础数据卷)章节中将cfct_config配置文件中的视频流云手机镜像名更新为自定义的镜像名称。

3. 查看视频流云手机镜像（video:latest）是否制作成功。

    ```shell
    docker images
    ```

    回显如下，表示镜像制作成功。

    ```shell
    REPOSITORY    TAG       IMAGE ID        CREATED          SIZE
    video         latest    40e5f42c17d9    6 seconds ago    2.11GB
    ```

若使用Containerd容器运行时启动视频流云手机时，需要通过以下步骤使用Docker工具导出符合OCI格式的镜像，并通过nerdctl工具导入以供Containerd使用。

1. 导出制作好的视频流云手机镜像，以镜像名为video:latest的镜像为例（导出的容器tar包命名可自定义）。

    ```shell
    docker save video:latest > videolatest_oci.tar
    ```

2. 通过nerdctl导入符合OCI格式的视频流云手机镜像。

    ```shell
    nerdctl load -i videolatest_oci.tar
    ```

3. 确认镜像已成功导入。

    ```shell
    nerdctl images
    ```

4. 创建“containerd_config”配置文件使脚本可识别启动运行时为Containerd。

    ```shell
    cd /home/kbox_video
    touch containerd_config
    ```

    若要切换Docker作为视频流云手机的默认容器运行时，请将“containerd_config”文件删除。

5. 允许网络包转发策略。

    ```shell
    iptables -P FORWARD ACCEPT
    ```

#### 1.2.3 设置cfct_config配置文件（配置方案一）<a name="ZH-CN_TOPIC_0000002549706303"></a>

通过设置cfct_config配置文件可以灵活配置视频流云手机使用的资源，使性能达到最优。云手机启动时必须在启动路径下存放cfct_config配置文件，云手机容器会使用该文件中的配置，使用时应确保cfct_config配置文件中的配置正确。

**cfct_config文件配置步骤<a name="section9436102613100"></a>**

1. 解压cfct_config配置文件并设置文件权限，使文件拥有者有读写权限而其他属组用户和其他用户只有读权限。

    ```shell
    cd /home/kbox_video/
    tar -xvf DemoVideoEngine.tar.gz cfct_config
    chmod 644 cfct_config
    ```

2. 通过配置GPU、CPU、ENC、USERDATA等map中对应路数的值，选择该路容器使用的GPU、CPU、NETINT编码卡，以及数据卷存放路径。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >为确保视频流云手机的稳定运行与最佳性能，请保障每个容器所绑定的CPU物理核和GPU渲染节点同属于一个CPU片。

3. NETINT编码卡的节点在不同服务器中会有区别，应根据实际情况修改cfct_config中NETINT的值，保证编码不会因跨片导致性能损失。
4. 如果要使能Quadra/T432编码卡硬解，需要将cfct_config中的“T432_QUADRA_DECODE_ENABLE”设置为“1”。
5. 针对1张GPU卡环境：需要修改cfct_config配置文件中VIDEO_CPU_MAP_{_CPU总核数_}CORE_MODE{_CPU_BIND_MODE变量值_}。NETINT编码卡芯片节点所属NUMA查询方式请参见[NETINT编码卡芯片节点所属NUMA查询方式](#section2507154233510)。

    以VIDEO_CPU_MAP_128CORE_MODE0为例，保留该配置变量下与GPU绑定的CPU配置，删除其他配置，当GPU卡插在CPU0上时，删除MODE0_CPUS2和MODE0_CPUS3所有相关引用；若GPU卡插在CPU1上时，删除MODE0_CPUS0和MODE0_CPUS1所有相关引用。GPU卡所属NUMA查询方式请参见[AMD GPU渲染节点所属NUMA的查询方式](#section20575115322416)。

6. 针对1张编码卡环境：需要修改cfct_config配置文件中“VIDEO_ENC_MAP_CORE”。
7. 当编码卡插在CPU0上时，删除\${NETINT1}；若编码卡插在CPU1上时，删除\${NETINT0}。
8. 如果要使能WebRTC特性，需要将cfct_config中的“ENABLE_WEBRTC_CONNECTION”设置为“1”。若视频帧采用CPU进行软编码，需要将cfct_config中的“CPU_BIND_MODE”设置为“1”，以防卡顿。
9. 如果需要使能图形加速层功能，需要将cfct_config中的“**ENABLE_RENDER_LAYER**”设置为“1”。详细说明请参见[图形加速层的基本功能和使用说明](#图形加速层的基本功能和使用说明)。
10. 如果要使能C2解码器，需要将cfct_config中的“**ENABLE_AMD_C2_DECODE**”设置为“1”

**NETINT编码卡芯片节点所属NUMA查询方式<a name="section2507154233510"></a>**

1. <a name="li1256022316361"></a>通过**nvme list**命令查看编码卡芯片对应节点号。

    ```shell
    nvme list
    ```

    以下回显为NETINT编码芯片NVMe节点，该内容为回显示例，请以实际为准。

    ```shell
    Node          SN                   Model            Namespace Usage                    Format           FW Rev
    ------------- -------------------- ---------------- --------- ------------------------ ---------------- --------
    /dev/nvme0n1  Q2A325A11DC082-0454A QuadraT2A        1         8.59  TB /   8.59  TB    4 KiB +  0 B     48F6rKr1
    /dev/nvme1n1  Q2A325A11DC082-0454B QuadraT2A        1         8.59  TB /   8.59  TB    4 KiB +  0 B     48F6rKr1
    ```

2. 查看NVMe节点与PCIe bus号对应关系。

    _{index}_为[1](#li1256022316361)回显信息所示的NVMe节点编号。例如/dev/nvme1n1，该节点_{index}_即为1。

    ```shell
    find /sys/devices/ -name nvme{index}
    ```

    回显如下，其中0000:05:00.0为该设备对应的busID：

    ```shell
    /sys/devices/pci0000:00/0000:00:0e.0/0000:05:00.0/nvme/nvme1
    /sys/devices/virtual/nvme-subsystem/nvme-subsys1/nvme1
    ```

3. 通过bus号找到该节点与NUMA从属关系。

    _{busID}_为上一步骤获取的bus号。以nvme1设备的回显为例，_{busID}_即为0000:05:00.0。

    ```shell
    lspci -vvvs {busID} | grep NUMA
    ```

    回显如下。

    ```shell
    NUMA node: 0
    ```

4. 根据编码卡NVMe设备节点对应的NUMA修改cfct_config中NETINT的值。

    鲲鹏920 7265F/7260服务器：从属于0、1号NUMA的NVMe节点写在NETINT0字段中，从属于2、3号NUMA的NVMe节点写在NETINT1字段中。

    字段中每个设备需添加两个节点。例如2号NVMe设备，需添加“/dev/nvme2”、“/dev/nvme2n1”两个节点。

    ```shell
    # NETINT编码卡设备节点
    NETINT0="/dev/nvme0,/dev/nvme0n1,/dev/nvme1,/dev/nvme1n1"
    NETINT1="/dev/nvme2,/dev/nvme2n1,/dev/nvme3,/dev/nvme3n1"
    ```

**AMD GPU渲染节点所属NUMA的查询方式<a name="section20575115322416"></a>**

>![](public_sys-resources/icon-note.gif) **说明：** 
>AMD GPU每张卡对应1个GPU渲染节点。

1. <a name="li34656503552"></a>获取GPU渲染节点命令。

    ```shell
    ll /dev/dri/by-path/ | grep renderD
    ```

    回显示例如下。

    ```shell
    lrwxrwxrwx 1 root root 13 Oct 25 10:58 pci-0000:03:00.0-render -> ../renderD128
    lrwxrwxrwx 1 root root 13 Oct 25 10:58 pci-0000:83:00.0-render -> ../renderD129
    ```

    说明该服务器插了两张AMD GPU，渲染节点分别为renderD128，renderD129。

2. 查询NUMA节点命令。

    ```shell
    cat /sys/bus/pci/devices/0000\:XX\:00.0/numa_node 
    ```

    其中，指令中的“XX”应按[1](#li34656503552)中的实际回显IP地址进行修改。以回显renderD128为例，查询指令应为：

    ```shell
    cat /sys/bus/pci/devices/0000\:03\:00.0/numa_node
    ```

    回显如下所示。

    ```shell
    0
    ```

    该回显表明GPU渲染节点renderD128所在NUMA节点为0。

**图形加速层的基本功能和使用说明<a name="section1156712526252" id="图形加速层的基本功能和使用说明"></a>**

当前图形加速层使能了以下两个功能：

- GPUMock：对GPU厂商、GPU型号、OpenGL ES版本、GLMax能力值、OpenGL ES拓展进行模拟。
- ShaderCache：通过预构建着色器二进制、多云手机共享缓存，消除着色器编译链接等处理时间，降低OpenGL ES大型应用运行卡顿率。

图形加速层的使能步骤如下：

1. 将云手机启动配置文件cfct_config中的“**ENABLE_RENDER_LAYER**”设置为“1”。
2. 从软件包Kbox-patches-AOSP11.zip中复制kbox_render_accelerating_configuration.xml配置文件到启动路径“/home/kbox_video/”。

    ```shell
    cp /home/kbox_video/Kbox-patches-AOSP11/deploy_scripts/kbox_render_accelerating_configuration.xml /home/kbox_video/
    ```

3. 打开kbox_render_accelerating_configuration.xml配置文件，对应用的图形加速层功能进行配置。具体配置项描述请参见[图形加速层配置项](user_guide.md#图形加速层配置项)章节。

>![](public_sys-resources/icon-note.gif) **说明：** 
>
>- 首次启动云手机时，若需要修改图形加速层功能的配置，则修改配置文件中应用对应的配置，手动将其拷贝到云手机容器“/data/local/tmp”路径，重启应用即可生效。
>- 宿主机上多容器共享一个着色器缓存路径，可以先启动一路云手机预收集应用尽可能完整的着色器，其他云手机通过将配置文件对应的应用设置为只读模式来使能ShaderCache功能，此时性能最佳。
>- ShaderCache功能没有缓存淘汰机制，若是缓存文件系统存储已满或者游戏版本更新，为了避免着色器和二进制文件不能对应，请清理整个文件系统的缓存。

#### 1.2.4 设置cfct_config配置文件（配置方案二、三、四）<a name="ZH-CN_TOPIC_0000002518186514"></a>

通过设置cfct_config配置文件可以灵活配置视频流云手机使用的资源，使性能达到最优。云手机启动时必须在启动路径下存放cfct_config配置文件，云手机容器会使用该文件中的配置，使用时应确保cfct_config配置文件中的配置正确。

cfct_config配置文件配置项和配置方法如下所示。

1. 解压cfct_config配置文件并设置文件权限，使文件拥有者有读写权限而其他属组用户和其他用户只有读权限。

    ```shell
    cd /home/kbox_video/
    tar -xvf DemoVideoEngine.tar.gz cfct_config
    chmod 644 cfct_config
    ```

2. 通过配置GPU、CPU、USERDATA等map中对应路数的值，选择该路容器使用的GPU、CPU以及数据卷存放路径。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >为确保视频流云手机的稳定运行与最佳性能，请保障每个容器所绑定的CPU物理核和GPU渲染节点同属于一个CPU片。

3. 当前视频流云手机默认使能DC1000 GPU硬解的硬解功能（即默认**ENABLE_HARD_DECODE=1**），如需使用软解，需设置**ENABLE_HARD_DECODE=0**并重启容器。
4. 如果要使能WebRTC特性，需要更改cfct_config中的ENABLE_WEBRTC_CONNECTION=1。
5. 绑核和确认绑核生效。针对1张GPU卡环境：需要修改cfct_config配置文件中VIDEO_CPU_MAP_{_CPU总核数_}CORE_MODE{_CPU_BIND_MODE变量值_}。

    以VIDEO_CPU_MAP_128CORE_MODE0为例，保留该配置变量下与GPU绑定的CPU配置，删除其他配置，当GPU卡插在CPU0上时，删除MODE0_CPUS2和MODE0_CPUS3所有相关引用；若GPU卡插在CPU1上时，删除MODE0_CPUS0和MODE0_CPUS1所有相关引用。

    - 如何确认当前环境只有一张GPU？

        查询服务器中道客DC1000信息。

        ```shell
        lspci -D | grep 0200
        ```

        回显如下所示，可知该服务器上只有一张道客DC1000，其中0000:04:00.0为busID。

        ```shell
        0000:04:00.0 3D controller: Device 1f4f:0200
        0000:04:00.1 3D controller: Device 1f4f:0200
        0000:04:00.2 3D controller: Device 1f4f:0200
        0000:04:00.3 3D controller: Device 1f4f:0200
        ```

    - 如何确认GPU与CPU的绑定关系？

        查询该显卡所属的NUMA。

        ```shell
        lspci -vvvs {busID} | grep NUMA
        ```

        回显如下所示，说明该卡绑定在cpu的NUMA 0上。

        ```shell
        NUMA node: 0
        ```

6. 如果要使能图形加速层，需要将cfct_config中“ENABLE_RENDER_LAYER”设置为“1”。详细说明请参见[图形加速层的基本功能和使用说明](#图形加速层的基本功能和使用说明)。

#### 1.2.5 制作基础数据卷<a name="ZH-CN_TOPIC_0000002518346430" id="制作基础数据卷"></a>

确认并根据需要调整默认的镜像名称和数据卷存放目录。删除或备份现有数据卷，解压并设置启动脚本权限，使用脚本启动云手机并预装应用，最后删除初始容器。

1. <a name="li16219132415811"></a>确认数据卷存放目录和镜像名称。

    默认镜像名称为video:latest，默认数据卷存放目录为“/home/mount”，可根据实际情况自行更改，修改方法为将“cfct_config”文件中“DOCKER_IMAGE”和“USERDATA”值调整为实际的名称或目录。

    ```shell
    DOCKER_IMAGE=video:latest
    USERDATA="/home/mount"
    ```

2. 删除原有数据卷或备份到其他位置，其中{USERDATA}为[1](#li16219132415811)中的实际数据卷存放目录，若存在多个数据卷存放目录，则需要分别对每个数据卷存放目录进行本章节余下所有操作。

    ```shell
    rm -rf {USERDATA}/data/android_base
    ```

3. 从DemoVideoEngine.tar.gz中解压获取启动脚本cfct_video，并赋予权限，使文件拥有者有读、写、执行权限而属组用户和其他用户只有读和执行权限。

    ```shell
    cd /home/kbox_video/
    tar -xvf DemoVideoEngine.tar.gz cfct_video
    chmod 755 cfct_video
    ```

4. 使用cfct_video脚本启动1路云手机，本文以android_1为例。

    ```shell
    ./cfct_video start 1  
    ```

5. 将所需的应用（例地铁跑酷等）预装到该云手机容器中，将android_1作为新数据卷，供启动视频流云手机时使用。

    ```shell
    cd {USERDATA}/data/
    cp -rp android_1 android_base
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >如果使用nfs挂载启动的容器，由于性能考虑，不支持cp -rp直接拷贝数据目录，应该直接拷贝img。
    >
    >将所需的应用（例如地铁跑酷等）预装到该云手机容器中，将android_1.img拷贝为android_base.img作为新数据卷。
    >
    >```shell
    >cd \${USERDATA}/img/
    >cp -rp android_1.img android_base.img
    >```
    >
    >在启动指定容器前手动拷贝android_base.img为相应容器编号。
    >
    >```shell
    >cd \${USERDATA}/img/
    >cp -rp android_base.img android_\${index}.img
    >```

6. 删除android_1容器。

    ```shell
    cd /home/kbox_video/
    ./cfct_video delete 1
    ```

### 1.3 K8s集群下部署视频流云手机（配置方案二）<a name="ZH-CN_TOPIC_0000002518346436"></a>

#### 1.3.1 环境准备<a name="ZH-CN_TOPIC_0000002518346440"></a>

视频流云手机支持使用Containerd启动，使用K8s集群管理。在K8s集群下部署视频流云手机时需准备至少2台服务器，1台作为master节点，1台或者多台作为工作节点。

各节点规划详情如[**表 1** K8s集群节点详情](#K8s集群节点详情)所示。

**表 1** K8s集群节点详情<a id="K8s集群节点详情"></a>

|节点名称（即主机名，可自定义）|节点角色|服务器个数|环境准备|节点功能|
|--|--|--|--|--|
|k8s-master|master节点|1台|基于鲲鹏服务器和openEuler 22.03 LTS SP1系统。|整个集群的大脑和控制中心，负责协调和管理集群中的各种资源，以实现高可用性、可扩展性和自动化运维，实际不运行云手机相关业务。|
|k8s-slave1|工作节点|1台及以上|请参见软件部署章节完成视频流云手机的环境部署。|运行云手机业务。|

>![](public_sys-resources/icon-note.gif) **说明：** 
>
>- K8s是容器编排平台，其工作节点需实际运行云手机业务，在部署K8s前或重启节点后，需确保工作节点完成视频流云手机的环境部署，完成环境部署的校验方式可启动一个视频流云手机验证。
>- K8s集群环境部署和部署镜像涉及从Docker镜像仓拉取镜像的操作，需确保部署的服务器网络环境能够从Docker镜像仓拉取镜像。

#### 1.3.2 搭建K8s集群<a name="ZH-CN_TOPIC_0000002549826297"></a>

##### 1.3.2.1 所有节点公共操作<a name="ZH-CN_TOPIC_0000002549706309"></a>

在所有master和工作节点下完成K8s集群软件安装、Containerd配置以及其他相关操作。

1. 修改hostname，保证每台服务器hostname不重复。

    例如：

    - 在master节点上将hostname修改为k8s-master。

        ```shell
        hostnamectl set-hostname k8s-master
        bash
        ```

    - 在工作节点上将hostname修改为k8s-slave1。

        ```shell
        hostnamectl set-hostname k8s-slave1
        bash
        ```

2. 将所有服务器密码修改为相同的密码。
3. 关闭防火墙。

    ```shell
    systemctl stop firewalld
    systemctl disable firewalld
    ```

4. 关闭交换分区。
    - 单次生效，执行如下命令。

        ```shell
        swapoff -a   
        ```

    - 永久生效，在“fstab”文件中注释swap自动挂载。

        ```shell
        sed -i "/\/dev\/mapper\/openeuler-swap/ s|^|#|" /etc/fstab
        ```

5. 配置安装K8s集群所需软件的源。

    ```shell
    touch /etc/yum.repos.d/kubernetes.repo
    cat >/etc/yum.repos.d/kubernetes.repo <<EOF
    [kubernetes]
    name=Kubernetes
    baseurl=https://pkgs.k8s.io/core:/stable:/v1.28/rpm/
    enabled=1
    gpgcheck=1
    gpgkey=https://pkgs.k8s.io/core:/stable:/v1.28/rpm/repodata/repomd.xml.key
    EOF
    ```

6. 安装K8s集群软件。

    ```shell
    yum install -y kubelet kubeadm kubectl kubernetes-cni --disableexcludes=kubernetes
    systemctl enable --now kubelet
    ```

7. 请参见[（可选）部署Containerd环境](#部署Containerd环境)的[1](#部署Containerd环境1)至[3](#部署Containerd环境3)安装Containerd和runc组件。在完成Containerd和runc的组件安装后，工作节点需要额外执行[6](#部署Containerd环境6)进行Docker服务的重启。
8. 修改Containerd配置。

    ```shell
    mkdir -p /etc/containerd/
    cd /etc/containerd/
    containerd config default > /etc/containerd/config.toml
    sed -i "s|SystemdCgroup =.*|SystemdCgroup = true|g" /etc/containerd/config.toml
    ```

9. 配置crictl，并重启containerd。

    ```shell
    echo "runtime-endpoint: unix:///run/containerd/containerd.sock" >> /etc/crictl.yaml
    echo "image-endpoint: unix:///run/containerd/containerd.sock" >> /etc/crictl.yaml
    echo "timeout: 10" >> /etc/crictl.yaml
    systemctl daemon-reload
    systemctl restart containerd
    ```

10. 配置网络转发。该步骤服务器重启后需重新执行。

    ```shell
    modprobe overlay
    modprobe br_netfilter
    echo "net.bridge.bridge-nf-call-ip6tables=1" >> /etc/sysctl.d/k8s.conf
    echo "net.bridge.bridge-nf-call-iptables=1" >> /etc/sysctl.d/k8s.conf
    echo "net.ipv4.ip_forward=1" >> /etc/sysctl.d/k8s.conf
    sysctl -p /etc/sysctl.d/k8s.conf
    ```

##### 1.3.2.2 master节点操作<a name="ZH-CN_TOPIC_0000002549706297"></a>

在master节点上初始化集群。

1. 下载必备镜像。

    ```shell
    kubeadm config images pull 
    ```

    此过程若无报错则下载成功。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >国内网络环境需要配置镜像仓，例如：
    >
    >```shell
    >kubeadm config images pull --image-repository registry.aliyuncs.com/google_containers
    >```

2. 修改containerd镜像配置，根据拉取的镜像中pause的版本更改config.toml的配置，查看pause镜像版本

    ```shell
    crictl images
    ```

    ​    **图 1** 镜像拉取信息<a name="fig1579095614545"></a><a id="镜像拉取信息"></a>
    ​    ![](figures/镜像拉取信息.png "镜像拉取信息")

    ​    以[**图 1** 镜像拉取信息](#镜像拉取信息) 镜像拉取信息](#fig1579095614545)中registry.aliyuncs.com/google_containers/pause:3.9为例：

        ```shell
        sed -i 's|sandbox_image =.*|sandbox_image = "registry.aliyuncs.com/google_containers/pause:3.9"|g' /etc/containerd/config.toml
        ```

3. 重启Containerd。

    ```shell
    systemctl restart containerd
    ```

4. 集群初始化。

    ```shell
    kubeadm init --pod-network-cidr=10.244.0.0/16
    ```

    初始化成功后有如[**图 2** 集群初始化成功打印信息](#集群初始化成功打印信息)所示信息打印。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >如果在下载镜像时配置了镜像仓，集群初始化也需要配置相同镜像仓，例如：
    >
    >```shell
    >kubeadm init --pod-network-cidr=10.244.0.0/16 --image-repository registry.aliyuncs.com/google_containers
    >```

    **图 2** 集群初始化成功打印信息<a name="fig1336104663519"></a><a id="集群初始化成功打印信息"></a>

    ![](figures/zh-cn_image_0000002518346460.png)

    需执行在[**图 1** 集群初始化成功打印信息](#集群初始化成功打印信息)中黄框信息命令配置集群，红框信息表示工作节点加入集群的token命令，请保存该段命令。

    ```shell
    rm -rf $HOME/.kube
    mkdir -p $HOME/.kube
    sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
    sudo chown $(id -u):$(id -g) $HOME/.kube/config
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >当在master节点上集群初始化失败后，需按照提示查找原因并进行重置，重置后重新执行初始化命令。重置命令如下。
    >
    >```shell
    >kubeadm reset
    >systemctl stop kubelet
    >rm -rf /var/lib/cni/
    >rm -rf /var/lib/kubelet/*
    >rm -rf /etc/cni/
    >ifconfig cni0 down
    >ifconfig flannel.1 down
    >ip link delete cni0
    >ip link delete flannel.1
    >```

5. 启动kube-flannel网络插件。

    请参见[视频流引擎](#视频流引擎)获取DemoVideoEngine.tar.gz软件包，获取后将软件包上传至服务器的“/home/k8s”目录。

    ```shell
    cd /home/k8s
    tar -xvf DemoVideoEngine.tar.gz
    cd /home/k8s/k8s/script
    kubectl apply -f kube-flannel.yml
    ```

6. 查看集群状态。

    1. 查看当前节点的状态。

        ```shell
        kubectl get nodes -A -o wide
        ```

        预期结果为此master节点的状态（STATUS）列是Ready，运行时（CONTAINER-RUNTIME）列是containerd://x.x.x。

    2. 查看pod状态。

        ```shell
        kubectl get pod -A -o wide
        ```

        预期结果为所有的pod的状态（STATUS）列都是Running。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >若查看当前节点的状态（STATUS）列是NotReady，及查看kubelet服务状态（systemctl status kubelet）时有明显报错（Network plugin returns error: cni plugin not initialized），此情况建议将集群重置并将服务器重启后重新初始化。

##### 1.3.2.3 工作节点操作<a name="ZH-CN_TOPIC_0000002518186532"></a>

将工作节点加入到集群中。

请参见[视频流引擎](#视频流引擎)获取DemoVideoEngine.tar.gz软件包，获取后将软件包上传至服务器的“/home/k8s”目录。

1. <a id="工作节点操作1"></a> 容器存储隔离和大小设置。该步骤服务器重启后需重新执行。

    ```shell
    cd /home/k8s
    tar -xvf DemoVideoEngine.tar.gz k8s/
    cd /home/k8s/k8s/DevicesPlugin
    chmod +x storage_manager.sh
    ./storage_manager.sh $ACTION $STORAGE_START_INDEX $STORAGE_END_INDEX $STORAGE_SIZE_GB $IMG_BASE
    ```

    命令参数说明如[**表 1** 容器存储隔离和大小设置参数说明](#容器存储隔离和大小设置参数说明)所示。

    **表 1** 容器存储隔离和大小设置参数说明<a id="容器存储隔离和大小设置参数说明"></a>

    |参数|说明|
    |--|--|
    |ACTION|参数值为create，fcreate或delete，普通创建、创建f2fs格式启动的容器或者删除。|
    |STORAGE_START_INDEX|数据卷删除或创建起始编号。|
    |STORAGE_END_INDEX|数据卷删除或创建结束编号，结束编号必须大于或者等于起始编号。|
    |STORAGE_SIZE_GB|存储大小，单位为GB。删除时可不传。|
    |IMG_BASE|基础数据卷img文件，若无基础数据卷可不传，基础数据卷img文件制作请参考。删除时可不传。参数STORAGE_SIZE_GB和IMG_BASE只传其中一个。|

    例如：

    - 创建100个存储大小为32GB的存储隔离数据卷，名称为video1~video100。

        ```shell
        ./storage_manager.sh create 1 100 32
        ```

    - 创建1个存储大小为32GB的存储隔离数据卷，名称为video1，并且容器内部以f2fs格式启动。

        ```shell
        ./storage_manager.sh fcreate 1 1 32
        ```

    - 如果在此基础上，要增加20个存储大小为32GB的存储隔离数据卷，名称为video101~video120。

        ```shell
        ./storage_manager.sh create 101 120 32
        ```

    - 删除名称为video1~video100数据卷。

        ```shell
        ./storage_manager.sh delete 1 100
        ```

    - 如果在此基础上，要删除名称为video101~video120这剩余20个数据卷。

        ```shell
        ./storage_manager.sh delete 101 120
        ```

    - 通过videobase.img为基础制作名为video1~video100的数据卷。

        ```shell
        ./storage_manager.sh create 1 100 /home/mount/img/videobase.img
        ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >若已执行该步骤命令，重新修改某个编号的数据卷存储大小时需先删除对应编号的数据卷再重新创建。
    
    >此处创建的数据卷的文件格式需要和'k8s-video.sh'拉起pod时的配置保持一致。例如：若通过'fcreate'为video1创建了f2fs格式的数据卷，那么使用启动脚本'k8s-video.sh'拉起video1的时候必须将f2fs开关设置为1。

2. 修改containerd镜像配置，根据master节点拉取的镜像中pause的版本更改config.toml的配置，以[**图 1** 镜像拉取信息](#镜像拉取信息) 镜像拉取信息](master节点操作.md#fig1579095614545)中registry.aliyuncs.com/google_containers/pause:3.9为例

    ```shell
    sed -i 's|sandbox_image =.*|sandbox_image = "registry.aliyuncs.com/google_containers/pause:3.9"|g' /etc/containerd/config.toml
    systemctl restart containerd
    ```

3. 执行在master初始化成功时保存的[**图 1** 集群初始化成功打印信息](#集群初始化成功打印信息)红框中加入集群的token命令。

    例如：

    ```shell
    kubeadm join xx.xx.xx.xx:xxxx --token 7h0hpd.1av4cdcb4fb0on5x \
    --discovery-token-ca-cert-hash sha256:357c6d1dbefe6f7adf3c80987a90d3765965b1c43e1757b655ea8586c8ade10a
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >- 工作节点重启后，重新加入集群时，需保证此工作节点可运行视频流云手机。
    >- xx.xx.xx.xx为IP地址，xxxx为映射端口号。
    >- 加入集群的**token**命令若失效可重新在master节点执行如下命令重新生成。
    >
    > ```shell
    > kubeadm token create --print-join-command
    > ```

4. 拷贝master节点的kube config文件到工作节点

    ```shell
    rm -rf $HOME/.kube
    mkdir -p $HOME/.kube
    sudo scp root@xxx.xxx.xxx.xxx:$HOME/.kube/config $HOME/.kube/config
    sudo chown $(id -u):$(id -g) $HOME/.kube/config
    ```

5. 查看集群状态。
    1. 需在master节点查看状态。

        ```shell
        kubectl get nodes -A -o wide
        ```

        预期结果为此工作节点的状态（STATUS）列是Ready，运行时（CONTAINER-RUNTIME）列是containerd://x.x.x。

    2. 需在master节点查看pod状态。

        ```shell
        kubectl get pod -A -o wide
        ```

        预期结果为此工作节点上的pod的状态（STATUS）列都是Running。

    3. 在此工作节点查看容器状态。

        ```shell
        crictl ps
        ```

        预期结果为所有的容器状态（STATE）列都是Running。

6. （可选）配置NUMA亲和。
    1. 编译环境配置和插件时需要保证Golang版本1.23或以上，将新的1.23版本的Golang go目录放至“/usr/lib”下，将“go/bin/go”和“go/bin/gofmt”放至“/usr/bin”下。

        ```shell
        systemctl stop kubeletexport GOROOT=/usr/lib/go
        go env -w GO111MODULE=on
        go env -w GOPROXY=https://goproxy.io,direct
        ```

    2. 请参见[视频流引擎](#视频流引擎)获取K8s NUMA亲和插件软件包topo-affinity-plugin-master.zip，获取后将软件包上传至服务器的“/home/k8s”目录。
    3. 解压topo-affinity-plugin-master.zip，进入软件包目录并编译插件。

        ```shell
        unzip topo-affinity-plugin-master.zip
        cd topo-affinity-plugin-master
        go mod tidy
        make build
        ```

        构建完成后，请确认在“bin”目录下生成“kunpeng-tap”二进制文件。

    4. 安装Containerd（容器运行时）的版本。

        ```shell
        make install-service-containerd
        ```

        如果需要修改启动参数，则在源代码目录下的“hack/kunpeng-tap.service.containerd”文件的“ExecStart=”下进行修改，用户可根据需求修改相关参数后启动。参数说明请参见[**表 2** 启动参数说明](#启动参数说明)。

        ```shell
        [Unit]
        Description=Kunpeng Topology-Affinity Plugin Service
        After=network.target
        
        [Service]
        ExecStart=/usr/local/bin/kunpeng-tap --runtime-proxy-endpoint="/var/run/kunpeng/tap-runtime-proxy.sock" \
            --container-runtime-service-endpoint="/var/run/containerd/containerd.sock" --container-runtime-mode="Containerd" \
            --resource-policy="topology-aware" --v=2
        Restart=always
        RestartSec=5
        
        [Install]
        WantedBy=multi-user.target
        ```

        **表 2** 启动参数说明<a id="启动参数说明"></a>

        |参数名称|参数描述|默认值|说明|
        |--|--|--|--|
        |container-runtime-mode|插件对接的容器运行时，对应集群运行时设置Docker或Containerd。|Containerd|依照K8s集群使用的容器运行时决定。|
        |resource-policy|容器资源的优化策略，目前支持numa-aware和topology-aware。numa-aware策略支持Burstable类型容器进行CPU的NUMA亲和。topology-aware策略提供Socket、Die、NUMA等拓扑层次的CPU亲和，支持内存、GPU的优化配置。|topology-aware|依照需求进行选择。|
        |v|日志信息等级，调整范围2至5。|2|等级越高，日志输出越详细。|

    5. 启动TAP服务。

        ```shell
        make start-service
        ```

        启动成功后回显信息中输出的Status为active。

    6. 修改并重启Kubelet。
        1. 重启前先需确保该节点上未部署容器。
        2. 修改kubelet参数配置文件“/var/lib/kubelet/kubeadm-flags.env”。

            初始配置内容如下：

            ```shell
            KUBELET_KUBEADM_ARGS="... --container-runtime=remote --container-runtime-endpoint=unix:///var/run/containerd/containerd.sock ..."
            ```

            修改为如下内容：

            ```shell
            KUBELET_KUBEADM_ARGS="... --container-runtime=remote --container-runtime-endpoint=unix:///var/run/kunpeng/tap-runtime-proxy.sock ..."
            ```

    7. 重新启动kubelet并查看，并查看是否重启成功。

        ```shell
        systemctl daemon-reload
        systemctl restart kubelet
        systemctl status kubelet
        ```

        >![](public_sys-resources/icon-note.gif) **说明：** 
        >卸载TAP插件步骤：
        >- “/var/lib/kubelet/kubeadm-flags.env”文件为初始配置内容并重启kubelet。
        >
        > ```shell
        > systemctl daemon-reload
        > systemctl restart kubelet
        > systemctl status kubelet
        > ```
        >
        >- 进入“topology-affinity-plugin”源码目录，并执行插件卸载命令。
        >
        > ```shell
        > cd /home/k8s/topo-affinity-plugin-master
        > make uninstall-service
        > ```

#### 1.3.3 部署镜像<a name="ZH-CN_TOPIC_0000002518346452"></a>

##### 1.3.3.1 部署道客设备插件镜像<a name="部署道客设备插件镜像"></a>

在所有工作节点完成部署道客设备插件镜像的操作。

道客设备插件由道客提供，本文档配套v0.0.5版本。请先获取相关的安装文档和软件包，并按照文档完成道客设备插件的部署。

1. 请参见《[Kbox云手机容器 安装指南](https://gitcode.com/boostkit/Kbox-patches/blob/AOSP11/docs/zh/install_guide.md)》中软件部署的“环境准备”章节获取显卡驱动VAGPU-25.03.01.01-RC20.tgz软件包。解压获取k8s-v0.0.5-1.tar.gz压缩包。
2. 解压k8s-v0.0.5-1.tar.gz获取相关的安装文档和软件包。
3. 请参见《DC1000加速卡Va Docker安装指南  01.pdf》中第四章（安装Va Docker）安装Va Docker。
4. 请参见《DC1000加速卡Kubernetes设备插件安装指南 03.pdf》中第三章（安装部署）安装设备插件。

>![](public_sys-resources/icon-note.gif) **说明：** 
>道客设备插件版本支持v0.0.5版本及以上。

##### 1.3.3.2 部署设备插件镜像<a name="ZH-CN_TOPIC_0000002518186506"></a>

在所有工作节点完成部署设备插件镜像的操作。

1. 安装golang，版本需在1.17以上。

    ```shell
    yum install golang
    ```

2. 下载device-plugin的代码并切换到指定commitid。

    ```shell
    git clone https://github.com/everpeace/k8s-host-device-plugin.git
    cd  k8s-host-device-plugin
    git checkout 15e0a180dd4fbea7ea09b563b9e0713d3b90579a
    ```

3. 合入device-plugin.patch。

    将device-plugin.patch（此文件位于DemoVideoEngine.tar.gz中的“k8s/DevicesPlugin”文件夹下）拷贝到“k8s-host-device-plugin”目录。

    ```shell
    cd k8s-host-device-plugin
    patch -p1 < device-plugin.patch
    ```

4. 编译device-plugin，修改go语言的镜像仓库地址。

    ```shell
    export GOPROXY=https://goproxy.cn
    go build
    ```

5. 制作镜像。

    ```shell
    docker build -f Dockerfile  -t k8s-hostdev-plugin:0.1 .
    docker save k8s-hostdev-plugin:0.1 -o k8s-hostdev-plugin.tar
    ```

6. 导入镜像。

    将k8s-hostdev-plugin.tar拷贝到所有工作节点，然后导入镜像。

    ```shell
    ctr -n k8s.io images import k8s-hostdev-plugin.tar
    ```

##### 1.3.3.3 部署input设备权限写入插件镜像<a name="ZH-CN_TOPIC_0000002549706311"></a>

在所有工作节点完成部署input设备权限写入插件镜像的操作。

因为视频流云手机启动后，会通过uinput内核模块生成一个虚拟输入设备，所以生成的虚拟输入设备在容器中访问需配置对应的权限，input设备权限写入插件主要完成容器创建后，自动写入虚拟输入设备的权限。

1. 制作input设备权限写入插件镜像并导入。

    ```shell
    cd /home/k8s/k8s/InputPermission
    ./make_image.sh
    ```

    make_image.sh中包含了镜像制作和导入的步骤。

2. 将input-device-permission.tar拷贝到其他工作节点并完成导入。

    ```shell
    ctr -n k8s.io images import input-device-permission.tar
    ```

1.3.3.4 部署视频流镜像<a name="ZH-CN_TOPIC_0000002518186530" id="部署视频流镜像"></a>

选择一台工作节点机器进行镜像制作，然后在所有工作节点导入并完成部署视频流镜像操作。

1. 将DemoVideoEngine.tar.gz软件包放在指定目录下，假设DemoVideoEngine.tar.gz已经放在“/home/k8s”目录下。

    ```shell
    mkdir -p /home/k8s/tmp 
    cd /home/k8s/tmp 
    tar -xvf  ../DemoVideoEngine.tar.gz
    ```

2. <a id="部署视频流镜像2"></a>修改编码器类型，重新制作DemoVideoEngine.tar.gz软件包。

    “default.prop”文件中设置编码器默认类型为“1”，而道客需要使用编码器类型为“2”，故解压修改后需要重新打包。此外“default.prop”文件还可以修改帧率等设置信息，设置完成后需要重新制作镜像。需保证制作后的镜像通过Docker方式可正常运行云手机。

    1. 打开“default.prop”文件。

        ```shell
        vi vendor/default.prop
        ```

    2. 按“i”键进入编辑模式，修改文件中“vmi.video.encodertype”值为“2”，“vmi.video.encode.rcmode”值为“2”或者“3”。
    3. 按“Esc”键，输入**:wq!**并按“Enter”键保存并退出编辑。
    4. 重新制作DemoVideoEngine.tar.gz软件包。

        ```shell
        tar -zcvf DemoVideoEngine.tar.gz  *
        ```

3. 使用[2](#部署视频流镜像2)制作的DemoVideoEngine.tar.gz，请参见[制作镜像](#制作镜像)重新制作视频流镜像。例如：制作出的镜像名为video:version。
4. 使用**docker**导出视频流镜像。

    ```shell
    docker save video:version -o video.tar
    ```

5. 将视频流镜像拷贝到所有工作节点并导入。

    ```shell
    ctr -n k8s.io images import video.tar
    ```

    **crictl images**命令可查看镜像名称和tag，例如：镜像名为docker.io/library/video:version。

## 2 虚拟机环境部署<a name="ZH-CN_TOPIC_0000002518304958"></a>

### 2.1 环境要求<a name="ZH-CN_TOPIC_0000002549944725"></a>

建议在鲲鹏920 7280Z处理器上openEuler 22.03 LTS SP4操作系统中搭建视频流引擎的虚拟机环境，环境搭建前请确保您的硬件环境满足要求。

**硬件要求<a name="section217mcpsimp"></a>**

硬件要求如[**表 1** Kbox安卓容器环境部署硬件环境要求](#Kbox安卓容器环境部署硬件环境要求) Kbox安卓容器环境部署硬件环境要求](#table220mcpsimp)所示，硬件配置及参数如[**表 2** 鲲鹏服务器配置及参数](#鲲鹏服务器配置及参数) 鲲鹏服务器配置及参数](#table242mcpsimp)所示。

**表 1** Kbox安卓容器环境部署硬件环境要求<a id="Kbox安卓容器环境部署硬件环境要求"></a>

|序号|设备型号|用途|
|--|--|--|
|1|鲲鹏920 7280Z处理器|用于构建视频流容器运行的虚拟机环境，运行视频流云手机|

**表 2** 鲲鹏服务器配置及参数<a id="鲲鹏服务器配置及参数"></a>

|配置项|参数|
|--|--|
|CPU|2\*鲲鹏920 7280Z处理器，80 <Core@2.9GHz>|
|内存|16\*DDR5 DIMM内存-64GB-4800MT/s|
|硬盘系统盘|ES3600C V5固态硬盘-6400GB-NVMe SSD|
|硬盘数据盘|ES3600C V5固态硬盘-6400GB-NVMe SSD|
|网卡|1\*（4\*GE接口卡， 1\*5902L板载灵活网卡|
|Riser卡|1* 16X SLOT(PCIe X16) + 2\*8X SLOT(PCIe X8)-RISER1&2模组， 2\*8X SLOT(PCIe X8)-后置Riser|
|GPU|4\*DC1000|
|操作系统|openEuler 22.03LTS SP4|
|系统/内核版本|5.10.0-216.0.0|

**表 3** 虚拟机规格<a id="虚拟机规格"></a>

|虚拟机总个数|CPU个数（单个虚拟机）|内存（单个虚拟机）|磁盘大小（单个虚拟机）|
|--|--|--|--|
|4|80|180GiB|512GiB|

>![](public_sys-resources/icon-note.gif) **说明：** 
>上述虚拟机内存以及磁盘容量仅作为演示，具体容量根据实际情况分配。

**操作系统要求<a name="section305mcpsimp"></a>**

宿主机/虚拟机操作系统要求如[**表 4** 宿主机操作系统要求](#宿主机操作系统要求) 宿主机操作系统要求](#table308mcpsimp)、[**表 5** 虚拟机操作系统要求](#虚拟机操作系统要求) 虚拟机操作系统要求](#table339mcpsimp)所示。

**表 4** 宿主机操作系统要求<a id="宿主机操作系统要求"></a>

|项目|版本|下载地址|
|--|--|--|
|openEuler|22.03 LTS SP4|[获取链接](https://www.openeuler.openatom.cn/zh/download/archive/detail/?version=openEuler%2022.03%20LTS%20SP4)|
|Kernel|基于5.10.0-216.0.0|[获取链接](https://gitee.com/openeuler/kernel/repository/archive/5.10.0-216.0.0.zip)|

**表 5** 虚拟机操作系统要求<a id="虚拟机操作系统要求"></a>

|项目|版本|下载地址|
|--|--|--|
|openEuler|22.03 LTS SP4|[获取链接](https://www.openeuler.openatom.cn/zh/download/archive/detail/?version=openEuler%2022.03%20LTS%20SP4)|
|Kernel|基于5.10.0-216.0.0|请参见《Kbox云手机容器 特性指南》“软件部署”中的“编译内核”章节进行编译。|

**获取虚拟机软件包<a name="section1543425619147" id="获取虚拟机软件包"></a>**

虚拟机部署所需的补丁和脚本文件如[**表 6** 虚拟机部署所需文件获取方式](#虚拟机部署所需文件获取方式) 虚拟机部署所需文件获取方式](#table10743166141716)所示。

**表 6** 虚拟机部署所需文件获取方式<a id="虚拟机部署所需文件获取方式"></a>

|软件包|文件|文件路径|获取地址|
|--|--|--|--|
|Kbox-AOSP11.zip|虚拟机内核补丁|Kbox-AOSP11/deploy_scripts/vm_deploy/patchForKernel/general.patch|[获取链接](https://mirrors.huaweicloud.com/kunpeng/archive/kunpeng_solution/ARMNative/BoostKit25.1.RC1_Demo/Kbox_Demo/Kbox-AOSP11.zip)|
|Kbox-AOSP11.zip|虚拟机调优脚本|Kbox-AOSP11/deploy_scripts/vm_deploy/setup_vm.sh|[获取链接](https://mirrors.huaweicloud.com/kunpeng/archive/kunpeng_solution/ARMNative/BoostKit25.1.RC1_Demo/Kbox_Demo/Kbox-AOSP11.zip)|

### 2.2 宿主机环境配置<a name="ZH-CN_TOPIC_0000002550064713" id="宿主机环境配置"></a>

#### 2.2.1 修改BIOS配置<a name="ZH-CN_TOPIC_0000002518464882"></a>

通过在宿主机中修改BIOS相关配置选项，使宿主机达到部署虚拟机环境的最优条件。

在部署虚拟机环境之前，需要在宿主机中修改如[**表 1** BIOS配置项说明](#BIOS配置项说明)所示的BIOS选项配置。

**表 1** BIOS配置项说明<a id="BIOS配置项说明"></a>

|配置项|配置路径|取值|
|--|--|--|
|SMMU选项|Advanced > MISC Configuration > Support Smmu|Enabled|
|CPU超线程|Advanced > Power And Performance Configuration > CPU PM Control > SMT2|Enabled|
|性能策略|Advanced > Power And Performance Configuration > Power Policy|Performance|
|GICv4.1|Advanced >Processor Configuration >GIC Version|4.1|

#### 2.2.2 修改内核模块<a name="ZH-CN_TOPIC_0000002518304960"></a>

使用DC1000 GPU硬件环境时，在虚拟机内安装驱动需要对宿主机内核做适配，请提前获取内核源码。

1. 请参见[**表 4** 宿主机操作系统要求](#宿主机操作系统要求)获取内核源码。
2. 解压内核源码并进入根目录。

    ```shell
    unzip 5.10.0-216.0.0.zip
    cd 5.10.0-216.0.0
    ```

3. 请参见[获取虚拟机软件包](#获取虚拟机软件包)，获取内核patch文件general.patch。
4. 在内核源码目录“5.10.0-216.0.0”下，合入patch。

    ```shell
    patch -p1 < general.patch
    ```

5. 生成.config文件到源码目录。

    ```shell
    cp /boot/config-5.10.0-216.0.0.115.oe2203sp4.aarch64 .config
    make menuconfig
    ```

6. 执行命令后，在出现的界面中选择“Load”，如图所示。

    ![](figures/1_zh-cn_image_0000002085094168.png)

7. 出现如图所示的界面时，选择“OK”。

    ![](figures/1_zh-cn_image_0000002120534141.png)

8. 在内核配置界面中，配置如[**表 1** 内核编译选项配置说明](#内核编译选项配置说明)所示的内核编译选项。

    **表 1** 内核编译选项配置说明<a id="内核编译选项配置说明"></a>

    |配置项|配置要求|
    |--|--|
    |LOCALVERSION|-patched-vm|
    |DEBUG_INFO_BTF|N|

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >配置方法说明：
    >- “/”用于搜索。
    >- “Y”将选中项编译进内核，对应项显示为：\[\*\]。
    >- “N”将选中项排除，对应项显示为：\[\]。
    >- “M”键将选中的项编译成模块（编译成ko的形式），对应项显示为：<M\>。
    >- “Enter”编辑选中项内容。
    >- 数字选择搜索结果。
    >- 修改完成后单击最下方<Save\>保存修改。
    >- 保存后单击最下方<Exit\>选项退出。

9. 安装依赖并启用LXCFS服务。若命令分多行，需要在行末加上“\\”符号。

    ```shell
    yum install -y dwarves dpkg dpkg-devel openssl openssl-devel ncurses ncurses-devel bison flex bc libdrm build elfutils-libelf-devel docker lxc lxcfs lxcfs-tools git tar patch make gcc
    systemctl start lxcfs
    systemctl enable lxcfs
    ```

    ![](figures/zh-cn_image_0000002550068121.png)

10. 编译内核代码。

    ```shell
    make -j72
    ```

11. 安装新内核。

    ```shell
    make modules_install 
    make install
    ```

#### 2.2.3 修改grub配置<a name="ZH-CN_TOPIC_0000002549944727"></a>

修改grub配置文件，在宿主机中使能内存大页，提高宿主机的内存管理效率和性能。使能iommu passthrough，提高访存效率。使能GICv4.1，直通中断降低CPU损耗和时延以及提升虚拟硬盘IO性能。

1. 在宿主机系统中，编辑grub配置文件。

    ```shell
    vim /etc/default/grub
    ```

2. 按“i”进入编辑模式，在GRUB_CMDLINE_LINUX后添加如下所示配置。

    ```shell
    "default_hugepagesz=1G hugepagesz=1G hugepages=800 pci=realloc transparent_hugepage=never iommu.passthrough=1 arm64.nopauth kvm-arm.vgic_v4_enable=1 kvm-arm.virt_msi_bypass=1 irqchip.gicv3_rsv_buses_start=30 irqchip.gicv3_rsv_buses_count=10"
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >内存大页配置参考：hugepages = 当前总内存\*0.8（向下取整），在本环境下设置为1TB \*0.8 = 800GB。

3. 按“Esc”键退出编辑模式，输入**:wq!**并按“Enter”键保存并退出文件。
4. 更新grub配置文件。

    ```shell
    grub2-mkconfig -o /boot/efi/EFI/openEuler/grub.cfg
    ```

5. 设置启动内核。

    ```shell
    grub2-set-default 'openEuler (5.10.0-patched-vm) 22.03 (LTS-SP4)'
    ```

6. 重启服务器。

    ```shell
    reboot
    ```

7. 重启完毕后检查内核是否切换为“5.10.0-patched-vm”。

    ```shell
    uname -r
    ```

8. <a id="修改grub配置8"></a> 查看内存大页是否成功配置。

    ```shell
    cat /sys/devices/system/node/node*/meminfo | grep Huge
    ```

    回显如下图所示，“Node  _x_  HugePages_Total”字段总和为“800”则为配置成功。

    ![](figures/1_zh-cn_image_0000002084933924.png)

9. 查看iommu passthrough是否成功配置。

    ```shell
    dmesg | grep iommu | head -n 10
    ```

    回显信息中显示“iommu: Default domain type: Passthrough”则配置成功。

    ![](figures/zh-cn_image_0000002518468278.png)

10. 查看GICv4.1是否成功配置。

    ```shell
    dmesg | grep GICv4.1
    ```

    回显信息中显示“GICv4.1 support enabled”则配置成功。

    ![](figures/zh-cn_image_0000002549948129.png)

11. 虚拟机启动后，执行下述指令。

    ```shell
    dmesg | grep "Create shadow device"
    ```

    回显信息中显示“Create shadow device”字段，则GICv4.1配置成功。

    ![](figures/zh-cn_image_0000002550068123.png)

#### 2.2.4 安装虚拟机相关依赖<a name="ZH-CN_TOPIC_0000002550064715"></a>

在搭建虚拟机环境的宿主机中安装虚拟机环境所需的依赖。

1. 安装libvirt和virt-manager及相关依赖。

    ```shell
    yum install libvirt virt-manager edk2-aarch64 sshpass mesa-libGLES-devel mesa-dri-drivers virt-install -y
    ```

2. 安装x11 server用于支持virt-manager图形化管理界面。

    ```shell
    yum install xorg-x11-server
    ```

3. 打开sshd配置文件。

    ```shell
    vi /etc/ssh/sshd_config
    ```

4. 按“i”进入编辑模式，将“X11Forwarding”字段设置为“yes”。
5. 按“Esc”键退出编辑模式，输入**:wq!**并按“Enter”键保存并退出文件。
6. 重启sshd服务。

    ```shell
    systemctl restart sshd
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >若后续**virt-manager**指令报错则需要重新启动一个SSH终端界面。

#### 2.2.5 查询GPU卡PCIe节点信息<a name="ZH-CN_TOPIC_0000002518464884" id="查询GPU卡PCIe节点信息"></a>

鲲鹏920 7280Z处理器有4个NUMA，总共会创建4个虚拟机，所利用的资源分别对应宿主机的4个NUMA。因每个NUMA上都会有两张GPU卡，为避免产生跨NUMA访问而造成性能损失，在创建虚拟机前，需要确认每个虚拟机使用GPU卡的PCIe节点，用于设备的添加。

1. <a name="查询GPU卡PCIe节点信息1"></a>确认瀚博GPU卡所有PCIe节点的ID。

    ```shell
    lspci | grep 0200
    ```

    ![](figures/zh-cn_image_0000002518308362.png)

2. 确认所对应的NUMA，此返回顺序符合[1](#查询GPU卡PCIe节点信息1)中的ID顺序，即可确定每个GPU卡节点所对应的NUMA ID。下图所示回显信息仅为示例，如17:00.0~18:00.3（即前八个节点）对应宿主机的NUMA 1。

    ```shell
    lspci -vvv -d 1f4f:0200 | grep NUMA
    ```

    ![](figures/zh-cn_image_0000002518468280.png)

#### 2.2.6 配置宿主机网络<a name="ZH-CN_TOPIC_0000002518304962" id="配置宿主机网络"></a>

创建宿主机网络设备，支撑后续虚拟机网络配置。

1. <a id="配置宿主机网络1"></a>查看宿主机使用的网卡。

    ```shell
    ip a
    ```

    ![](figures/zh-cn_image_0000002549948131.png)

2. 查看该网卡的PCI节点。

    ```shell
    lshw -c network -businfo
    ```

    ![](figures/zh-cn_image_0000002550068125.png)

3. <a id="配置宿主机网络3"></a>查看该网卡最多支持的VF网卡数量。

    ```shell
    cat /sys/bus/pci/devices/0000:75:00.0/sriov_totalvfs
    ```

    ![](figures/zh-cn_image_0000002518308364.png)

    - 如果该步骤执行成功，说明设备支持SR-IOV虚拟网卡直通方案。回显为7表示可以生成7个虚拟网卡，最多可以支撑7个虚拟机使用虚拟网卡。
    - 如果该步骤执行失败，或者回显的虚拟网卡数小于准备部署的虚拟机数量，则可以选择第二个方案网桥模式。网桥模式会带来额外的计算性能损耗以及时延。网桥模式配置详情请查看[6](#配置宿主机网络6)~[7](#配置宿主机网络7)。

4. 生成VF虚拟网卡。

    ```shell
    echo 4 > /sys/bus/pci/devices/0000:75:00.0/sriov_numvfs
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >每次服务器重启都需要重新执行该步骤，建议将其配置在“~/.bashrc”等文件中，确保每次重启后都会自动执行。

5. <a id="配置宿主机网络5"></a>查看生成的VF节点。

    ```shell
    lshw -c network -businfo
    ```

    回显如下图所示，新生成了4个虚拟网卡，表示操作成功。

    ![](figures/zh-cn_image_0000002518468282.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >[1](#配置宿主机网络1)~[5](#配置宿主机网络5)已经完成了SR-IOV虚拟网卡方案中，虚拟网卡的生成。后续步骤可以跳过。
    >如果设备不支持SR-IOV，考虑使用下面的网桥方案。
    >如果[3](#配置宿主机网络3)中网卡最多支持的VF网卡数量回显小于4。例如为2，那么考虑2个虚拟机使用SR-IOV方案，2个虚拟机使用网桥方案。

6. <a id="配置宿主机网络6"></a>查看当前网卡配置文件并备份。

    ```shell
    cd /etc/sysconfig/network-scripts/
    cp ifcfg-eno5 ifcfg-eno5.bak
    ```

7. <a id="配置宿主机网络7"></a>新建网桥配置文件“ifcfg-br0”并修改网卡配置文件。

    将网卡配置文件的IPADDR，NETMASK，GATEWAY，DNS全部移植到网桥配置文件中。

    ```shell
    touch ifcfg-br0
    cat >ifcfg-br0 <<EOF
    TYPE=Bridge
    NAME=br0
    DEVICE=br0
    ONBOOT=yes
    BOOTPROTO=static
    IPADDR=XX.XX.XX.XX
    NETMASK=XX.XX.XX.XX
    GATEWAY=XX.XX.XX.XX
    DNS1=XX.XX.XX.XX
    EOF
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >1. 网桥配置文件中的IPADDR，NETMASK，GATEWAY，DNS根据宿主机网卡配置修改。
    >2. 宿主机网卡配置文件中删除IPADDR，NETMASK，GATEWAY，DNS配置并在最后新增配置“BRIDGE=br0”。
    > ![](figures/zh-cn_image_0000002549948133.png)

8. 重启libvirtd和NetworkManager服务并重启服务器。

    ```shell
    systemctl restart libvirtd
    systemctl restart NetworkManager
    reboot
    ```

9. 查看br0网桥是否成功创建。

    ```shell
    ip a
    ```

    出现如下配置说明网桥创建成功。

    ![](figures/zh-cn_image_0000002550068129.png)

### 2.3 虚拟机配置<a name="ZH-CN_TOPIC_0000002549944729" id="虚拟机配置"></a>

#### 2.3.1 使用virt-manager创建虚拟机<a name="ZH-CN_TOPIC_0000002550064717"></a>

总共需要创建4个虚拟机，需要顺序操作执行4次该章节的操作步骤。或者先执行1次，后续通过虚拟机拷贝操作步骤创建虚拟机。

虚拟机拷贝操作步骤请参见[虚拟机拷贝](#虚拟机拷贝)。

1. 打开virt-manager，选择红框所示按钮打开虚拟机配置界面。

    ```shell
    virt-manager
    ```

    ![](figures/1_zh-cn_image_0000002120521137.png)

2. 选择“Local install media \(ISO image or COROM\)”后单击“Forward”。

    ![](figures/1_zh-cn_image_0000002120521229.png)

3. 单击“Browse”，选择提前下载好的openEuler 22.03 LTS SP4镜像，取消勾选下面的“Automatically detect from installation media / source”，填写“Generic default”，最后单击“Forward”。

    ![](figures/zh-cn_image_0000002549948137.png)

4. <a id="使用virt-manager创建虚拟机4"></a>“Memory”分配额度填写“180000”，“CPUs”处填写“80”，然后单击“Forward”。

    ![](figures/1_zh-cn_image_0000002085084878.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >内存180000仅供参考。建议根据[2.2.3-8](#修改grub配置8)中内存大页分配情况，将NUMA分配的内存大页全部分配到对应的虚拟机中。

5. 磁盘处分配512GiB，然后单击“Forward”。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >此处的512GiB仅作为示例，请根据实际情况分配磁盘空间。如果后续准备直通磁盘分区作为数据盘以提升虚拟机的IO性能，此处可以分配少量空间比如50GiB，将该磁盘仅作为系统盘。

    ![](figures/1_zh-cn_image_0000002120684817.png)

    如果当前磁盘没有足够的空间，可以在其他盘创建虚拟硬盘文件。操作步骤如下所示：

    1. 创建磁盘镜像文件vm0.qcow2。

        ```shell
        qemu-img create -f qcow2 vm0.qcow2 1024G
        ```

    2. 勾选“Select orcreate custom storage”，单击“Manage”。

        ![](figures/zh-cn_image_0000002518468288.png)

    3. 单击“Browse Local”。

        ![](figures/zh-cn_image_0000002549948139.png)

    4. 选择刚刚创建的vm0.qcow2镜像文件。

        ![](figures/zh-cn_image_0000002550068133.png)

6. 名称分别起名为vm_X_（X = 0,1,2,3） ，同时需要勾选“Customize configuration before install”，如果[2.2.6-配置宿主机网络](#配置宿主机网络)中宿主机配置了网桥模式，此处Network selection应选择Bridge br0，然后单击“Finish”。

    ![](figures/zh-cn_image_0000002518308370.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >如果[2.2.6-配置宿主机网络](#配置宿主机网络)选择了SR-IOV虚拟网卡直通配置，则无需考虑Network selection，后续会删除这些虚拟网络接口。

7. 进入界面后单击下方“Add Hardware”依次添加以下相关设备。

    ![](figures/zh-cn_image_0000002518468290.png)

    1. 添加外设1：“Add Hardware \> Input \> Generic USB Keyboard \> Finish”
    2. 添加外设2：“Add Hardware \> Input \> Virtio Tablet \> Finish”
    3. 添加GPU卡PCIe设备（每个虚拟机需要2张GPU卡，因此依次添加8个节点） :  “Add Hardware \> PCI Host Device  \> 选择对应节点 \> Finish”。GPU节点和[2.3.2-2](#虚拟机配置调优2)中的NUMA节点需与[查询GPU卡PCIe节点信息](#查询GPU卡PCIe节点信息)对应选择。
    4. 如果[2.2.6-配置宿主机网络](#配置宿主机网络)中采用了SR-IOV方案，添加虚拟网卡到虚拟机：“Add Hardware \> PCI Host Device  \> 选择对应节点 \> Finish”。网卡节点请参见[2.2.6-5](#配置宿主机网络5)回显信息。

        ![](figures/zh-cn_image_0000002549948141.png)

    5. 如果默认没有“Display VNC”设备，则需手动添加。单击“Add Hardware \> Graphics”，Type选择“VNC server”，然后单击“Finish”添加该设备。

8. 单击“Begin Installation”，在弹出界面选择第一个选项，开始进行系统的安装。
9. 在安装主界面选择安装目的地。

    ![](figures/zh-cn_image_0000002550068135.png)

10. 在安装目标位置界面选择自定义“Custom”后左上角单击“Done”。

    ![](figures/1_zh-cn_image_0000002084928358.png)

11. 选择分区方式为“Standard Partition”，即标准分区。

    ![](figures/1_zh-cn_image_0000002084507570.png)

12. 单击“+”号后添加对应挂载点，如[**表 1** 分区对应挂载点](#分区对应挂载点)所示。

    ![](figures/1_zh-cn_image_0000002084931158.png)

    **表 1** 分区对应挂载点<a id="分区对应挂载点"></a>

    |挂载点|容量大小|
    |--|--|
    |/boot|2G|
    |/boot/efi|2G|
    |/swap|32G|

    分完以上3个以后直接添加即可，剩余空间会分配给根目录。完成后单击“Done”，在弹出的窗口中选择“接受更改”。

13. 在开始安装前查看服务器当前空闲的内存。

    ```shell
    free -h
    ```

    如果空闲内存小于[4](#使用virt-manager创建虚拟机4)中设置的虚拟机内存，则提前执行[2.3.2-3](#虚拟机配置调优3)和[2.3.2-5](#虚拟机配置调优5)，请参见[获取虚拟机软件包](#获取虚拟机软件包)获取虚拟机调优脚本setup_vm.sh，使能虚拟机内存大页。如果空闲内存足够，可以跳过该步骤。

    ```shell
    ./setup_vm.sh vm0 --numatune {绑定NUMA}
    ./setup_vm.sh vm0 --enable_hugepages
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >如果没有使能虚拟机内存大页，虚拟机默认会使用宿主机的空闲内存，如果宿主机系统空闲内存不足，则虚拟机系统会安装失败。此时就需要提前将内存大页分配给虚拟机，让其直接使用内存大页安装系统。

14. 配置root账号和密码，完成后单击“Begin Installation”开始安装。

    ![](figures/zh-cn_image_0000002550068137.png)

15. <a id="使用virt-manager创建虚拟机15"></a>安装完成后，重新打开virt-manager，关闭虚拟机。

    ![](figures/zh-cn_image_0000002518308374.png)

16. （可选）直通磁盘分区进入虚拟机内部作为数据盘。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >直通磁盘分区可以提升虚拟机磁盘的IO性能，推荐在磁盘IO密集场景比如说高密度云游戏场景使能该特性。

    1. 查看宿主机磁盘分区信息。

        ```shell
        lsblk
        ```

        选择一个分区作为虚拟机的数据盘，此处以nvme0n1p7为例。

        ![](figures/zh-cn_image_0000002518468294.png)

    2. 打开virt-manager，单击“Add Hardware \> Manage”。将“Bus type”配置为“VirtIO”，“Cache mode”配置为“none”，“IO mode”配置为“native”。

        ![](figures/zh-cn_image_0000002549948145.png)

    3. 单击“Browse Local \> dev \> nvme0n1p7 \> open”。

        ![](figures/zh-cn_image_0000002550068139.png)

    4. 单击“Finish”。

        ![](figures/zh-cn_image_0000002518308380.png)

17. 如果采用了SR-IOV虚拟网卡直通方案，则需要删除多余的虚拟网络接口。

    ![](figures/zh-cn_image_0000002518468298.png)

18. 删除虚拟显卡。

    ![](figures/1_zh-cn_image_0000002084932842.png)

    ![](figures/zh-cn_image_0000002550068143.png)

#### 2.3.2 虚拟机配置调优<a name="ZH-CN_TOPIC_0000002518464886" id="虚拟机配置调优"></a>

虚拟机配置调优需要根据GPU和NUMA的对应关系进行。

**此小节4个虚拟机均需执行，本章节根据GPU与NUMA的对应关系示例vm0对应NUMA 1进行修改。**请参见[查询GPU卡PCIe节点信息](#查询GPU卡PCIe节点信息)查询GPU与NUMA的对应关系，请参见[获取虚拟机软件包](#获取虚拟机软件包)获取虚拟机调优脚本。

1. 编辑虚拟机xml文件。

    ```shell
    virsh edit vm0
    ```

2. <a id="虚拟机配置调优2"></a> 按“i”进入编辑模式，在&lt;/cputune&gt;中添加如下图所示文本。设置虚拟机vCPU与宿主机的CPU映射关系。

    “cpuset”的值为绑定的宿主机CPU核ID，其中NUMA 0为0-79，NUMA 1为80-159，NUMA 2为160-239，NUMA 3为240-319。

    ![](figures/zh-cn_image_0000002518308382.png)

    ![](figures/zh-cn_image_0000002518468300.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >上述操作也可使用如下命令完成设置。
    >
    >```shell
    >./setup_vm.sh vm0 --cputune 80,159
    >```

3. <a id="虚拟机配置调优3"></a>在&lt;/cputune&gt;下方添加如下所示文本，设置虚拟机与宿主机NUMA的内存绑定。（此处示例为虚拟机绑定了NUMA 1。）

    ```shell
    <numatune>
          <memory mode='strict' nodeset='1'/>
    </numatune>
    ```

    ![](figures/zh-cn_image_0000002549948149.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >上述操作也可使用如下命令完成设置。
    >
    >```shell
    >./setup_vm.sh vm0 --numatune 1
    >```

4. 在&lt;/cputune&gt;上方添加如下所示文本，绑定QEMU模拟器。

    ```shell
    <emulatorpin cpuset='80-159' />
    ```

    ![](figures/1_zh-cn_image_0000002120702289.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >上述操作也可使用如下命令完成设置。
    >
    >```shell
    >./setup_vm.sh vm0 --emulatorpin 80-159
    >```

5. <a id="虚拟机配置调优5"></a>使用内存大页。在如图所示位置添加红框文本。

    ```shell
    <memoryBacking>
         <hugepages/>
    </memoryBacking>
    ```

    ![](figures/zh-cn_image_0000002518308386.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >上述操作也可使用如下命令完成设置。
    >
    >```shell
    >./setup_vm.sh vm0 --enable_hugepages
    >```

6. 使能CPU拓扑。

    找到“<cpu mode='host-passthrough' check='none'\>”元素，补充并修改以下内容，修改示例如下图所示。

    ```shell
    <cpu mode='host-passthrough' check='none'>
      <topology sockets='1' dies='1' clusters='10' cores='4' threads='2'/>
    </cpu>
    ```

    ![](figures/zh-cn_image_0000002518468302.png)

7. 按“Esc”键退出编辑模式，输入**:wq!**并按“Enter”键保存并退出文件。
8. 启动虚拟机。

    ![](figures/zh-cn_image_0000002549948153.png)

9. 虚拟机内部执行如下指令，回显**0-7**表示CPU拓扑生效。

    ```shell
    cat /sys/devices/system/cpu/cpu0/topology/cluster_cpus_list
    ```

    ![](figures/zh-cn_image_0000002550068147.png)

10. 执行如下指令使能cluster调度优化。

    ```shell
    echo 1 > /proc/sys/kernel/sched_cluster
    ```

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >每次虚拟机重启都需要执行该步骤操作使能cluster调度优化，建议将其配置在“~/.bashrc”等同类型文件中，确保每次重启后都会自动执行。

#### 2.3.3 配置虚拟机网络<a name="ZH-CN_TOPIC_0000002518304964"></a>

##### 2.3.3.1 配置虚拟机网卡配置文件<a name="ZH-CN_TOPIC_0000002549944731"></a>

配置虚拟机内部的网卡配置文件，即可打通虚拟机的网络通信。如果使用了SR-IOV方案，那么需要根据本章节操作手动生成虚拟机网卡配置文件。

1. 查看虚拟机网卡名称。

    ```shell
    ip a
    ```

    ![](figures/zh-cn_image_0000002518308390.png)

2. 生成配置文件。

    - 如果在[2.2.6-配置宿主机网络](#配置宿主机网络)章节中选择了SR-IOV网卡直通，则执行如下指令，生成网络配置文件。

        ```shell
        nmcli connection add ifname enp1s0 con-name enp1s0 type ethernet
        cd /etc/sysconfig/network-scripts/
        ls
        ```

        如下图所示虚拟机网卡配置文件生成。

        ![](figures/zh-cn_image_0000002518468304.png)

    - 如果[2.2.6-配置宿主机网络](#配置宿主机网络)中采用的网桥模式，那么虚拟机中会自动生成网络配置文件，文件名和**ip a**中网卡名称一致，则跳过该步骤。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >如果条件允许，建议使用SR-IOV虚拟网卡方案，采用该方案的虚拟机具有更强的计算性能以及更短的时延。

3. 进入虚拟机配置虚拟机的IPADDR，NETMASK，GATEWAY，DNS，确保ONBOOT=yes。

    虚拟机和宿主机共用NETMASK，GATEWAY，DNS。IPADDR可自定义，需要和网络管理员确认，请不要与局域网内其他的IP地址冲突。

    ```shell
    vi ifcfg-enp1s0
    ```

    ![](figures/zh-cn_image_0000002549948157.png)

    如果配置文件中有下图红框内容，请删除对应内容。

    ![](figures/zh-cn_image_0000002550068149.png)

4. 重新配置网络连接，建议在virt-manager的Shell中执行，通过SSH远程操作会因为网络配置修改断开连接。

    ```shell
    virt-manager
    ```

    ![](figures/zh-cn_image_0000002518308392.png)

    ```shell
    nmcli connection reload
    nmcli connection down enp1s0
    nmcli connection up enp1s0
    ```

    ![](figures/zh-cn_image_0000002518468306.png)

5. ping网关，SSH远程连接验证配置是否生效，网关在br0网桥的配置文件中可以看到。

    ```shell
    ping 192.168.20.1
    ```

    ![](figures/zh-cn_image_0000002549948159.png)

    同一网段内任意服务器**ssh**连接虚拟机。

    ```shell
    ssh 192.168.20.150
    ```

    ![](figures/zh-cn_image_0000002550068153.png)

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >至此已经实现了虚拟机在服务器局域网内的数据通信，如果希望在外网访问局域网内的该虚拟机，请联系网络管理员按照局域网内服务器的相关配置对虚拟机进行配置即可

### 2.4 视频流启动环境配置（虚拟机）<a name="ZH-CN_TOPIC_0000002549947651" id="视频流启动环境配置"></a>

在搭建好的虚拟机环境中部署云手机容器环境和视频流容器时，需要根据虚拟机内部CPU以及GPU核数对cfct_video和cfct_config文件做相应调整和修改视频流启动和配置文件。

请参见《Kbox云手机容器 特性指南》的“[软件部署](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/kboxcpc/kunpengcpskbox_20_0130.html)”以及《视频流引擎 特性指南》的“[软件部署](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/videostreamengine/kunpengcpsvideo_20_0048.html)”章节在虚拟机内部署云手机容器环境和运行视频流云手机。

具体操作步骤如下所示：

1. 请参见《视频流引擎 特性指南》的“[软件部署](https://www.hikunpeng.com/document/detail/zh/kunpengcps/cpturbokit/videostreamengine/kunpengcpsvideo_20_0048.html)”解压缩出cfct_video和cfct_config文件。
2. 修改cfct_config脚本适配虚拟机80核CPU和虚拟机4 GPU节点。
    1. 打开cfct_config脚本。

        ```shell
        vim cfct_config
        ```

    2. 按“i”进入编辑模式，增加以下内容。

        ```shell
        VIDEO_CPU_MAP_80CORE_MODE0=(
        "${MODE0_CPUS0_320[0]}"
        "${MODE0_CPUS0_320[1]}"
        "${MODE0_CPUS0_320[2]}"
        "${MODE0_CPUS0_320[3]}"
        "${MODE0_CPUS0_320[4]}"
        "${MODE0_CPUS0_320[5]}"
        "${MODE0_CPUS0_320[6]}"
        "${MODE0_CPUS0_320[7]}"
        "${MODE0_CPUS0_320[8]}"
        "${MODE0_CPUS0_320[9]}"
        "${MODE0_CPUS0_320[10]}"
        "${MODE0_CPUS0_320[11]}"
        "${MODE0_CPUS0_320[12]}"
        "${MODE0_CPUS0_320[13]}"
        "${MODE0_CPUS0_320[14]}"
        "${MODE0_CPUS0_320[15]}"
        "${MODE0_CPUS0_320[16]}"
        "${MODE0_CPUS0_320[17]}"
        "${MODE0_CPUS0_320[18]}"
        "${MODE0_CPUS0_320[19]}"
        "${MODE0_CPUS0_320[20]}"
        "${MODE0_CPUS0_320[21]}"
        "${MODE0_CPUS0_320[22]}"
        "${MODE0_CPUS0_320[23]}"
        "${MODE0_CPUS0_320[24]}"
        "${MODE0_CPUS0_320[25]}"
        "${MODE0_CPUS0_320[26]}"
        "${MODE0_CPUS0_320[27]}"
        "${MODE0_CPUS0_320[28]}"
        "${MODE0_CPUS0_320[29]}"
        "${MODE0_CPUS0_320[30]}"
        "${MODE0_CPUS0_320[31]}"
        "${MODE0_CPUS0_320[32]}"
        "${MODE0_CPUS0_320[33]}"
        "${MODE0_CPUS0_320[34]}"
        "${MODE0_CPUS0_320[35]}"
        "${MODE0_CPUS0_320[36]}"
        "${MODE0_CPUS0_320[37]}"
        "${MODE0_CPUS0_320[38]}"
        )
        VIDEO_CPU_MAP_80CORE_MODE1=(
        "${MODE1_CPUS0_320}"
        )
        VIDEO_GPU_MAP_80CORE=(
        "${GPUS[0]}"
        "${GPUS[1]}"
        "${GPUS[2]}"
        "${GPUS[3]}"
        )
        ```

        ![](figures/zh-cn_image_0000002549948163.png)

    3. 按“Esc”键退出编辑模式，输入**:wq!**并按“Enter”键保存并退出文件。

        >![](public_sys-resources/icon-note.gif) **说明：** 
        >上述的配置的CPU核心以及GPU节点仅供参考，请根据实际虚拟机的资源分配以及业务的需要，灵活地调整该配置。

3. 修改cfct_video脚本适配当前虚拟机80核。
    1. 打开cfct_video脚本。

        ```shell
        vim cfct_video
        ```

    2. 按“i”进入编辑模式，新增以下内容至**elif \[ $num_of_cpus -eq 64 \]; then**上方。

        ```shell
        elif [ $num_of_cpus -eq 80 ]; then
        if [ ${CPU_BIND_MODE} -eq 0 ]; then
        VIDEO_CPU_MAP=(${VIDEO_CPU_MAP_80CORE_MODE0[*]})
        elif [ ${CPU_BIND_MODE} -eq 1 ]; then
        VIDEO_CPU_MAP=(${VIDEO_CPU_MAP_80CORE_MODE1[*]})
        else
        EXIT_ERROR "CPU_BIND_MODE error: ${CPU_BIND_MODE}"
        fi
        VIDEO_GPU_MAP=(${VIDEO_GPU_MAP_80CORE[*]})
        ```

        ![](figures/zh-cn_image_0000002550068157.png)

    3. 按“Esc”键退出编辑模式，输入**:wq!**并按“Enter”键保存并退出文件。

4. 请参见《视频流引擎 特性指南》的“启动视频流云手机”调用cfct_video脚本即可成功在虚拟机启动视频流容器。

    ![](figures/zh-cn_image_0000002518308398.png)

### 2.5 虚拟机拷贝<a name="ZH-CN_TOPIC_0000002518304966" id="虚拟机拷贝"></a>

#### 2.5.1 拷贝说明<a name="ZH-CN_TOPIC_0000002549944733"></a>

- 如果已经按照[2.3-虚拟机配置](#虚拟机配置)顺序创建了4个虚拟机，则跳过本章节之后的所有操作。
- 如果按照[2.3-虚拟机配置](#虚拟机配置)创建了1个虚拟机，则使用本章节后续内容拷贝创建虚拟机剩下的3个虚拟机。

    推荐在完成[2.2-宿主机环境配置](#宿主机环境配置)、[2.3-虚拟机配置](#虚拟机配置)、[2.4-视频流启动环境配置（虚拟机）](#视频流启动环境配置)后进行虚拟机的拷贝，可以避免创建新虚拟机时进行大量重复配置。若完成了上述章节，拷贝后的虚拟机仅需对[2.3-虚拟机配置](#虚拟机配置)中少量配置进行调整后即可正常使用。

#### 2.5.2 拷贝虚拟硬盘与配置文件<a name="ZH-CN_TOPIC_0000002550064721"></a>

本章节提供虚拟机拷贝的详细步骤，包括对虚拟硬盘的拷贝和配置文件的修改。

1. 确认想要拷贝的虚拟硬盘位置。

    ```shell
    virsh dumpxml vm0 | grep "source file"
    ```

    ![](figures/zh-cn_image_0000002518468316.png)

2. <a name="li14352145276"></a>拷贝虚拟硬盘，需要确保存放新虚拟硬盘的硬盘空间足够。

    ```shell
    cd 新虚拟硬盘存放地址(根据情况自行决定)
    cp /var/lib/libvirt/images/vm0.qcow2 vm1.qcow2
    ```

3. 拷贝虚拟机配置文件。

    ```shell
    virsh dumpxml vm0 > vm1.xml
    ```

4. 修改配置文件。
    1. 打开配置文件。

        ```shell
        vim vm1.xml
        ```

    2. 按“i”进入编辑模式，修改“name”，“uuid”和“mac”的值，保证不同虚拟机的name，uuid和mac地址不同即可，同时修改虚拟硬盘地址“source”为[2](#li14352145276)中的地址。

        ```shell
        <name>vm1</name>
        <uuid>dfbd8ad1-34ef-423d-8b9c-f7551654b09f</uuid>
        ```

        ![](figures/zh-cn_image_0000002549948165.png)

        ```shell
        <mac address='52:54:00:be:e7:69' />
        ```

        ```shell
        <source file=' /磁盘镜像存放地址/vm1.qcow2' index='2' />
        ```

    3. 按“Esc”键退出编辑模式，输入**:wq!**并按“Enter”键保存并退出文件。

5. 创建虚拟机。

    ```shell
    virsh define vm1.xml
    ```

    ![](figures/zh-cn_image_0000002550068159.png)

6. 验证虚拟机创建与否。

    ```shell
    virsh list --all
    ```

7. 请参见[查询GPU卡PCIe节点信息](#查询GPU卡PCIe节点信息)获取虚拟机绑定NUMA对应的GPU节点。
8. 进入虚拟机删除当前GPU节点。

    ```shell
    virt-manager
    ```

    ![](figures/zh-cn_image_0000002518308400.png)

    ![](figures/zh-cn_image_0000002518468320.png)

9. 请参见[查询GPU卡PCIe节点信息](#查询GPU卡PCIe节点信息)获取当前虚拟机匹配的GPU卡的PCIe节点，再通过“Add Hardware \> PCI Host Device  \> 选择对应节点 \> Finish”，配置所有的PCIe节点。

    ![](figures/zh-cn_image_0000002549948167.png)

10. 如果虚拟机使能SR-IOV网卡直通，需要先删除原虚拟机使用的SR-IOV虚拟网卡。

    ![](figures/zh-cn_image_0000002550068161.png)

    请参见[配置宿主机网络](#配置宿主机网络)，添加新的SR-IOV虚拟网卡。

    ![](figures/zh-cn_image_0000002518308404.png)

11. 如果虚拟机使能了磁盘分区直通，需要先删除原虚拟机直通的磁盘分区。

    ![](figures/zh-cn_image_0000002518468322.png)

    请参见[2.3.1-15](#使用virt-manager创建虚拟机15)，添加新的磁盘分区。

    ![](figures/zh-cn_image_0000002549948169.png)

#### 2.5.3 虚拟机配置调优<a name="ZH-CN_TOPIC_0000002518464890"></a>

由于每个虚拟机分配的资源不一样，除了内存大页的其他调优需要重新配置。

请参见[2.3.2-虚拟机配置调优](#虚拟机配置调优)进行虚拟机配置调优。

#### 2.5.4 配置网络<a name="ZH-CN_TOPIC_0000002518304968"></a>

对拷贝后的虚拟机进行网络配置。

1. 进入虚拟机。

    ```shell
    virsh start vm1
    virt-manager
    ```

    ![](figures/zh-cn_image_0000002550068163.png)

2. 修改虚拟机IP地址。

    ```shell
    vi /etc/sysconfig/network-scripts/ifcfg-enp1s0
    ```

    ![](figures/zh-cn_image_0000002518308406.png)

3. 重新配置网络连接。

    ```shell
    nmcli connection reload
    nmcli connection down enp1s0
    nmcli connection up enp1s0
    ```

4. ping网关，SSH远程连接验证配置是否生效，网关在br0网桥的配置文件中可以看到。

    ```shell
    ping 192.168.20.1
    ```

    ![](figures/zh-cn_image_0000002518468324.png)

#### 2.5.5 虚拟硬盘扩容<a name="ZH-CN_TOPIC_0000002549944735"></a>

根据场景不同，需要对硬盘进行扩容（不同的游戏对硬盘的占用空间不同）。

1. 确认虚拟硬盘的位置。

    ```shell
    virsh dumpxml <domain>
    ```

    ![](figures/zh-cn_image_0000002549948171.png)

2. 调整虚拟硬盘的空间。

    ```shell
    qemu-img resize /home/VirtualMachine/Disks/oe22.03-lts-sp1-64cores.qcow2 600G
    ```

    上述命令将虚拟硬盘空间扩大到600GB。

3. 进入虚拟机调整虚拟机分区大小。
    1. 查看根目录所在分区，后续扩展该分区。

        ```shell
        lsblk
        ```

        ![](figures/zh-cn_image_0000002550068165.png)

    2. 选定指定的磁盘，查看磁盘当前的分区信息。

        ```shell
        parted /dev/sda
        print
        ```

        执行**print**命令后选择修复选项“Fix”。

        ![](figures/zh-cn_image_0000002518308410.png)

    3. 扩展4号分区。

        ```shell
        resizepart 4 -1
        ```

        ![](figures/zh-cn_image_0000002518468326.png)

4. 按“Ctrl+C”键退出parted后调整文件系统大小。

    ```shell
    resize2fs /dev/sda4
    ```

    ![](figures/zh-cn_image_0000002549948173.png)
