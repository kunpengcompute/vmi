/**
 * 版权所有 (c) 华为技术有限公司 2023-2023
 * 抽象VmiSocket实现类，适配了Demo部分的网络库
 */
#ifndef CONNECTION_SOCKET_H
#define CONNECTION_SOCKET_H
#include "Socket/VmiSocket.h"

namespace Vmi {
class __attribute__ ((visibility ("default"))) ConnectionSocket : public VmiSocket {
public:
    explicit ConnectionSocket(int fd);

    ~ConnectionSocket() override;

    ssize_t Send(const std::pair<uint8_t*, size_t> &sendBuf) override;

    ssize_t Recv(const std::pair<uint8_t*, size_t> &recvBuf) override;

    void CloseSocket() override;

    uint64_t GetBytesRecv() const override;

    uint64_t GetBytesSend() const override;
private:
    uint32_t m_curSendMsgSeq = 0;
    uint64_t m_totalSendByte = 0;
    uint64_t m_totalRecvByte = 0;
    bool m_msgHeadVerifyFeature = true;
};
}
#endif