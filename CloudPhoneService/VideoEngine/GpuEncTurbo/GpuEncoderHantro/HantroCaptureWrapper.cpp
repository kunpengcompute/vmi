/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：瀚博GPU抓图接口包裹层
 */

#include <memory>
#include "logging.h"
#include "DisplayServer/DisplayServerWrap.h"
#include "HantroCaptureWrapperBase.h"

namespace Vmi {
namespace {
    pDisplayServer g_capture { nullptr };
    FrameComeCallback g_frameCallback { nullptr };
}
class HantroCaptureWrapper : public HantroCaptureWrapperBase {
public:
    void SetFrameCallback(FrameComeCallback frameCome) override
    {
        g_frameCallback = frameCome;
    }

    bool RefreshDisplay() override
    {
        RefreshReqWrapper req;
        return DisplayServerRefreshDisplay(g_capture, req);
    }

    bool GetDisplayInfo(unsigned int &width, unsigned int &height, unsigned int &density, unsigned int &fps) override
    {
        return DisplayServerGetDisplayInfo(g_capture, &width, &height, &density, &fps);
    }

    HantroCaptureWrapper() = default;
    ~HantroCaptureWrapper() = default;

    static int InitCallback(pVaGraphicBuffer buffer)
    {
        HantroCaptureBuffer* capBuff = new (std::nothrow) HantroCaptureBuffer();
        if (capBuff == nullptr) {
            ERR("Failed to handle hantro init callback, make hantro capture buffer failed!");
            return -1;
        }
        capBuff->data = getPaddrWrap(getHandleWrap(buffer));
        capBuff->tmpStreamHeight = getHeightWrap(getHandleWrap(buffer));
        capBuff->tmpStreamWidth = getWidthWrap(getHandleWrap(buffer));
        capBuff->rgbBuffer = buffer;
        capBuff->acquireBufferCaller = std::bind(&HantroCaptureWrapper::AcquireBuffer,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        capBuff->releaseBufferCaller = std::bind(&HantroCaptureWrapper::ReleaseBuffer, std::placeholders::_1);
        capBuff->SetWmSizeCaller = std::bind(&HantroCaptureWrapper::SetWmSize, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        int ret = g_frameCallback(capBuff);
        if (ret != 0) {
            ERR("Hantro capture try to import rgb buffer failed: %d", ret);
            delete capBuff;
        }
        return ret;
    }

    bool Init(bool sync) override
    {
        DisplayServerMode mode;
        if (sync) {
            mode = DisplayServerMode::Sync;
        } else {
            mode = DisplayServerMode::Async;
        }
        g_capture = CreateDisplayServer(mode);
        return DisplayServerInit(g_capture, InitCallback, -1);
    }

    void DeInit() override
    {
        DestroyDisplayServer(g_capture);
        g_capture = nullptr;
    }

    bool Start() override
    {
        DisplayServerRun(g_capture);
        return true;
    }

    void Stop() override
    {
        DisplayServerStop(g_capture);
    }

    static void AcquireBuffer(pVaGraphicBuffer buffer, int *acquireFence, int *releaseFence)
    {
        DisplayServerAcquireBuffer(g_capture, buffer, acquireFence, releaseFence);
    }

    static void ReleaseBuffer(pVaGraphicBuffer buffer)
    {
        DisplayServerReleaseBuffer(g_capture, buffer);
    }

    static void SetWmSize(pVaGraphicBuffer buffer, unsigned int wmWidth, unsigned int wmHeight)
    {
        DisplayServerSetWmSize(g_capture, wmWidth, wmHeight);
    }
};
}

void *CreateHantroCaptureWrapper()
{
    auto ptr = std::make_unique<Vmi::HantroCaptureWrapper>().release();
    return reinterpret_cast<void *>(ptr);
}