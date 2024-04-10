LOCAL_PATH := $(call my-dir)
REPO_ROOT := $(LOCAL_PATH)/../../
include $(CLEAR_VARS)
LOCAL_MODULE := VideoEngineClient

LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
$(info "$(LOCAL_MODULE) ENABLE ASAN")
LOCAL_CFLAGS   += -fsanitize=address -fsanitize-recover=address,all -O0
LOCAL_CPPFLAGS += -fsanitize=address -fsanitize-recover=address,all -O0
LOCAL_LDFLAGS  += -fsanitize=address -L $(LOCAL_PATH)/Decoder/libs/arm64-v8a
LOCAL_LDFLAGS  += -L $(REPO_ROOT)/Common/build
else
$(info "$(LOCAL_MODULE) ENABLE FORTIFY SOURCE")
LOCAL_CFLAGS   += -D_FORTIFY_SOURCE=2 -O2 -DVMICLIENT=1
LOCAL_CPPFLAGS += -D_FORTIFY_SOURCE=2 -O2 -DVMICLIENT=1
LOCAL_LDFLAGS  += -L $(LOCAL_PATH)/Decoder/libs/arm64-v8a
LOCAL_LDFLAGS  += -L $(REPO_ROOT)/Common/build
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

LOCAL_LDLIBS := -lmediandk -llog -lDecoder -lCommunication
include $(BUILD_SHARED_LIBRARY)
