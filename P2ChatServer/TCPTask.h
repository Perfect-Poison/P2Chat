#pragma once
#include "Common/common.h"
#include "Task.h"

P2_NAMESPACE_BEG

#define TCPTASK_DEBUG 1

class TCPSocket;
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