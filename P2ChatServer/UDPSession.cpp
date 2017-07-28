#include "UDPSession.h"
#include "UDPSocket.h"

P2_NAMESPACE_BEG

UDPSession::UDPSession(UDPSocket *udpSocket):
    Task(udpSocket),
    fUDPSocket(udpSocket)
{
    this->SetTaskName("UDPSession");
}

UDPSession::~UDPSession()
{
}

int64 UDPSession::Run()
{
    Task::EventFlags eventbits = this->GetEventFlags();
    if (eventbits & kKillEvent)
        return -1;
    else if (eventbits & kReadEvent)
    {
        Message *message = ReadMessage();
        switch (message->GetCode()) 
        {
        case MSG_SERVER_GET_INFO:
        {
            Message *responseMsg = new Message;
            responseMsg->SetCode(MSG_REQUEST_SUCCEED);
            responseMsg->SetFlags(mf_none);
            responseMsg->SetID(message->GetID());
            SendMessage(responseMsg);
            break;
        }
        case MSG_LOGIN:
            break;
        case MSG_USER_GET_INFO:
            break;
        default:
            break;
        }

        //fUDPSocket->SetTask(nullptr);
        fUDPSocket->RequestEvent(EV_RE);
        return -1;
    }
    else
    {
        if (UDPSESSION_DEBUG)
            printf("UDPSession::Run 未处理的事件类型(0x%x)\n", eventbits);
        return -1;
    }
    return -1;
}

Message* UDPSession::ReadMessage()
{
    char buffer[UDPSocket::kMaxUDPPacket];
    int32 recvSize = 0;
    ::memset(buffer, 0, sizeof(buffer));
    recvSize = fUDPSocket->RecvFrom(buffer, UDPSocket::kMaxUDPPacket, fRemoteAddress);
    if (recvSize != -1)
    {
        MESSAGE *rawMsg = (MESSAGE *)buffer;
        if (UDPSESSION_DEBUG)
        {
            char hexStr[UDPSocket::kMaxUDPPacket * 2 + 1];
            ::memset(hexStr, 0, sizeof(hexStr));
            bin_to_str((BYTE *)rawMsg, recvSize, hexStr);
            printf("UDPSession::ReadMessage 收到%s:%u发送的%uB消息[raw:%s]\n", fRemoteAddress.GetIP().c_str(), fRemoteAddress.GetPort(), recvSize, hexStr);
        }
        Message *message = new Message(rawMsg);
        return message;
    }
    return nullptr;
}

void UDPSession::SendMessage(Message *message)
{
    size_t sendSize = message->GetSize();
    MESSAGE *rawMsg = message->CreateMessage();
    Assert(sendSize == ntohl(rawMsg->size));
    sendSize = fUDPSocket->SendTo(fRemoteAddress, (char *)rawMsg, sendSize);

    if (UDPSESSION_DEBUG)
    {
        if (sendSize != -1)
        {
            char hexStr[UDPSocket::kMaxUDPPacket * 2 + 1];
            ::memset(hexStr, 0, sizeof(hexStr));
            bin_to_str((BYTE *)rawMsg, sendSize, hexStr);
            printf("UDPSession::SendMessage 已向%s:%u发送%uB消息[raw:%s]\n", fRemoteAddress.GetIP().c_str(), fRemoteAddress.GetPort(), sendSize, hexStr);
        }
        else 
            printf("[error]UDPSession::SendMessage 发送消息失败\n");
    }

    safe_free(rawMsg);
}

P2_NAMESPACE_END



