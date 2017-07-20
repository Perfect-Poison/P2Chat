#pragma once
#include "Common/common.h"
#include "TCPSocket.h"
#include "Task.h"
#include "TCPSession.h"

P2_NAMESPACE_BEG

#define TCPLISTENERSOCKET_DEBUG 1

class TCPListenerSocket :
    public TCPSocket, public Task
{
public:
    TCPListenerSocket();
    virtual ~TCPListenerSocket();
    void Initialize(const Address& inAddress);
    virtual int64 Run();
private:
    virtual void ProcessEvent(int eventBits);
};

P2_NAMESPACE_END