/*
 * 版权所有 (c) 华为技术有限公司 2025-2025
 * 功能描述：芯动Inno抓图接口包裹层基类（参照HantroCaptureWrapperBase.h实现）
 */

#ifndef INNO_CAPTURE_WRAPPER_BASE
#define INNO_CAPTURE_WRAPPER_BASE

#include <cstdint>
#include <functional>

namespace Vmi {

// Inno帧描述：data为DMA-BUF fd（不透明句柄，由GpuEncoderInno还原）
struct InnoCaptureBuffer {
    uint64_t data = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t transform = 0;
};

using InnoFrameComeCallback = std::function<int(InnoCaptureBuffer *)>;

class InnoCaptureWrapperBase {
public:
    virtual void SetFrameCallback(InnoFrameComeCallback frameCome) = 0;

    virtual bool Init(bool sync) = 0;

    virtual void DeInit() = 0;

    virtual bool Start() = 0;

    virtual void Stop() = 0;

    // 抓帧超时依据的帧率，Hantro默认忽略
    virtual void SetFrameRate(uint32_t /*fps*/) {}

    InnoCaptureWrapperBase() = default;
    virtual ~InnoCaptureWrapperBase() = default;
};

}

#ifdef __cplusplus
extern "C" {
#endif
    void *CreateInnoCaptureWrapper();
#ifdef __cplusplus
}
#endif
#endif
