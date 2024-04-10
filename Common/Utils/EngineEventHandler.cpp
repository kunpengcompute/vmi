/*
 * 版权所有 (c) 华为技术有限公司 2017-2021
 * 功能说明: 统一处理引擎的上报事件
 */

#define LOG_TAG "EngineEventHandler"

#include "EngineEventHandler.h"
#include <algorithm>
#include "logging.h"

using namespace std;

namespace Vmi {
namespace {
    const uint32_t ENGINE_EVENT_SIZE = static_cast<uint32_t>(sizeof(EngineEvent));
}
EngineEventHandler::~EngineEventHandler()
{
    std::lock_guard<std::mutex> lk(m_lock);
    m_status = ThreadStatusType::VMI_THREAD_STATUS_EXIT;
}

EngineEventHandler& EngineEventHandler::GetInstance()
{
    static EngineEventHandler instance;
    return instance;
}

void *ThreadRunProxy(void* args)
{
    if (args == nullptr) {
        ERR("Failed to run thread in thread run proxy, args is nullptr");
        return nullptr;
    }
    auto thread = static_cast<EngineEventHandler *>(args);
    thread->Run();
    return args;
}

void EngineEventHandler::SetEventCallback(OnVmiEngineEvent callback)
{
    if (callback == nullptr) {
        ERR("Failed to set event callback in engine event handler, input callback is null.");
        return;
    }

    // 当前版本仅支持回调函数有效注册一次
    if (m_eventCallback != nullptr) {
        ERR("Failed to set event callback in engine event handler, callback function already seted.");
        return;
    }

    m_eventCallback = callback;
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_tid > 0) {
        INFO("Engine envent handler tid:%lu already started", m_tid);
    }

    m_status = ThreadStatusType::VMI_THREAD_STATUS_RUNNING;
    if (pthread_create(&m_tid, nullptr, ThreadRunProxy, this) != 0) {
        ERR("Failed to create thread in engine event handler, call pthread_create failed.");
        m_status = ThreadStatusType::VMI_THREAD_STATUS_EXIT;
        m_tid = 0;
    }
}

void EngineEventHandler::CreateEvent(EngineEvent event)
{
    EngineEvent* storeEvent = reinterpret_cast<EngineEvent*>(malloc(ENGINE_EVENT_SIZE));
    if (storeEvent == nullptr) {
        ERR("Failed to create event in engine event handler, malloc store event:%d(%d, %d, %d, %d) failed.",
            event.event, event.para1, event.para2, event.para3, event.para4);
        return;
    }
    *storeEvent =  event;
    m_eventQue.PutPkt(make_pair(reinterpret_cast<uint8_t*>(storeEvent), ENGINE_EVENT_SIZE));
}

pthread_t EngineEventHandler::GetThreadID()
{
    std::lock_guard<std::mutex> lk(m_lock);
    return m_tid;
}

void EngineEventHandler::Run()
{
    INFO("Event handle thread(%ld) is running.", GetThreadID());
    while (m_status == ThreadStatusType::VMI_THREAD_STATUS_RUNNING) {
        pair<uint8_t *, uint32_t> eventPair = m_eventQue.GetNextPkt();
        EngineEvent* event = reinterpret_cast<EngineEvent*>(eventPair.first);
        if (event == nullptr) {
            ERR("Failed to run engine event handler, get null event!");
            continue;
        }

        if (eventPair.second != ENGINE_EVENT_SIZE) {
            ERR("Failed to run engine event handler, wrong event size! size:%u, valid size:%u",
                eventPair.second, ENGINE_EVENT_SIZE);
            free(event);
            event = nullptr;
            continue;
        }

        if (m_eventCallback != nullptr) {
            INFO("Handle event:%d(%d, %d, %d, %d).", event->event,
                event->para1, event->para2, event->para3, event->para4);
            m_eventCallback(*event);
        }

        free(event);
        event = nullptr;
    }
    INFO("Event handle thread(%ld) stop.", GetThreadID());
}
} // namespace Vmi
