/*
 * 版权所有 (c) 华为技术有限公司 2024-2024
 * 功能说明:对外接口的测试套，测试接口的安全性、可用性、性能指标
 */
#ifndef API_TEST_H
#define API_TEST_H
#include <string>
#include <chrono>
#include "VmiApi.h"
#include "ApiTestList.h"
#include "logging.h"
#include "Looper.h"

#define RUN_BEGIN     RunTestHelp helper(true, __FUNCTION__, __LINE__)
#define RUN_ABNORMAL(fmt, ...)  helper.Abnormal(__FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define EXPECT_EQ(actual, expect) \
if ((actual) != (expect)) { \
    RUN_ABNORMAL("actual:%d, expect:%d", (actual), (expect)); \
    return false; \
}

#define EXECUTE(func) \
if (!func()) { \
    return; \
}

namespace Vmi {
class ExitRunnable : public Runnable {
public:
    int Run() override;
};

class RunTestHelp {
public:
    explicit RunTestHelp(bool isPrintTime, const char* functionName, int line);
    ~RunTestHelp();
    void Abnormal(const char* functionName, int line, const char* fmt, ...);
    uint32_t GetRunTime(); // 单位毫秒
private:
    bool m_isPrintTime = false;
    bool m_isAbnormal = false;
    int m_startLine = 0;
    std::string m_functionName;
    std::chrono::time_point<std::chrono::steady_clock> m_startTime;
};

class ApiTest {
public:
    void RunTests(uint32_t tests);
private:
    bool VersionTest();
    bool DeInitVmiEngineAbnormalTest();
    bool InitVmiEngineAbnormalTest();
    bool CallOtherApiWithoutInit();
    bool InitVmiEngineNormalTest();
    bool InitVmiEngineCompleteAbnormalTest();
    bool StartModuleAbnormalTest();
    bool StartModuleNormalTest();
    bool StopModuleNormalTest();
    bool StopModuleAbnormalTest();
    bool InjectDataAbnormalTest();
    bool InjectDataNormalTest();
    bool SetParamAbnormalTest();
    bool SetParamNormalTest();
    bool GetParamAbnormalTest();
    bool GetParamNormalTest();
    bool RepeatStartAndStopModule();
    bool InjectDataPerforTimes();
    bool InjectMicDataPerforTimes();
    bool SetParamPerforTimes();
    bool DeInitVmiEngineNormalTest();
    void InitNormalConfigEngine(VmiConfigEngine& config);
    void InitNormalTouchInputData(VmiTouchInputData& data);
    void InitMicData(uint8_t* micData, uint32_t size);

    void BaseTest();
    void KeepRunningTest(uint32_t tests);
    void StartStopTest();
    void ResolutionChangeTest();
};
}
#endif