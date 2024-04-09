LOCAL_CFLAGS += \
    -DMAJOR_IN_SYSMACROS=1 \
    -DHAVE_ALLOCA_H=0 \
    -DHAVE_VISIBILITY=1 \
    -fvisibility=hidden \
    -DHAVE_SYS_SELECT_H=0 \
    -DHAVE_SYS_SYSCTL_H=0 \
    -DHAVE_LIBDRM_ATOMIC_PRIMITIVES=1

LOCAL_CFLAGS += \
    -Wno-error \
    -Wno-pointer-arith \
    -Wno-enum-conversion \
    -Wno-unused-parameter \
    -Wno-missing-field-initializers

LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)
