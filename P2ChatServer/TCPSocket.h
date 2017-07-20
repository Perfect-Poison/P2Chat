#pragma once
#include "EventContext.h"
#include "Common/common.h"
#include "Common/CommonSocket.h"
#include "TCPTask.h"
P2_NAMESPACE_BEG

#define TCPSOCKET_DEBUG 1

class TCPSocket :
    public EventContext
{
public:
    TCPSocket(int inSocketID = kInvalidSocketID, Task *notifytask = nullptr);
    virtual ~TCPSocket();
    void Open();
    void Listen(const USHORT& inPort);
    int32 Connect(const Address& inAddress);
    int32 Connect(const std::string& inIP, const USHORT& inPort);
    TCPSocket* AcceptClient();
    int32 Send(const char* inData, const size_t inSize);
    int32 Recv(char* buffer, const size_t inSize);
    void SetRemoteAddress(const Address& inAddress) { fRemoteAddress = inAddress; }
    Address GetRemoteAddress(void) { return fRemoteAddress; }
    std::string GetRemoteIP(void) { return fRemoteAddress.GetIP(); }
    USHORT GetRemotePort(void) { return fRemoteAddress.GetPort(); }
private:
    virtual void ProcessEvent(int eventBits);
    TCPSocket(const TCPSocket& inTCPSocket);    // ½ûÖ¹¸´ÖÆ
    Address fRemoteAddress;
};

P2_NAMESPACE_END