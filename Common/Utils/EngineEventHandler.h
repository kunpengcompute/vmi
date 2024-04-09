/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明: 统一处理引擎的上报事件
 */

#ifndef ENGINE_EVENT_HANDLER
#define ENGINE_EVENT_HANDLER

#include <mutex>
#include <pthread.h>
#include <cstdint>
#include "VmiEngine.h"
#include "PacketQueue.h"

namespace Vmi {
enum class ThreadStatusType {
    VMI_THREAD_STATUS_EXIT = -1,
    VMI_THREAD_STATUS_NEW = 0,
    VMI_THREAD_STATUS_RUNNING = 1
};

class __attribute__ ((visibility ("default"))) EngineEventHandler {
public:
    static EngineEventHandler& GetInstance();
    void SetEventCallback(OnVmiEngineEvent callback);
    void CreateEvent(EngineEvent event);
    void Run();

private:
    EngineEventHandler() = default;
    ~EngineEventHandler();
    EngineEventHandler(const EngineEventHandler&) = delete;
    EngineEventHandler& operator=(const EngineEventHandler&) = delete;
    EngineEventHandler(EngineEventHandler&&) = delete;
    EngineEventHandler& operator=(EngineEventHandler&&) = delete;

    pthread_t GetThreadID();

    PacketQueue m_eventQue {true};
    OnVmiEngineEvent m_eventCallback = nullptr;
    pthread_t m_tid = 0;
    volatile ThreadStatusType m_status = ThreadStatusType::VMI_THREAD_STATUS_NEW;
    std::mutex m_lock = {};
};

} // namespace Vmi
#endif
