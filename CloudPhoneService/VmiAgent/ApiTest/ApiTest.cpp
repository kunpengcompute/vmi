/*
 * 版权所有 (c) 华为技术有限公司 2024-2024
 * 功能说明:对外接口的测试套，测试接口的安全性、可用性、性能指标
 */
#define LOG_TAG "APITEST"
#include "ApiTest.h"
#include "ThreadPool.h"
#include <chrono>
#include <random>
#include <string>
#include <unistd.h>
#include "logging.h"
#include "MainLooper.h"
#include "NetworkCommManager.h"
#include "VmiApi.h"
#include "SmartPtrMacro.h"

namespace Vmi {
int ExitRunnable::Run()
{
    return -1;
}

RunTestHelp::RunTestHelp(bool isPrintTime, const char* functionName, int line) : m_isPrintTime(isPrintTime) {
    m_startTime = std::chrono::steady_clock::now();
    if (!isPrintTime) {
        return;
    }
    m_startLine = line;
    m_functionName = functionName;
    INFO("Begin Run %s:%d tests", functionName, line);
}

RunTestHelp::~RunTestHelp()
{
    if (!m_isPrintTime || m_isAbnormal) {
        return;
    }
    std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
    auto diff = endTime - m_startTime;
    auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
    INFO("End Run %s:%d tests, success, run time:%d ms", m_functionName.c_str(), m_startLine, static_cast<uint32_t>(diffTime));
}

void RunTestHelp::Abnormal(const char* functionName, int line, const char* fmt, ...)
{
    m_isAbnormal = true;
    constexpr size_t maxLogLen = 1024;
    char szBuff[maxLogLen] = { 0 };
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(szBuff, maxLogLen, fmt, ap);
    va_end(ap);
    if (ret <= 0) {
        WARN("Failed to vsnprintf, ret:%d", ret);
    }
    szBuff[maxLogLen - 1] = '\0';
    ERR("Abnormal, End Run %s:%d tests, err:%s", functionName, line, szBuff);
    // send exit task
    std::unique_ptr<ExitRunnable> exitRunable = nullptr;
    MAKE_UNIQUE_NOTHROW(exitRunable, ExitRunnable);
    if (exitRunable == nullptr) {
        ERR("Failed to new exit task");
        return;
    }
    (void) MainLooper::GetInstance().SendMsg(std::move(exitRunable));
}

uint32_t RunTestHelp::GetRunTime()
{
    std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
    auto diff = endTime - m_startTime;
    auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
    return static_cast<uint32_t>(diffTime);
}

void ApiTest::BaseTest()
{
    EXECUTE(VersionTest);
    EXECUTE(DeInitVmiEngineAbnormalTest);
    EXECUTE(InitVmiEngineAbnormalTest);
    EXECUTE(CallOtherApiWithoutInit);
    EXECUTE(InitVmiEngineNormalTest);
    EXECUTE(InitVmiEngineCompleteAbnormalTest);
    EXECUTE(StartModuleAbnormalTest);
    sleep(15); // 睡眠15s, 等待VmiInputFlinger启动，否则Start会失败
    EXECUTE(StartModuleNormalTest);
    EXECUTE(StopModuleNormalTest);
    EXECUTE(StopModuleAbnormalTest);
    EXECUTE(InjectDataAbnormalTest);
    EXECUTE(InjectDataNormalTest);
    EXECUTE(SetParamAbnormalTest);
    EXECUTE(SetParamNormalTest);
    EXECUTE(GetParamAbnormalTest);
    EXECUTE(GetParamNormalTest);
    EXECUTE(RepeatStartAndStopModule);
    EXECUTE(InjectDataPerforTimes);
    EXECUTE(InjectMicDataPerforTimes);
    EXECUTE(SetParamPerforTimes);
    EXECUTE(DeInitVmiEngineNormalTest);
}

void GenVideoParam(EncodeParams &encoderParams)
{
    std::random_device rd;
    std::default_random_engine engine(rd());

    // 1/2概率使用错误的分布尝试生成错误参数
    std::uniform_int_distribution<> errorDistrib(0, 1);
    bool error = errorDistrib(engine) == 1;

    // valid range 1000000 - 10000000
    std::uniform_int_distribution<> bitrateDistrib(1000000, 10000000);
    if (error) {
        bitrateDistrib = std::uniform_int_distribution<>(500000, 15000000);
    }
    encoderParams.bitrate = bitrateDistrib(engine);

    // valid range 30 - 3000
    std::uniform_int_distribution<> gopSizeDistrib(30, 3000);
    if (error) {
        gopSizeDistrib = std::uniform_int_distribution<>(15, 5000);
    }
    encoderParams.gopSize = gopSizeDistrib(engine);

    // valid range BASELINE, MAIN, HIGH
    std::uniform_int_distribution<> profileDistrib(BASELINE, HIGH);
    if (error) {
        profileDistrib = std::uniform_int_distribution<>(BASELINE, HIGH + 2);
    }
    encoderParams.profile = static_cast<ProfileType>(profileDistrib(engine));

    // 当前该参数不会修改，暂不做变异
    // encoderParams.rcMode = CBR;

    // valid range 0, 1，大概率不强制I帧，这里特殊处理，1/10概率强制I帧
    std::uniform_int_distribution<> keyFrameDistrib(1, 10);
    int shouldForceKeyFrame = keyFrameDistrib(engine);
    if (shouldForceKeyFrame == 1) {
        encoderParams.forceKeyFrame = 1;
    }

    // 1/10的概率反转补帧参数
    static bool interpolation = true;
    std::uniform_int_distribution<> interpolationDistrib(1, 10);
    int shouldChangeInterpolation = interpolationDistrib(engine);
    if (shouldChangeInterpolation == 1) {
        interpolation = !interpolation;
    }
    encoderParams.interpolation = interpolation;
}

void GenAudioParam(AudioPlayParams &audioParams)
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    // 1/2概率使用错误的分布尝试生成错误参数
    std::uniform_int_distribution<> errorDistrib(0, 1);
    bool error = errorDistrib(engine) == 1;

    // valid range 13200 - 512000
    std::uniform_int_distribution<> bitrateDistrib(13200, 512000);
    if (error) {
        bitrateDistrib = std::uniform_int_distribution<>(0, 1280000);
    }
    audioParams.bitrate = bitrateDistrib(engine);

    // 目前只支持10ms，不做变异
    audioParams.sampleInterval = 10;
}

void ApiTest::KeepRunningTest(uint32_t tests)
{
    RUN_BEGIN;
    if (!InitVmiEngineNormalTest()) {
        return;
    }

    sleep(15); // 睡眠15s, 等待VmiInputFlinger启动，否则Start会失败

    if (!StartModuleNormalTest()) {
        return;
    }

    sleep(5);

    using namespace std::chrono;
    using namespace std::chrono_literals;
    constexpr uint64_t STEP_TIME_US = 1000;

    auto startTime = system_clock::now();
    bool running = true;
    ThreadPool pool;
    // 最多4个线程，对应视频参数，音频参数，麦克风注入，触控注入
    pool.Init(4);

    auto lastSetVideoParam = system_clock::now();
    auto lastSetAudioParam = system_clock::now();
    auto lastSendMicData = system_clock::now();

    while (running) {
        auto stepStart = system_clock::now();

        // 如果启动视频编码参数测试，每20ms修改一次视频参数
        if ((tests & VIDEO_SET_PARAM) && (stepStart - lastSetVideoParam >= 20ms)) {
            pool.Enqueue([this](){
                EncodeParams params;
                GenVideoParam(params);
                return SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, 
                    reinterpret_cast<uint8_t*>(&params), sizeof(params));
            });
            lastSetVideoParam = stepStart;
        }

        // 如果启动音频编码参数测试，每15ms修改一次音频参数
        if ((tests & AUDIO_SET_PARAM) && (stepStart - lastSetAudioParam >= 15ms)) {
            pool.Enqueue([this](){
                AudioPlayParams params;
                GenAudioParam(params);
                return SetParam(VmiDataType::DATA_AUDIO, VmiCmd::AUDIO_SET_AUDIOPLAY_PARAM, 
                    reinterpret_cast<uint8_t*>(&params), sizeof(params));
            });
            lastSetAudioParam = stepStart;
        }

        // 如果启动麦克风注入测试，每10ms注入一次麦克风数据
        if ((tests & MIC_INJECT_DATA) &&  (stepStart - lastSendMicData >= 10ms)) {
            pool.Enqueue([this](){
                uint8_t micData[240] = {0};
                InitMicData(micData, sizeof(micData));
                return InjectData(VmiDataType::DATA_MIC, VmiCmd::MIC_SEND_MIC_DATA,
                    micData, sizeof(micData));
            });
            lastSendMicData = stepStart;
        }

        // 如果启动触控注入测试，不做频率限制
        if (tests & TOUCH_INJECT_DATA) {
            pool.Enqueue([this](){
                VmiTouchInputData inputData;
                InitNormalTouchInputData(inputData);
                return InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
                    reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
            });
        }

        auto now = system_clock::now();
        auto duration = now - startTime;
        // 运行7*24H
        if (duration > (7 * 24h)) {
            running = false;
            break;
        }

        auto stepDuration = duration_cast<microseconds>(now - stepStart).count();
        if (stepDuration < STEP_TIME_US) {
            uint32_t sleepTime = STEP_TIME_US - stepDuration;
            usleep(sleepTime);
        }
    }

    pool.DeInit();
}


void ApiTest::StartStopTest()
{
    RUN_BEGIN;
    if (!InitVmiEngineNormalTest()) {
        return;
    }
 
    sleep(15); // 睡眠15s, 等待VmiInputFlinger启动，否则Start会失败

    bool running = true;
    std::random_device rd;
    std::default_random_engine engine(rd());
    // start和stop间隔随机1-500ms
    std::uniform_int_distribution<int> dist(1, 500000);

    using namespace std::chrono;
    using namespace std::chrono_literals;
    auto startTime = system_clock::now();

    while (running) {
        // 启动视频
        VmiConfigVideo videoConfig;
        StartModule(VmiDataType::DATA_VIDEO, reinterpret_cast<uint8_t*>(&videoConfig), sizeof(videoConfig));
        // 启动音频
        VmiConfigAudio audioConfig;
        StartModule(VmiDataType::DATA_AUDIO, reinterpret_cast<uint8_t*>(&audioConfig), sizeof(audioConfig));
        // 启动触控
        VmiConfig config;
        StartModule(VmiDataType::DATA_TOUCH, reinterpret_cast<uint8_t*>(&config), sizeof(config));
        // 启动麦克风
        VmiConfigMic micConfig;
        StartModule(VmiDataType::DATA_MIC, reinterpret_cast<uint8_t*>(&micConfig), sizeof(micConfig));

        auto sleepTime = dist(engine);
        usleep(sleepTime);
 
        for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
            VmiErrCode ret = StopModule(static_cast<VmiDataType>(i));
        }

        auto now = system_clock::now();
        auto duration = now - startTime;
        // 运行7*24H
        if (duration > (7 * 24h)) {
            running = false;
            break;
        }
    }
}

void ApiTest::ResolutionChangeTest()
{
    RUN_BEGIN;
    if (!InitVmiEngineNormalTest()) {
        return;
    }
 
    sleep(15); // 睡眠15s, 等待VmiInputFlinger启动，否则Start会失败

    bool running = true;
    std::random_device rd;
    std::default_random_engine engine(rd());

    std::pair<uint32_t, uint32_t> possibleRes[] = {
        {720, 1280},
        {720, 1440},
        {720, 1520},
        {720, 1560},
        {720, 1600},
        {1080, 1920},
        {972, 1944},
        {972, 2052},
        {972, 2106},
        {972, 2160},
    };

    // 随机选择一个分辨率
    std::uniform_int_distribution<int> dist(0, sizeof(possibleRes) / sizeof(possibleRes[0]) - 1);

    using namespace std::chrono;
    using namespace std::chrono_literals;
    auto startTime = system_clock::now();
    int lastSecond = 0;

    while (running) {
        int i = dist(engine);

        auto now  = system_clock::now();
        auto time = system_clock::to_time_t(now);
        auto local_time = std::localtime(&time);
        // 固定每分钟每5秒修改一次分辨率，可以用于多容器同时设置分辨率
        while (local_time->tm_sec == lastSecond || local_time->tm_sec % 5 != 0) {
            usleep(10000);
            now  = system_clock::now();
            time = system_clock::to_time_t(now);
            local_time = std::localtime(&time);
        }
        lastSecond = local_time->tm_sec;

        VmiConfigVideo videoConfig;
        videoConfig.resolution.width = possibleRes[i].first;
        videoConfig.resolution.height = possibleRes[i].second;

        StartModule(VmiDataType::DATA_VIDEO, reinterpret_cast<uint8_t*>(&videoConfig), sizeof(videoConfig));
        
        usleep(100000);
 
        StopModule(VmiDataType::DATA_VIDEO);

        now = system_clock::now();
        auto duration = now - startTime;
        // 运行7*24H
        if (duration > (7 * 24h)) {
            running = false;
            break;
        }
    }
}

void ApiTest::RunTests(uint32_t tests)
{
    if (tests & BASE_TEST) {
        BaseTest();
    } else if (tests & KEEP_RECV_DATA) {
        KeepRunningTest(tests);
    } else if (tests & START_STOP) {
        StartStopTest();
    } else if (tests & RESOLUTION_CHANGE) {
        ResolutionChangeTest();
    }
}

int EngineDataCallback(VmiDataType module, VmiCmd cmd, uint8_t *data, uint32_t size)
{
    (void) data;
    INFO("Recv modle:%d cmd:%u len:%u data", module, cmd, size);
    return 0;
}

// 版本号检验接口测试
bool ApiTest::VersionTest()
{
    RUN_BEGIN;
    const char* version = GetVersion();
    const char* expectString = "Product Name: Kunpeng BoostKit\n"
        "Product Version: 24.0.RC1\n"
        "Component Name: BoostKit-videoengine\n"
        "Component Version: 6.0.RC1\n"
        "Component AppendInfo: 11.0.0_r48\n";
    if (strcmp(version, expectString) != 0) {
        RUN_ABNORMAL("expect:%s, actual:%s", expectString, version);
        return false;
    }
    return true;
}

// 未调InitVmiEngine情况下，先调DeInitVmiEngine
bool ApiTest::DeInitVmiEngineAbnormalTest()
{
    RUN_BEGIN;
    VmiErrCode ret = DeInitVmiEngine();
    EXPECT_EQ(ret, VmiErrCode::OK);
    return true;
}

// InitVmiEngine输入参数不合法测试
bool ApiTest::InitVmiEngineAbnormalTest()
{
    RUN_BEGIN;
    // 输入的参数为空
    VmiErrCode ret;
    ret = InitVmiEngine(nullptr);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    VmiConfigEngine config;
    // 参数中callback为空
    InitNormalConfigEngine(config);
    config.dataCallback = nullptr;
    ret = InitVmiEngine(&config);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // 参数中无可初始化的模块
    InitNormalConfigEngine(config);
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        config.dataTypeConfig[i].shouldInit = false;
    }
    ret = InitVmiEngine(&config);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // 参数中预留的长度大于1024
    InitNormalConfigEngine(config);
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        config.dataTypeConfig[i].sendDataOffset = 1025;
    }
    ret = InitVmiEngine(&config);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    return true;
}

// InitVmiEngine未成功，调其他的API
bool ApiTest::CallOtherApiWithoutInit()
{
    RUN_BEGIN;
    // 获取模块状态
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiModuleStatus status = GetStatus(static_cast<VmiDataType>(i));
        EXPECT_EQ(status, VmiModuleStatus::MODULE_NOT_INIT);
    }
    // 调StartModule
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiConfig config;
        VmiErrCode ret = StartModule(static_cast<VmiDataType>(i), reinterpret_cast<uint8_t*>(&config), sizeof(config));
        EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_INIT);
    }
    // 调StopModule
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiErrCode ret = StopModule(static_cast<VmiDataType>(i));
        EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_INIT);
    }
    // 调InjectData
    VmiTouchInputData inputData;
    VmiErrCode ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
        reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_INIT);
    // 调SetParam
    EncodeParams params;
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&params), sizeof(params));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_INIT);
    return true;
}

bool ApiTest::InitVmiEngineNormalTest()
{
    RUN_BEGIN;
    VmiConfigEngine config;
    InitNormalConfigEngine(config);
    VmiErrCode ret = InitVmiEngine(&config);
    EXPECT_EQ(ret, VmiErrCode::OK);
    return true;
}

bool ApiTest::InitVmiEngineCompleteAbnormalTest()
{
    RUN_BEGIN;
    // InitVmiEngine成功后，重复进行InitVmiEngine
    VmiConfigEngine config;
    InitNormalConfigEngine(config);
    VmiErrCode ret = InitVmiEngine(&config);
    EXPECT_EQ(ret, VmiErrCode::ERR_REPEAT_INITIALIZATION);
    // InitVmiEngine成功后，获取状态
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiModuleStatus status = GetStatus(static_cast<VmiDataType>(i));
        EXPECT_EQ(status, VmiModuleStatus::MODULE_INITED);
    }
    // status传入一个异常值
    VmiModuleStatus status = GetStatus(VmiDataType::DATA_TYPE_MAX);
    EXPECT_EQ(status, VmiModuleStatus::MODULE_NOT_INIT);
    // 调StopModule
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiErrCode ret = StopModule(static_cast<VmiDataType>(i));
        EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    }
    // 调InjectData
    VmiTouchInputData inputData;
    ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
        reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    // 调SetParam
    EncodeParams params;
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&params), sizeof(params));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    return true;
}

bool ApiTest::StartModuleAbnormalTest()
{
    RUN_BEGIN;
    VmiConfig config;
    // module不合法
    VmiErrCode ret = StartModule(VmiDataType::DATA_TYPE_MAX, reinterpret_cast<uint8_t*>(&config), sizeof(config));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // config为空
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiErrCode ret = StartModule(static_cast<VmiDataType>(i), nullptr, sizeof(config));
        EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    }
    // size小于sizeof(VmiConfig)
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiErrCode ret = StartModule(static_cast<VmiDataType>(i), reinterpret_cast<uint8_t*>(&config), sizeof(config) - 1);
        EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    }
    // 未StartModule调其他接口
    // 调StopModule
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiErrCode ret = StopModule(static_cast<VmiDataType>(i));
        EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    }
    // 调InjectData
    VmiTouchInputData inputData;
    ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
        reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    // 调SetParam
    EncodeParams params;
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&params), sizeof(params));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    return true;
}

bool ApiTest::StartModuleNormalTest()
{
    RUN_BEGIN;
    VmiErrCode ret;
    // 启动视频
    VmiConfigVideo videoConfig;
    ret = StartModule(VmiDataType::DATA_VIDEO, reinterpret_cast<uint8_t*>(&videoConfig), sizeof(videoConfig));
    EXPECT_EQ(ret, VmiErrCode::OK);
    // 启动音频
    VmiConfigAudio audioConfig;
    ret = StartModule(VmiDataType::DATA_AUDIO, reinterpret_cast<uint8_t*>(&audioConfig), sizeof(audioConfig));
    EXPECT_EQ(ret, VmiErrCode::OK);
    // 启动触控
    VmiConfig config;
    ret = StartModule(VmiDataType::DATA_TOUCH, reinterpret_cast<uint8_t*>(&config), sizeof(config));
    EXPECT_EQ(ret, VmiErrCode::OK);
    // 启动麦克风
    VmiConfigMic micConfig;
    ret = StartModule(VmiDataType::DATA_MIC, reinterpret_cast<uint8_t*>(&micConfig), sizeof(micConfig));
    EXPECT_EQ(ret, VmiErrCode::OK);
    VmiModuleStatus status = GetStatus(VmiDataType::DATA_VIDEO);
    EXPECT_EQ(status, VmiModuleStatus::MODULE_STARTED);
    status = GetStatus(VmiDataType::DATA_AUDIO);
    EXPECT_EQ(status, VmiModuleStatus::MODULE_STARTED);
    status = GetStatus(VmiDataType::DATA_TOUCH);
    EXPECT_EQ(status, VmiModuleStatus::MODULE_STARTED);
    status = GetStatus(VmiDataType::DATA_MIC);
    EXPECT_EQ(status, VmiModuleStatus::MODULE_STARTED);
    return true;
}

bool ApiTest::StopModuleNormalTest()
{
    RUN_BEGIN;
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        VmiErrCode ret = StopModule(static_cast<VmiDataType>(i));
        EXPECT_EQ(ret, VmiErrCode::OK);
        VmiModuleStatus status = GetStatus(static_cast<VmiDataType>(i));
        EXPECT_EQ(status, VmiModuleStatus::MODULE_STOPED);
    }
    return true;
}

bool ApiTest::StopModuleAbnormalTest()
{
    RUN_BEGIN;
    // Pos不对
    VmiErrCode ret = StopModule(VmiDataType::DATA_TYPE_MAX);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // 调StopModule
    for (int i = 0; i < VmiDataType::DATA_MIC; ++i) {
        VmiErrCode ret = StopModule(static_cast<VmiDataType>(i));
        EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_REPEAT_STOP);
    }
    // 调InjectData
    VmiTouchInputData inputData;
    ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
        reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    // 调SetParam
    EncodeParams params;
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&params), sizeof(params));
    EXPECT_EQ(ret, VmiErrCode::ERR_MODULE_NOT_START);
    return true;
}

bool ApiTest::InjectDataAbnormalTest()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    // DataType不合法
    VmiTouchInputData inputData;
    VmiErrCode ret = InjectData(VmiDataType::DATA_TYPE_MAX, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
        reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // cmd不合法
    ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd(0), reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // 内存地址为nullptr
    ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT, nullptr, sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // size < 4
    ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT, reinterpret_cast<uint8_t*>(&inputData), 3);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // size > 16MB
    ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT, reinterpret_cast<uint8_t*>(&inputData), 16 * 1024 * 1024 + 1);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::InjectDataNormalTest()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    // 注入触控数据
    VmiTouchInputData inputData;
    InitNormalTouchInputData(inputData);
    VmiErrCode ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
        reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
    EXPECT_EQ(ret, VmiErrCode::OK);
    // 注入麦克风数据
    uint8_t micData[100] = {0};
    InitMicData(micData, sizeof(micData));
    ret = InjectData(VmiDataType::DATA_MIC, VmiCmd::MIC_SEND_MIC_DATA,
        micData, sizeof(micData));
    EXPECT_EQ(ret, VmiErrCode::OK);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::SetParamAbnormalTest()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    EncodeParams encoderParams;
    // DataType不合法
    VmiErrCode ret = SetParam(VmiDataType::DATA_TYPE_MAX, VmiCmd::VIDEO_SET_ENCODER_PARAM,
        reinterpret_cast<uint8_t*>(&encoderParams), sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // cmd不合法
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd(0xffffffff), reinterpret_cast<uint8_t*>(&encoderParams), sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // 内存地址不合法
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, nullptr, sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // size < 4
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&encoderParams), 3);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // size > 16MB
    ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&encoderParams), 16 * 1024 * 1024 + 1);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::SetParamNormalTest()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    sleep(1); // 等待Video来一帧，否则对应的编码器还可能未初始化
    EncodeParams encoderParams;
    VmiErrCode ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM,
        reinterpret_cast<uint8_t*>(&encoderParams), sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::OK);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::GetParamAbnormalTest()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    EncodeParams encoderParams;
    // DataType不合法
    VmiErrCode ret = GetParam(VmiDataType::DATA_TYPE_MAX, VmiCmd::VIDEO_GET_ENCODER_PARAM,
        reinterpret_cast<uint8_t*>(&encoderParams), sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // cmd不合法
    ret = GetParam(VmiDataType::DATA_VIDEO, VmiCmd(0xffffffff), reinterpret_cast<uint8_t*>(&encoderParams), sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // 内存地址不合法
    ret = GetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_GET_ENCODER_PARAM, nullptr, sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // size < 4
    ret = GetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_GET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&encoderParams), 3);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    // size > 16MB
    ret = GetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_GET_ENCODER_PARAM, reinterpret_cast<uint8_t*>(&encoderParams), 16 * 1024 * 1024 + 1);
    EXPECT_EQ(ret, VmiErrCode::ERR_ILLEGAL_INPUT);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::GetParamNormalTest()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    EncodeParams encoderParams;
    VmiErrCode ret = GetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_GET_ENCODER_PARAM,
        reinterpret_cast<uint8_t*>(&encoderParams), sizeof(encoderParams));
    EXPECT_EQ(ret, VmiErrCode::OK);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::RepeatStartAndStopModule()
{
    for (int i = 0; i < 20; ++i) {
        if (!StartModuleNormalTest()) {
            return false;
        }
        sleep(1);
        if (!StopModuleNormalTest()) {
            return false;
        }
        INFO("Number of times to execute start and stop:%d", i + 1);
    }
    return true;
}

bool ApiTest::InjectDataPerforTimes()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    RunTestHelp timer(false, __FUNCTION__, __LINE__);
    int runTimes = 0;
    while (timer.GetRunTime() < 1000) { // 1s
        // 注入触控数据
        VmiTouchInputData inputData;
        InitNormalTouchInputData(inputData);
        RunTestHelp onceTime(false, __FUNCTION__, __LINE__);
        VmiErrCode ret = InjectData(VmiDataType::DATA_TOUCH, VmiCmd::TOUCH_SEND_TOUCH_EVENT,
            reinterpret_cast<uint8_t*>(&inputData), sizeof(inputData));
        EXPECT_EQ(ret, VmiErrCode::OK);
        int actual = onceTime.GetRunTime() > 200 ? 1 : 0;
        EXPECT_EQ(actual, 0);
        ++runTimes;
        if (runTimes > 1000) {
            break;
        }
    }
    INFO("Run %s times:%d", __FUNCTION__, runTimes);
    int actual = runTimes > 1000 ? 1 : 0;
    EXPECT_EQ(actual , 1);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::InjectMicDataPerforTimes()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    RunTestHelp timer(false, __FUNCTION__, __LINE__);
    int runTimes = 0;
    while (timer.GetRunTime() < 1000) { // 1s
        // 注入麦克风数据
        uint8_t micData[1000] = {0};
        InitMicData(micData, sizeof(micData));
        RunTestHelp onceTime(false, __FUNCTION__, __LINE__);
        VmiErrCode ret = InjectData(VmiDataType::DATA_MIC, VmiCmd::MIC_SEND_MIC_DATA,
            micData, sizeof(micData));
        EXPECT_EQ(ret, VmiErrCode::OK);
        int actual = onceTime.GetRunTime() > 200 ? 1 : 0;
        EXPECT_EQ(actual, 0);
        ++runTimes;
        if (runTimes >= 100) { // 当前麦克风采样间隔支持的间隔是10ms，最大支持1秒注入100次
            break;
        }
    }
    INFO("Run %s times:%d", __FUNCTION__, runTimes);
    int actual = runTimes >= 100 ? 1 : 0;
    EXPECT_EQ(actual , 1);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::SetParamPerforTimes()
{
    RUN_BEGIN;
    if (!StartModuleNormalTest()) {
        return false;
    }
    sleep(1); // 等待Video来一帧，否则对应的编码器还可能未初始化
    RunTestHelp timer(false, __FUNCTION__, __LINE__);
    int runTimes = 0;
    while (timer.GetRunTime() < 1000) { // 1s
        // 设置参数
        EncodeParams encoderParams;
        RunTestHelp onceTime(false, __FUNCTION__, __LINE__);
        VmiErrCode ret = SetParam(VmiDataType::DATA_VIDEO, VmiCmd::VIDEO_SET_ENCODER_PARAM,
            reinterpret_cast<uint8_t*>(&encoderParams), sizeof(encoderParams));
        EXPECT_EQ(ret, VmiErrCode::OK);
        int actual = onceTime.GetRunTime() > 200 ? 1 : 0;
        EXPECT_EQ(actual, 0);
        ++runTimes;
        if (runTimes > 1000) {
            break;
        }
    }
    INFO("Run %s times:%d", __FUNCTION__, runTimes);
    int actual = runTimes > 1000 ? 1 : 0;
    EXPECT_EQ(actual , 1);
    if (!StopModuleNormalTest()) {
        return false;
    }
    return true;
}

bool ApiTest::DeInitVmiEngineNormalTest()
{
    RUN_BEGIN;
    VmiErrCode ret = DeInitVmiEngine();
    EXPECT_EQ(ret, VmiErrCode::OK);
    return true;
}

void ApiTest::InitNormalConfigEngine(VmiConfigEngine& config)
{
    config.dataCallback = EngineDataCallback;
    for (int i = 0; i < VmiDataType::DATA_TYPE_MAX; ++i) {
        config.dataTypeConfig[i].shouldInit = true;
        config.dataTypeConfig[i].sendDataOffset = 256;
    }
}

void ApiTest::InitNormalTouchInputData(VmiTouchInputData& data)
{
    data.orientation = 0;
    data.uiWidth = 720;
    data.uiHeight = 1920;
    data.touchEvent.pointerID = 1;
    data.touchEvent.action = 1;
    data.touchEvent.x = 10;
    data.touchEvent.y = 10;
    data.touchEvent.pressure = 10;
}

void ApiTest::InitMicData(uint8_t *micData, uint32_t size)
{
    if (size < sizeof(AudioData)) {
        ERR("Failed to init mic data");
        return;
    }
    ExtDataAudio data;
    data.sampleInterval = 10;  // 目前只采样间隔为10ms
    memcpy(micData, &data, sizeof(data));
    uint32_t reaminSize = size - sizeof(AudioData);
    memcpy(micData + sizeof(ExtDataAudio), &reaminSize, sizeof(reaminSize));
    AudioData* testData = reinterpret_cast<AudioData*>(micData);
}
}