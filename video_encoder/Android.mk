LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wformat -Wall -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS := -Wformat -Wall -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_LDFLAGS := -Wformat -Wl,--build-id=none -Wl,-z,relro -fPIE -Wl,-z,now,-z,noexecstack

LOCAL_SRC_FILES := \
    VideoEncoderWrapper.cpp \
    VideoEncoderLog.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    system/core/liblog/include \
    $(LOCAL_PATH)/vendor/video_codec

LOCAL_SHARED_LIBRARIES := \
    liblog

LOCAL_MODULE := libVideoEncoder

LOCAL_VENDOR_MODULE := true

include $(BUILD_SHARED_LIBRARY)
