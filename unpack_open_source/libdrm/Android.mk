LIBDRM_ANDROID_COMMON_MK := $(call my-dir)/Android.common.mk
LOCAL_PATH_MOCK := $(call my-dir)
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    INCLUDE_LD_DIR := $(LOCAL_PATH_MOCK)/libs/arm64-v8a
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    INCLUDE_LD_DIR := $(LOCAL_PATH_MOCK)/libs/armeabi-v7a
endif

# for libInsDrm.so
include $(CLEAR_VARS)
 
LOCAL_PATH := $(LOCAL_PATH_MOCK)/codes
LOCAL_MODULE := libInsDrm
LOCAL_VENDOR_MODULE := true

LOCAL_SRC_FILES := \
    xf86drm.c \
    xf86drmHash.c \
    xf86drmRandom.c \
    xf86drmSL.c \
    xf86drmMode.c
 
LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/android \
    $(LOCAL_PATH)/include/drm \
 
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include/drm

include $(LIBDRM_ANDROID_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)
 
# for libInsDrm_amdgpu.so
include $(CLEAR_VARS)
 
LOCAL_PATH := $(LOCAL_PATH_MOCK)/codes/amdgpu
LOCAL_MODULE := libInsDrm_amdgpu

LOCAL_LDFLAGS += -L $(INCLUDE_LD_DIR)
LOCAL_LDLIBS := -lInsDrm
LOCAL_VENDOR_MODULE := true

LOCAL_SRC_FILES := \
    amdgpu_asic_id.c \
    amdgpu_bo.c \
    amdgpu_cs.c \
    amdgpu_device.c \
    amdgpu_gpu_info.c \
    amdgpu_vamgr.c \
    amdgpu_vm.c \
    handle_table.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH_MOCK)/codes \
    $(LOCAL_PATH_MOCK)/codes/include/drm

LOCAL_CFLAGS += \
    -DAMDGPU_ASIC_ID_TABLE=\"/vendor/etc/hwdata/amdgpu.ids\"
 
LOCAL_REQUIRED_MODULES := amdgpu.ids

include $(LIBDRM_ANDROID_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)
 
# for libInsDrm_radeon.so
include $(CLEAR_VARS)
 
LOCAL_PATH := $(LOCAL_PATH_MOCK)/codes/radeon
LOCAL_MODULE := libInsDrm_radeon

LOCAL_LDFLAGS += -L $(INCLUDE_LD_DIR)
LOCAL_LDLIBS := -lInsDrm
LOCAL_VENDOR_MODULE := true

LOCAL_SRC_FILES := \
    radeon_bo_gem.c \
    radeon_cs_gem.c \
    radeon_cs_space.c \
    radeon_bo.c \
    radeon_cs.c \
    radeon_surface.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH_MOCK)/codes \
    $(LOCAL_PATH_MOCK)/codes/include/drm

include $(LIBDRM_ANDROID_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)

# for libvmidrm.so
include $(CLEAR_VARS)

LOCAL_PATH := $(LOCAL_PATH_MOCK)/codes
LOCAL_MODULE := libvmidrm

LOCAL_SRC_FILES := \
    xf86drm.c \
    xf86drmHash.c \
    xf86drmRandom.c \
    xf86drmSL.c \
    xf86drmMode.c

LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/android \
    $(LOCAL_PATH)/include/drm

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include/drm

include $(LIBDRM_ANDROID_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)

# for libvmidrm_amdgpu.so
include $(CLEAR_VARS)

LOCAL_PATH := $(LOCAL_PATH_MOCK)/codes/amdgpu
LOCAL_MODULE := libvmidrm_amdgpu

LOCAL_LDFLAGS += -L $(INCLUDE_LD_DIR)
LOCAL_LDLIBS := -lvmidrm

LOCAL_SRC_FILES := \
    amdgpu_asic_id.c \
    amdgpu_bo.c \
    amdgpu_cs.c \
    amdgpu_device.c \
    amdgpu_gpu_info.c \
    amdgpu_vamgr.c \
    amdgpu_vm.c \
    handle_table.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH_MOCK)/codes \
    $(LOCAL_PATH_MOCK)/codes/include/drm

LOCAL_CFLAGS += \
    -DAMDGPU_ASIC_ID_TABLE=\"/vendor/etc/hwdata/amdgpu.ids\"

LOCAL_REQUIRED_MODULES := amdgpu.ids

include $(LIBDRM_ANDROID_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)

# for libvmidrm_radeon.so
include $(CLEAR_VARS)

LOCAL_PATH := $(LOCAL_PATH_MOCK)/codes/radeon
LOCAL_MODULE := libvmidrm_radeon

LOCAL_LDFLAGS += -L $(INCLUDE_LD_DIR)
LOCAL_LDLIBS := -lvmidrm

LOCAL_SRC_FILES := \
    radeon_bo_gem.c \
    radeon_cs_gem.c \
    radeon_cs_space.c \
    radeon_bo.c \
    radeon_cs.c \
    radeon_surface.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH_MOCK)/codes \
    $(LOCAL_PATH_MOCK)/codes/include/drm

include $(LIBDRM_ANDROID_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)
