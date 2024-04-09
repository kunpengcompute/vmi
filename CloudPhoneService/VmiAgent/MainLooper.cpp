/**
 * 版权所有 (c) 华为技术有限公司 2023-2024
 * 主线程执行的Looper
 * main函数初始化完毕后，将调用MainLooper::GetInstance().Run()等待其他任务并执行
 * 复用Looper功能，构造一个单例对象，保证其他地方也可以投递任务让主线程执行
 */
#include "MainLooper.h"
namespace Vmi {
MainLooper& MainLooper::GetInstance()
{
    static MainLooper instance;
    return instance;
}
}