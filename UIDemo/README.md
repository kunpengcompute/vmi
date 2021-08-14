# 编译指南

## 1 搭建编译环境

### 1.1 硬件环境

需要一台x86 pc机, 操作系统为window系统

### 1.2 软件环境

安装最新的Android studio, 并在SDK Manager中装好Android sdk、NDK、cmake组件。

## 2 UIDemo工程编译

### 2.1 导入工程

使用Android studio导入UI Demo工程

### 2.2 修改gradle版本

修改gradle-wrapper.properties文件中distributionUrl中gradle版本为本地的gradle版本

### 2.3 修改cmake版本

修改app/build.gradle中的cmake为本地的cmake版本
externalNativeBuild {
    cmake {
        version "3.18.1"
        path "src/main/cpp/CMakeLists.txt"
    }
}

### 2.4 编译工程

执行Android studio的“Make project”菜单命令即可编译成功




