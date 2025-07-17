LOCAL_PATH := $(call my-dir)
REPO_ROOT := $(LOCAL_PATH)/../../

########################################################################
# NativeWindowMock
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/Control/NativeWindowMock.cpp \

#add shared libraries
LOCAL_LDLIBS := -llog

LOCAL_VENDOR_MODULE := true
LOCAL_MULTILIB := 64
LOCAL_MODULE := libnativewindow
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/Control \
    $(REPO_ROOT)/unpack_open_source

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\" -DCONFIG_VASTAPI
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)
########################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := Decoder
LOCAL_SRC_FILES := $(LOCAL_PATH)/Decoder/libs/arm64-v8a/libDecoder.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := Communication
LOCAL_SRC_FILES := $(REPO_ROOT)/Common/build/libCommunication.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := VideoEngineClient

LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
$(info "$(LOCAL_MODULE) ENABLE ASAN")
LOCAL_CFLAGS   += -fsanitize=address -fsanitize-recover=address,all -O0
LOCAL_CPPFLAGS += -fsanitize=address -fsanitize-recover=address,all -O0
LOCAL_LDFLAGS  += -fsanitize=address
else
$(info "$(LOCAL_MODULE) ENABLE FORTIFY SOURCE")
LOCAL_CFLAGS   += -D_FORTIFY_SOURCE=2 -O2 -DVMICLIENT=1
LOCAL_CPPFLAGS += -D_FORTIFY_SOURCE=2 -O2 -DVMICLIENT=1
endif

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/Control \
        $(LOCAL_PATH)/Decoder \
        $(REPO_ROOT)/Common/Include \
        $(REPO_ROOT)/Common/Utils \
        $(REPO_ROOT)/Common/Communication \
        $(REPO_ROOT)/Common/Log \
        $(REPO_ROOT)/Common/Connection \
        $(REPO_ROOT)/unpack_open_source

LOCAL_SRC_FILES := \
        $(LOCAL_PATH)/Control/CloudPhoneController.cpp \
        $(LOCAL_PATH)/Control/Parser.cpp \
        $(LOCAL_PATH)/Control/NetController.cpp \
        $(LOCAL_PATH)/Control/VideoEngineClient.cpp \
        \
        $(REPO_ROOT)/Common/Communication/Heartbeat/Heartbeat.cpp \
        $(REPO_ROOT)/Common/Communication/MsgFragment/MsgReassemble.cpp \
        $(REPO_ROOT)/Common/Communication/MsgFragment/MsgFragment.cpp \
        $(REPO_ROOT)/Common/Communication/PacketHandle/PacketHandle.cpp \
        $(REPO_ROOT)/Common/Communication/PacketManager/PacketManager.cpp \
        $(REPO_ROOT)/Common/Communication/Socket/VmiSocket.cpp \
        $(REPO_ROOT)/Common/Communication/StreamParse/StreamParser.cpp \
        $(REPO_ROOT)/Common/Communication/NetComm/NetComm.cpp \
        $(REPO_ROOT)/Common/Communication/Connection/ConnectionSocket.cpp \
        $(REPO_ROOT)/Common/Utils/PacketQueue.cpp \
        $(REPO_ROOT)/Common/Log/logging.cpp \
        $(REPO_ROOT)/Common/Log/LogInfo.cpp \
        $(REPO_ROOT)/Common/Utils/SystemProperty.cpp \
        $(REPO_ROOT)/Common/Utils/VersionCheck.cpp \
        $(REPO_ROOT)/Common/Utils/MurmurHash.cpp \
        $(REPO_ROOT)/Common/Utils/EngineEventHandler.cpp

LOCAL_LDLIBS := -lmediandk -llog
LOCAL_SHARED_LIBRARIES := libDecoder libCommunication libnativewindow
include $(BUILD_SHARED_LIBRARY)
