NDK_TOOLCHAIN_VERSION := clang

APP_ABI := arm64-v8a armeabi-v7a
APP_PLATFORM := android-26
APP_CPPFLAGS += -std=c++14 -fexceptions -ftrapv -fvisibility=hidden
APP_STL := c++_shared
