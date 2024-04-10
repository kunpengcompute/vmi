NDK_TOOLCHAIN_VERSION := clang

APP_ABI := arm64-v8a armeabi-v7a
APP_PLATFORM := android-24
APP_CPPFLAGS += -std=c++14 -fexceptions
APP_STL := c++_shared
