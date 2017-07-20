#pragma once
#include "Task.h"
#include "UDPSocket.h"
P2_NAMESPACE_BEG

#define UDPTASK_DEBUG 1

class UDPTask :
    public Task
{
public:
    UDPTask(UDPSocket *udpSocket);
    virtual ~UDPTask();
    virtual int64 Run();
private:
    UDPSocket *fUDPSocket;
};

P2_NAMESPACE_END