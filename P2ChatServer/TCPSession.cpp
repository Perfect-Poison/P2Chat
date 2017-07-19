#include "TCPSession.h"

P2_NAMESPACE_BEG

TCPSession::TCPSession(TCPSocket *tcpSocket):
    fTCPSocket(tcpSocket)
{
    this->SetTaskName("TCPSession");
}


TCPSession::~TCPSession()
{
}

int64 TCPSession::Run()
{
    return -1;
}

P2_NAMESPACE_END

