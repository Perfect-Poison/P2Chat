#pragma once
#include "Task.h"
#include "Address.h"
#include "p2_cpapi.h"

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
	void SuccessfulRespondMsg(Message *,Message *);
	void UnSuccessfulRespondMsg(Message *,Message *);
private:
    UDPSocket *fUDPSocket;
    Address fRemoteAddress;
};

P2_NAMESPACE_END