LOCAL_PATH := $(call my-dir)
DEMOCOMMON_DIR :=$(LOCAL_PATH)/../
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    CPP_SHARED_SO := $(DEMOCOMMON_DIR)/libs/arm64-v8a/libc++_shared.so
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    CPP_SHARED_SO := $(DEMOCOMMON_DIR)/libs/armeabi-v7a/libc++_shared.so
endif

#----------------------------
# libVmiCommunication
#----------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := libVmiCommunication
LOCAL_VENDOR_MODULE := true

LOCAL_SRC_FILES := \
    Heartbeat/Heartbeat.cpp \
    MsgFragment/MsgReassemble.cpp \
    MsgFragment/MsgFragment.cpp \
    PacketHandle/PacketHandle.cpp \
    PacketManager/PacketManager.cpp \
    Socket/VmiSocket.cpp \
    StreamParse/StreamParser.cpp \
    NetComm/NetComm.cpp \
    Connection/ServerConnection.cpp \
    Connection/ClientConnection.cpp \
    Connection/ConnectionSocket.cpp

LOCAL_C_INCLUDES := \
    $(DEMOCOMMON_DIR)/Include \
    $(DEMOCOMMON_DIR)/Utils \
    $(DEMOCOMMON_DIR)/Log \
    $(DEMOCOMMON_DIR)/Connection

#add safety compile options
LOCAL_CFLAGS   += -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_CPPFLAGS += -fexceptions -DENABLE_TEXTURE_CACHE=1 -DENABLE_BUFFER_DATA_CACHE=1 -DREFERENCE_FRAME_MEM_MAX=150 -Werror
LOCAL_CPPFLAGS += -Wformat -Wall -fstack-protector-strong --param=ssp-buffer-size=4 -fPIE -D_FORTIFY_SOURCE=2 -O2 -fPIC
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS += -L $(DEMOCOMMON_DIR)/libs/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS += -L $(DEMOCOMMON_DIR)/libs/armeabi-v7a
endif
LOCAL_LDLIBS := -llog -lDemoUtils -lCommunication

include $(BUILD_SHARED_LIBRARY)
