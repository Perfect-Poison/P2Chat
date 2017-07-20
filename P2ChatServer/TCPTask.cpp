#include "TCPTask.h"

P2_NAMESPACE_BEG

TCPTask::TCPTask(TCPSocket *tcpSocket):
    fTCPSocket(tcpSocket)
{
    this->SetTaskName("TCPTask");
}


TCPTask::~TCPTask()
{
}

int64 TCPTask::Run()
{
    Task::EventFlags eventbits = this->GetEvents();
    if (eventbits & kKillEvent)
        return -1;
    else if (eventbits & kCloseEvent)
    {
        if (TCPTASK_DEBUG)
            printf("TCPTask::Run 断开连接%s:%d\n", fTCPSocket->GetRemoteIP().c_str(), fTCPSocket->GetRemotePort());
        delete fTCPSocket;
        return -1;
    }
    else if (eventbits & kReadEvent)
    {
        char buffer[1024], reply[1124];
        int32 recvSize = 0;
        while (recvSize != -1)
        {
            ::memset(buffer, 0, sizeof(buffer));
            recvSize = fTCPSocket->Recv(buffer, 1500);
            if (recvSize != -1)
            {
                printf("recv: [%dB] %s\n", recvSize, buffer);
                sprintf_s(reply, "Hello, received data: %s\n", buffer);
                fTCPSocket->Send(reply, strlen(reply));
            }
        }

        TCPTask *task = new TCPTask(fTCPSocket);
        fTCPSocket->SetTask(task);
        fTCPSocket->RequestEvent(EV_RE);
    }
    else 
    {
        if (TCPTASK_DEBUG)
            printf("TCPTask::Run 未处理的事件类型(0x%x)\n", eventbits);
        return -1;
    }
    return -1;
}

P2_NAMESPACE_END

