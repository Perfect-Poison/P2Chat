#pragma once
#include "Common/common.h"
#include "Task.h"
#include "Common/CommonSocket.h"
#include "TCPSocket.h"
#include "EventContext.h"

P2_NAMESPACE_BEG

#define TCPTASK_DEBUG 1

class TCPTask :
    public Task
{
public:
    TCPTask(TCPSocket *tcpSocket);
    virtual ~TCPTask();
    virtual int64 Run();
private:
    TCPSocket *fTCPSocket;
};

P2_NAMESPACE_END