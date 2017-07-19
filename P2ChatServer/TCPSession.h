#pragma once
#include "Common/common.h"
#include "Task.h"
#include "Common/CommonSocket.h"

P2_NAMESPACE_BEG

class TCPSession :
    public Task, public TCPSocket
{
public:
    TCPSession(TCPSocket *tcpSocket);
    virtual ~TCPSession();
    virtual int64 Run();
private:
    TCPSocket *fTCPSocket;
};

P2_NAMESPACE_END