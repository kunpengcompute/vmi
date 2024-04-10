/**
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * 主线程执行的Looper
 * main函数初始化完毕后，将调用MainLooper::GetInstance().Run()等待其他任务并执行
 * 复用Looper功能，构造一个单例对象，保证其他地方也可以投递任务让主线程执行
 */
#ifndef MAIN_LOOPER_H
#define MAIN_LOOPER_H
#include "Utils/Looper.h"
namespace Vmi {
class MainLooper : public Looper {
public:
    static MainLooper& GetInstance();
private:
    MainLooper() = default;
    ~MainLooper() = default;
    MainLooper(const MainLooper&) = delete;
    MainLooper(MainLooper &&) = delete;
    MainLooper& operator=(const MainLooper&) = delete;
    MainLooper& operator=(MainLooper &&) = delete;
};
}
#endif