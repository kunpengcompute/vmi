/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明：AOSP libnativewindow.so库假实现
 */

#include "android/native_window.h"

void ANativeWindow_acquire(ANativeWindow* window) {}

void ANativeWindow_release(ANativeWindow* window) {}

void ANativeWindow_getWidth(ANativeWindow* window)
{
   return 0;
}

void ANativeWindow_getHeight(ANativeWindow* window)
{
   return 0;
}

void ANativeWindow_getFormat(ANativeWindow* window)
{
   return 0;
}

void ANativeWindow_setBuffersGeometry(ANativeWindow* window, int32_t width, int32_t height, int32_t format)
{
   return 0;
}

void ANativeWindow_lock(ANativeWindow* window, ANativeWindow_Buffer* outBuffer, ARect* inOutDirtyBounds)
{
   return 0;
}

void ANativeWindow_unlockAndPost(ANativeWindow* window)
{
   return 0;
}

#if __ANDROID_API__ >= 26

void ANativeWindow_setBuffersTranform(ANativeWindow* window, int32_t transform)
{
   return 0;
}

#endif // __ANDROID_API__ >= 26

#if __ANDROID_API__ >= 28

void ANativeWindow_setBuffersDataSpace(ANativeWindow* window, int32_t dataSpace)
{
   return 0;
}

void ANativeWindow_getBuffersDataSpace(ANativeWindow* window)
{
   return 0;
}

#endif // __ANDROID_API__ >= 28

#if __ANDROID_API__ >= 30

void ANativeWindow_setFrameRate(ANativeWindow* window, float frameRate, int8_t compatibility)
{
   return 0;
}

void ANativeWindow_tryAllocateBuffers(ANativeWindow* window) {}

#endif // __ANDROID_API__ >= 30
