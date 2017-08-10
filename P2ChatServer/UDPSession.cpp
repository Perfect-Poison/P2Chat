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
        Message *responseMsg = new Message;
        switch (message->GetCode())
        {
        case MSG_SERVER_GET_INFO:                              // 获取服务端信息（0）
        {
			this->SuccessfulRespondMsg(responseMsg, message);
            break;
        }
		case MSG_SERVER_SET_INFO:                              // 设置服务端信息（1）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_USER_GET_INFO:                                 // 获取用户信息（2）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_USER_SET_INFO:                                 // 设置用户信息（3）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_GROUP_GET_INFO:                                // 获取群组信息（4）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_GROUP_SET_INFO:                                // 设置群组信息（5）
		{ 
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_LOGIN:                                         // 用户登录消息（6）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_LOGOUT:                                        // 用户退出消息（7）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_USER_MSG_PACKET:                               // 用户消息包（8）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_GROUP_MSG_PACKET:                               // 群组消息包（9）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_USER_ONLINE :                                    // 用户在线消息（10）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
		case MSG_USER_OFFLINE:                                    // 用户离线消息（11）
		{
			this->SuccessfulRespondMsg(responseMsg, message);
			break;
		}
        default:
		{
			UnSuccessfulRespondMsg(responseMsg, message);
            if (UDPSESSION_DEBUG)
			    log_debug(7, _T("UDPSession::Run 非法请求！"));
			break;
		}
       }
        safe_free(responseMsg);
        safe_free(message);
        //fUDPSocket->SetTask(nullptr);
        fUDPSocket->RequestEvent(EV_RE);
        return -1;
    }
    else
    {
        if (UDPSESSION_DEBUG)
            log_debug(7, _T("UDPSession::Run 未处理的事件类型(0x%x)\n"), eventbits);
        return -1;
    }
    return -1;
}

Message* UDPSession::ReadMessage()
{
    BYTE buffer[UDPSocket::kMaxUDPPacket];
    int32 recvSize = 0;
    ::memset(buffer, 0, sizeof(buffer));
    recvSize = fUDPSocket->RecvFrom(buffer, sizeof(buffer), fRemoteAddress);
    if (recvSize != -1)
    {
        MESSAGE *rawMsg = (MESSAGE *)buffer;
        if (UDPSESSION_DEBUG)
        {
            TCHAR hexStr[UDPSocket::kMaxUDPPacket * 2 + 1];
            ::memset(hexStr, 0, sizeof(hexStr));
            bin_to_str((BYTE *)rawMsg, recvSize, hexStr);
            log_debug(0, _T("UDPSession::ReadMessage 收到%s:%u发送的%uB消息[raw:%s]\n"), fRemoteAddress.GetIP().c_str(), fRemoteAddress.GetPort(), recvSize, hexStr);
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
    sendSize = fUDPSocket->SendTo(fRemoteAddress, (BYTE *)rawMsg, sendSize);

    if (UDPSESSION_DEBUG)
    {
        if (sendSize != -1)
        {
            TCHAR hexStr[UDPSocket::kMaxUDPPacket * 2 + 1];
            ::memset(hexStr, 0, sizeof(hexStr));
            bin_to_str((BYTE *)rawMsg, sendSize, hexStr);
            log_debug(0, _T("UDPSession::SendMessage 已向%s:%u发送%uB消息[raw:%s]\n"), fRemoteAddress.GetIP().c_str(), fRemoteAddress.GetPort(), sendSize, hexStr);
        }
        else 
            log_debug(7, _T("[error]UDPSession::SendMessage 发送消息失败\n"));
    }

    safe_free(rawMsg);
}
void  UDPSession::SuccessfulRespondMsg(Message *responseMsg, Message *message)
{
	responseMsg->SetCode(MSG_REQUEST_SUCCEED);
	responseMsg->SetFlags(mf_none);
	responseMsg->SetID(message->GetID());
	SendMessage(responseMsg);
}
void UDPSession::UnSuccessfulRespondMsg(Message *responseMsg, Message *message)
{
	responseMsg->SetCode(MSG_REQUEST_FAILED);
	SendMessage(responseMsg);
}
P2_NAMESPACE_END



