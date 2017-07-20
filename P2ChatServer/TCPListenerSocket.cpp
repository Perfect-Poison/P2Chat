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
    this->Open();
    this->SetSocketRecvBufferSize(kMaxRecvBufSize);
    this->Listen(inAddress.GetPort());
}

int64 TCPListenerSocket::Run()
{
    EventFlags events = this->GetEvents();

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
        printf("TCPListenerSocket::ProcessEvent 收到来自地址%s:%d的TCP连接\n", theTCPClient->GetRemoteIP().c_str(), theTCPClient->GetRemotePort());
    theTCPClient->NoDelay();
    theTCPClient->KeepAlive();
    theTCPClient->SetSocketSendBufferSize(kMaxSendBufSize);

    TCPTask *theTCPTask = new TCPTask(theTCPClient);
    theTCPClient->SetTask(theTCPTask);
    theTCPClient->RequestEvent(EV_RE);

    this->RequestEvent(EV_RE);
}

P2_NAMESPACE_END

