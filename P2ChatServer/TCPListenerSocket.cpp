#include "TCPListenerSocket.h"

P2_NAMESPACE_BEG

TCPListenerSocket::TCPListenerSocket():
    Task(this)
{
    this->SetTaskName("TCPListenerSocket");
    this->SetEventName("TCPListenerSocket");
}


TCPListenerSocket::~TCPListenerSocket()
{
}

int64 TCPListenerSocket::Run()
{
    EventFlags events = this->GetEventFlags();

    if (events & Task::kKillEvent)
        return -1;

    this->ProcessEvent(EV_RE);
    return 0;
}

void TCPListenerSocket::ProcessEvent(int eventBits)
{
    Task *theTask = nullptr;
    TCPSocket *theTCPClient = AcceptClient();
    if (TCPLISTENERSOCKET_DEBUG)
        printf("TCPListenerSocket::ProcessEvent �յ����Ե�ַ%s:%d��TCP����\n", theTCPClient->GetRemoteIP().c_str(), theTCPClient->GetRemotePort());
    theTCPClient->NoDelay();
    theTCPClient->KeepAlive();
    theTCPClient->SetSocketSendBufferSize(kMaxSendBufSize);

//     TCPTask *theTCPTask = new TCPTask(theTCPClient);
//     theTCPClient->SetTask(theTCPTask);
    theTCPClient->RequestEvent(EV_RE);

    this->RequestEvent(EV_RE);
}

P2_NAMESPACE_END
