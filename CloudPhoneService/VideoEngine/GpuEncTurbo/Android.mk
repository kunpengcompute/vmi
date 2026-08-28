LOCAL_PATH:= $(call my-dir)
REPO_ROOT_DIR := $(LOCAL_PATH)/../../..
COMMON_DIR := $(REPO_ROOT_DIR)/Common/Log
UNPACK_OPEN_SOURCE_DIR := $(REPO_ROOT_DIR)/unpack_open_source
LIBVA_LIBS_DIR := $(UNPACK_OPEN_SOURCE_DIR)/libva/libs

########################################################################
# prebuilt
########################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libDemoUtils
LOCAL_SRC_FILES := $(COMMON_DIR)/../libs/$(TARGET_ARCH_ABI)/libDemoUtils.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libva
LOCAL_SRC_FILES := $(LIBVA_LIBS_DIR)/$(TARGET_ARCH_ABI)/libva.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libva-drm
LOCAL_SRC_FILES := $(LIBVA_LIBS_DIR)/$(TARGET_ARCH_ABI)/libva-drm.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libva-android
LOCAL_SRC_FILES := $(LIBVA_LIBS_DIR)/$(TARGET_ARCH_ABI)/libva-android.so
include $(PREBUILT_SHARED_LIBRARY)

########################################################################
# GpuEncTurboSys
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncodeFactory.cpp

#add shared libraries
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := libDemoUtils

LOCAL_MODULE := libVmiEncTurboSys
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(COMMON_DIR)/../Utils \
    $(UNPACK_OPEN_SOURCE_DIR) \

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\"
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)
########################################################################

########################################################################
# GpuEncTurboSys
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncodeFactory.cpp

#add shared libraries
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES:= libDemoUtils

LOCAL_VENDOR_MODULE := true
LOCAL_MODULE := libVmiEncTurbo
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(COMMON_DIR)/../Utils \
    $(UNPACK_OPEN_SOURCE_DIR) \

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\"
LOCAL_CFLAGS += -DIS_VENDOR
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)
########################################################################

########################################################################
# GpuEncTurboCpuSys
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    CpuEncoder/CpuEncoder.cpp \
    CpuEncoder/Rgb2Yuv/VmiRgb2Yuv.cpp \
    CpuEncoder/Rgb2Yuv/Rgb2Yuv.S \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog

LOCAL_MODULE := libVmiEncTurboCpuSys
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR) \

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\"
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)
########################################################################

########################################################################
# GpuEncTurboCpu
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    CpuEncoder/CpuEncoder.cpp \
    CpuEncoder/Rgb2Yuv/VmiRgb2Yuv.cpp \
    CpuEncoder/Rgb2Yuv/Rgb2Yuv.S \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog

LOCAL_VENDOR_MODULE := true
LOCAL_MODULE := libVmiEncTurboCpu
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR) \

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\"
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)
########################################################################

########################################################################
# GpuEncTurboAcard
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    VaBitstream.cpp \
    GpuEncoderAmd/GpuEncoderAmd.cpp \
    GpuEncoderAmd/VaEncoderAmd.cpp \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := libva libva-drm

LOCAL_MODULE := libVmiEncTurboAcard
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR)/libva/codes \
    $(UNPACK_OPEN_SOURCE_DIR)/libva/codes/va \
    $(UNPACK_OPEN_SOURCE_DIR)/libva/codes/va/drm \
    $(UNPACK_OPEN_SOURCE_DIR)/libva/Include

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\"
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)
########################################################################

########################################################################
# GpuEncTurboInno
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncoderInno/GpuEncoderInno.cpp \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := libva libva-android

LOCAL_VENDOR_MODULE := true
LOCAL_MODULE := libVmiEncTurboInno
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR)/libva/codes \
    $(UNPACK_OPEN_SOURCE_DIR)/libva/codes/va \
    $(UNPACK_OPEN_SOURCE_DIR)/libva/Include

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\"
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

ifeq ($(ENABLE_ASAN), 1)
LOCAL_SANITIZE := hwaddress alignment bounds null unreachable integer
LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer
endif

include $(BUILD_SHARED_LIBRARY)
########################################################################

########################################################################
# InnoCapture
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncoderInno/InnoCaptureWrapper.cpp \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog

LOCAL_VENDOR_MODULE := true
LOCAL_MODULE := libVmiInnoCapture
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(LOCAL_PATH)/GpuEncTurbo/GpuEncoderInno

LOCAL_CFLAGS += -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_CFLAGS += -Wno-address-of-packed-member -DLOG_TAG=\"INativeGpuEncTurbo\"
LOCAL_CFLAGS   += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall
LOCAL_CPPFLAGS += -fstack-protector-strong --param ssp-buffer-size=4 -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -fPIC -Wformat -Werror -Wall -fexceptions
LOCAL_LDFLAGS  += -Wl,--build-id=none -Wl,-z,relro -fPIE  -Wl,-z,now,-z,noexecstack -Wformat

include $(BUILD_SHARED_LIBRARY)
########################################################################

########################################################################
# DisplayServerMock
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncoderHantro/DisplayServerMock.cpp \

#add shared libraries
LOCAL_LDLIBS := -llog

LOCAL_VENDOR_MODULE := true
LOCAL_MULTILIB := 64
LOCAL_MODULE := libDisplayServer
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR)/hantro \

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

########################################################################
# HantroCapture
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncoderHantro/HantroCaptureWrapper.cpp \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := libDisplayServer

LOCAL_VENDOR_MODULE := true
LOCAL_MULTILIB := 64
LOCAL_MODULE := libVmiHantroCapture
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR)/hantro \

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

########################################################################
# GpuEncTurboHantro
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp \
    GpuEncoderHantro/GpuEncoderHantro.cpp \
    GpuEncoderHantro/sync.c

#add shared libraries
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := libDemoUtils libVmiHantroCapture

LOCAL_VENDOR_MODULE := true
LOCAL_MULTILIB := 64
LOCAL_MODULE := libVmiEncTurboHantro
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
    $(COMMON_DIR)/../Utils \
    $(UNPACK_OPEN_SOURCE_DIR) \
    $(UNPACK_OPEN_SOURCE_DIR)/hantro

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
