#include "TCPListenerSocket.h"

P2_NAMESPACE_BEG

TCPListenerSocket::TCPListenerSocket():
    Task(this)
{
    this->SetTaskName(_T("TCPListenerSocket"));
    this->SetEventName(_T("TCPListenerSocket"));
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
    TCPSocket *theTCPClient = AcceptClient();
    if (TCPLISTENERSOCKET_DEBUG)
    {
        TCHAR *buffer = theTCPClient->GetRemoteIP();
        log_debug(1, _T("TCPListenerSocket::ProcessEvent 收到来自地址%s:%d的TCP连接\n"), buffer, theTCPClient->GetRemotePort());
        safe_free(buffer);
    }
    theTCPClient->NoDelay();
    theTCPClient->KeepAlive();
    theTCPClient->SetSocketSendBufferSize(kMaxSendBufSize);
    theTCPClient->RequestEvent(EV_RE);

    this->RequestEvent(eventBits);
}

P2_NAMESPACE_END

