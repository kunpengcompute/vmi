/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明：AOSP libnativewindow.so库假实现
 */

#include "android/native_window.h"

#define VMI_API __attribute__((visibility("default")))

VMI_API void ANativeWindow_acquire(ANativeWindow* window) {}

VMI_API void ANativeWindow_release(ANativeWindow* window) {}

VMI_API int32_t ANativeWindow_getWidth(ANativeWindow* window)
{
   return 0;
}

VMI_API int32_t ANativeWindow_getHeight(ANativeWindow* window)
{
   return 0;
}

VMI_API int32_t ANativeWindow_getFormat(ANativeWindow* window)
{
   return 0;
}

VMI_API int32_t ANativeWindow_setBuffersGeometry(ANativeWindow* window, int32_t width, int32_t height, int32_t format)
{
   return 0;
}

VMI_API int32_t ANativeWindow_lock(ANativeWindow* window, ANativeWindow_Buffer* outBuffer, ARect* inOutDirtyBounds)
{
   return 0;
}

VMI_API int32_t ANativeWindow_unlockAndPost(ANativeWindow* window)
{
   return 0;
}

#if __ANDROID_API__ >= 26

VMI_API int32_t ANativeWindow_setBuffersTransform(ANativeWindow* window, int32_t transform)
{
   return 0;
}

#endif // __ANDROID_API__ >= 26

#if __ANDROID_API__ >= 28

VMI_API int32_t ANativeWindow_setBuffersDataSpace(ANativeWindow* window, int32_t dataSpace)
{
   return 0;
}

VMI_API int32_t ANativeWindow_getBuffersDataSpace(ANativeWindow* window)
{
   return 0;
}

#endif // __ANDROID_API__ >= 28

#if __ANDROID_API__ >= 30

VMI_API int32_t ANativeWindow_setFrameRate(ANativeWindow* window, float frameRate, int8_t compatibility)
{
   return 0;
}

VMI_API void ANativeWindow_tryAllocateBuffers(ANativeWindow* window) {}

#endif // __ANDROID_API__ >= 30
