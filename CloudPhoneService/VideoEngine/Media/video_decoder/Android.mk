LOCAL_PATH:= $(call my-dir)
########################################################################
include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wformat -Wall -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS := -Wformat -Wall -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_LDFLAGS := -Wformat -Wl,--build-id=none -Wl,-z,relro -fPIE -Wl,-z,now,-z,noexecstack

LOCAL_SRC_FILES := \
    VideoDecoderApi.cpp \
    VideoDecoderQuadra.cpp \
    VideoDecoderNetint.cpp \
    prop/Property.cpp
 
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/prop \
    $(LOCAL_PATH)/../vendor/quadraV460 \
    $(LOCAL_PATH)/../vendor/netintV333

LOCAL_LDLIBS := -llog

LOCAL_MODULE := libVideoDecoder
LOCAL_VENDOR_MODULE := true

include $(BUILD_SHARED_LIBRARY)