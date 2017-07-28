#pragma once
#include "Task.h"

P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define UDPTASK_DEBUG 1
#else
#define UDPTASK_DEBUG 0
#endif

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