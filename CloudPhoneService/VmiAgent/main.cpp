/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/*
 * 功能说明:Agent程序main入口函数
 */
#define LOG_TAG "VmiExecutor"
#include <iostream>
#include <string>
#include "logging.h"
#include "VmiAgent.h"
#include "MainLooper.h"
#include "ApiTest.h"
#include "SystemProperty.h"

using namespace Vmi;

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;
    try {
        INFO("Agent begin");
        int32_t isApiTest = GetPropertyWithDefault("vmi.api.test", 0);
        if (isApiTest == 0) {
            static VmiAgent agent;
            if (!agent.Initialize()) {
                ERR("Failed to init agent");
                return -1;
            }
        } else {
            static ApiTest apiTest;
            apiTest.RunTests(static_cast<uint32_t>(isApiTest));
        }
        MainLooper::GetInstance().Run();
        return 0;
    } catch (const std::exception& e) {
        ERR("exception caught: %s !", e.what());
        return -1;
    }
}

