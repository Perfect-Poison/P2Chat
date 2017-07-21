#pragma once
#include "Common/common.h"
#include "Task.h"

P2_NAMESPACE_BEG

//#if P2CHAT_DEBUG
#define TCPTASK_DEBUG 1
// #else
// #define TCPTASK_DEBUG 0
// #endif


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