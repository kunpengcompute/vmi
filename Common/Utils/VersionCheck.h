/*
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 功能说明: 提供服务端和客户端的版本号校验接口，特别说明的点：
 * 1. 在TCP连接建立后，会使用特定消息格式进行版本号的校验，保证此功能和业务代码解耦，后续版本迭代将不影响此功能
 * 2. 服务端会发送版本号信息给客户端后会等待客户端的校验结果，客户端返回校验通过后服务端才会进行后续的逻辑，保证版本号检验不通过时，服务端不会传输任何数据给客户端
 * 3. 为实现在规定时间后获取版本号信息，在版本号校验时，会将socket连接改成非阻塞的方式，版本校验完毕后，将恢复成阻塞方式
 */
#ifndef VERSION_CHECK_H
#define VERSION_CHECK_H
#include <string>
#include <memory>
namespace Vmi {
// socket先设置成非阻塞再设置成阻塞的帮助类，防止出现未恢复成阻塞的情况
class SocketBlockHelper {
public:
    explicit SocketBlockHelper(int connection) : m_connection(connection)
    {
        SetBlock(false);
    }
    ~SocketBlockHelper()
    {
        SetBlock(true);
    }
private:
    void SetBlock(bool isBlock);
    int m_connection = -1;
};

class __attribute__ ((visibility ("default"))) VersionCheck {
public:
    enum class EngineType : uint32_t {
        UNKNOWN,
        VIDEO_ENGINE_TYPE,
        INSTRUCTION_ENGINE_TYPE,
    };

    VersionCheck(int connection, EngineType engineType);

    ~VersionCheck() = default;

    // timeout单位ms
    bool CheckVersionServer(const int timeout = 1000);

    // timeout单位ms
    bool CheckVersionClient(const int timeout = 1000);
private:
    struct VersionMatchResultInfo {
        bool isGet = false;
        bool isMatch = false;
        std::string serverVersion;
        std::string clientVersion;
        std::string serverEngine;
        std::string clientEngine;
    };
    struct VersionHead {
        uint32_t startFlag;
        uint32_t payloadSize;
    };
private:
    bool RecvAndMatchVersion(VersionMatchResultInfo& matchInfo, int timeout);
    void TriggerCallback(const VersionMatchResultInfo& matchResult);
    void InitFailedMsg(VersionMatchResultInfo& matchInfo);
    std::string GetEngineInfo();
    bool SendPayloadData(std::pair<uint8_t*, size_t> payloadData);
    void MatchVersion(VersionMatchResultInfo& matchInfo);
    bool RecvPayloadData(int timeout);
    bool RecvOnceData(std::pair<uint8_t*, size_t> data, int timeout);
private:
    int m_connection = -1;
    EngineType m_type = EngineType::UNKNOWN;
    std::unique_ptr<uint8_t[]> m_recvData = nullptr;
    uint32_t m_recvDataSize = 0;
};
}
#endif