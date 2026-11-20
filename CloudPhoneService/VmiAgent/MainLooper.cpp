/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/**
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