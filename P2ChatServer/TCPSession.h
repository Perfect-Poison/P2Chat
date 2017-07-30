#pragma once
#include "Task.h"

P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define TCPSESSION_DEBUG 1
#else
#define TCPSESSION_DEBUG 1
#endif


class TCPSocket;
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