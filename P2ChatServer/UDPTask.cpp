#include "UDPTask.h"
#include "UDPSocket.h"

P2_NAMESPACE_BEG

UDPTask::UDPTask(UDPSocket *udpSocket):
    Task(udpSocket),
    fUDPSocket(udpSocket)
{
    this->SetTaskName("UDPTask");
}

UDPTask::~UDPTask()
{
}

int64 UDPTask::Run()
{
    Task::EventFlags eventbits = this->GetEventFlags();
    if (eventbits & kKillEvent)
        return -1;
    else if (eventbits & kReadEvent)
    {
        char buffer[UDPSocket::kMaxUDPPacket], reply[UDPSocket::kMaxUDPPacket];
        int32 recvSize = 0;
        while (recvSize != -1)
        {
            ::memset(buffer, 0, sizeof(buffer));
            Address remoteAddress;
            recvSize = fUDPSocket->RecvFrom(buffer, UDPSocket::kMaxUDPPacket, remoteAddress);
            if (recvSize != -1)
            {
                printf("收到来自%s:%u的数据: [%dB] %s\n", remoteAddress.GetIP().c_str(), remoteAddress.GetPort(), recvSize, buffer);
                sprintf_s(reply, "Hello, received data: %s\n", buffer);
                fUDPSocket->SendTo(remoteAddress, reply, strlen(reply));
            }
        }

        //fUDPSocket->SetTask(nullptr);
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

