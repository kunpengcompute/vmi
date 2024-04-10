/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能描述：THREAD_POOL
 */

#ifndef VMI_THREAD_POOL
#define VMI_THREAD_POOL

#include <thread>
#include <future>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>
#include "logging.h"

namespace Vmi {
class ThreadPool {
public:
    ThreadPool() = default;
    ~ThreadPool()
    {
        DeInit();
    }

    void Init(uint32_t threads)
    {
        std::unique_lock<std::mutex> lk(m_lock);
        m_exit = false;
        m_queue = {};
        for (uint32_t i = 0; i < threads; ++i) {
            m_threads.emplace_back(std::thread(&ThreadPool::WorkerFunc, this));
        }
    }

    void DeInit()
    {
        std::unique_lock<std::mutex> lk(m_lock);
        if (m_exit) {
            return;
        }
        m_exit = true;
        lk.unlock();
        m_cv.notify_all();
        for (auto &&it : m_threads) {
            if (it.joinable()) {
                it.join();
            }
        }
        m_threads.clear();
    }

    template <typename F, typename ...T>
    auto Enqueue(F&& f, T&&... args)
    {
        using R = typename std::result_of<F(T...)>::type;
        constexpr uint32_t MAX_TASK_NUM = 64;
        if (m_queue.size() > MAX_TASK_NUM) {
            // 避免任务堆积
            std::future<R> dummy;
            return dummy;
        }
        auto task = new std::packaged_task<R()>(std::bind(std::forward<F>(f), std::forward<T>(args)...));
        auto result = task->get_future();
        auto threadTask = [task]() {
            (*task)();
            delete task;
        };
        std::lock_guard<std::mutex> l(m_lock);
        m_queue.emplace(std::move(threadTask));
        m_cv.notify_one();
        return result;
    };

private:
    void WorkerFunc()
    {
        while (true) {
            std::unique_lock<std::mutex> lk(m_lock);
            m_cv.wait(lk, [this]() { return this->m_exit || !this->m_queue.empty(); });
            if (m_exit && this->m_queue.empty()) {
                break;
            }
            auto task = std::move(m_queue.front());
            m_queue.pop();
            lk.unlock();
            task();
        }
    }

    std::mutex m_lock {};
    std::condition_variable m_cv {};
    std::queue<std::function<void()>> m_queue {};
    std::vector<std::thread > m_threads {};
    bool m_exit {false};
};
}
#endif
