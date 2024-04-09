/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：瀚博GPU抓图接口包裹层
 */

#include <memory>
#include "logging.h"
#include "DisplayServer/DisplayServer.h"
#include "HantroCaptureWrapperBase.h"

namespace Vmi {
class HantroCaptureWrapper : public HantroCaptureWrapperBase {
public:
    void SetFrameCallback(FrameComeCallback frameCome) override
    {
        m_frameCallback = frameCome;
    }

    bool RefreshDisplay() override
    {
        RefreshReq req;
        return m_capture->RefreshDisplay(req);
    }

    bool GetDisplayInfo(unsigned int &width, unsigned int &height, unsigned int &density, unsigned int &fps)
    {
        return m_capture->GetDisplayInfo(&width, &height, &density, &fps);
    }

    HantroCaptureWrapper() = default;
    ~HantroCaptureWrapper() = default;

    bool Init(bool sync) override
    {
        DisplayServer::Mode mode;
        if (sync) {
            mode = DisplayServer::Mode::Sync;
        } else {
            mode = DisplayServer::Mode::Async;
        }
        m_capture = std::make_unique<DisplayServer>(mode);
        auto callback = [this](android::GraphicBuffer *buffer) {
            HantroCaptureBuffer* capBuff = new HantroCaptureBuffer();
            capBuff->data = getPaddr(buffer->handle);
            capBuff->rgbBuffer = buffer;
            capBuff->acquireBufferCaller = std::bind(&HantroCaptureWrapper::AcquireBuffer, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            capBuff->releaseBufferCaller = std::bind(&HantroCaptureWrapper::ReleaseBuffer, this, std::placeholders::_1);
            int ret = m_frameCallback(capBuff);
            if (ret != 0) {
                ERR("Hantro capture try to import rgb buffer failed: %d", ret);
                delete capBuff;
            }
            return ret;
        };
        return m_capture->Init(callback);
    }

    void DeInit() override
    {
        m_capture = nullptr;
    }

    bool Start() override
    {
        m_capture->Run();
        return true;
    }

    void Stop() override
    {
        m_capture->Stop();
    }

    void AcquireBuffer(android::GraphicBuffer *buffer, int *acquireFence, int *releaseFence)
    {
        m_capture->AcquireBuffer(buffer, acquireFence, releaseFence);
    }

    void ReleaseBuffer(android::GraphicBuffer *buffer)
    {
        m_capture->ReleaseBuffer(buffer);
    }
private:
    std::unique_ptr<DisplayServer> m_capture { nullptr };
    FrameComeCallback m_frameCallback { nullptr };
};
}

void *CreateHantroCaptureWrapper()
{
    auto ptr = std::make_unique<Vmi::HantroCaptureWrapper>().release();
    return reinterpret_cast<void *>(ptr);
}