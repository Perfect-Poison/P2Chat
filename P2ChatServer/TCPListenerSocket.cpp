#include "TCPListenerSocket.h"

P2_NAMESPACE_BEG

TCPListenerSocket::TCPListenerSocket()
{
    this->SetTaskName("TCPListenerSocket");
}


TCPListenerSocket::~TCPListenerSocket()
{
}

void TCPListenerSocket::Initialize(const Address& inAddress)
{
    this->Open(SOCK_STREAM, IPPROTO_TCP);
    this->SetSocketRecvBufferSize(kMaxRecvBufSize);
    this->Listen(inAddress.GetPort());
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

    theTCPClient->NoDelay();
    theTCPClient->KeepAlive();
    theTCPClient->SetSocketSendBufferSize(kMaxSendBufSize);

    TCPSession *theTCPSession = new TCPSession(theTCPClient);
    theTCPClient->SetTask(theTCPSession);
    theTCPClient->RequestEvent(EV_RE);

    this->RequestEvent(EV_RE);
}

P2_NAMESPACE_END

