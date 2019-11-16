#include "UDPSession.h"
#include "UDPSocket.h"
#include "Session.h"
#include "p2server_version.h"
#include "p2_dbapi.h"
P2_NAMESPACE_BEG

Mutex UDPSession::sRegMutex;

UDPSession::UDPSession(UDPSocket *udpSocket):
    Task(udpSocket),
    fUDPSocket(udpSocket)
{
    this->SetTaskName(_T("UDPSession"));
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
        if (message == nullptr)
        {
            Message *responseMsg = new Message;
            responseMsg->SetCode(MSG_REQUEST_FAILED);
            SendMessage(responseMsg);
            safe_delete(responseMsg);

            if (UDPSESSION_DEBUG)
                log_debug(7, _T("UDPSession::Run �Ƿ�����\n"));
            fUDPSocket->RequestEvent(EV_RE);
            return -1;
        }

        switch (message->GetCode())
        {
        case MSG_SERVER_GET_INFO:                              // ��ȡ�������Ϣ��0��
        {
            // response message
            Message *responseMsg = new Message;
            // ��Ϣͷ��
            responseMsg->SetCode(MSG_REQUEST_SUCCEED);
            responseMsg->SetFlags(mf_none);
            responseMsg->SetID(message->GetID());
            // ��Ϣ����
            responseMsg->SetAttr(ATTR_SERVER_VERSION, P2CHAT_SERVER_VERSION_STRING, lstrlen(P2CHAT_SERVER_VERSION_STRING));
            SendMessage(responseMsg);
            safe_delete(responseMsg);
            break;
        }
		case MSG_SERVER_SET_INFO:                              // ���÷������Ϣ��1��
		{
			break;
		}
        case MSG_USER_REGISTRATION_INFO:                        // ע���û���Ϣ
        {
            // ע���û���Ϣ
            int32 userID;
            int64 userPP;
            TCHAR buffer[256];
            memset(buffer, 0, sizeof(buffer));
            {
                MutexLocker locker(&sRegMutex);
                bool ret = user_registration_info(
                    TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_PASSWORD)),
                    TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_NICKNAME)),
					TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_SIGNATURE)),
                    message->GetAttrAsUInt64(ATTR_USER_BIRTHDAY),
                    TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_SEX)),
                    TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_ICON)),
                    TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_PROFILE)),
                    message->GetAttrAsInt64(ATTR_USER_QQ),
                    TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_EMAIL)),
                    message->GetAttrAsInt64(ATTR_USER_PHONE),
                    TCharArrayDeleter(message->GetAttrAsString(ATTR_USER_WALLPAPER)),
                    &userID,
                    &userPP);
            }


            // response message
            Message *responseMsg = new Message;
            // ��Ϣͷ��
            responseMsg->SetCode(MSG_REQUEST_SUCCEED);
            responseMsg->SetFlags(mf_none);
            responseMsg->SetID(message->GetID());
            // ��Ϣ����
            responseMsg->SetAttr(ATTR_USER_ID, userID);
            responseMsg->SetAttr(ATTR_USER_PP, userPP);
            SendMessage(responseMsg);
            safe_delete(responseMsg);
            break;
        }
        case MSG_USER_UNREGISTRATION_INFO:                      // ע���û���Ϣ
        {
            break;
        }
		case MSG_USER_GET_INFO:                                 // ��ȡ�û���Ϣ��2��
		{
			int64 session_id = message->GetAttrAsInt64(ATTR_SESSION_ID);
			Session *session = SessionTable::GetSession(session_id);
			UserInfo userinfo = session->GetUserInfo();

			// response message
			Message *responseMsg = new Message;
			// ��Ϣͷ��
			responseMsg->SetCode(MSG_REQUEST_SUCCEED);
			responseMsg->SetFlags(mf_none);
			responseMsg->SetID(message->GetID());

			// ��Ϣ����
			responseMsg->SetAttr(ATTR_USER_ID, userinfo.id);
			responseMsg->SetAttr(ATTR_USER_PP, userinfo.pp);
			responseMsg->SetAttr(ATTR_USER_PASSWORD, userinfo.passwd, _tcslen(userinfo.passwd));
			responseMsg->SetAttr(ATTR_USER_NICKNAME, userinfo.nickname, _tcslen(userinfo.nickname));
			responseMsg->SetAttr(ATTR_USER_SIGNATURE, userinfo.signature, _tcslen(userinfo.signature));
			responseMsg->SetAttr(ATTR_USER_BIRTHDAY, userinfo.birthday);
			responseMsg->SetAttr(ATTR_USER_SEX, userinfo.sex, _tcslen(userinfo.sex));
			responseMsg->SetAttr(ATTR_USER_ICON, userinfo.icon, _tcslen(userinfo.icon));
			responseMsg->SetAttr(ATTR_USER_PROFILE, userinfo.profile, _tcslen(userinfo.profile));
			responseMsg->SetAttr(ATTR_USER_QQ, userinfo.qq);
			responseMsg->SetAttr(ATTR_USER_EMAIL, userinfo.email, _tcslen(userinfo.email));
			responseMsg->SetAttr(ATTR_USER_PHONE, userinfo.phone);
			responseMsg->SetAttr(ATTR_USER_WALLPAPER, userinfo.wallpaper, _tcslen(userinfo.wallpaper));
			responseMsg->SetAttr(ATTR_USER_LAST_LOGIN, userinfo.last_login);
			responseMsg->SetAttr(ATTR_USER_DATE_JOINED, userinfo.date_joined);
			responseMsg->SetAttr(ATTR_USER_IS_LOCKED, userinfo.is_locked);
			responseMsg->SetAttr(ATTR_USER_TRY_TIMES, userinfo.try_times);
			responseMsg->SetAttr(ATTR_USER_STATUS_ID, userinfo.user_status_id);
			responseMsg->SetAttr(ATTR_USER_ENCRYPT_REPO_ID, userinfo.encrypt_repo_id);
			responseMsg->SetAttr(ATTR_USER_IS_DELETED, userinfo.is_deleted);

			SendMessage(responseMsg);
			safe_delete(responseMsg);

			break;
		}
		case MSG_USER_SET_INFO:                                 // �����û���Ϣ��3��
		{
			break;
		}
		case MSG_GROUP_GET_INFO:                                // ��ȡȺ����Ϣ��4��
		{
			break;
		}
		case MSG_GROUP_SET_INFO:                                // ����Ⱥ����Ϣ��5��
		{ 
			break;
		}
        case MSG_GROUP_REGISTRATION_INFO:                       // ע��Ⱥ����Ϣ
        {
            break;
        }
        case MSG_GROUP_UNREGISTRATION_INFO:                     // ע��Ⱥ����Ϣ
        {
            break;
        }
		case MSG_LOGIN:                                         // �û���¼��Ϣ��6��
		{

            // ��¼
			int64 pp = message->GetAttrAsInt64(ATTR_USER_PP);
			TCHAR* passwd = message->GetAttrAsString(ATTR_USER_PASSWORD);
			UserInfo userinfo;
			// 0: wrong pp 1:wrong password 2:correct
			int ret = user_login(pp, passwd, &userinfo);
			if (ret == LOGIN_WRONG_PP || ret == LOGIN_WRONG_PASSWORD || ret == LOGIN_ERR)
			{
				// response message
				Message *responseMsg = new Message;
				// ��Ϣͷ��
				responseMsg->SetCode(MSG_REQUEST_FAILED);
				responseMsg->SetFlags(mf_none);
				responseMsg->SetID(message->GetID());

				// ��Ϣ����
				if (ret == LOGIN_WRONG_PP)
				{
					responseMsg->SetAttr(ATTR_ERR_CODE, (int32)ERR_LOGIN_WRONG_PP);
				}
				else if (ret == LOGIN_WRONG_PASSWORD)
				{
					responseMsg->SetAttr(ATTR_ERR_CODE, (int32)ERR_LOGIN_WRONG_PASSWORD);
				}
				else if (ret == LOGIN_ERR)
				{
					responseMsg->SetAttr(ATTR_ERR_CODE, (int32)ERR_LOGIN_UNKNOWN);
				}
				SendMessage(responseMsg);
				safe_delete(responseMsg);
			}
			else if (ret == LOGIN_SUCCEED)
			{
				// �ɹ�
				Session *session = new Session;
				session->SetState(MSG_LOGIN);
				session->SetSessionID(chrono::system_clock::now().time_since_epoch().count());
				if (!SessionTable::AddSession(session))
				{
					log_debug(6, _T("UDPSession::Run �Ѿ��иûỰID %I64d"), session->GetSessionID());
					break;
				}
				session->SetUserInfo(userinfo);

				// response message
				Message *responseMsg = new Message;
				// ��Ϣͷ��
				responseMsg->SetCode(MSG_REQUEST_SUCCEED);
				responseMsg->SetFlags(mf_none);
				responseMsg->SetID(message->GetID());
				// ��Ϣ����
				responseMsg->SetAttr(ATTR_SESSION_ID, session->GetSessionID());
				responseMsg->SetAttr(ATTR_USER_ID, userinfo.id);
				SendMessage(responseMsg);
				safe_delete(responseMsg);
			}
			break;
		}
		case MSG_LOGOUT:                                        // �û��˳���Ϣ��7��
		{
			break;
		}
		case MSG_USER_MSG_PACKET:                               // �û���Ϣ����8��
		{
			break;
		}
		case MSG_GROUP_MSG_PACKET:                               // Ⱥ����Ϣ����9��
		{
			break;
		}
		case MSG_USER_ONLINE :                                    // �û�������Ϣ��10��
		{
			break;
		}
		case MSG_USER_OFFLINE:                                    // �û�������Ϣ��11��
		{
			break;
		}
        default:
		{
            
            Message *responseMsg = new Message;
            responseMsg->SetCode(MSG_REQUEST_FAILED);
            SendMessage(responseMsg);
            safe_delete(responseMsg);
            if (UDPSESSION_DEBUG)
			    log_debug(7, _T("UDPSession::Run �Ƿ�����\n"));
            break;
		}
       }
       safe_delete(message);
        //fUDPSocket->SetTask(nullptr);
        fUDPSocket->RequestEvent(EV_RE);
        return -1;
    }
    else
    {
        if (UDPSESSION_DEBUG)
            log_debug(7, _T("UDPSession::Run δ�������¼�����(0x%x)\n"), eventbits);
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
        
        //-------------------------------
        // TODO: �ж���Ϣ��ʽ�Ƿ���ȷ����ʱֻ�Ǽ��ж�һ��
        if (ntohs(*(msg_code *)rawMsg) > 20)
            return nullptr;

        if (UDPSESSION_DEBUG)
        {
            TCHAR hexStr[UDPSocket::kMaxUDPPacket * 2 + 1];
            ::memset(hexStr, 0, sizeof(hexStr));
            bin_to_str((BYTE *)rawMsg, recvSize, hexStr);
            TCHAR *buffer = fRemoteAddress.GetIP();
            log_debug(0, _T("UDPSession::ReadMessage �յ�%s:%u���͵�%uB��Ϣ[raw:%s]\n"), buffer, fRemoteAddress.GetPort(), recvSize, hexStr);
            safe_free(buffer);
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
            TCHAR *buffer = fRemoteAddress.GetIP();
            log_debug(0, _T("UDPSession::SendMessage ����%s:%u����%uB��Ϣ[raw:%s]\n"), buffer, fRemoteAddress.GetPort(), sendSize, hexStr);
            safe_free(buffer);
        }
        else 
            log_debug(7, _T("[error]UDPSession::SendMessage ������Ϣʧ��\n"));
    }

    safe_free(rawMsg);
}

P2_NAMESPACE_END


