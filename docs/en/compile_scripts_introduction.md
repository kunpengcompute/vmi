# auto_install_tools.sh

<!-- md-trans-meta sourceCommit=98996e95cca45e7a6ce35559eba64329d9532638 translatedAt=2026-08-29T08:15:08.503Z pushedAt=2026-08-29T09:18:29.931Z -->

## Usage Command

```bash
./scripts/auto_install_tools.sh ${installation_directory}
source ~/.bashrc
```

## Overall Workflow

1. Download the compilation tool package to *${installation_directory}*. If not specified, the installation directory defaults to `~/NativeCompileToolsDir`.
2. Extract the compilation tool within *${installation_directory}*.
3. Configure the environment variables in the `.bashrc` file to map the paths of each compilation tool to its corresponding directory under *${installation_directory}*.

## Function Description

### DownloadToolPkg()

Downloads the following files to *${installation_directory}*.

If a file already exists, it will not be downloaded repeatedly. Therefore, you can create an installation directory in advance and download the following files. Then the script will skip the file download step.

|Software Name|Version|Download URL|
| :---: | :---: | :---: |
|Android NDK|r25b|<https://dl.google.com/android/repository/android-ndk-r25b-linux.zip>|
|Android SDK Build Tools|33.0.1|<https://dl.google.com/android/repository/build-tools_r33.0.1-linux.zip>|
|gradle|8.4.0|<https://mirrors.cloud.tencent.com/gradle/gradle-8.4-bin.zip>|
|CMake|3.28.2|<https://github.com/Kitware/CMake/releases/download/v4.3.2/cmake-4.3.2-linux-x86_64.tar.gz>|
|AdoptOpenJDK|11+28|<https://download.java.net/openjdk/jdk11/ri/openjdk-11+28_linux-x64_bin.tar.gz>|
|ninja|v1.12.0|<https://github.com/ninja-build/ninja/releases/download/v1.12.0/ninja-linux.zip>|
|SDK platform|33_r02|<https://dl.google.com/android/repository/platform-33_r02.zip>|
|SDK platform tools|33.0.3|<https://dl.google.com/android/repository/platform-tools_r33.0.3-linux.zip>|

### UnpackTools()

Extracts the downloaded packages and organizes them within *${installation_directory}* according to a predefined directory structure.

### DelOldEnv()

Deletes previously configured environment variables from `~/.bashrc`.

The environment variables configured by `auto_install_tools.sh` start with the **#tools_env_start:** marker and end with the **#tools_env_end:** marker. This function locates these two markers in `~/.bashrc` and deletes the content between them.

If **#tools_env_end:** appears before **#tools_env_start:**, the script cannot automatically delete the content, and you must resolve the conflict manually.

### CheckEnvConflict()

Checks whether the environment variables intended for configuration already exist in `~/.bashrc`. If a conflict is detected, the script logs an error and exits. In this case, you must manually clear or comment out the previously set environment variables.

The environment variables to be configured are as follows:

```bash
USER_LOCAL_PATH
JAVA_HOME
AN_JDKPATH
AN_JAVADIR
CLASSPATH
AN_SDKDIR
AN_NDKDIR
GRADLE_HOME
AN_GRADLEDIR
CMAKE_PATH
ANDROID_NDK_HOME
ANDROID_NDK
```

### SetEnvConfig()

Adds environment variables to `~/.bashrc`, starting with the **#tools_env_start:** marker and ending with the **#tools_env_end:** marker.

# download_open_source.sh

## Overall Workflow

Download open-source software packages to the `open_source_download` directory, and then extract them into the corresponding directories under `unpack_open_source`.

If a package already exists in the `open_source_download` directory, it will not be downloaded again. However, regardless of whether extracted files already exist under `unpack_open_source`, the script will re-extract and overwrite the previously extracted files during each code compilation.

<a id="opensource_repo.conf"></a>

## Introduction to opensource_repo.conf

The `opensource_repo.conf` file is used to record the download URLs and extraction methods for open-source software. It is called every time `download_open_source.sh` is executed.

To add a new download URL for an open-source software package in `opensource_repo.conf`, you must configure four parameters based on the required format. The parameters are explained as follows:

```bash
Parameter [0]: Download URL, used by wget for downloading.
Parameter [1]: Downloaded file name.
Parameter [2]: Extraction directory. The downloaded file will be extracted to unpack_open_source/${Parameter [2]}. Note that the extraction directory is also affected by Parameter [3].
Parameter [3]: Extraction method, with the following three options:
0: Extract directly into unpack_open_source/${Parameter [2]}.
1: Create a new directory named unpack_open_source/${Parameter [2]}/codes, and then extract the file into this codes directory. This is typically used when other compilation files already exist in unpack_open_source/${Parameter [2]}.
2: Do not extract the downloaded file; copy it directly into unpack_open_source/${Parameter [2]}.
```

## Function and Variable Description

### open_sources

Specifies the open-source software to be downloaded. The member variable names must be identical to the names defined in the `opensource_repo.conf` file.

### check_download_file_exist()

Checks whether the packages intended for download already exist in the `open_source_download` directory.

### download_open_source()

Uses the `wget` command to download packages from the download URLs into the `open_source_download` directory.

When downloading files, `wget` automatically renames a file to the segment following the last forward slash (`/`) in the URL. This can sometimes result in file names that lack a proper archive extension, making extraction impossible. Therefore, the target file names after download must be explicitly defined in `opensource_repo.conf`.

### unpack_source()

Extracts the packages into the `unpack_open_source` directory. The extraction path is determined by parameter [2] and parameter [3] configured in `opensource_repo.conf`. For details, see [Introduction to opensource_repo.conf](#opensource_repo.conf).

Additionally, the script automatically checks whether the extracted content consists of only a single top-level folder. If so, it assumes that the package contains a redundant nested folder level. It will then move all files within that folder up one level and delete the empty folder.
