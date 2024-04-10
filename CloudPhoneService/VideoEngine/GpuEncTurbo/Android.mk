LOCAL_PATH:= $(call my-dir)
REPO_ROOT_DIR := $(LOCAL_PATH)/../../..
COMMON_DIR := $(REPO_ROOT_DIR)/Common/Log
UNPACK_OPEN_SOURCE_DIR := $(REPO_ROOT_DIR)/unpack_open_source
LIBVA_LIBS_DIR := $(UNPACK_OPEN_SOURCE_DIR)/libva/libs

########################################################################
# GpuEncTurboSys
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncodeFactory.cpp \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog

LOCAL_MODULE := libVmiEncTurboSys
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
# GpuEncTurboSys
########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    GpuEncodeFactory.cpp \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
LOCAL_LDLIBS := -llog

LOCAL_VENDOR_MODULE := true
LOCAL_MODULE := libVmiEncTurbo
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/GpuEncTurbo \
    $(COMMON_DIR) \
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
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS := -L $(LIBVA_LIBS_DIR)/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS := -L $(LIBVA_LIBS_DIR)/armeabi-v7a
endif
LOCAL_LDLIBS := -llog -lva -lva-drm

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
    GpuEncoderInno/VaEncInno.cpp \
    ../../../Common/Log/LogInfo.cpp \
    ../../../Common/Log/logging.cpp

#add shared libraries
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS := -L $(LIBVA_LIBS_DIR)/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS := -L $(LIBVA_LIBS_DIR)/armeabi-v7a
endif
LOCAL_LDLIBS := -llog -lva -lva-android

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
