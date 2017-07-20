#pragma once
#include "Common/common.h"
#include "TCPSocket.h"
#include "Task.h"
#include "TCPTask.h"

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
    void Initialize(const Address& inAddress);
    virtual int64 Run();
private:
    virtual void ProcessEvent(int eventBits);
};

P2_NAMESPACE_END