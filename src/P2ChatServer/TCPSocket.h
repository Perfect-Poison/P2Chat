#pragma once
#include "Event.h"
#include "Socket.h"
P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define TCPSOCKET_DEBUG 1
#else
#define TCPSOCKET_DEBUG 0
#endif

class TCPSocket :
    public Event
{
public:
    enum
    {
        kMaxTCPPacket = 1500
    };
    TCPSocket(int inSocketID = kInvalidSocketID);
    virtual ~TCPSocket();
    void Open();
    void Listen(const USHORT& inPort);
    int32 Connect(const Address& inAddress);
    int32 Connect(const TCHAR* inIP, const USHORT& inPort);
    TCPSocket* AcceptClient();
    int32 Send(const BYTE* inData, const size_t inSize);
    int32 Recv(BYTE* buffer, const size_t inSize);
    void SetRemoteAddress(const Address& inAddress) { fRemoteAddress = inAddress; }
    Address GetRemoteAddress(void) { return fRemoteAddress; }
    TCHAR* GetRemoteIP(void) { return fRemoteAddress.GetIP(); }
    USHORT GetRemotePort(void) { return fRemoteAddress.GetPort(); }
    BOOL IsConnectionClosed();
    void SetConnectionClose();
private:
    virtual void ProcessEvent(int eventBits);
    TCPSocket(const TCPSocket& inTCPSocket);    // ½ûÖ¹¸´ÖÆ
    Address fRemoteAddress;
    BOOL fConnClosed;
    Mutex fMutex;
};

P2_NAMESPACE_END