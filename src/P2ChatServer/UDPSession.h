#pragma once
#include "Task.h"
#include "Address.h"
#include "p2_cpapi.h"
#include "Mutex.h"
P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define UDPSESSION_DEBUG 1
#else
#define UDPSESSION_DEBUG 0
#endif

class UDPSocket;
class UDPSession :
    public Task
{
public:
    UDPSession(UDPSocket *udpSocket);
    virtual ~UDPSession();
    virtual int64 Run();
    Message* ReadMessage();
    void SendMessage(Message *rawMsg);
private:
    UDPSocket *fUDPSocket;
    Address fRemoteAddress;
    static Mutex sRegMutex;
};

P2_NAMESPACE_END