/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：瀚博GPU抓图接口包裹层
 */

#ifndef HANTRO_CAPTURE_WRAPPER
#define HANTRO_CAPTURE_WRAPPER

#include <functional>
#include <ui/GraphicBuffer.h>

namespace Vmi {
struct HantroCaptureBuffer {
    uint64_t data = 0;
    android::GraphicBuffer *rgbBuffer = nullptr;
    std::function<void(android::GraphicBuffer *, int *, int *)> acquireBufferCaller = nullptr;
    std::function<void(android::GraphicBuffer *)> releaseBufferCaller = nullptr;
};
using FrameComeCallback = std::function<int(HantroCaptureBuffer *)>;
struct RefreshRequest {};
class HantroCaptureWrapperBase {
public:
    virtual void SetFrameCallback(FrameComeCallback frameCome) = 0;

    virtual bool RefreshDisplay() = 0;

    virtual bool GetDisplayInfo(unsigned int &width, unsigned int &height, unsigned int &density, unsigned int &fps);

    HantroCaptureWrapperBase() = default;
    virtual ~HantroCaptureWrapperBase() = default;

    virtual bool Init(bool sync) = 0;

    virtual void DeInit() = 0;

    virtual bool Start() = 0;

    virtual void Stop() = 0;
};
}

#ifdef __cplusplus
extern "C" {
#endif
    void *CreateHantroCaptureWrapper();
#ifdef __cplusplus
}
#endif
#endif