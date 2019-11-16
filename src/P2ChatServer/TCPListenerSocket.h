#pragma once
#include "TCPSocket.h"

P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define TCPLISTENERSOCKET_DEBUG 1
#else
#define TCPLISTENERSOCKET_DEBUG 0
#endif

class TCPListenerSocket :
    public TCPSocket, public Task
{
public:
    TCPListenerSocket();
    virtual ~TCPListenerSocket();
    virtual int64 Run();
private:
    virtual void ProcessEvent(int eventBits);
};

P2_NAMESPACE_END