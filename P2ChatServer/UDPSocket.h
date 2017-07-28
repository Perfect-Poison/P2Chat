#pragma once
#include "Event.h"
#include "Socket.h"
#include "UDPTask.h"

P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define UDPSOCKET_DEBUG 1
#else
#define UDPSOCKET_DEBUG 0
#endif

class UDPSocket :
    public Event
{
public:
    enum
    {
        kMaxUDPPacket = 1500
    };
    UDPSocket(int inSocketID = kInvalidSocketID);
    virtual ~UDPSocket();
    void Open();
    void Bind(const USHORT& inPort);
    int32 SendTo(const Address& inAddress, const char* inContent, const size_t& inSize);
    int32 SendTo(const string& inIP, const USHORT& inPort, const char* inContent, const size_t& inSize);
    int32 RecvFrom(char* outContent, const size_t& inSize);
    int32 RecvFrom(char* outContent, const size_t& inSize, Address& outAddress);
    int32 RecvFrom(char* outContent, const size_t& inSize, string& outIP, USHORT& outPort);
private:
    virtual void ProcessEvent(int eventBits);
    UDPSocket(const UDPSocket& inUDP);
};

P2_NAMESPACE_END