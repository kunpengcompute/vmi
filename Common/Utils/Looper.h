/*
 * 版权所有 (c) 华为技术有限公司 2022-2022
 * 功能说明: 消息处理线程，将消息处理函数需继承自Task类，重新实现的Run函数即为消息处理的函数。
 */
#ifndef LOOPER_H
#define LOOPER_H
#include <vector>
#include <mutex>
#include <functional>
#include <condition_variable>
namespace Vmi {
using RunnableDealFunc = std::function<void(void)>;
class Runnable {
public:
    Runnable() = default;
    virtual ~Runnable() = default;
    /*
     * 返回-1表示需退出整个线程
     */
    virtual int Run() = 0;
};

class Looper {
public:
    Looper() = default;
    virtual ~Looper() = default;
    void SendMsg(std::unique_ptr<Runnable> runnable);
    void Run();
private:
    std::vector<std::unique_ptr<Runnable>> m_runnables;
    std::mutex m_looperLock;
    std::condition_variable m_looperCondition;
};
}
#endif