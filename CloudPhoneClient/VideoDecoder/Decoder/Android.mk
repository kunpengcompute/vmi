LOCAL_PATH := $(call my-dir)
REPO_ROOT := $(LOCAL_PATH)/../../../
include $(CLEAR_VARS)
LOCAL_MODULE := Decoder

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
LOCAL_CFLAGS   += -D_FORTIFY_SOURCE=2 -O2
LOCAL_CPPFLAGS += -D_FORTIFY_SOURCE=2 -O2
endif

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH) \
        $(REPO_ROOT)/Common/Include \
        $(REPO_ROOT)/Common/Utils \
        $(REPO_ROOT)/Common/Log

LOCAL_SRC_FILES := \
        DecodeController.cpp \
        VideoUtil.cpp \
        $(REPO_ROOT)/Common/Log/logging.cpp \
        $(REPO_ROOT)/Common/Log/LogInfo.cpp

LOCAL_LDLIBS := -lmediandk -llog
include $(BUILD_SHARED_LIBRARY)
