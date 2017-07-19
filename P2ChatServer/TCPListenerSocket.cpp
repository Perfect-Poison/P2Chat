#include "TCPListenerSocket.h"

P2_NAMESPACE_BEG

TCPListenerSocket::TCPListenerSocket()
{
}


TCPListenerSocket::~TCPListenerSocket()
{
}

int64 TCPListenerSocket::Run()
{
    EventFlags events = this->GetEvents();

    if (events & Task::kKillEvent)
        return -1;

    this->ProcessEvent(Task::kReadEvent);
    return 0;
}

void TCPListenerSocket::ProcessEvent(int eventBits)
{
    Task *theTask = nullptr;
    TCPSocket *theTCPClient = AcceptClient();
    TCPSession *theTCPSession = new TCPSession(theTCPClient);
    theTCPClient->RequestEvent(EV_RE);
}

P2_NAMESPACE_END

