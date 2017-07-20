#pragma once
#include "Common/common.h"
#include "Task.h"
P2_NAMESPACE_BEG

#define UDPTASK_DEBUG 1

class UDPSocket;
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