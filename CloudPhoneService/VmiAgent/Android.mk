LOCAL_PATH := $(call my-dir)
REPO_ROOT_DIR := $(LOCAL_PATH)/../..
########################################################################
# prebuilt
########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libDemoUtils
LOCAL_SRC_FILES := $(REPO_ROOT_DIR)/Common/libs/$(TARGET_ARCH_ABI)/libDemoUtils.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libVmiCommunication
LOCAL_SRC_FILES := $(REPO_ROOT_DIR)/Common/Communication/libs/$(TARGET_ARCH_ABI)/libVmiCommunication.so
include $(PREBUILT_SHARED_LIBRARY)

########################################################################
# MediaEngine
########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libMediaEngine
LOCAL_VENDOR_MODULE := true
LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/VmiApi.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../Common/Include

LOCAL_CFLAGS   += -Werror -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS += -Werror -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wno-unused-parameter
LOCAL_LDFLAGS  += -Wl -s -Wl --param=build-id=none -Wl,-z,relro -fPIE -Wl,-z,now -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)

########################################################################
# VmiAgent
########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := VmiAgent
LOCAL_VENDOR_MODULE := true
LOCAL_INIT_RC := VmiAgentAndroidP.rc
LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/main.cpp \
    $(LOCAL_PATH)/VmiAgent.cpp \
    $(LOCAL_PATH)/MainLooper.cpp \
    $(LOCAL_PATH)/NetworkComm/INetworkComm.cpp \
    $(LOCAL_PATH)/NetworkComm/NetworkComm.cpp \
    $(LOCAL_PATH)/NetworkComm/NetworkCommManager.cpp \
    $(LOCAL_PATH)/NetworkComm/NetworkExport.cpp \
    $(LOCAL_PATH)/ApiTest/ApiTest.cpp \
    $(LOCAL_PATH)/Latency/Latency.cpp \
    $(LOCAL_PATH)/Fpsperf/DeviceWatcher.cpp \
    $(LOCAL_PATH)/Fpsperf/Fpsperf.cpp

LOCAL_C_INCLUDES := \
    $(REPO_ROOT_DIR)/Common/Include \
    $(REPO_ROOT_DIR)/Common/Utils \
    $(REPO_ROOT_DIR)/Common/Log \
    $(REPO_ROOT_DIR)/Common \
    $(REPO_ROOT_DIR)/Common/Communication \
    $(LOCAL_PATH)/NetworkComm \
    $(LOCAL_PATH)/ApiTest \
    $(LOCAL_PATH)/Fpsperf

LOCAL_CFLAGS   += -Werror -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS += -Werror -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS += -Wno-unused-const-variable -Wno-unused-variable -Wno-unused -Wno-error -Wno-unused-private-field
LOCAL_LDFLAGS  += -Wl -s -Wl --param=build-id=none -Wl,-z,relro -fPIE -Wl,-z,now -Wformat
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := libMediaEngine libDemoUtils libVmiCommunication

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_EXECUTABLE)
