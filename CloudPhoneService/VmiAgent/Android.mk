LOCAL_PATH := $(call my-dir)
REPO_ROOT_DIR := $(LOCAL_PATH)/../..
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
    $(LOCAL_PATH)/ApiTest/ApiTest.cpp

LOCAL_C_INCLUDES := \
    $(REPO_ROOT_DIR)/Common/Include \
    $(REPO_ROOT_DIR)/Common/Utils \
    $(REPO_ROOT_DIR)/Common/Log \
    $(REPO_ROOT_DIR)/Common \
    $(REPO_ROOT_DIR)/Common/Communication \
    $(LOCAL_PATH)/NetworkComm \
    $(LOCAL_PATH)/ApiTest

LOCAL_CFLAGS   += -Werror -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS += -Werror -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS += -Wno-unused-const-variable -Wno-unused-variable -Wno-unused -Wno-error -Wno-unused-private-field
LOCAL_LDFLAGS  += -Wl -s -Wl --param=build-id=none -Wl,-z,relro -fPIE -Wl,-z,now -Wformat
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS += -L $(LOCAL_PATH)/libs/arm64-v8a -L $(REPO_ROOT_DIR)/Common/libs/arm64-v8a -L $(REPO_ROOT_DIR)/Common/Communication/libs/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS += -L $(LOCAL_PATH)/libs/armeabi-v7a -L $(REPO_ROOT_DIR)/Common/libs/armeabi-v7a -L $(REPO_ROOT_DIR)/Common/Communication/libs/armeabi-v7a
endif
LOCAL_LDLIBS   := -lMediaEngine -lDemoUtils -lVmiCommunication

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_EXECUTABLE)