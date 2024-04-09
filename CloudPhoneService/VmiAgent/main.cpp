/*
 * 版权所有 (c) 华为技术有限公司 2017-2023
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

