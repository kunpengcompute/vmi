/*
 * 版权所有 (c) 华为技术有限公司 2025-2025
 * 功能描述：芯动Inno抓图接口包裹层实现（参照HantroCaptureWrapper.cpp实现）
 */

#include <dlfcn.h>
#include <memory>
#include <mutex>
#include <thread>
#include <pthread.h>
#include "logging.h"
#include "InnoCaptureWrapperBase.h"
#include "ifbc_api.h"

namespace Vmi {
namespace {
#ifdef __LP64__
constexpr char INNO_IFBC_LIB_PATH[] = "/system/lib64/libifbc.so";
#else
constexpr char INNO_IFBC_LIB_PATH[] = "/system/lib/libifbc.so";
#endif
constexpr uint32_t DEFAULT_FRAME_RATE = 30;
constexpr uint32_t TIMEOUT_EXTRA_US = 4 * 1000;
}

class InnoCaptureWrapper : public InnoCaptureWrapperBase {
public:
    InnoCaptureWrapper() = default;
    ~InnoCaptureWrapper() override = default;

    void SetFrameCallback(InnoFrameComeCallback frameCome) override
    {
        m_frameCallback = frameCome;
    }

    bool Init(bool sync) override
    {
        (void)sync;
        m_libHandle = dlopen(INNO_IFBC_LIB_PATH, RTLD_LAZY);
        if (m_libHandle == nullptr) {
            ERR("Cannot open ifbc lib: %s", dlerror());
            return false;
        }

        m_ifbcCaptureInit = reinterpret_cast<IfbcCaptureInitFunc>(dlsym(m_libHandle, "ifbc_capture_init"));
        m_ifbcCaptureDeinit = reinterpret_cast<IfbcCaptureDeinitFunc>(dlsym(m_libHandle, "ifbc_capture_deinit"));
        m_ifbcCaptureGrab = reinterpret_cast<IfbcCaptureGrabFunc>(dlsym(m_libHandle, "ifbc_capture_grab"));
        m_ifbcReleaseFrame = reinterpret_cast<IfbcReleaseFrameFunc>(dlsym(m_libHandle, "ifbc_release_frame"));
        if (m_ifbcCaptureInit == nullptr || m_ifbcCaptureDeinit == nullptr ||
            m_ifbcCaptureGrab == nullptr || m_ifbcReleaseFrame == nullptr) {
            ERR("Cannot find ifbc capture symbols: %s", dlerror());
            UnloadIfbcLib();
            return false;
        }

        uint32_t captureAttribs[] = {
            CAPTURE_FEATURE_ID_MODE,        FEATURE_MODE_VD_PVRIC,
            CAPTURE_FEATURE_ID_GRAB,        FEATURE_GRAB_ASYNC,
            CAPTURE_FEATURE_ID_ORIENTATION, FEATURE_ORIENTATION_VERTICAL,
            CAPTURE_FEATURE_ID_NONE};
        m_captureHandle = m_ifbcCaptureInit(captureAttribs);
        if (m_captureHandle == nullptr) {
            ERR("ifbc_capture_init failed");
            UnloadIfbcLib();
            return false;
        }
        return true;
    }

    void DeInit() override
    {
        Stop();
        if (m_captureHandle != nullptr) {
            m_ifbcCaptureDeinit(m_captureHandle);
            m_captureHandle = nullptr;
        }
        UnloadIfbcLib();
    }

    bool Start() override
    {
        std::lock_guard<std::mutex> lk(m_threadLock);
        if (m_captureThread.joinable()) {
            return true;
        }
        m_running = true;
        m_captureThread = std::thread(&InnoCaptureWrapper::CaptureThreadFunc, this);
        return true;
    }

    void Stop() override
    {
        {
            std::lock_guard<std::mutex> lk(m_threadLock);
            m_running = false;
        }
        if (m_captureThread.joinable()) {
            m_captureThread.join();
        }
    }

    void SetFrameRate(uint32_t fps) override
    {
        if (fps != 0) {
            m_frameRate = fps;
        }
    }

private:
    void CaptureThreadFunc()
    {
        pthread_setname_np(pthread_self(), "inno_capture");
        while (IsRunning()) {
            uint32_t timeout = TIMEOUT_EXTRA_US + 1000 * 1000 / m_frameRate;

            ifbc_frame_t frameArray[1] = {};
            frameArray[0].buf = nullptr;
            frameArray[0].fd = -1;
            ifbc_frame_desc_t srcFrame = {};
            srcFrame.num = 1;
            srcFrame.frame = frameArray;

            int result = m_ifbcCaptureGrab(m_captureHandle, &srcFrame, timeout);
            if (result == IFBC_ERR_COMMON) {
                continue;   // 超时或屏幕未更新
            } else if (result != IFBC_ERR_NONE) {
                ERR("ifbc_capture_grab error=%d", result);
                break;      // 异常错误退出抓帧线程
            }

            // 分辨率变化检测
            if (m_screenInfo.width != frameArray[0].width || m_screenInfo.height != frameArray[0].height) {
                m_screenInfo.width = frameArray[0].width;
                m_screenInfo.height = frameArray[0].height;
                INFO("inno capture screen size change to %ux%u", m_screenInfo.width, m_screenInfo.height);
            }

            InnoCaptureBuffer capBuf;
            capBuf.data = static_cast<uint64_t>(static_cast<uintptr_t>(frameArray[0].fd));
            capBuf.width = frameArray[0].width;
            capBuf.height = frameArray[0].height;
            int ret = (m_frameCallback != nullptr) ? m_frameCallback(&capBuf) : 0;
            // 与DisplayDataClientInno语义一致：回调返回后归还帧
            m_ifbcReleaseFrame(m_captureHandle, &srcFrame);
            if (ret != 0) {
                continue;
            }
        }
        INFO("InnoCaptureWrapper capture thread exit");
    }

    bool IsRunning()
    {
        std::lock_guard<std::mutex> lk(m_threadLock);
        return m_running;
    }

    void UnloadIfbcLib()
    {
        m_ifbcCaptureInit = nullptr;
        m_ifbcCaptureDeinit = nullptr;
        m_ifbcCaptureGrab = nullptr;
        m_ifbcReleaseFrame = nullptr;
        if (m_libHandle != nullptr) {
            (void)dlclose(m_libHandle);
            m_libHandle = nullptr;
        }
    }

    using IfbcCaptureInitFunc = ifbc_capture_t(*)(const uint32_t*);
    using IfbcCaptureDeinitFunc = void(*)(ifbc_capture_t);
    using IfbcCaptureGrabFunc = int(*)(ifbc_capture_t, ifbc_frame_desc_t*, uint32_t);
    using IfbcReleaseFrameFunc = int(*)(ifbc_capture_t, const ifbc_frame_desc_t*);

    InnoFrameComeCallback m_frameCallback {nullptr};
    void* m_libHandle {nullptr};
    ifbc_capture_t m_captureHandle {nullptr};
    IfbcCaptureInitFunc m_ifbcCaptureInit {nullptr};
    IfbcCaptureDeinitFunc m_ifbcCaptureDeinit {nullptr};
    IfbcCaptureGrabFunc m_ifbcCaptureGrab {nullptr};
    IfbcReleaseFrameFunc m_ifbcReleaseFrame {nullptr};
    ifbc_screen_info_t m_screenInfo {};
    uint32_t m_frameRate {DEFAULT_FRAME_RATE};
    std::thread m_captureThread {};
    std::mutex m_threadLock {};
    bool m_running {false};
};
}

void *CreateInnoCaptureWrapper()
{
    auto ptr = std::make_unique<Vmi::InnoCaptureWrapper>().release();
    return reinterpret_cast<void *>(ptr);
}
