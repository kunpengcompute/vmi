NDK_TOOLCHAIN_VERSION := clang

APP_ABI := arm64-v8a
APP_PLATFORM := android-26
# Get C++11 working
APP_CPPFLAGS += -std=c++14 -fexceptions -ftrapv -fvisibility=hidden
APP_STL := c++_shared
#APP_STL := stlport_static
