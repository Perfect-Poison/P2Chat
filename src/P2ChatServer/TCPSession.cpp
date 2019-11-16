#include "TCPSession.h"
#include "TCPSocket.h"

P2_NAMESPACE_BEG

TCPSession::TCPSession(TCPSocket *tcpSocket):
    Task(tcpSocket),
    fTCPSocket(tcpSocket)
{
    this->SetTaskName(_T("TCPSession"));
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
        {
            TCHAR *buffer = fTCPSocket->GetRemoteIP();
            log_debug(1, _T("TCPSession::Run 断开连接%s:%d\n"), buffer, fTCPSocket->GetRemotePort());
            safe_free(buffer);
        }

        SetDeleteEventWhenAllRefTasksFinished(TRUE);

        return -1;
    }
    else if (eventbits & kReadEvent)
    {
        if (!fTCPSocket->IsConnectionClosed())
        {
            BYTE buffer[TCPSocket::kMaxTCPPacket];
            TCHAR reply[TCPSocket::kMaxTCPPacket];
            TCHAR hexStr[TCPSocket::kMaxTCPPacket];
            int32 recvSize = 0;
            Address remoteAddress = fTCPSocket->GetRemoteAddress();
            while (recvSize != -1)
            {
                ::memset(buffer, 0, sizeof(buffer));
                recvSize = fTCPSocket->Recv((BYTE *)buffer, sizeof(buffer));
                if (recvSize != -1)
                {
                    bin_to_str(buffer, recvSize, hexStr);
                    TCHAR *buf = remoteAddress.GetIP();
                    _tprintf(_T("收到来自%s:%u的数据: [%dB] %s\n"), buf, remoteAddress.GetPort(), recvSize, hexStr);
                    _stprintf(reply,  _T("Hello, received data: %s\n"), hexStr);
                    fTCPSocket->Send((BYTE *)reply, _tcslen(reply) * sizeof(TCHAR));
                    safe_free(buf);
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

