LOCAL_PATH := $(call my-dir)
DEMOCOMMON_DIR := $(LOCAL_PATH)

###############################################################################
# libCommunication
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libCommunication
LOCAL_VENDOR_MODULE := true
LOCAL_C_INCLUDES += \
    $(DEMOCOMMON_DIR)/Include \
    $(DEMOCOMMON_DIR)/Log

LOCAL_SRC_FILES := \
    Log/logging.cpp \
    Log/LogInfo.cpp \
    Connection/Connection.cpp

LOCAL_LDLIBS := -llog

LOCAL_CPPFLAGS += -fexceptions

include $(BUILD_SHARED_LIBRARY)

###############################################################################
# libDemoUtils
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libDemoUtils
LOCAL_VENDOR_MODULE := true
LOCAL_SRC_FILES := \
    Log/logging.cpp \
    Log/LogInfo.cpp \
    Utils/EngineEventHandler.cpp \
    Utils/Looper.cpp \
    Utils/MurmurHash.cpp \
    Utils/PacketQueue.cpp \
    Utils/VersionCheck.cpp \
    Utils/SystemProperty.cpp

LOCAL_C_INCLUDES += \
    $(DEMOCOMMON_DIR) \
    $(DEMOCOMMON_DIR)/Include \
    $(DEMOCOMMON_DIR)/Log \
    $(DEMOCOMMON_DIR)/Connection

LOCAL_CFLAGS += -fexceptions -DENABLE_TEXTURE_CACHE=1 -DENABLE_BUFFER_DATA_CACHE=1 -DREFERENCE_FRAME_MEM_MAX=150 -DDISABLE_FLOAT_API
LOCAL_CFLAGS += -DFIXED_POINT=1 -Werror -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS += -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_LDFLAGS += -Wl,--build-id=none -Wl,-z,relro -fPIE -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS += -L $(DEMOCOMMON_DIR)/libs/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS += -L $(DEMOCOMMON_DIR)/libs/armeabi-v7a
endif
LOCAL_LDLIBS := -llog -lCommunication

include $(BUILD_SHARED_LIBRARY)