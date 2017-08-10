#include "TCPSession.h"
#include "TCPSocket.h"

P2_NAMESPACE_BEG

TCPSession::TCPSession(TCPSocket *tcpSocket):
    Task(tcpSocket),
    fTCPSocket(tcpSocket)
{
    this->SetTaskName("TCPSession");
}


TCPSession::~TCPSession()
{
}

int64 TCPSession::Run()
{
    Task::EventFlags eventbits = this->GetEventFlags();
    if (eventbits & kKillEvent)
        return -1;
    else if (eventbits & kCloseEvent)
    {
        fTCPSocket->SetConnectionClose();
        if (TCPSESSION_DEBUG)
            log_debug(1, _T("TCPSession::Run 断开连接%s:%d\n"), fTCPSocket->GetRemoteIP().c_str(), fTCPSocket->GetRemotePort());

        SetDeleteEventWhenAllRefTasksFinished(TRUE);

        return -1;
    }
    else if (eventbits & kReadEvent)
    {
        if (!fTCPSocket->IsConnectionClosed())
        {
            BYTE buffer[TCPSocket::kMaxTCPPacket];
            int32 recvSize = 0;
            Address remoteAddress = fTCPSocket->GetRemoteAddress();
            while (recvSize != -1)
            {
                ::memset(buffer, 0, sizeof(buffer));
                recvSize = fTCPSocket->Recv((BYTE *)buffer, sizeof(buffer));
                if (recvSize != -1)
                {
                    //_tprintf(_T("收到来自%s:%u的数据: [%dB] %s\n"), remoteAddress.GetIP().c_str(), remoteAddress.GetPort(), recvSize, buffer);
                    //_stprintf(reply,  _T("Hello, received data: %s\n"), buffer);
                    //fTCPSocket->Send((BYTE *)reply, _tcslen(reply) * sizeof(TCHAR));
                }
            }
            fTCPSocket->RequestEvent(EV_RE);
        }
        return -1;
    }
    else 
    {
        if (TCPSESSION_DEBUG)
            log_debug(7, _T("TCPSession::Run 未处理的事件类型(0x%x)\n"), eventbits);
        return -1;
    }
    return -1;
}

P2_NAMESPACE_END

