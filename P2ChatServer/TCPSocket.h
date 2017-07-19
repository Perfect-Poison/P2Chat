#pragma once
#include "EventContext.h"
#include "Common/common.h"
#include "Common/CommonSocket.h"

P2_NAMESPACE_BEG

class TCPSocket :
    public EventContext, public CommonSocket
{
public:
    TCPSocket(Task *notifytask = nullptr, IOType inIOType = kBlocking);
    virtual ~TCPSocket();
    void Listen(const USHORT& inPort);
    int32 Connect(const Address& inAddress);
    int32 Connect(const std::string& inIP, const USHORT& inPort);
    TCPSocket* AcceptClient(IOType inIOType = kBlocking);
    int32 Send(const char* inContent, const size_t& inSize);
    int32 Receive(char* outContent, const size_t& inSize, IOType inIOType = kBlocking);
    Address GetAddress(void) { return fRemoteAddress; }
    std::string GetIP(void) { return fRemoteAddress.GetIP(); }
    USHORT GetPort(void) { return fRemoteAddress.GetPort(); }
private:
    TCPSocket(const TCPSocket& inTCPSocket);    // ½ûÖ¹¸´ÖÆ
    Address fRemoteAddress;
};

P2_NAMESPACE_END