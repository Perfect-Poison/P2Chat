#pragma once
#include "EventContext.h"
#include "Common/common.h"
#include "Common/CommonSocket.h"

P2_NAMESPACE_BEG

class UDPSocket :
    public EventContext
{
public:
    UDPSocket(int inSocketID = kInvalidSocketID, Task *notifytask = nullptr);
    virtual ~UDPSocket();
    void Bind(const USHORT& inPort);
    int32 SendTo(const Address& inAddress, const char* inContent, const size_t& inSize);
    int32 SendTo(const string& inIP, const USHORT& inPort, const char* inContent, const size_t& inSize);
    int32 RecvFrom(char* outContent, const size_t& inSize);
    int32 RecvFrom(char* outContent, const size_t& inSize, Address& outAddress);
    int32 RecvFrom(char* outContent, const size_t& inSize, string& outIP, USHORT& outPort);
private:
    UDPSocket(const UDPSocket& inUDP);
};

P2_NAMESPACE_END