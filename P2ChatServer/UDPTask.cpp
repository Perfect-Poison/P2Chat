#include "UDPTask.h"
#include "UDPSocket.h"

P2_NAMESPACE_BEG

UDPTask::UDPTask(UDPSocket *udpSocket):
    fUDPSocket(udpSocket)
{
    this->SetTaskName("UDPTask");
}

UDPTask::~UDPTask()
{
}

int64 UDPTask::Run()
{
    Task::EventFlags eventbits = this->GetEvents();
    if (eventbits & kKillEvent)
        return -1;
    else if (eventbits & kReadEvent)
    {
        char buffer[1024], reply[1124];
        int32 recvSize = 0;
        while (recvSize != -1)
        {
            ::memset(buffer, 0, sizeof(buffer));
            Address remoteAddress;
            recvSize = fUDPSocket->RecvFrom(buffer, 1500, remoteAddress);
            if (recvSize != -1)
            {
                printf("recv: [%dB] %s\n", recvSize, buffer);
                sprintf_s(reply, "Hello, received data: %s\n", buffer);
                fUDPSocket->SendTo(remoteAddress, reply, strlen(reply));
            }
        }

//         UDPTask *task = new UDPTask(fUDPSocket);
//         fUDPSocket->SetTask(task);
        fUDPSocket->SetTask(nullptr);
        fUDPSocket->RequestEvent(EV_RE);
        return -1;
    }
    else
    {
        if (UDPTASK_DEBUG)
            printf("UDPTask::Run 未处理的事件类型(0x%x)\n", eventbits);
        return -1;
    }
    return -1;
}

P2_NAMESPACE_END

