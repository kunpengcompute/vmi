/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能说明: 提供服务端和客户端的版本号校验接口，特别说明的点：
 * 1. 在TCP连接建立后，会使用特定消息格式进行版本号的校验，保证此功能和业务代码解耦，后续版本迭代将不影响此功能
 * 2. 服务端会发送版本号信息给客户端后会等待客户端的校验结果，客户端返回校验通过后服务端才会进行后续的逻辑，保证版本号检验不通过时，服务端不会传输任何数据给客户端
 * 3. 为实现在规定时间后获取版本号信息，在版本号校验时，会将socket连接改成非阻塞的方式，版本校验完毕后，将恢复成阻塞方式
 */
#define LOG_TAG "VersionCheck"
#include "VersionCheck.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <chrono>
#include "logging.h"
#include "Communication.h"
#include "CommonDefine.h"
#include "SmartPtrMacro.h"
#include "EngineEventHandler.h"
#include "Connection.h"

namespace Vmi {
namespace {
    const std::string VIDEO_ENGINE = "VideoEngine";
    const std::string INSTRUCTION_ENGINE = "InstructionEngine";
    const uint32_t VERSION_STRAT_FLAG = 0x7665722E; // 版本号校验消息的起始标志，其ASCI值对应的字符串是"ver."

    int64_t GetPassedSteadyTime(const std::chrono::time_point<std::chrono::steady_clock>& startTime)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime).count();
    }
}

void SocketBlockHelper::SetBlock(bool isBlock)
{
    if (m_connection < 0) {
        ERR("Failed to set connect:%d block, connnect is error", m_connection);
    }
    uint32_t flags = static_cast<uint32_t>(fcntl(m_connection, F_GETFL, 0));
    if (isBlock) {
        flags &= ~O_NONBLOCK;  // 设置成阻塞
    } else {
        flags |= O_NONBLOCK;   // 设置成非阻塞
    }
    if (fcntl(m_connection, F_SETFL, static_cast<int>(flags)) != 0) {
        ERR("Failed to set connect:%d block status, err:%d:%s",
            m_connection, errno, strerror(errno));
    }
}

VersionCheck::VersionCheck(int connection, EngineType engineType) : m_connection(connection), m_type(engineType) {}

// timeout单位ms
bool VersionCheck::CheckVersionServer(const int timeout)
{
    SocketBlockHelper socketHelp(m_connection);
    std::string versionInfo = ENGINE_VERSION;
    std::string sendVersionInfo = versionInfo + "\n" + GetEngineInfo();
    uint8_t* sendData = reinterpret_cast<uint8_t*>(const_cast<char *>(sendVersionInfo.c_str()));
    size_t sendLen = sendVersionInfo.length() + 1;
    std::pair<uint8_t*, size_t> palyloadData = {sendData, sendLen};
    if (!SendPayloadData(palyloadData)) {
        ERR("Failed to check server version, send version failed, server version:%s", sendVersionInfo.c_str());
        return false;
    }
    if (!RecvPayloadData(timeout)) {
        ERR("Failed to check server version, recv payload failed, server version:%s", sendVersionInfo.c_str());
        return false;
    }
    constexpr uint32_t matchResultSize = 4;
    if (m_recvDataSize != matchResultSize) {
        ERR("Failed to check server version, recv data size:%u not match valid size:%u, server version:%s",
            m_recvDataSize, matchResultSize, sendVersionInfo.c_str());
        return false;
    }
    uint32_t result = 0;
    memcpy(&result, m_recvData.get(), sizeof(result));
    constexpr uint32_t matchSuccess = 1;
    if (result != matchSuccess) {
        ERR("Failed to check server version, recv match failed, server version:%s", sendVersionInfo.c_str());
        return false;
    }
    INFO("Version match success, server version:%s", sendVersionInfo.c_str());
    return true;
}

void VersionCheck::TriggerCallback(const VersionMatchResultInfo& matchResult)
{
    EngineEvent engineEvent = {0, 0, 0, 0, 0, {0}};
    if (!matchResult.isGet) {
        engineEvent.event = VMI_ENGINE_EVENT_GET_VERSION_TIMEOUT;
    } else if (!matchResult.isMatch) {
        std::string additionInfo = "";
        if (matchResult.serverVersion != matchResult.clientVersion) {
            engineEvent.event = VMI_ENGINE_EVENT_VERSION_MISMATCH;
            additionInfo = matchResult.serverVersion + "\n" + matchResult.clientVersion;
        } else {
            engineEvent.event = VMI_ENGINE_EVENT_ENGINE_MISMATCH;
            additionInfo = matchResult.serverEngine + "\n" + matchResult.clientEngine;
        }
        size_t destSize = sizeof(engineEvent.additionInfo);
        size_t copySize = additionInfo.length() + 1;
        if (destSize < copySize) {
            ERR("Failed to trigger callback, dest size:%zu < copy size:%zu, addition info:%s",
                destSize, copySize, additionInfo.c_str());
        } else {
            memcpy(engineEvent.additionInfo, additionInfo.c_str(), copySize);
        }
    }
    EngineEventHandler::GetInstance().CreateEvent(engineEvent);
}

bool VersionCheck::CheckVersionClient(const int timeout)
{
    VersionMatchResultInfo matchResult;
    if (RecvAndMatchVersion(matchResult, timeout)) {
        return true;
    }
    // 匹配失败，则触发回调发送不匹配详情
    TriggerCallback(matchResult);
    return false;
}

// timeout单位ms
bool VersionCheck::RecvAndMatchVersion(VersionMatchResultInfo& matchInfo, int timeout)
{
    SocketBlockHelper socketHelp(m_connection);
    InitFailedMsg(matchInfo);
    if (!RecvPayloadData(timeout)) {
        ERR("Failed to receive and match version, recv payload failed, client verison:%s",
            matchInfo.clientVersion.c_str());
        return false;
    }
    MatchVersion(matchInfo);
    // 发送匹配结果给服务端
    int matchResult = static_cast<int>(matchInfo.isMatch);
    std::pair<uint8_t*, size_t> matchResultData = {reinterpret_cast<uint8_t*>(&matchResult), sizeof(matchResult)};
    if (!SendPayloadData(matchResultData)) {
        ERR("Failed to receive and match version, send version result failed, result:%d, client version:%s",
            matchResult, matchInfo.clientVersion.c_str());
        matchInfo.isGet = false; // 若发送到服务端失败了，则服务端也将断开连接，那么客户端提示获取版本号超时
        return false;
    }
    if (matchInfo.isMatch) {
        INFO("Version match success, version:%s", matchInfo.clientVersion.c_str());
    }
    return matchInfo.isMatch;
}

void VersionCheck::MatchVersion(VersionMatchResultInfo& matchInfo)
{
    std::string recvVersionInfo = std::string(reinterpret_cast<const char *>(m_recvData.get()));
    auto split = recvVersionInfo.find('\n');
    if (split == std::string::npos) {
        ERR("Failed to match version, find split char, info:%s", recvVersionInfo.c_str());
    }
    matchInfo.serverVersion = recvVersionInfo.substr(0, split);
    matchInfo.serverEngine = recvVersionInfo.substr(split + 1);
    matchInfo.isGet = true;
    if (matchInfo.serverEngine != matchInfo.clientEngine) {
        ERR("Failed to match version, engine mismatch, server:%s, client:%s", matchInfo.serverEngine.c_str(),
            matchInfo.clientEngine.c_str());
        return;
    }
    if (matchInfo.serverVersion != matchInfo.clientVersion) {
        ERR("Failed to match version, version mismatch, server:%s, client:%s", matchInfo.serverVersion.c_str(),
            matchInfo.clientVersion.c_str());
        return;
    }
    matchInfo.isMatch = true;
}

void VersionCheck::InitFailedMsg(VersionMatchResultInfo& matchInfo)
{
    matchInfo.isGet = false;
    matchInfo.isMatch = false;
    matchInfo.serverVersion = "";
    matchInfo.clientVersion = ENGINE_VERSION;
    matchInfo.serverEngine = "";
    matchInfo.clientEngine = GetEngineInfo();
}

std::string VersionCheck::GetEngineInfo()
{
    if (m_type == EngineType::VIDEO_ENGINE_TYPE) {
        return VIDEO_ENGINE;
    } else if (m_type == EngineType::INSTRUCTION_ENGINE_TYPE) {
        return INSTRUCTION_ENGINE;
    } else {
        ERR("Failed to get engine info, type:%u", static_cast<uint32_t>(m_type));
        return "ERROR";
    }
}

bool VersionCheck::SendPayloadData(std::pair<uint8_t*, size_t> payloadData)
{
    if (payloadData.first == nullptr || payloadData.second == 0) {
        ERR("Failed to send payload data, input data[%d, %zu] is error", static_cast<int>(payloadData.first == nullptr),
            payloadData.second);
    }
    struct VersionHead headInfo = {VERSION_STRAT_FLAG, static_cast<uint32_t>(payloadData.second)};
    std::pair<uint8_t*, size_t> headData = {reinterpret_cast<uint8_t*>(&headInfo), sizeof(headInfo)};
    if (VmiSend(m_connection, headData.first, headData.second) !=
        static_cast<ssize_t>(sizeof(headInfo))) {
        ERR("Failed to send payload info, send head failed, err:%d:%s", errno, strerror(errno));
        return false;
    }
    if (VmiSend(m_connection, payloadData.first, payloadData.second) !=
        static_cast<ssize_t>(payloadData.second)) {
        ERR("Failed to send payload info, send version failed, err:%d:%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool VersionCheck::RecvPayloadData(int timeout)
{
    struct VersionHead headInfo = {0, 0};
    std::pair<uint8_t*, size_t> headData = {reinterpret_cast<uint8_t*>(&headInfo), sizeof(headInfo)};
    if (!RecvOnceData(headData, timeout)) {
        ERR("Failed to recv payload data, recv head info failed");
        return false;
    }
    if (headInfo.startFlag != VERSION_STRAT_FLAG) {
        ERR("Failed to recv payload data, start flag:%u is error", headInfo.startFlag);
        return false;
    }
    constexpr uint32_t maxVersionPayloadSize = 100 * 1024; // 100k
    if (headInfo.payloadSize > maxVersionPayloadSize) {
        ERR("Failed to recv payload data, pyaload size:%u limit exceeded", headInfo.payloadSize);
        return false;
    }
    m_recvDataSize = headInfo.payloadSize;
    MAKE_UNIQUE_NOTHROW(m_recvData, uint8_t[], m_recvDataSize + 1);
    if (m_recvData == nullptr) {
        ERR("Failed to recv payload data, allocate %u data failed", m_recvDataSize);
        return false;
    }
    std::pair<uint8_t*, size_t> payloadData = {m_recvData.get(), m_recvDataSize};
    if (!RecvOnceData(payloadData, timeout)) {
        ERR("Failed to recv payload data, recv payload data failed");
        return false;
    }
    m_recvData[m_recvDataSize] = '\0';
    return true;
}

bool VersionCheck::RecvOnceData(std::pair<uint8_t*, size_t> data, int timeout)
{
    if (data.first == nullptr || data.second == 0) {
        ERR("Failed to Recv once data, input data[%d, %zu] is error", static_cast<int>(data.first == nullptr),
            data.second);
        return false;
    }
    auto startTime = std::chrono::steady_clock::now();
    while (timeout == 0 || GetPassedSteadyTime(startTime) <= timeout) {
        ssize_t ret = VmiRecv(m_connection, data.first, data.second);
        if (ret == SOCKET_RECV_FAIL_RETRY) {
            constexpr uint32_t sleepInterval = 500; // 0.5ms
            usleep(sleepInterval);
            continue;
        } else if (ret == static_cast<ssize_t>(data.second)) {
            return true;
        } else {
            ERR("Failed to recv one data, recv failed, ret:%zd, err %d:%s", ret, errno, strerror(errno));
            return false;
        }
    }
    ERR("Failed to recv once data, timeout");
    return false;
}
}