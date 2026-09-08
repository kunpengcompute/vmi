# Installation Guide<a name="ZH-CN_TOPIC_0000002521895634"></a>

## 1 Software Deployment<a name="ZH-CN_TOPIC_0000002518386576"></a>

### 1.1 Environment Requirements<a name="ZH-CN_TOPIC_0000002518226658"></a>

#### 1.1.1 Hardware Environment<a name="ZH-CN_TOPIC_0000002518226662"></a>

Before deploying the Kbox cloud phone container environment, ensure that your hardware environment meets the requirements.

For details, see [**Table 1** Hardware configuration schemes for deploying the Kbox cloud phone container](#hardware-configuration-schemes-for-deploying-the-kbox-cloud-phone-container).

**Table 1** Hardware configuration schemes for deploying the Kbox cloud phone container<a id="hardware-configuration-schemes-for-deploying-the-kbox-cloud-phone-container"></a>

|Option|Hardware Configuration Scheme 1|Hardware Configuration Scheme 2|Hardware Configuration Scheme 3|Hardware Configuration Scheme 4|
|--|--|--|--|--|
|Server|Kunpeng server|Kunpeng server|Kunpeng server|Kunpeng server|
|CPU|2 x Kunpeng 920, 64 cores@2.6 GHz|2 x Kunpeng 920, 64 cores@2.6 GHz|2 x new Kunpeng 920 processor model, 80 cores@2.9 GHz|2 x new Kunpeng 920 processor model, 64 cores@2.2 GHz|
|Memory|16 x DDR4 RDIMM-32 GB-2933 MT/s|16 x DDR4 RDIMM-32 GB-2933 MT/s|16 x DDR5 DIMM-64 GB-4800 MT/s|16 x DDR5 DIMM-64 GB-5200 MT/s|
|Drive|System drive: 2 x SSD, 480 GB, SATA 6 Gbit/s, read-intensive; data drive: 2 x ES3521A V6 SSD, 1920 GB, SATA 6 Gbit/s, read-intensive|System drive: 2 x SSD, 480 GB, SATA 6 Gbit/s, read-intensive; data drive: 2 x ES3521A V6 SSD, 1920 GB, SATA 6 Gbit/s, read-intensive|System drive: 1 x S3521A V6 SSD, 1920 GB, SATA 6 Gbit/s, read-intensive; data drive: 2 x S3521A V6 SSD, 1920 GB, SATA 6 Gbit/s, read-intensive|System drive: 1 x SSD, 480 GB, SATA 6 Gbit/s, 2.5-inch height, read-intensive<br>1 x S4510 SSD, 960 GB, SATA 6 Gbit/s, read-intensive<br>Data drive: 1 x ES3600P V6 SSD, 6400 GB, NVMe 64 Gbit/s<br>1 x ES3500P V5 SSD, 4000 GB, NVMe 32 Gbit/s|
|NIC|Onboard: 1 x NIC (4 x GE); 1 x TM280 flexible LOM, 25GE/10GE optical port, 4 ports, SFP28 (without optical modules). External: 1 x Mellanox NIC|Onboard: 1 x NIC (4 x GE); 1 x TM280 flexible LOM, 25GE/10GE optical port, 4 ports, SFP28 (without optical modules). External: 1 x Mellanox NIC|Onboard: 1 x NIC (4 x GE); 1 x TM280 flexible LOM, 2 x 25GE/10GE optical port, 4 ports, SFP28 (without optical modules). External: 1 x Mellanox NIC|Onboard: 1 x NIC (4 x GE); 1 x TM280 flexible LOM, 2 x 25GE/10GE optical port, 4 ports, SFP28 (without optical modules)|
|Riser card|PCIe x16 + PCIe x8 for both riser 1 and riser 2|3 x PCIe x8 for both riser 1 and riser 2|2 x front riser (x8 x 2) + 2 x rear riser (x8 x 2) + 1 x riser 3 (x8 x 2)|2 x rear riser (x16 + x8 x 2) + 1 x riser 3 (x8 x 2)|
|Encoding card|1 x NETINT Quadra T2A (x8)|None|None|None|
|GPU|2 x AMD W6800|4 x DaoCloud DC1000|8 x DaoCloud DC1000|8 x DaoCloud DC1000|
|OS|openEuler 24.03 LTS SP1|openEuler 24.03 LTS SP1|openEuler 24.03 LTS SP1|openEuler 24.03 LTS SP1|
|System/Kernel version|6.6.0-72.0.0|6.6.0-72.0.0|6.6.0-72.0.0|6.6.0-72.0.0|

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>- Select the Mellanox NICs compatible with the Kunpeng server. You can visit [Kunpeng Computing Compatibility Query](https://info.support.huawei.com/computing/tools/compatibility-query/enterprise/kunpeng-computing/component-compatibility?lang=en) to query compatible NIC models.
>- The NETINT driver supports only Quadra cards in the Android 15 environment.

#### 1.1.2 Software Environment<a name="ZH-CN_TOPIC_0000002549866439"></a>

##### 1.1.2.1 Kbox<a name="ZH-CN_TOPIC_0000002549866441"></a>

For details about the software environment for deploying the Kbox cloud phone container, see "Software Deployment > Preparing the Environment > Software Environment" in [Kbox Cloud Phone Container Installation Guide (Android 15)](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/kboxcpc_ad15/docs/en/install_guide.md#d22-software-environment).

##### 1.1.2.2 Video Stream Engine<a name="ZH-CN_TOPIC_0000002549746419" id="video-stream-engine"></a>

Before deploying the video stream cloud phone, obtain the required software packages from the addresses provided in this section and verify the integrity of the software packages provided by Huawei.

**Obtaining Software Packages<a name="section9155930113616"></a>**

**Table 1** Software requirements for deploying the video stream engine<a id="software-requirements-for-deploying-the-video-stream-engine"></a>

|No.|Software Package|Description|How to Obtain|Configuration Scheme 1|Configuration Scheme 2|Configuration Scheme 3|Configuration Scheme 4|
|--|--|--|--|--|--|--|--|
| 1 | BoostKit-boostcph-videoengine_*_15.zip | Binary package of the video stream engine.| Please submit an ISSUE feedback. | √ | √ | √ | √ |
| 2 | DemoVideoEngine.tar.gz | TAR package of the video stream engine server. It is used to obtain the audio and video data of the Kbox container.| Please submit an ISSUE feedback.| √ | √ | √ | √ |
| 3 | DemoVideoEngine_sha256.txt | SHA256 file corresponding to the TAR package of the video stream engine server. It is used to check the software integrity.| Please submit an ISSUE feedback.| √ | √ | √ | √ |
| 4 | CloudPhoneApk.tar.gz | TAR package of the video stream engine client. It is used to decode and play audio and video data.| Please submit an ISSUE feedback.| √ | √ | √ | √ |
| 5 | CloudPhoneApk_sha256.txt | SHA256 file corresponding to the TAR package of the video stream engine client. It is used to check the software integrity.| Please submit an ISSUE feedback.| √ | √ | √ | √ |
| 6 | VideoClientEmulator.tar.gz | Stress test tool for the video stream cloud phone.| Please submit an ISSUE feedback.| √ | √ | √ | √ |
| 7 | NETINT-v*XXX*.tar.gz | TAR package of the video stream NETINT encoding card for hardware encoding. The matching version is 4.8.F-Android15.| [Link](https://www.netint.cn/quadra-firmware-downloads-android15)<br>Download password: **test123**| √ | - | - | - |
| 8 | Quadra_V*XXX*.zip | Quadra software, firmware, and document packages of the NETINT encoding card.| [Link](https://www.netint.cn/quadra-firmware-downloads-android15)<br>Download password: **test123**| √ | - | - | - |
| 9 | topo-affinity-plugin-master.zip | Kubernetes NUMA affinity plugin.| [Link](https://gitee.com/kunpeng_compute/topo-affinity-plugin)| √ | √ | √ | √ |

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>- **√** indicates that the software needs to be installed for the respective configuration scheme.
>- **-** indicates that the software is not required for the respective configuration scheme.

**Verifying Software Package Integrity<a name="section111981239163615"></a>**

To prevent software packages from being maliciously tampered with during transfer or storage, download also the corresponding digital signature files for integrity verification while obtaining the software packages from the Kunpeng community.

1. Obtain software packages based on [Table 1 Software requirements for deploying the video stream engine](#software-requirements-for-deploying-the-video-stream-engine).
2. <a name="li1273482318125"></a>Obtain the verification tool and guide from the [Huawei enterprise website](https://support.huawei.com/enterprise/en/tool/pgp-verify-TL1000000054) or [Huawei carrier website](http://support.huawei.com/carrier/digitalSignatureAction).
3. Based on the *OpenPGP Signature Verification Guide* obtained in [2](#li1273482318125), verify the PGP digital signatures of the software packages.

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>If the verification fails, do not use the software package. Please submit an ISSUE feedback.
>Before a software package is used for installation or upgrade, its digital signature also needs to be verified to ensure that the software package is not tampered with.
>Before using the software package, read and agree to [Kunpeng BoostKit User License Agreement 2.0](https://www.hikunpeng.com/en/legal/developer/boostkit/software/protocol).

### 1.2 Deploying a Cloud Phone<a name="ZH-CN_TOPIC_0000002549746425"></a>

#### 1.2.1 Environment Configuration<a name="ZH-CN_TOPIC_0000002518386582"></a>

##### 1.2.1.1 Configuring the BIOS<a name="ZH-CN_TOPIC_0000002518226660"></a>

The BIOS version of the specified server has restrictions on the DIMM insertion method. Before configuring the BIOS, ensure that the DIMM insertion method is correct. Then, configure MISC, performance, and memory options according to the hardware configuration scheme to improve server performance.

For details, see "Software Deployment > Configuring the BIOS" in [Kbox Cloud Phone Container Installation Guide (Android 15)](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/kboxcpc_ad15/docs/en/install_guide.md#d4-configuring-the-bios).

##### 1.2.1.2 (Configuration Scheme 1, Optional) Upgrading the NVMe Firmware<a name="ZH-CN_TOPIC_0000002518226644"></a>

This section is required only when hardware configuration scheme 1 is used and the hardware decoding function of the encoding card needs to be enabled. If hardware decoding is not required, skip this section.

Before deploying the environment, check whether the encoding card is correctly detected by the NVMe driver and check the NVMe firmware version. If the version is different from that provided in this section, upgrade the firmware.

1. Check whether the encoding card is correctly detected by the NVMe driver.

    ```shell
    nvme list
    ```

    If the following information is displayed, the encoding card is correctly detected. The command output is only an example.

    ```shell
    Node          SN                   Model            Namespace Usage                    Format           FW Rev
    ------------- -------------------- ---------------- --------- ------------------------ ---------------- --------
    /dev/nvme0n1  Q2A325A11DC082-0454A QuadraT2A        1         8.59  TB /   8.59  TB    4 KiB +  0 B     48F6rKr1
    /dev/nvme1n1  Q2A325A11DC082-0454B QuadraT2A        1         8.59  TB /   8.59  TB    4 KiB +  0 B     48F6rKr1
    ```

    If the firmware version (the `FW Rev` column) is inconsistent with the 4.8.F-Android15 firmware version, refer to the following steps to upgrade the encoding card firmware.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >About the NVMe firmware version: The larger the numbers and the later the letters, the newer the version.

2. Extract the 4.8.F-Android15 firmware upgrade package from `Quadra_V_XXX_.zip` (*XXX* indicates the version number. Use the actual package name in the following commands) and upgrade the firmware.

    ```shell
    unzip Quadra_VXXX.zip
    cd Quadra_VXXX/
    tar -zxvf Quadra_FW_VXXX.tar.gz
    cd Quadra_FW_VXXX/
    chmod +x quadra_auto_upgrade.sh
    ./quadra_auto_upgrade.sh
    ```

    The upgrade takes about 1 minute.

3. After the upgrade is complete, reboot the system for the upgrade to take effect.

    ```shell
    reboot
    ```

##### 1.2.1.3 (Configuration Scheme 1) Configuring the GPU Working Mode<a name="ZH-CN_TOPIC_0000002518226676"></a>

If hardware configuration scheme 1 is used, set the GPU working mode to the high-performance mode to enable the GPU to run at the maximum frequency and maintain the optimal GPU performance. This operation needs to be performed each time the system is restarted.

Run the following command:

```shell
find /sys -name power_dpm_force_performance_level | xargs -I {} sh -c "echo high > '{}'"
```

##### 1.2.1.4 Deploying the Basic Environment for the Kbox Container<a name="ZH-CN_TOPIC_0000002549746421" id="deploying-the-basic-environment-for-the-kbox-container"></a>

For details about how to deploy a Kbox cloud phone container, see "Software Deployment" in [Kbox Cloud Phone Container Installation Guide](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/kboxcpc_ad15/docs/en/install_guide.md#d21-hardware-environment). Perform all operations before section "Starting and Uninstalling a Cloud Phone Instance" in this chapter.

##### 1.2.1.5 (Optional) Deploying the Containerd Environment<a name="ZH-CN_TOPIC_0000002549866409" id="deploying-the-containerd-environment"></a>

In addition to the Docker container runtime, the video stream cloud phone also supports the containerd runtime. If you choose to run the video stream cloud phone with containerd, refer to this section to deploy containerd-related software packages.

**Obtaining Software Packages<a name="section11783175614511"></a>**

[**Table 1**](#containerd-related-software-packages) lists the software packages required for deploying the containerd environment.

**Table 1** Containerd-related software packages<a id="containerd-related-software-packages"></a>

|Software|Version|Description|How to Obtain|
|--|--|--|--|
| Containerd | v1.7.14 | A container runtime.| Containerd binary software package: **containerd-1.7.14-linux-arm64.tar.gz**; containerd service file: [Link](https://raw.githubusercontent.com/containerd/containerd/main/containerd.service)|
| runc | v1.1.12 | runC is a lightweight container runtime that complies with the Open Container Initiative (OCI). It is a dependency component of containerd.| [Link](https://github.com/opencontainers/runc/releases/download/v1.1.12/runc.arm64)|
| /rCNI Plugin | v1.4.1 | The Container Network Interface (CNI) is a specification and libraries for configuring network interfaces in Linux containers.| [Link](https://github.com/containernetworking/plugins/releases/download/v1.4.1/cni-plugins-linux-arm64-v1.4.1.tgz)|
| nerdctl | v1.7.5 | nerdctl is a Docker-compatible CLI for managing containerd-based containers and images.| [Link](https://github.com/containerd/nerdctl/releases/download/v1.7.5/nerdctl-1.7.5-linux-arm64.tar.gz)|

**Deploying the Containerd Environment<a name="section343716111874"></a>**

1. <a id="deploying-the-containerd-environment-1"></a>Download and decompress the containerd binary package to the `/usr/local` directory.

    ```shell
    mkdir -p /root/containerdenv/downloads
    cd /root/containerdenv/downloads
    wget https://github.com/containerd/containerd/releases/download/v1.7.14/containerd-1.7.14-linux-arm64.tar.gz --no-check-certificate
    tar Cxzvf /usr/local containerd-1.7.14-linux-arm64.tar.gz
    ```

    Check that the containerd version is v1.7.14.

    ```shell
    containerd --version
    ```

2. Download the containerd service file and configure it as a system service.

    ```shell
    cd /root/containerdenv/downloads
    wget https://raw.githubusercontent.com/containerd/containerd/main/containerd.service --no-check-certificate
    mkdir -p /usr/local/lib/systemd/system/
    cp containerd.service /usr/local/lib/systemd/system/
    systemctl daemon-reload
    systemctl enable --now containerd
    ```

    Check whether the containerd service is started.

    ```shell
    systemctl status containerd
    ```

    ![](figures/zh-cn_image_0000002518386596.png)

    If the command output shown in the preceding figure is displayed, the containerd service has been started.

3. <a id="deploying-the-containerd-environment-3"></a>Download and install runC.

    ```shell
    cd /root/containerdenv/downloads
    wget https://github.com/opencontainers/runc/releases/download/v1.1.12/runc.arm64 --no-check-certificate
    install -m 755 runc.arm64 /usr/local/sbin/runc
    ```

    Check that the runC version is 1.1.12.

    ```shell
    runc --version
    ```

4. Download and install the CNI plugin.

    ```shell
    cd /root/containerdenv/downloads
    mkdir -p /opt/cni/bin
    wget https://github.com/containernetworking/plugins/releases/download/v1.4.1/cni-plugins-linux-arm64-v1.4.1.tgz --no-check-certificate
    tar Cxzvf /opt/cni/bin cni-plugins-linux-arm64-v1.4.1.tgz
    ```

5. <a id="deploying-the-containerd-environment-5"></a>Download and install nerdctl.

    ```shell
    cd /root/containerdenv/downloads
    wget https://github.com/containerd/nerdctl/releases/download/v1.7.5/nerdctl-1.7.5-linux-arm64.tar.gz --no-check-certificate
    tar Cxzvf /usr/local/bin nerdctl-1.7.5-linux-arm64.tar.gz
    ```

    Check that the nerdctl version is 1.7.5.

    ```shell
    nerdctl --version
    ```

6. <a id="deploying-the-containerd-environment-6"></a>Restart the Docker service and start a new terminal for the new container runtime to take effect.

    ```shell
    systemctl restart docker
    ```

    To switch to the Docker container runtime, remove the software binaries installed in [1](#deploying-the-containerd-environment-1) to [5](#deploying-the-containerd-environment-5) from the corresponding directories. After the removal is complete, run the preceding command to restart the Docker service and start a new terminal.

#### 1.2.2 (Configuration Scheme 2/3/4) Installing the GPU Driver<a name="ZH-CN_TOPIC_0000002549866411"></a>

You need to install the GPU driver each time the server is restarted if you use hardware configuration scheme 2/3/4.

1. Obtain `VAGPU-25.03.01.01-RC13-A15.tgz` according to [Software Environment](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/kboxcpc_ad15/docs/en/install_guide.md#d22-software-environment), upload it to the `~/dependency/` directory, and decompress it to obtain the kernel-space GPU driver.

    ```shell
    cd ~/dependency/
    tar -zxvf VAGPU-25.03.01.01-RC13-A15.tgz
    ```

2. Install the PCIe driver for the GPU.

    ```shell
    cd ~/dependency/VAGPU-25.03.01.01-RC13-A15/openEuler-6.6.0+/ko_fw
    insmod va_pci.ko
    ```

3. Copy the firmware in the driver package to the `/lib/firmware/` directory of the OS.

    ```shell
    cp rgx* /lib/firmware/
    ```

4. Install the GPU driver.

    The GPU driver starts a kworker process for each GPU node. A single DaoCloud DC1000 card has four nodes. To improve the performance of kworker processes, you are advised to use the `kworkerCores` parameter to bind kworker processes to CPU cores. Each value of the `kworkerCores` parameter indicates a core bound to the kworker process of the corresponding GPU node.

    When binding GPU driver processes to CPU cores, **ensure that the CPU cores bound to the kworker processes and GPU rendering nodes belong to the same CPU socket**. For details about how to query the CPU socket to which a GPU rendering node belongs, see [Determining the GPU Topology](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/kboxcpc_ad15/docs/en/install_guide.md#d81-determining-the-gpu-topology).

    The following core binding methods are for reference only. You can make adjustments based on actual circumstances.

    Configuration scheme 2 (Kunpeng 920 + 4 x DaoCloud DC1000)

    ```shell
    insmod va_gfx.ko kworkerCores=0,0,1,1,32,32,33,33,64,64,65,65,96,96,97,97
    ```

    Configuration scheme 3 (new Kunpeng 920 processor model + 8 x DaoCloud DC1000)

    ```shell
    insmod va_gfx.ko kworkerCores=80,80,81,81,82,82,83,83,0,0,1,1,2,2,3,3,240,240,241,241,242,242,243,243,160,160,161,161,162,162,163,163
    ```

    Configuration scheme 4 (new Kunpeng 920 processor model + 8 x DaoCloud DC1000)

    ```shell
    insmod va_gfx.ko kworkerCores=64,64,65,65,66,66,67,67,0,0,1,1,2,2,3,3,192,192,193,193,194,194,195,195,128,128,129,129,130,130,131,131
    ```

5. Wait until the script execution is complete and check the kernel logs.

    ```shell
    dmesg | grep VAGPU | grep version
    ```

    In the command output, if the kernel-mode driver version and GPU firmware version are the same (content in bold), the GPU driver is installed.

    ```shell
    PVR_K:  28823: Meta firmware version: 1.18@6276027 build: release branch:  commit: 67e785a8 tag: VAGPU-25.03.01.01-RC13-A15
    ...
    ```

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>To change the driver version, you need to uninstall the drivers and install the drivers of another version.
>
>1. Delete all containers to release the drivers.
>2. Uninstall the drivers in sequence.
>
> ```shell
> rmmod va_gfx
> rmmod va_pci
> ```

#### 1.2.3 Creating an Image<a name="ZH-CN_TOPIC_0000002549866429" id="creating-an-image"></a>

##### 1.2.3.1 Creating a Kbox Image<a name="ZH-CN_TOPIC_0000002549866417"></a>

Before creating a video stream cloud phone image, create a Kbox image first.

1. Obtain the Kbox container startup dependency components `android.tar` and `Kbox-patches-AOSP15.zip` based on [Deploying the Basic Environment for the Kbox Container](#deploying-the-basic-environment-for-the-kbox-container), and upload them to the `/home/kbox_video` directory on the server. (This directory is used as an example. You can customize a directory as required.)
2. Decompress the `Kbox-patches-AOSP15.zip` package, extract the `android_kbox_aosp15.sh` and `base_box_aosp15.sh` files from `deploy_scripts` to the`/home/kbox_video` directory, and grant permissions on the files. Ensure that the file owner has the read, write, and execute permissions while users in the owner group and others have only the read and execute permissions.

    ```shell
    unzip Kbox-patches-AOSP15.zip
    cp Kbox-patches-AOSP15/deploy_scripts/base_box_aosp15.sh /home/kbox_video/
    cp Kbox-patches-AOSP15/deploy_scripts/android_kbox_aosp15.sh /home/kbox_video/
    chmod 755 /home/kbox_video/base_box_aosp15.sh
    chmod 755 /home/kbox_video/android_kbox_aosp15.sh
    ```

3. Create a Kbox image named `kbox:origin`.
    1. Upload the Kbox demo image package `android.tar` to the `~/dependency` directory (this directory is only an example and can be customized) and mount the image package.

        You can customize the image name and tag in the format of *{Name}:{Tag}*. In this example, the image name is `kbox:demo`.

        ```shell
        cd ~/dependency
        docker import android.tar kbox:demo
        ```

    2. Upload the `deploy_scripts` directory in the `Kbox-patches-AOSP15` folder to the `~/dependency` directory on the server.
    3. Upload the Android Kbox binary package `BoostKit-boostcph-kbox_*.zip` to `~/dependency/deploy_scripts`.
    4. (Hardware configuration scheme 2/3/4) If hardware configuration scheme 2/3/4 is used, decompress the GPU driver package `VAGPU-25.03.01.01-RC13-A15.tgz` to obtain `va_driver.tgz` and upload `va_driver.tgz` to the `~/dependency/deploy_scripts` directory on the server.
    5. Create a Kbox image that contains the Android Kbox binary. In the following commands, `kbox:demo` is the official Kbox demo image, and `kbox:origin` is the new image that contains the Android Kbox binary.
        - For configuration scheme 1:

            ```shell
            cd ~/dependency/deploy_scripts
            chmod +x make_image_aosp15.sh
            ./make_image_aosp15.sh kbox:demo kbox:origin
            ```

        - For configuration scheme 2/3/4:

            ```shell
            cd ~/dependency/deploy_scripts
            chmod +x make_image_aosp15.sh
            ./make_image_aosp15.sh kbox:demo kbox:origin va_driver.tgz
            ```

            >![](public_sys-resources/icon-note.gif) **NOTE**
            >
            >After step 3 is performed, multiple alarms related to linker configuration are displayed in the command output. The alarms do not affect normal functionality and can be ignored.
            >![](figures/zh-cn_image_0000002549746455.png)

4. Check whether the Kbox image (`kbox:origin`) is successfully created.

    ```shell
    docker images
    ```

    The image is created successfully if information similar to the following is displayed:

    ```shell
    REPOSITORY    TAG       IMAGE ID        CREATED          SIZE
    kbox          origin    d1f5cfd2e722    6 seconds ago    2.09GB
    ```

##### 1.2.3.2 Creating a Video Stream Cloud Phone Image<a name="ZH-CN_TOPIC_0000002518386578"></a>

Obtain the TAR packages of the video stream engine client and server, binary package of the video stream engine, and the TAR package of the video stream NETINT encoding card to create a video stream cloud phone image.

**Verifying Software Package Integrity<a name="section1286473717216"></a>**

1. Obtain `CloudPhoneApk.tar.gz`, `DemoVideoEngine.tar.gz`, and `BoostKit-boostcph-videoengine_*_15.zip` based on [1.1.2.2-Video Stream Engine](#video-stream-engine) and upload them to the `/home/kbox_video` directory on the server.
2. Obtain the SHA256 verification codes of the following components.

    ```shell
    sha256sum DemoVideoEngine.tar.gz
    sha256sum CloudPhoneApk.tar.gz
    ```

3. Compare the obtained verification codes with `DemoVideoEngine_sha256.txt` and `CloudPhoneApk_sha256.txt`.

    If the codes are consistent, the obtained software packages are complete and you can proceed with the following operations. If not, stop the deployment and obtain the complete software packages.

4. (Configuration scheme 1) If hardware configuration scheme 1 is used, obtain the `NETINT-vXXX.tar.gz` package based on [1.1.2.2-Video Stream Engine](#video-stream-engine), upload the package to the `/home/kbox_video` directory on the server, and rename the package `NETINT.tar.gz`.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >- NETINT support for Android 15 is limited exclusively to the Quadra encoding cards.

**Creating an Image<a name="section118652371219"></a>**

1. Decompress the `DemoVideoEngine.tar.gz` package to obtain the image creation script and grant the execute permission on the script.
    - For configuration scheme 1:

        ```shell
        tar -xvf DemoVideoEngine.tar.gz Dockerfile_NoVPU Dockerfile_QuadraT2A make_image.sh
        chmod +x Dockerfile_NoVPU Dockerfile_QuadraT2A make_image.sh
        ```

    - For configuration scheme 2/3/4:

        ```shell
        tar -xvf DemoVideoEngine.tar.gz Dockerfile_NoVPU  make_image.sh
        chmod +x Dockerfile_NoVPU  make_image.sh
        ```

2. Create a video stream cloud phone image. You can use the default image name or specify an image name.
    - If the default image name is used, run the following command. The default image names of the Kbox basic cloud phone and video stream cloud phone are `kbox:latest` and `video:latest`, respectively.

        ```shell
        ./make_image.sh
        ```

    - If a customized image name is used, run the following command. Specify the image names of the Kbox basic cloud phone and video stream cloud phone in the format of *{image_name}:{tag}*. In the following command, `kbox` and `video` are image names, and `origin` and `latest` are tags.

        ```shell
        ./make_image.sh kbox:origin video:latest
        ```

        >![](public_sys-resources/icon-note.gif) **NOTE**
        >
        >The image name can contain only digits and lowercase letters, and must start with a lowercase letter. The tag can contain only digits and letters. If the image name of the video stream cloud phone is changed, refer to [1.2.6-Creating a Base Data Volume](#creating-a-base-data-volume) and change the image name in the `cfct_config` file to the customized image name.
        >After this step is performed, multiple alarms related to linker configuration are displayed in the command output. The alarms do not affect normal functionality and can be ignored.
        >![](figures/zh-cn_image_0000002549866449.png)

3. Check whether the video stream cloud phone image (`video:latest`) is successfully created.

    ```shell
    docker images
    ```

    The image is created successfully if information similar to the following is displayed:

    ```shell
    REPOSITORY    TAG       IMAGE ID        CREATED          SIZE
    video         latest    40e5f42c17d9    6 seconds ago    2.11GB
    ```

#### 1.2.4 Setting the cfct_config File (Configuration Scheme 1)<a name="ZH-CN_TOPIC_0000002518386584"></a>

You can use the `cfct_config` file to flexibly configure the resources used by the video stream cloud phone to achieve optimal performance. Before starting a cloud phone, ensure that the `cfct_config` file is stored in the startup path and the configurations in the file are correct. The cloud phone container uses the configurations in this file.

The configuration items in the `cfct_config` file and the configuration method are as follows:

1. Extract the `cfct_config` file and grant permissions on the file. Ensure that the file owner has read and write permissions while users in the owner group and others have only the read permission.

    ```shell
    cd /home/kbox_video/
    tar -xvf DemoVideoEngine.tar.gz cfct_config
    chmod 644 cfct_config
    ```

2. Change map configurations such as `GPU`, `CPU`, `ENC`, and `USERDATA` of the corresponding channel to select the GPU, CPU, NETINT encoding card, and path to the data volume used by the container of this channel.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >To ensure the stable running and optimal performance of the video stream cloud phone, ensure that the physical CPU cores and GPU rendering nodes bound to a container belong to the same CPU socket.

3. The nodes of the NETINT encoding card vary according to the server. You need to change the value of `NETINT` in the `cfct_config` file based on [4](#li5561723173614) and your environment to prevent performance loss caused by cross-socket encoding.
4. If only one GPU is used in your environment, change the value of `VIDEO_CPU_MAP_{total_CPU_core_count}CORE_MODE{CPU_BIND_MODE}` in the `cfct_config` file. You can query the NUMA node to which a NETINT card chip belongs based on [Querying the NUMA Node to Which a NETINT Card Chip Belongs](#section2507154233510).

    Take `VIDEO_CPU_MAP_128CORE_MODE0` as an example. Retain the configuration of the CPU bound to the GPU and delete other configurations. If the GPU is inserted into CPU 0, delete all references related to `MODE0_CPUS2` and `MODE0_CPUS3`. If the GPU is inserted into CPU 1, delete all references related to `MODE0_CPUS0` and `MODE0_CPUS1`. For details about how to query the NUMA node to which a GPU belongs, see [Querying the NUMA Node to Which an AMD GPU Rendering Node Belongs](#section20575115322416).

5. If only one encoding card is used in your environment, change the value of `VIDEO_ENC_MAP_CORE` in the `cfct_config` file.
6. If the encoding card is inserted into CPU 0, delete `${NETINT1}`. If the encoding card is inserted into CPU 1, delete `${NETINT0}`.
7. If the CPU is used for software encoding of video frames, set `CPU_BIND_MODE` to `1` in `cfct_config` to prevent frame freezing.
8. Enable the graphics acceleration layer as required. For details, see [Basic Functions and Usage of the Graphics Acceleration Layer](#section9932195417616).

**Querying the NUMA Node to Which a NETINT Card Chip Belongs<a name="section2507154233510"></a>**

1. <a name="li1256022316361"></a>Run the `nvme list` command to view the nodes of the NETINT card chips.

    ```shell
    nvme list
    ```

    The following command output is an example of the NVMe nodes of the NETINT card chips.

    ```shell
    Node          SN                   Model            Namespace Usage                    Format           FW Rev
    ------------- -------------------- ---------------- --------- ------------------------ ---------------- --------
    /dev/nvme0n1  Q2A325A11DC082-0454A QuadraT2A        1         8.59  TB /   8.59  TB    4 KiB +  0 B     48F6rKr1
    /dev/nvme1n1  Q2A325A11DC082-0454B QuadraT2A        1         8.59  TB /   8.59  TB    4 KiB +  0 B     48F6rKr1
    ```

2. Check the mapping between NVMe nodes and PCIe bus numbers.

    *{index}* indicates the NVMe node number returned in [1](#li1256022316361). For example, for `/dev/nvme1n1`, the value of *{index}* is `1`.

    ```shell
    find /sys/devices/ -name nvme{index}
    ```

    In the following command output, `0000:05:00.0` indicates the bus number of the device.

    ```shell
    /sys/devices/pci0000:00/0000:00:0e.0/0000:05:00.0/nvme/nvme1
    /sys/devices/virtual/nvme-subsystem/nvme-subsys1/nvme1
    ```

3. Check the mapping between the node and NUMA based on the bus number.

    *{busID}* indicates the bus number obtained in the previous step. For example, in the command output for `nvme1`, *{busID}* is `0000:05:00.0`.

    ```shell
    lspci -vvvs {busID} | grep NUMA
    ```

    Command output:

    ```shell
    NUMA node: 0
    ```

4. <a name="li5561723173614"></a>Change the value of `NETINT` in `cfct_config` based on the NUMA information corresponding to the NVMe node of the encoding card.

    For servers powered by Kunpeng 920 processors, write NVMe nodes belonging to NUMA0 and NUMA1 in the `NETINT0` field, and write NVMe nodes belonging to NUMA2 and NUMA3 in the `NETINT1` field.

    Two nodes need to be added for each device in a field. For example, for NVMe device 2, you need to add nodes `/dev/nvme2` and `/dev/nvme2n1`.

    ```shell
    # Nodes of NETINT encoding card devices
    NETINT0="/dev/nvme0,/dev/nvme0n1,/dev/nvme1,/dev/nvme1n1"
    NETINT1="/dev/nvme2,/dev/nvme2n1,/dev/nvme3,/dev/nvme3n1"
    ```

**Querying the NUMA Node to Which an AMD GPU Rendering Node Belongs<a name="section20575115322416"></a>**

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>Each AMD GPU corresponds to one GPU rendering node.

1. <a name="li34656503552"></a>Query GPU rendering nodes.

    ```shell
    ll /dev/dri/by-path/ | grep renderD
    ```

    Example command output:

    ```shell
    lrwxrwxrwx 1 root root 13 Oct 25 10:58 pci-0000:03:00.0-render -> ../renderD128
    lrwxrwxrwx 1 root root 13 Oct 25 10:58 pci-0000:83:00.0-render -> ../renderD129
    ```

    This indicates that two AMD GPUs are inserted into the server, and the rendering nodes are `renderD128` and `renderD129`.

2. Query the NUMA node to which a GPU rendering node belongs.

    ```shell
    cat /sys/bus/pci/devices/0000\:XX\:00.0/numa_node 
    ```

    Replace *XX* in the command with the IP address of a node queried in [1](#li34656503552). Take `renderD128` as an example. The query command is as follows:

    ```shell
    cat /sys/bus/pci/devices/0000\:03\:00.0/numa_node
    ```

    Command output:

    ```shell
    0
    ```

    This indicates that `renderD128` belongs to NUMA node 0.

**Basic Functions and Usage of the Graphics Acceleration Layer<a name="section9932195417616" id="basic-functions-and-usage-of-the-graphics-acceleration-layer"></a>**

The graphics acceleration layer supports the following functions:

- GPU mock: emulates the GPU vendor, GPU model, OpenGL ES version, GL_MAX capability value, and OpenGL ES extension.
- Shader cache: pre-builds shader binaries and shares cache across multiple cloud phones to cut shader compilation and linking time, thereby reducing the stuttering of large OpenGL ES applications.

The preceding functions can be configured in the `kbox_render_accelerating_configuration.xml` file. To enable the graphics acceleration layer, perform the following steps:

1. Set `ENABLE_RENDER_LAYER` in the cloud phone startup configuration file `cfct_config` to `1`.
2. Copy the `kbox_render_accelerating_configuration.xml` configuration file from the `Kbox-patches-AOSP15.zip` software package to the `/home/kbox_video/` startup path.

    ```shell
    cp /home/kbox_video/Kbox-patches-AOSP15/deploy_scripts/kbox_render_accelerating_configuration.xml /home/kbox_video/
    ```

3. Open the configuration file and configure the graphics acceleration layer functions for an application. For details about the configuration items, see [Configuration Items of the Graphics Acceleration Layer](user_guide.md#configuration-items-of-the-graphics-acceleration-layer).

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>- After the cloud phone is started for the first time, if you need to modify the graphics acceleration layer configuration, modify the application configuration in the configuration file, manually copy the file to the `/data/local/tmp` directory of the cloud phone container, and restart the application for the modification to take effect.
>- Multiple containers on the host share the same shader cache path. You can start a cloud phone to pre-collect as many shaders as possible. For other cloud phones, you can enable the shader cache function by setting the corresponding applications in the configuration file to read-only mode. In this case, the performance is optimal.
>- The shader cache function has no cache eviction mechanism. If the cache file system storage is full or the game application needs to be updated, clear the entire file system's cache to avoid a mismatch between the shaders and binary files.

#### 1.2.5 Setting the cfct_config File (Configuration Scheme 2/3/4)<a name="ZH-CN_TOPIC_0000002518386592"></a>

You can use the `cfct_config` file to flexibly configure the resources used by the video stream cloud phone to achieve optimal performance. Before starting a cloud phone, ensure that the `cfct_config` file is stored in the startup path and the configurations in the file are correct. The cloud phone container uses the configurations in this file.

The configuration items in the `cfct_config` file and the configuration method are as follows:

1. Extract the `cfct_config` file and grant permissions on the file. Ensure that the file owner has read and write permissions while users in the owner group and others have only the read permission.

    ```shell
    cd /home/kbox_video/
    tar -xvf DemoVideoEngine.tar.gz cfct_config
    chmod 644 cfct_config
    ```

2. Change map configurations such as `GPU`, `CPU`, and `USERDATA` of the corresponding channel to select the GPU, CPU, and path to the data volume used by the container of this channel.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >To ensure the stable running and optimal performance of the video stream cloud phone, ensure that the physical CPU cores and GPU rendering nodes bound to a container belong to the same CPU socket.

3. If only one GPU is used in your environment, change the value of `VIDEO_CPU_MAP_{total_CPU_core_count}CORE_MODE{CPU_BIND_MODE}` in the `cfct_config` file.

    Take `VIDEO_CPU_MAP_128CORE_MODE0` as an example. Retain the configuration of the CPU bound to the GPU and delete other configurations. If the GPU is inserted into CPU 0, delete all references related to `MODE0_CPUS2` and `MODE0_CPUS3`. If the GPU is inserted into CPU 1, delete all references related to `MODE0_CPUS0` and `MODE0_CPUS1`.

    - Checking the number of GPUs in the current environment

        Query the information of the DaoCloud DC1000 GPU on the server.

        ```shell
        lspci -D | grep 0200
        ```

        The following example output indicates that only one DaoCloud DC1000 GPU exists on the server. `0000:04:00.0` is the bus number.

        ```shell
        0000:04:00.0 3D controller: Device 1f4f:0200
        0000:04:00.1 3D controller: Device 1f4f:0200
        0000:04:00.2 3D controller: Device 1f4f:0200
        0000:04:00.3 3D controller: Device 1f4f:0200
        ```

    - Checking the binding relationship between GPUs and CPUs

        Query the NUMA node to which the GPU belongs.

        ```shell
        lspci -vvvs {busID} | grep NUMA
        ```

        The following example output indicates that the GPU is bound to CPUs in NUMA node 0.

        ```shell
        NUMA node: 0
        ```

4. Enable the graphics acceleration layer as required. For details, see [Basic Functions and Usage of the Graphics Acceleration Layer](#basic-functions-and-usage-of-the-graphics-acceleration-layer).

#### 1.2.6 Creating a Base Data Volume<a name="ZH-CN_TOPIC_0000002518386594" id="creating-a-base-data-volume"></a>

Confirm or adjust the default image name and data volume storage directory as required. Delete or back up the existing data volume. After that, extract the startup script, set permissions, and run the script to start a cloud phone and pre-install an application. Finally, delete the initial container.

1. <a name="li16219132415811"></a>Determine the directory where the data volume is stored and the image name.

    The default image name is `video:latest`, and the default directory for storing the data volume is `/home/mount`. You can change the values of `DOCKER_IMAGE` and `USERDATA` in the `cfct_config` file to the actual name and directory, respectively.

    ```shell
    DOCKER_IMAGE=video:latest
    USERDATA="/home/mount"
    ```

2. Delete the original data volume or back up the data volume to another location. *{USERDATA}* indicates the directory for storing the data volume configured in [1](#li16219132415811). If there are multiple directories, perform the following operations in this section for each directory.

    ```shell
    rm -rf ${USERDATA}/data/android_base
    ```

3. Extract the `cfct_video` startup script from `DemoVideoEngine.tar.gz` and grant permissions on the script. Ensure that the file owner has the read, write, and execute permissions while users in the owner group and others have only read and execute permissions.

    ```shell
    cd /home/kbox_video/
    tar -xvf DemoVideoEngine.tar.gz cfct_video
    chmod 755 cfct_video
    ```

4. Use the `cfct_video` script to start a cloud phone.

    ```shell
    ./cfct_video start ${index}  
    ```

5. Pre-install an app (for example, Subway Surfers) in the cloud phone container, and use `android_${index}` as a new data volume for starting the video stream cloud phone.

    ```shell
    cd ${USERDATA}/data/
    cp -rp android_${index} android_base
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >If containers are started using NFS mounts, copying the data directory directly via `cp -rp` is not supported due to performance considerations. Instead, you should copy the image.
    >
    >Pre-install the required application (such as Subway Surfers) into the cloud phone container, then copy `android_${index}.img` as `android_base.img` to serve as the new data volume.
    >
    >```shell
    >cd ${USERDATA}/img/
    >cp -rp android_${index}.img android_base.img
    >```
    >
    >Manually copy `android_base.img` to the corresponding container index before starting the specified container.
    >
    >```shell
    >cd ${USERDATA}/img/
    >cp -rp android_base.img android_${index}.img
    >```

6. Delete the `android_${index}` container.

    ```shell
    cd /home/kbox_video/
    ./cfct_video delete ${index}
    ```

### 1.3 Video Stream Cloud Phone Deployment in a Kubernetes Cluster (Configuration Scheme 2)<a name="ZH-CN_TOPIC_0000002518226646"></a>

#### 1.3.1 Preparing the Environment<a name="ZH-CN_TOPIC_0000002549866425"></a>

You can start video stream cloud phones with containerd and manage them as a Kubernetes cluster. To deploy video stream cloud phones in a Kubernetes cluster, at least two servers are required. One server functions as the master node, and the others function as worker nodes.

[Table 1](#kubernetes-cluster-nodes) describes the node planning.

**Table 1** Kubernetes cluster nodes<a id="kubernetes-cluster-nodes"></a>

|Node Name (Host Name, Customizable)|Node Role|Number of Servers|Environment Preparation|Node Function|
|--|--|--|--|--|
| k8s-master | Master node| 1| Install openEuler 24.03 LTS SP1 on the Kunpeng server.| Coordinates and manages various resources in the cluster to achieve high availability, high scalability, and automatic O&M. This node does not run cloud phone services.|
| k8s-slave1 | Worker node| ≥ 1| Deploy the video stream cloud phone environment based on section "2 Software Deployment".| Runs cloud phone services.|

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>- Kubernetes is a container orchestration platform. The worker nodes need to run cloud phone services. Before deploying Kubernetes or after restarting a node, ensure that the video stream cloud phone environment has been deployed on the worker nodes. You can start a video stream cloud phone to check whether the environment deployment is complete.
>- To deploy a Kubernetes cluster environment and images, you need to pull images from the Docker image repository. Ensure that the server network is able to pull images from the Docker image repository.

#### 1.3.2 Setting Up a Kubernetes Cluster<a name="ZH-CN_TOPIC_0000002518386560"></a>

##### 1.3.2.1 Common Operations on All Nodes<a name="ZH-CN_TOPIC_0000002518386570"></a>

Install Kubernetes cluster software, configure containerd, and perform other related operations on all master and worker nodes.

1. Change the host names to ensure that the host name of each server is unique.

    Example:

    - On the master node, change the host name to `k8s-master`.

        ```shell
        hostnamectl set-hostname k8s-master
        bash
        ```

    - On a worker node, change the host name to `k8s-slave1`.

        ```shell
        hostnamectl set-hostname k8s-slave1
        bash
        ```

2. Change the passwords of all servers to the same.
3. Disable the firewall.

    ```shell
    systemctl stop firewalld
    systemctl disable firewalld
    ```

4. Disable SWAP partitions.
    - Run the following command. The setting becomes invalid after the server restarts.

        ```shell
        swapoff -a   
        ```

    - Comment out the code for automatic mounting of SWAP partitions in the `fstab` file. The setting is still valid after the server restarts.

        ```shell
        sed -i "/\/dev\/mapper\/openeuler-swap/ s|^|#|" /etc/fstab
        ```

5. Configure the repository for installing Kubernetes cluster software.

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

6. Install Kubernetes cluster software.

    ```shell
    yum install -y kubelet kubeadm kubectl kubernetes-cni --disableexcludes=kubernetes
    systemctl enable --now kubelet
    ```

7. Install containerd and runC based on [1](#deploying-the-containerd-environment-1) to [3](#deploying-the-containerd-environment-3) in [(Optional) Deploying the Containerd Environment](#deploying-the-containerd-environment). After the installation is successful, restart the Docker service based on [6](#deploying-the-containerd-environment-6) on the worker node.
8. Modify the containerd configuration file.

    ```shell
    mkdir -p /etc/containerd/
    cd /etc/containerd/
    containerd config default > /etc/containerd/config.toml
    sed -i "s|SystemdCgroup =.*|SystemdCgroup = true|g" /etc/containerd/config.toml
    ```

9. Configure crictl and restart containerd.

    ```shell
    echo "runtime-endpoint: unix:///run/containerd/containerd.sock" >> /etc/crictl.yaml
    echo "image-endpoint: unix:///run/containerd/containerd.sock" >> /etc/crictl.yaml
    echo "timeout: 10" >> /etc/crictl.yaml
    systemctl daemon-reload
    systemctl restart containerd
    ```

10. Install the yq tool to dynamically adjust the YAML file using a script.

    ```shell
    wget https://github.com/mikefarah/yq/releases/latest/download/yq_linux_arm64 --no-check-certificate -O /usr/local/bin/yq
    chmod +x /usr/local/bin/yq
    ```

11. Configure network forwarding. Perform this step again after the server is restarted.

    ```shell
    modprobe overlay
    modprobe br_netfilter
    modprobe xt_multiport
    echo "net.bridge.bridge-nf-call-ip6tables=1" >> /etc/sysctl.d/k8s.conf
    echo "net.bridge.bridge-nf-call-iptables=1" >> /etc/sysctl.d/k8s.conf
    echo "net.ipv4.ip_forward=1" >> /etc/sysctl.d/k8s.conf
    sysctl -p /etc/sysctl.d/k8s.conf
    ```

##### 1.3.2.2 Operations on the Master Node<a name="ZH-CN_TOPIC_0000002549746441"></a>

Initialize the cluster on the master node.

1. Download necessary images.

    ```shell
    kubeadm config images pull 
    ```

    If no error is reported, the download is successful.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >Configure an image repository if required, for example:
    >
    >```shell
    >kubeadm config images pull --image-repository registry.aliyuncs.com/google_containers
    >```

2. Modify the containerd image configuration in the `config.toml` configuration file based on the `pause` version in the pulled image information. Run the following command to check the `pause` version:

    ```shell
    crictl images
    ```

    **Figure 1** Image pull information<a id="image-pull-information"></a>

    ![](figures/image-pull-information.png)

    The following uses `registry.aliyuncs.com/google_containers/pause:3.9` in [**Figure 1**](#image-pull-information) as an example:

    ```shell
    sed -i 's|sandbox_image =.*|sandbox_image = "registry.aliyuncs.com/google_containers/pause:3.9"|g' /etc/containerd/config.toml
    ```

3. Restart containerd.

    ```shell
    systemctl restart containerd
    ```

4. Initialize the cluster.

    ```shell
    kubeadm init --pod-network-cidr=10.244.0.0/16
    ```

    After the initialization is complete, information shown in [**Figure 2**](#cluster-initialization-success) is displayed.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >If an image repository is configured for image download, the same image repository must be configured during cluster initialization. For example:
    >
    >```shell
    >kubeadm init --pod-network-cidr=10.244.0.0/16 --image-repository registry.aliyuncs.com/google_containers
    >```

    **Figure 2** Cluster initialization success<a id="cluster-initialization-success"></a>
    
    ![](figures/zh-cn_image_0000002549746463.png)

    Run the commands in the yellow box in [**Figure 2**](#cluster-initialization-success) to configure the cluster. Content in the red box indicates the token command for a worker node to join the cluster. Please save this command.

    ```shell
    rm -rf $HOME/.kube
    mkdir -p $HOME/.kube
    sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
    sudo chown $(id -u):$(id -g) $HOME/.kube/config
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >If cluster initialization fails on the master node, locate the cause as prompted, reset the node, and run the initialization command again. Reset commands:
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

5. Start the kube-flannel network plugin.

    Obtain `DemoVideoEngine.tar.gz` based on [1.1.2.2-Video Stream Engine](#video-stream-engine) and upload it to the `/home/k8s` directory on the server.

    ```shell
    cd /home/k8s
    tar -xvf DemoVideoEngine.tar.gz
    cd /home/k8s/k8s/script
    kubectl apply -f kube-flannel.yml
    ```

6. Check the cluster status.

    1. Check the status of the current node.

        ```shell
        kubectl get nodes -A -o wide
        ```

        It is expected that the `STATUS` column of the master node is `Ready` and the `CONTAINER-RUNTIME` column is `containerd://x.x.x`.

    2. Check the Pod status.

        ```shell
        kubectl get pod -A -o wide
        ```

        It is expected that the `STATUS` column of all Pods is `Running`.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >If the status of the current node is `NotReady` and the error message "Network plugin returns error: cni plugin not initialized" is displayed when you run the `systemctl status kubelet` command to check the kubelet service status, you are advised to reset the cluster, restart the server, and initialize the cluster again.

##### 1.3.2.3 Operations on a Worker Node<a name="ZH-CN_TOPIC_0000002549746443" id="operations-on-a-worker-node"></a>

Add a worker node to the cluster.

Obtain `DemoVideoEngine.tar.gz` based on [1.1.2.2-Video Stream Engine](#video-stream-engine) and upload it to the `/home/k8s` directory on the server.

1. Configure container storage isolation and storage size. Perform this step again after the server is restarted.

    ```shell
    cd /home/k8s
    tar -xvf DemoVideoEngine.tar.gz k8s/
    cd /home/k8s/k8s/DevicesPlugin
    chmod +x storage_manager.sh
    ./storage_manager.sh $ACTION $STORAGE_START_INDEX $STORAGE_END_INDEX $STORAGE_SIZE_GB $IMG_BASE
    ```

    [**Table 1**](#parameters-for-configuring-container-storage-isolation-and-storage-size) describes the command parameters.

    **Table 1** Parameters for configuring container storage isolation and storage size<a id="parameters-for-configuring-container-storage-isolation-and-storage-size"></a>

    |Parameter|Description|
    |--|--|
    | ACTION | Specifies the action. The value can be `create` or `delete`.|
    | STORAGE_START_INDEX | Specifies the start index of the data volume to be deleted or created.|
    | STORAGE_END_INDEX | Specifies the end index of the data volume to be deleted or created. The end index must be greater than or equal to the start index.|
    | STORAGE_SIZE_GB | Specifies the storage size, in GB. This parameter can be left blank when the action is `delete`.|
    | IMG_BASE | Specifies the image file of the base data volume. If there is no base data volume, this parameter can be left blank. For details about how to create an image file of the base data volume, see [Creating a Base Data Volume](#creating-a-base-data-volume). This parameter can be left blank when the action is `delete`. Transfer either `STORAGE_SIZE_GB` or `IMG_BASE`.|

    Example:
        
    Create 100 isolated data volumes from `video1` to `video100`, each with a storage size of 32 GB and the default `ext4` file system format.
        
    ```shell
    ./storage_manager.sh create 1 100 32
    ```
        
    Create 100 isolated data volumes from `video1` to `video100`, each with a storage size of 32 GB and the `f2fs` file system format.
        
    ```shell
    ./storage_manager.sh fcreate 1 100 32
    ```
        
    Add 20 isolated data volumes from `video101` to `video120`, each with a storage size of 32 GB and the default `ext4` file system format.
        
    ```shell
    ./storage_manager.sh create 101 120 32
    ```

    Add 20 isolated data volumes from `video101` to `video120`, each with a storage size of 32 GB and the `f2fs` file system format.
        
    ```shell
    ./storage_manager.sh fcreate 101 120 32
    ```
        
    Delete data volumes `video1` to `video100`.
        
    ```shell
    ./storage_manager.sh delete 1 100
    ```
        
    Delete the remaining 20 data volumes (`video101` to `video120`).
        
    ```shell
    ./storage_manager.sh delete 101 120
    ```
        
    Use `videobase.img` to create data volumes `video1` to `video100` with the default `ext4` file system format.
        
    ```shell
    ./storage_manager.sh create 1 100 /home/mount/img/videobase.img
    ```

    Use `videobase.img` to create data volumes `video1` to `video100` with the `f2fs` file system format.
        
    ```shell
    ./storage_manager.sh fcreate 1 100 /home/mount/img/videobase.img
    ```
        
    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >If you have run commands in this step and want to change the storage size of a data volume, you need to delete the data volume and then create it again with the required size.
    >
    >The file format of the created data volume must match the configuration used when `k8s-video.sh` starts the Pod. For example, if a data volume in f2fs format is created for `video1` using `fcreate`, the f2fs switch must be set to `1` when starting `video1` with the startup script `k8s-video.sh`.

2. Modify the containerd image configuration in the `config.toml` configuration file based on the `pause` version in the image information pulled on the master node. `registry.aliyuncs.com/google_containers/pause:3.9` in [**Figure 1** Image pull information](#image-pull-information) is used as an example.

    ```shell
    sed -i 's|sandbox_image =.*|sandbox_image = "registry.aliyuncs.com/google_containers/pause:3.9"|g' /etc/containerd/config.toml
    ```

3. Run the token command (saved after cluster initialization on the master node) for a worker node to join the cluster, as shown in the red box in [**Figure 2** Cluster initialization success](#cluster-initialization-success).

    Example:

    ```shell
    kubeadm join xx.xx.xx.xx:xxxx --token 7h0hpd.1av4cdcb4fb0on5x \
    --discovery-token-ca-cert-hash sha256:357c6d1dbefe6f7adf3c80987a90d3765965b1c43e1757b655ea8586c8ade10a
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >- After a worker node is restarted and added to the cluster, ensure that the worker node can run the video stream cloud phone.
    >- *xx*.*xx*.*xx*.*xx* indicates the IP address, and *xxxx* indicates the mapped port.
    >- If the token command for joining the cluster is invalid, run the following command on the master node to generate a new one:
    >
    >    ```shell
    >    kubeadm token create --print-join-command
    >    ```

4. Copy the kube configuration file of the master node to the worker node.

    ```shell
    rm -rf $HOME/.kube
    mkdir -p $HOME/.kube
    sudo scp root@xxx.xxx.xxx.xxx:$HOME/.kube/config $HOME/.kube/config
    sudo chown $(id -u):$(id -g) $HOME/.kube/config
    ```

5. Check the cluster status.
    1. Check the status on the master node.

        ```shell
        kubectl get nodes -A -o wide
        ```

        It is expected that the `STATUS` column of this worker node is `Ready` and the `CONTAINER-RUNTIME` column is `containerd://x.x.x`.

    2. Check the Pod status on the master node.

        ```shell
        kubectl get pod -A -o wide
        ```

        It is expected that the `STATUS` column of all Pods on the worker node is `Running`.

    3. Check the container status on the worker node.

        ```shell
        crictl ps
        ```

        It is expected that the `STATE` column of all containers is `Running`.

6. (Optional) Configure NUMA affinity.
    1. When configuring the compilation environment and compiling plugins, ensure that the Golang version is 1.23 or later. Place the `go` directory of Golang 1.23 in `/usr/lib`, and place `go/bin/go` and `go/bin/gofmt` in `/usr/bin`.

        ```shell
        systemctl stop kubeletexport GOROOT=/usr/lib/go
        go env -w GO111MODULE=on
        go env -w GOPROXY=https://goproxy.io,direct
        ```

    2. Obtain the Kubernetes NUMA affinity plugin package `topo-affinity-plugin-master.zip` based on [1.1.2.2-Video Stream Engine](#video-stream-engine) and upload it to the `/home/k8s` directory on the server.
    3. Decompress `topo-affinity-plugin-master.zip`, go to the package directory, and compile the plugin.

        ```shell
        unzip topo-affinity-plugin-master.zip
        cd topo-affinity-plugin-master
        go mod tidy
        make build
        ```

        After the build is complete, ensure that the `kunpeng-tap` binary file is generated in the `bin` directory.

    4. Install the containerd runtime.

        ```shell
        make install-service-containerd
        ```

        If you need to modify the startup parameters, modify the parameters under `ExecStart=` in the `hack/kunpeng-tap.service.containerd` file in the source code directory. [**Table 2**](#startup-parameters) describes the parameters.

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

        **Table 2** Startup parameters<a id="startup-parameters"></a>

        |Parameter|Description|Default Value|Remarks|
        |--|--|--|--|
        |container-runtime-mode|Container runtime connected to the plugin, which can be Docker or containerd.|Containerd|Determine the container runtime according to that used in the Kubernetes cluster.|
        |resource-policy|Container resource optimization policy. Currently, numa-aware and topology-aware are supported. numa-aware supports CPU NUMA affinity for containers of the Burstable type. topology-aware provides CPU affinity at the socket, die, and NUMA levels, and supports memory and GPU optimization configurations.|topology-aware|Select a policy as required.|
        |v|Log level. The value ranges from 2 to 5.|2|The higher the level, the more detailed the logs.|

    5. Start the TAP service.

        ```shell
        make start-service
        ```

        After the startup is successful, the value of `Status` in the command output is `active`.

    6. Modify and restart kubelet.
        1. Before the restart, ensure that no container is deployed on the node.
        2. Modify the kubelet configuration file `/var/lib/kubelet/kubeadm-flags.env`.

            The initial configuration is as follows:

            ```shell
            KUBELET_KUBEADM_ARGS="... --container-runtime=remote --container-runtime-endpoint=unix:///var/run/containerd/containerd.sock ..."
            ```

            Modify the file as follows:

            ```shell
            KUBELET_KUBEADM_ARGS="... --container-runtime=remote --container-runtime-endpoint=unix:///var/run/kunpeng/tap-runtime-proxy.sock ..."
            ```

    7. Restart kubelet and check whether the restart is successful.

        ```shell
        systemctl daemon-reload
        systemctl restart kubelet
        systemctl status kubelet
        ```

        >![](public_sys-resources/icon-note.gif) **NOTE**
        >
        >To uninstall the TAP plugin, perform the following steps:
        >- Restore the `/var/lib/kubelet/kubeadm-flags.env` file to its initial content, and then restart kubelet.
        >
        >    ```shell
        >    systemctl daemon-reload
        >    systemctl restart kubelet
        >    systemctl status kubelet
        >    ```
        >
        >- Go to the `topology-affinity-plugin` source code directory and run the following commands to uninstall the plugin:
        >
        >    ```shell
        >    cd /home/k8s/topo-affinity-plugin-master
        >    make uninstall-service
        >    ```

#### 1.3.3 Deploying Images<a name="ZH-CN_TOPIC_0000002549746431"></a>

##### 1.3.3.1 Deploying the DaoCloud Device Plugin Image<a name="ZH-CN_TOPIC_0000002518386564" id="deploying-the-daocloud-device-plugin-image"></a>

Deploy the DaoCloud device plugin image on all worker nodes.

DaoCloud provides this plugin. This document applies to plugin version v0.0.5. Obtain related installation documents and software packages, and deploy the DaoCloud device plugin according to the documents.

1. Obtain the `VAGPU-25.03.01.01-RC13-A15.tgz` GPU driver package according to "Software Environment" in [Kbox Cloud Phone Container Installation Guide (Android 15)](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/kboxcpc_ad15/docs/en/install_guide.md#d22-software-environment). Decompress the package to obtain the `k8s-v0.0.5-1.tar.gz` package.
2. Decompress `k8s-v0.0.5-1.tar.gz` to obtain installation documents and software packages.
3. Install Va Docker based on "Installing Va Docker" in the *DC1000 Accelerator Card Va Docker Installation Guide 01*.
4. Install the device plugin based on chapter 3 (from the beginning to section 3.2.3) in the *DC1000 Accelerator Card Kubernetes Device Plugin Installation Guide 03*.

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>The DaoCloud device plugin version should be v0.0.5 or later.

##### 1.3.3.2 Deploying the Device Plugin Image<a name="ZH-CN_TOPIC_0000002518226672"></a>

Deploy the device plugin image on all worker nodes.

1. Install Golang (later than 1.17).

    ```shell
    yum install golang
    ```

2. Download the device plugin code and switch to the specified commit ID.

    ```shell
    git clone https://github.com/everpeace/k8s-host-device-plugin.git
    cd  k8s-host-device-plugin
    git checkout 15e0a180dd4fbea7ea09b563b9e0713d3b90579a
    ```

3. Apply `device-plugin.patch`.

    Copy `device-plugin.patch` (in the `k8s/DevicesPlugin` folder of `DemoVideoEngine.tar.gz`) to the `k8s-host-device-plugin` directory.

    ```shell
    cd k8s-host-device-plugin
    patch -p1 < device-plugin.patch
    ```

4. Change the Go image repository address and compile the device plugin.

    ```shell
    export GOPROXY=https://goproxy.cn
    go build
    ```

5. Build an image.

    ```shell
    docker build -f Dockerfile  -t k8s-hostdev-plugin:0.1 .
    docker save k8s-hostdev-plugin:0.1 -o k8s-hostdev-plugin.tar
    ```

6. Import the image.

    Copy `k8s-hostdev-plugin.tar` to all worker nodes and import the image.

    ```shell
    ctr -n k8s.io images import k8s-hostdev-plugin.tar
    ```

##### 1.3.3.3 Deploying the Video Stream Image<a name="ZH-CN_TOPIC_0000002549746415"></a>

Create a video stream image on a worker node, and import and deploy the image on all worker nodes.

1. Place the `DemoVideoEngine.tar.gz` software package in a specified directory. Assume that this package is placed in `/home/k8s`.

    ```shell
    mkdir -p /home/k8s/tmp 
    cd /home/k8s/tmp 
    tar -xvf  ../DemoVideoEngine.tar.gz
    ```

2. <a id="deploying-the-video-stream-image-2"></a>Change the encoder type and re-create a `DemoVideoEngine.tar.gz` software package.

    In the `default.prop` file, the default encoder type is `1`. However, the encoder type required by DaoCloud is `2`. Therefore, you need to repack the package after modifying the value. In addition, you can modify the frame rate in the `default.prop` file. After the modification, you need to rebuild an image. Ensure that the image can run a cloud phone properly with Docker.

    1. Open the `default.prop` file.

        ```shell
        vi vendor/default.prop
        ```

    2. Press **i** to enter the insert mode and change `vmi.video.encodertype` to `2` and `vmi.video.encode.rcmode` to `2`.
    3. Press **Esc**, type `:wq!`, and press **Enter** to save the settings and exit.
    4. Re-create a `DemoVideoEngine.tar.gz` software package.

        ```shell
        tar -zcvf DemoVideoEngine.tar.gz  *
        ```

3. Use `DemoVideoEngine.tar.gz` created in [2](#deploying-the-video-stream-image-2) to re-create a video stream image based on [1.2.3-Creating an Image](#creating-an-image). Assume that the name of the created image is `video:version`.
4. Run `docker` to export the video stream image.

    ```shell
    docker save video:version -o video.tar
    ```

5. Copy the video stream image to all worker nodes and import the image.

    ```shell
    ctr -n k8s.io images import video.tar
    ```

    You can run the `crictl images` command to view the image name and tag. For example: `docker.io/library/video:version`.

## 2 VM Environment Deployment<a name="ZH-CN_TOPIC_0000002550093497"></a>

### 2.1 Environment Requirements<a name="ZH-CN_TOPIC_0000002518653638"></a>

You are advised to set up the VM environment of the video stream engine on the new Kunpeng 920 processor model. Before setting up the environment, ensure that your hardware environment meets the requirements.

**Hardware Requirements<a name="zh-cn_topic_0000002518345410_section217mcpsimp"></a>**

[**Table 1**](#hardware-requirements) describes the hardware requirements for deploying the Kbox Android container environment. [**Table 2**](#kunpeng-server-specifications) describes the hardware specifications.

**Table 1** Hardware requirements<a id="hardware-requirements"></a>

|No.|Device Model|Description|
|--|--|--|
| 1 | New Kunpeng 920 processor model| Used to set up the VM environment for running video stream containers and cloud phones.|

**Table 2** Kunpeng server specifications<a id="kunpeng-server-specifications"></a>

|Item|Sub-item|Specification|
|--|--|--|
|CPU|-|2 x new Kunpeng 920 processor model, 80 cores@2.9 GHz|
|Memory|-|16 x DDR5 RDIMM, 64 GB, 4800 MT/s|
|Drive|System drive|ES3600C V5, 6400 GB, NVMe SSD|
|Data drive|ES3600C V5, 6400 GB, NVMe SSD|
|NIC|Onboard|1 x NIC (4 x GE); 1 x 5902L onboard flexible NIC|
|Riser card|-|1 x x16 slot (PCIe x16) + 2 x x8 slot (PCIe x8)-riser modules 1 and 2, 2 x x8 slot (PCIe x8)-rear riser module|
|GPU|-|4 x DaoCloud DC1000|
|OS|-|openEuler 24.03 LTS SP1|
|System/Kernel version|-|6.6.0-72.0.0|

**Table 3** VM specifications<a id="vm-specifications"></a>

|VM Quantity|CPU Quantity per VM|Memory per VM|Drive Capacity per VM|
|--|--|--|--|
|4|80|180 GiB|512 GiB|

>![](public_sys-resources/icon-note.gif) **NOTE**
>
>The VM memory and drive capacity are for reference only. Adjust the configurations as required.

**OS Requirements<a name="zh-cn_topic_0000002518345410_section305mcpsimp"></a>**

[**Table 4**](#host-os-requirements) and [**Table 5**](#vm-os-requirements) list the OS requirements of the host and VM.

**Table 4** Host OS requirements<a id="host-os-requirements"></a>

|Item|Version|How to Obtain|
|--|--|--|
|openEuler|24.03 LTS SP1|[Link](https://www.openeuler.org/en/download/archive/detail/?version=openEuler%2024.03%20LTS%20SP1)|
|Kernel|Based on 6.6.0-72.0.0| [Link](https://gitee.com/openeuler/kernel/repository/archive/6.6.0-72.0.0.zip)|

**Table 5** VM OS requirements<a id="vm-os-requirements"></a>

|Item|Version|How to Obtain|
|--|--|--|
|openEuler|24.03 LTS SP1|[Link](https://www.openeuler.org/en/download/archive/detail/?version=openEuler%2024.03%20LTS%20SP1)|
| Kernel | Based on 6.6.0-72.0.0| For details, see "Software Deployment > Compiling the Kernel" in the *Kbox Cloud Phone Container Feature Guide (Android 15)*.|

**Software Requirements<a name="zh-cn_topic_0000002518345410_section1543425619147" id="software-requirements"></a>**

[**Table 6**](#files-required-for-vm-deployment) lists the patch and script required for VM deployment.

**Table 6** Files required for VM deployment<a id="files-required-for-vm-deployment"></a>

|Software Package|File|File Path|How to Obtain|
|--|--|--|--|
|Kbox-patches-AOSP15|VM kernel patch|Kbox-patches-AOSP15/deploy_scripts/vm_deploy/patchForKernel/general.patch|[Link](https://gitcode.com/boostkit/Kbox-patches/tree/AOSP15)|

### 2.2 Host Environment Configuration<a name="ZH-CN_TOPIC_0000002518493742"></a>

#### 2.2.1 Modifying BIOS Settings<a name="ZH-CN_TOPIC_0000002549973491"></a>

You can modify BIOS configuration items of the host to optimize the host for deploying the VM environment.

[**Table 1**](#bios-configuration-items) describes the BIOS configuration items for the host.

**Table 1** BIOS configuration items<a id="bios-configuration-items"></a>

|Item|Path|Value|
|--|--|--|
|SMMU|Advanced > MISC Configuration > Support Smmu|Enabled|
|CPU hyper-threading|Advanced > Power And Performance Configuration > CPU PM Control > SMT2|Enabled|
|Performance policy|Advanced > Power And Performance Configuration > Power Policy|Performance|
|GICv4.1|Advanced >Processor Configuration >GIC Version|4.1|

#### 2.2.2 Modifying the Kernel Module<a name="ZH-CN_TOPIC_0000002550093499"></a>

In the DaoCloud DC1000 hardware environment, the host kernel needs to be adapted for driver installation on the VM. Obtain the kernel source code in advance.

1. Obtain the kernel source code. See [Table 4](#host-os-requirements).
2. Decompress the kernel source code and go to the root directory.

    ```shell
    unzip kernel-6.6.0-72.0.0.zip
    cd kernel-6.6.0-72.0.0
    ```

3. Suppress the local version number.

    ```shell
    touch .scmversion
    ```

4. Obtain the kernel patch file `general.patch` according to [2.1-Software Requirements](#software-requirements).
5. Apply the patch in the kernel source code directory `kernel-6.6.0-72.0.0`.

    ```shell
    patch -p1 < general.patch
    ```

6. Generate a .config file in the source code directory.

    ```shell
    cp /boot/config-`uname -r` .config
    make menuconfig
    ```

7. On the displayed configuration page, select `Load`.

    ![](figures/1_zh-cn_image_0000002085094168.png)

8. Select `OK` in the following page:

    ![](figures/1_zh-cn_image_0000002120534141.png)

9. On the kernel configuration interface, configure the kernel compilation options listed in [**Table 1**](#kernel-compilation-options).

    **Table 1** Kernel compilation options<a id="kernel-compilation-options"></a>

|Option|Required Value|
|--|--|
|LOCALVERSION|-patched-vm|
|DEBUG_INFO_BTF|N|
|SYSTEM_TRUSTED_KEYS|Clear the content. The configuration result should be: `( ) Additional X.509 keys for default system keyring`|

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >Configuration methods:
    >- Press **/** for search.
    >- Press **Y** to compile the selected item into the kernel. The corresponding item is displayed as `[*]`.
    >- Press **N** to exclude the selected item. The corresponding item is displayed as `[]`.
    >- Press **M** to compile the selected item into a module (in KO format). The corresponding item is displayed as `<M>`.
    >- Press **Enter** to edit the selected item.
    >- Press the corresponding number to select a search result.
    >- After the modification is complete, select `<Save>` to save the modification.
    >- After saving the modification, select `<Exit>` to exit.

10. Install dependencies and enable the LXCFS service. If you need to divide the command into multiple lines, add a backslash `\` at the end of each line.

    ```shell
    yum install -y dwarves dpkg dpkg-devel openssl openssl-devel ncurses ncurses-devel bison flex bc libdrm build elfutils-libelf-devel docker lxc lxcfs lxcfs-tools git tar patch make gcc
    systemctl start lxcfs
    systemctl enable lxcfs
    ```

    ![](figures/zh-cn_image_0000002549825479.png)

11. Compile the kernel code.

    ```shell
    make -j72
    ```

12. Install the new kernel.

    ```shell
    make modules_install 
    make install
    ```

13. Set the boot kernel.

    ```shell
    grub2-set-default 'openEuler (6.6.0-patched-vm) 24.03 (LTS-SP1)'
    ```

14. Reboot the server.

    ```shell
    reboot
    ```

15. After the reboot, confirm that the kernel is switched to `6.6.0-patched-vm`.

    ```shell
    uname -r
    ```

#### 2.2.3 Installing VM Dependencies<a name="ZH-CN_TOPIC_0000002518493744"></a>

Install dependencies required for VM environment deployment on the host.

1. Install libvirt, virt-manager, and other dependencies.

    ```shell
    yum install libvirt virt-manager edk2-aarch64 sshpass mesa-libGLES-devel mesa-dri-drivers virt-install -y
    systemctl start libvirtd
    ```

2. Install the x11 server to enable the virt-manager GUI.

    ```shell
    yum install xorg-x11-server
    ```

3. Open the sshd configuration file.

    ```shell
    vi /etc/ssh/sshd_config
    ```

4. Press **i** to enter the insert mode and set `X11Forwarding` to `yes`.
5. Press **Esc** to exit the insert mode. Type `:wq!` and press **Enter** to save the settings and exit.
6. Restart the sshd service.

    ```shell
    systemctl restart sshd
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >In the event of a `virt-manager` execution error, re-initiate an SSH terminal.

#### 2.2.4 Querying PCIe Node Information of a GPU<a name="ZH-CN_TOPIC_0000002549973493"></a>

The new Kunpeng 920 processor model has four NUMA nodes. Four VMs will be created, and each VM uses resources of a NUMA node of the host. Each NUMA node corresponds to two GPUs. To prevent performance loss caused by cross-NUMA access, you need to confirm the PCIe nodes of GPUs used by each VM before creating VMs.

1. <a name="zh-cn_topic_0000002518185514_p644mcpsimp"></a>Check the IDs of all PCIe nodes of the Hantro GPUs.

    ```shell
    lspci | grep 0200
    ```

    ![](figures/zh-cn_image_0000002518185712.png)

2. Check the NUMA node of each GPU node. The NUMA IDs in the command output map to the GPU node IDs queried in [1](#zh-cn_topic_0000002518185514_p644mcpsimp). The command output in the following figure is only an example. 17:00.0 to 18:00.3 (the first eight nodes) correspond to NUMA node 1 of the host.

    ```shell
    lspci -vvv -d 1f4f:0200 | grep NUMA
    ```

    ![](figures/zh-cn_image_0000002518185706.png)

#### 2.2.5 Configuring the Host Network<a name="ZH-CN_TOPIC_0000002550093501" id="configuring-the-host-network"></a>

Create a host network device to support subsequent VM network configuration.

1. <a name="configuring-the-host-network-1"></a>Check the NIC used on the host.

    ```shell
    ip a
    ```

    ![](figures/zh-cn_image_0000002518185760.png)

2. Check the PCI node of the NIC.

    ```shell
    lshw -c network -businfo
    ```

    ![](figures/zh-cn_image_0000002518345672.png)

3. <a name="configuring-the-host-network-3"></a>Check the maximum number of VFs allowed by the NIC.

    ```shell
    cat /sys/bus/pci/devices/0000:75:00.0/sriov_totalvfs
    ```

    ![](figures/zh-cn_image_0000002518185756.png)

    - If this command is successfully executed, the device supports the single-root I/O virtualization (SR-IOV) solution. `7` in the command output indicates that seven vNICs can be created, and a maximum of seven VMs can use the vNICs.
    - If this command fails or the number of vNICs in the command output is less than the number of VMs to be deployed, select the second solution, that is, the bridge mode. This mode will cause extra computing performance loss and latency. To configure the bridge mode, see [6](#zh-cn_topic_0000002549705251_li9644845123820) and [7](#zh-cn_topic_0000002549705251_li55473504593).

4. Create VFs.

    ```shell
    echo 4 > /sys/bus/pci/devices/0000:75:00.0/sriov_numvfs
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >You need to perform this step every time the server is restarted. You are advised to write this step in a file such as `~/.bashrc` for automatic execution after each restart.

5. <a name="configuring-the-host-network-5"></a>Check the created VF nodes.

    ```shell
    lshw -c network -businfo
    ```

    If the following information is displayed, four vNICs are successfully created.

    ![](figures/zh-cn_image_0000002518345668.png)

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >[1](#configuring-the-host-network-1) to [5](#configuring-the-host-network-5) have completed the creation of vNICs in the SR-IOV solution. You can skip the following steps.
    >If the device does not support SR-IOV, you can perform the following steps to adopt the bridge solution.
    >For example, if the maximum number of VFs supported by the NIC in [3](#configuring-the-host-network-3) is 2 and four VMs need to be deployed, you can adopt the SR-IOV solution for two VMs and the bridge solution for the other two.

6. <a name="zh-cn_topic_0000002549705251_li9644845123820"></a>Check and back up the configuration file of the current NIC.

    ```shell
    cd /etc/sysconfig/network-scripts/
    cp ifcfg-eno5 ifcfg-eno5.bak
    ```

7. <a name="zh-cn_topic_0000002549705251_li55473504593"></a>Create a bridge configuration file `ifcfg-br0` and modify the NIC configuration file.

    Migrate the `IPADDR`, `NETMASK`, `GATEWAY`, and `DNS` information in the NIC configuration file to the bridge configuration file.

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

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >1. Modify `IPADDR`, `NETMASK`, `GATEWAY`, and `DNS` in the bridge configuration file based on the NIC configuration of the host.
    >2. Delete the `IPADDR`, `NETMASK`, `GATEWAY`, and `DNS` configurations from the host NIC configuration file and add `BRIDGE=br0` to the end of the file.
    > ![](figures/zh-cn_image_0000002518185748.png)

8. Restart the libvirtd and NetworkManager services and reboot the server.

    ```shell
    systemctl restart libvirtd
    systemctl restart NetworkManager
    reboot
    ```

9. Check whether `br0` is successfully created.

    ```shell
    ip a
    ```

    If the following information is displayed, the bridge is created.

    ![](figures/zh-cn_image_0000002518345676.png)

### 2.3 VM Configuration<a name="ZH-CN_TOPIC_0000002518653642"></a>

#### 2.3.1 Creating a VM Using virt-manager<a name="ZH-CN_TOPIC_0000002518493746"></a>

Four VMs are required in this document. You can perform operations in this section for four times in sequence.

1. Download the QCOW image provided by openEuler.

    ```shell
    wget https://mirrors.yacloud.net/openeuler/openEuler-24.03-LTS-SP1/virtual_machine_img/aarch64/openEuler-24.03-LTS-SP1-aarch64.qcow2.xz
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >Using the QCOW2 image provided by openEuler that has the OS installed can simplify operations by skipping the OS installation procedure. However, the `/boot` directory in the image uses the VFAT file system, which does not support the `ln` soft link. When the `make install` command is executed during kernel compilation and installation, the "Operation not permitted" error is reported, as shown in the following figure.
    >![](figures/zh-cn_image_0000002518345686.png)
    >Solution:
    >Go to the `/boot` directory, copy the compiled `System.map-6.6.0*` and `vmlinuz-6.6.0*` files, rename them `System.map` and `vmlinuz` respectively, and restart the server. You do not need to run `make install` again.

2. <a name="zh-cn_topic_0000002518345430_li1862819192232"></a>Decompress the QCOW package.

    ```shell
    unxz -k openEuler-24.03-LTS-SP1-aarch64.qcow2.xz
    ```

3. Open virt-manager and click the icon in the red box to open the VM configuration page.

    ```shell
    virt-manager
    ```

    ![](figures/1_zh-cn_image_0000002120521137.png)

4. Select **Import existing disk image** and click **Forward**.

    ![](figures/zh-cn_image_0000002518345648.png)

5. Choose **Browse** > **Browse Local**, select the openEuler 24.03 LTS SP1 image extracted in [2](#zh-cn_topic_0000002518345430_li1862819192232), and click **Open**. Enter **Generic Linux 2022** and click **Forward**.

    ![](figures/zh-cn_image_0000002549705547.png)

    ![](figures/zh-cn_image_0000002549705519.png)

    ![](figures/zh-cn_image_0000002549825491.png)

6. Set **Memory** to **180000** and **CPUs** to **80**, and click **Forward**.

    ![](figures/zh-cn_image_0000002518185732.png)

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >The memory setting **180000** is for reference only. You can allocate memory as required.

7. Set **Name** to **vm***X* (*X* = 0, 1, 2, or 3). If the bridge mode is configured for the host machine in [2.2.5-Configuring the Host Network](#configuring-the-host-network), select **Bridge device...** from **Network selection**, enter the bridge name, and click **Finish**.

    ![](figures/zh-cn_image_0000002549705529.png)

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >If SR-IOV is selected in [2.2.5-Configuring the Host Network](#configuring-the-host-network), you do not need to configure **Network selection**. These virtual network interfaces will be deleted later.

8. The OS boots automatically. Click the following icon to access the peripheral configuration page.

    ![](figures/zh-cn_image_0000002549825521.png)

9. On the following page, click **Add Hardware** and add the following devices in sequence.

    ![](figures/zh-cn_image_0000002549825527.png)

    1. Add peripheral 1: **Add Hardware > Input > USB Keyboard > Finish**
    2. Add peripheral 2: **Add Hardware > Input > Virtio Tablet > Finish**
    3. Add GPU PCIe devices (each VM (4 VMs in total) requires two GPUs and thus eight nodes need to be added in sequence): **Add Hardware > PCI Host Device > ***Select a node*** > Finish**
    4. If the SR-IOV solution is used in [2.2.5-Configuring the Host Network](#configuring-the-host-network), add a vNIC to the VM: **Add Hardware > PCI Host Device > ***Select a node*** > Finish**. For details about NIC nodes, see the command output in [2.2.5-5](#configuring-the-host-network-5).

        ![](figures/zh-cn_image_0000002549705511.png)

10. Create a data drive. Two methods are provided. Select either of them as required.

    **Method 1**: Use a drive image as the data drive. This method is relatively easy to implement, but its drive I/O performance is suboptimal, rendering it unsuitable for drive I/O-intensive scenarios.

    Create a drive image.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >512 GiB is for reference only. Allocate drive storage space as required.

    ![](figures/zh-cn_image_0000002549705499.png)

    **Method 2**: Mount a drive partition to improve the VM's drive I/O performance. This method is recommended for drive I/O-intensive scenarios, such as high-density cloud gaming.

    1. Mount a drive partition as the data drive of the VM.

        ```shell
        lsblk
        ```

        Select a partition as the data drive of the VM. The following uses `nvme0n1p7` as an example.

        ![](figures/zh-cn_image_0000002518185766.png)

    2. Open virt-manager and choose **Add Hardware > Storage > Manage**. Set **Bus type** to **VirtIO** and **Cache mode** to **none**.

        ![](figures/zh-cn_image_0000002549825535.png)

    3. Choose **Browse Local > dev > nvme0n1p7 > Open**.

        ![](figures/zh-cn_image_0000002549825517.png)

    4. Click **Finish**.

        ![](figures/zh-cn_image_0000002518185742.png)

11. If the SR-IOV solution is used, delete unnecessary virtual network interfaces.

    ![](figures/zh-cn_image_0000002549705521.png)

12. Restart the VM for the configurations to take effect.

    ![](figures/zh-cn_image_0000002518345658.png)

13. (Optional) Change the default password of the `root` account.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >The default password is **openEuler12\#$**. You can change the password after logging in to the OS.

    ```shell
    passwd root
    ```

#### 2.3.2 Configuring the VM Network<a name="ZH-CN_TOPIC_0000002550093503"></a>

##### 2.3.2.1 Configuring the VM NIC Configuration File<a name="ZH-CN_TOPIC_0000002518653644"></a>

Configure the VM NIC file to enable the network. If you adopt the SR-IOV solution, create a NIC configuration file following instructions in this section.

1. Check the VM NIC name.

    ```shell
    ip a
    ```

    ![](figures/zh-cn_image_0000002518345616.png)

2. Generate a configuration file.

    - If the SR-IOV solution is adopted in [Configuring the Host Network](#configuring-the-host-network), run the following commands to create a network configuration file:

        ```shell
        nmcli connection add ifname enp1s0 con-name enp1s0 type ethernet
        cd /etc/sysconfig/network-scripts/
        ls
        ```

        As shown in the following figure, the VM NIC configuration file is created.

        ![](figures/zh-cn_image_0000002518185690.png)

    - If the bridge mode is adopted in [Configuring the Host Network](#configuring-the-host-network), a network configuration file is automatically generated on the VM. The file name is the same as the NIC name in the `ip a` command output. In this case, skip this step.

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >If possible, you are advised to use the SR-IOV solution, which provides better computing performance and shorter latency.

3. Log in to the VM and configure `IPADDR`, `NETMASK`, `GATEWAY`, and `DNS` for the VM. Ensure that `ONBOOT` is set to `yes`.

    The VM and host share the values of `NETMASK`, `GATEWAY`, and `DNS`. `IPADDR` can be customized. Confirm with the network administrator to ensure that the IP address does not conflict with other IP addresses on the LAN.

    ```shell
    vi ifcfg-enp1s0
    ```

    ![](figures/1_zh-cn_image_0000002473918320.png)

    If the configuration file contains the content in the red box in the following figure, delete the content.

    ![](figures/zh-cn_image_0000002549705497.png)

4. Reload network connections. You are advised to run the commands in the shell of virt-manager. If you perform remote operations through SSH, connections will be interrupted due to network configuration modification.

    ```shell
    virt-manager
    ```

    ![](figures/zh-cn_image_0000002518345624.png)

    ```shell
    nmcli connection reload
    nmcli connection down enp1s0
    nmcli connection up enp1s0
    ```

    ![](figures/zh-cn_image_0000002549825463.png)

5. Ping the gateway and check whether the configuration takes effect over SSH remote connection. The gateway IP address can be queried in the configuration file of bridge `br0`.

    ```shell
    ping 192.168.20.1
    ```

    ![](figures/zh-cn_image_0000002549825485.png)

    On any server in the same network segment, connect to the VM using `ssh`.

    ```shell
    ssh IP_address
    ```

    ![](figures/1_zh-cn_image_0000002505918363.png)

    >![](public_sys-resources/icon-note.gif) **NOTE**
    >
    >In this example, data communication with the VM inside the LAN of the server has been achieved. If you want to access the VM inside the LAN from an external VM, contact the network administrator to configure the external VM following server configurations inside the LAN.

### 2.4 Video Stream Startup Environment Configuration (VM)<a name="ZH-CN_TOPIC_0000002550093505"></a>

After deploying cloud phones and video stream containers in the VM environment, you need to modify the `cfct_video` and `cfct_config` configuration files based on the number of VM CPU and GPU cores to correctly set the video stream startup and running parameters.

Deploy the cloud phone container environment on the VM and run video stream cloud phones. For details, see [Hardware Environment](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/kboxcpc_ad15/docs/en/install_guide.md#d21-hardware-environment) in the *Kbox Cloud Phone Container Installation Guide (Android 15)* and [Software Deployment](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/videostreamengine_ad15/docs/en/install_guide.md) in the *Video Stream Engine Installation Guide (Android 15)*.

The specific steps are as follows:

1. Extract the `cfct_video` and `cfct_config` files. For details, see [Software Deployment](https://www.hikunpeng.com/document/detail/en/kunpengcps/boostcph/videostreamengine_ad15/docs/en/install_guide.md) in the *Video Stream Engine Installation Guide (Android 15)*.
2. Modify the `cfct_config` file to adapt to the VM that uses 80 CPU cores and 4 GPU nodes.
    1. Open the `cfct_config` file.

        ```shell
        vim cfct_config
        ```

    2. Press **i** to enter the insert mode and add the following content:

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

        ![](figures/zh-cn_image_0000002518185626.png)

    3. Press **Esc** to exit the insert mode. Type `:wq!` and press **Enter** to save the settings and exit.

        >![](public_sys-resources/icon-note.gif) **NOTE**
        >
        >The CPU cores and GPU nodes in the preceding configuration are for reference only. Adjust them based on allocated resources of the VM and service requirements.

3. Modify the `cfct_video` file to adapt to the 80-core VM.
    1. Open the `cfct_video` file.

        ```shell
        vim cfct_video
        ```

    2. Press **i** to enter the insert mode and add the following content above `elif [ $num_of_cpus -eq 64 ]; then`.

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

        ![](figures/zh-cn_image_0000002518185600.png)

    3. Press **Esc** to exit the insert mode. Type `:wq!` and press **Enter** to save the settings and exit.

4. Invoke the `cfct_video` script to start the video stream container on the VM. For details, see "Starting a Video Stream Cloud Phone" in the *Video Stream Engine Feature Guide*.

    ![](figures/zh-cn_image_0000002518185616.png)
