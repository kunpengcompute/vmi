/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * vmi is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/*
 * 功能说明: 消息处理线程，将消息处理函数需继承自Runnable类，重新实现的Run函数即为消息处理的函数。
 */
#define LOG_TAG "Looper"
#include "Looper.h"
#include "logging.h"
#include "SmartPtrMacro.h"

namespace Vmi {
void Looper::SendMsg(std::unique_ptr<Runnable> runnable)
{
    std::unique_lock<std::mutex> lock(m_looperLock);
    m_runnables.push_back(std::move(runnable));
    m_looperCondition.notify_one();
}

void Looper::Run()
{
    INFO("Looper start running.");
    while (true) {
        std::vector<std::unique_ptr<Runnable>> curRunnables;
        {
            std::unique_lock<std::mutex> lock(m_looperLock);
            m_looperCondition.wait(lock, [this]() -> bool {
                return !m_runnables.empty();
            });
            curRunnables.swap(m_runnables);
        }
        for (auto& runnable : curRunnables) {
            if (runnable->Run() == -1) {
                INFO("Looper exit");
                return;
            }
        }
    }
}
}