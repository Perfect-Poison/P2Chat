#pragma once
#include "Common/common.h"
#include "Task.h"
#include "Common/CommonSocket.h"
#include "TCPSocket.h"
#include "EventContext.h"

P2_NAMESPACE_BEG

#define TCPSESSION_DEBUG 1

class TCPSession :
    public Task
{
public:
    TCPSession(TCPSocket *tcpSocket);
    virtual ~TCPSession();
    virtual int64 Run();
private:
    TCPSocket *fTCPSocket;
};

P2_NAMESPACE_END