# Copyright (c) 2007 Intel Corporation. All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sub license, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice (including the
# next paragraph) shall be included in all copies or substantial portions
# of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
# IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
# ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
# For libva
# =====================================================
 
LOCAL_PATH:= $(call my-dir)/codes/va
LIBVA_DRIVERS_PATH_32 := /vendor/lib/dri
LIBVA_DRIVERS_PATH_64 := /vendor/lib64/dri
UNPACK_LIBDRM_PATH := $(call my-dir)/../libdrm
 
include $(CLEAR_VARS)
 
#LIBVA_MINOR_VERSION := 31
#LIBVA_MAJOR_VERSION := 0
 
IGNORED_WARNNING = \
	-Wno-sign-compare \
	-Wno-missing-field-initializers \
	-Wno-unused-parameter \
 
LOCAL_SRC_FILES := \
	va.c \
	va_trace.c \
	va_fool.c  \
	va_str.c
 
#------------------------
LOCAL_CFLAGS := \
	$(IGNORED_WARNNING) \
	$(if $(filter user,$(TARGET_BUILD_VARIANT)),,-DENABLE_VA_MESSAGING) \
	-DLOG_TAG=\"libva\"
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_CFLAGS += -DVA_DRIVERS_PATH="\"$(LIBVA_DRIVERS_PATH_64)\""
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_CFLAGS += -DVA_DRIVERS_PATH="\"$(LIBVA_DRIVERS_PATH_32)\""
endif

LOCAL_CFLAGS += -DSYSCONFDIR=\"/etc\"
 
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../../Include
 
LOCAL_VENDOR_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libva
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
#LOCAL_PROPRIETARY_MODULE := true

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS := -L $(UNPACK_LIBDRM_PATH)/libs/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS := -L $(UNPACK_LIBDRM_PATH)/libs/armeabi-v7a
endif
LOCAL_LDLIBS := -ldl -lvmidrm -llog
 #---------------------------------
LOCAL_EXPORT_C_INCLUDE_DIRS := \
	$(LOCAL_C_INCLUDES)
 
include $(BUILD_SHARED_LIBRARY)
 
# For libva-android
# =====================================================
 
include $(CLEAR_VARS)
 
LOCAL_SRC_FILES := \
	android/va_android.cpp \
	drm/va_drm_utils.c
 
LOCAL_CFLAGS += \
	-DLOG_TAG=\"libva-android\" \
	$(IGNORED_WARNNING)
 
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/drm \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../../Include \
	$(UNPACK_LIBDRM_PATH)/codes \
	$(UNPACK_LIBDRM_PATH)/codes/include/drm
#---------------------
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libva-android
#LOCAL_PROPRIETARY_MODULE := true

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS := -L $(UNPACK_LIBDRM_PATH)/libs/arm64-v8a -L $(LOCAL_PATH)/../../libs/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS := -L $(UNPACK_LIBDRM_PATH)/libs/armeabi-v7a -L $(LOCAL_PATH)/../../libs/armeabi-v7a
endif
LOCAL_LDLIBS := -lva -lvmidrm -llog
 
include $(BUILD_SHARED_LIBRARY)
 
# For libva-drm
# =====================================================
 
include $(CLEAR_VARS)
 
LOCAL_SRC_FILES := \
        drm/va_drm.c \
        drm/va_drm_auth.c \
	drm/va_drm_utils.c
 
LOCAL_CFLAGS += \
        -DLOG_TAG=\"libva-drm\" \
        $(IGNORED_WARNNING)
 
LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/drm/ \
        $(LOCAL_PATH)/.. \
        $(LOCAL_PATH)/../../Include \
        $(UNPACK_LIBDRM_PATH)/codes \
        $(UNPACK_LIBDRM_PATH)/codes/include/drm
 
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libva-drm
#LOCAL_PROPRIETARY_MODULE := true

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_LDFLAGS := -L $(UNPACK_LIBDRM_PATH)/libs/arm64-v8a -L $(LOCAL_PATH)/../../libs/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_LDFLAGS := -L $(UNPACK_LIBDRM_PATH)/libs/armeabi-v7a -L $(LOCAL_PATH)/../../libs/armeabi-v7a
endif
LOCAL_LDLIBS := -lva -lvmidrm -llog
 
include $(BUILD_SHARED_LIBRARY)