LOCAL_PATH:= $(call my-dir)
REPO_ROOT_DIR := $(LOCAL_PATH)/../../../..
COMMON_DIR := $(REPO_ROOT_DIR)/Common/Log
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    VideoCodecApi.cpp \
	VideoEncoderCommon.cpp \
    VideoEncoderOpenH264.cpp \
    VideoEncoderNetint.cpp \
    VideoEncoderQuadra.cpp \
    VideoEncoderVastai.cpp \
    $(COMMON_DIR)/LogInfo.cpp \
    $(COMMON_DIR)/logging.cpp \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(COMMON_DIR) \
    $(LOCAL_PATH)/../vendor/openh264 \
    $(LOCAL_PATH)/../vendor/netintV333 \
    $(LOCAL_PATH)/../../../../unpack_open_source/hantro/

LOCAL_LDLIBS := -llog

LOCAL_CFLAGS := -Wformat -Wall -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS := -Wformat -Wall -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_LDFLAGS :=  -Wformat -Wl,--build-id=none -Wl,-z,relro -fPIE -Wl,-z,now,-z,noexecstack

LOCAL_MODULE := libVideoCodec
LOCAL_VENDOR_MODULE := true

include $(BUILD_SHARED_LIBRARY)