#include "ChatClient.h"
#include "LoginDiaglog.h"
#include "RegisterDialog.h"
#include "UserInfoDialog.h"
#include "UserSettingDialog.h"
#include <QtWidgets/QToolBar>
#include <QtWidgets/QSplitter>
#include <QtNetwork/QUdpSocket>
#include <QtGui/QList>
#include <QtNetwork/QNetworkDatagram>
#include <QMessageBox>
P2_NAMESPACE_BEG

ChatClient::ChatClient(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("P2Chat"));
    fUdpSocket = new QUdpSocket(this);
    fUdpSocket->bind(QHostAddress::LocalHost, CLIENT_PORT_FOR_UDP);

    // �������д���
    fRegisterDialog = new RegisterDialog(this);
    fLoginDialog = new LoginDiaglog(this);
    fUserSettingDialog = new UserSettingDialog(this);
    fUserInfoDialog = new UserInfoDialog(this);

    // ���ֽ���
    fIconToolBt = new QToolButton(this);
    fUserStatusCBox = new QComboBox(this);
    fNickNameLEdit = new QLineEdit(this);
    fSignatureLEdit = new QLineEdit(this);
    fIconToolBt->setFixedSize(50, 50);

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(fUserStatusCBox, 1);
    h1->addWidget(fNickNameLEdit, 1);
    QVBoxLayout *v1 = new QVBoxLayout;
    v1->addLayout(h1);
    v1->addWidget(fSignatureLEdit, 2);
    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(fIconToolBt, 1);
    h2->addLayout(v1, 2);

    fSearchLEdit = new QLineEdit(this);
    QVBoxLayout *v2 = new QVBoxLayout;
    v2->addLayout(h2);
    v2->addWidget(fSearchLEdit);

    fTabWidget = new QTabWidget(this);
    fTabWidget->addTab(new QWidget, tr("��ϵ��"));
    fTabWidget->addTab(new QWidget, tr("Ⱥ��"));
    fTabWidget->addTab(new QWidget, tr("�Ự"));
    fTabWidget->addTab(new QWidget, tr("�ҵĵ���"));
    v2->addWidget(fTabWidget);

    fToolBar = new QToolBar(this);
    fSettingAct = new QAction(tr("���˵�"), this);
    fAddAct = new QAction(tr("�Ӻ���"), this);
    fToolBar->addAction(fSettingAct);
    fToolBar->addAction(fAddAct);
    v2->addWidget(fToolBar);

    fUserStatusCBox->addItem(QIcon(""), tr("����"), p2UserStatusOnline);
    fUserStatusCBox->addItem(QIcon(""), tr("�뿪"), p2UserStatusAFK);
    fUserStatusCBox->addItem(QIcon(""), tr("æµ"), p2UserStatusBusy);
    fUserStatusCBox->addItem(QIcon(""), tr("����"), p2UserStatusInvisible);
    fUserStatusCBox->addItem(QIcon(""), tr("�������"), p2UserStatusNoDisturbing);
    fUserStatusCBox->addItem(QIcon(""), tr("����"), p2UserStatusOffline);
    fUserStatusCBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    fState = MSG_SERVER_GET_INFO;
    fSessionID = 0;
    fUserID = 0;
    fUserPP = 0;
    setLayout(v2);
    setFixedSize(250, 600);
    
    // �ۺ���
    connect(fIconToolBt, SIGNAL(clicked()), fUserInfoDialog, SLOT(show()));
    connect(fUdpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    //
    close();
    fLoginDialog->show();

    // 
    Message *message = new Message;
    message->SetCode(MSG_SERVER_GET_INFO);
    message->SetFlags(mf_none);
    message->SetID(g_messageCounter++);
    message->SetAttr(ATTR_SERVER_VERSION, _T(""), 0);
    SendMessageWithKeepTrac(message, SERVER_IP, SERVER_PORT_FOR_UDP);
}

ChatClient::~ChatClient()
{

}

void ChatClient::SendMessage(Message *inMessage, const TCHAR* inIP, uint16 inPort)
{
    size_t sendSize = inMessage->GetSize();
    MESSAGE *rawMsg = inMessage->CreateMessage();
    Assert(sendSize == ntohl(rawMsg->size));
    sendSize = fUdpSocket->writeDatagram((char *)rawMsg, sendSize, QHostAddress(QString::fromWCharArray(inIP)), inPort);

    if (CHAT_CLIENT_DEBUG)
    {
        if (sendSize != -1)
        {
            TCHAR hexStr[MAX_UDP_PACKET * 2 + 1];
            ::memset(hexStr, 0, sizeof(hexStr));
            bin_to_str((BYTE *)rawMsg, sendSize, hexStr);
            log_debug(0, _T("ChatClient::SendMessage ����%s:%u����%uB��Ϣ[raw:%s]\n"), inIP, inPort, sendSize, hexStr);
        }
        else
            log_debug(7, _T("[error]ChatClient::SendMessage ������Ϣʧ��\n"));
    }

    safe_free(rawMsg);
}

void ChatClient::SendMessageWithKeepTrac(Message *inMessage, const TCHAR* inIP, uint16 inPort)
{
    if (fMessageTable.count(inMessage->GetID()))
    {
        log_debug(6, _T("[error]ChatClient::SendMessageWithKeepTrac ������Ϣʧ��, ��Ϣ�Ѿ�����Ϣ����\n"));
        return;
    }
    
    fMessageTable[inMessage->GetID()] = inMessage;
    SendMessage(inMessage, inIP, inPort);
}

void ChatClient::readPendingDatagrams()
{
    BYTE recvBuffer[1500];
    while (fUdpSocket->hasPendingDatagrams() && fUdpSocket->pendingDatagramSize())
    {
        // ����������
        qint64 recvSize = fUdpSocket->pendingDatagramSize();
        QNetworkDatagram datagram = fUdpSocket->receiveDatagram();
        QString remoteHost = datagram.senderAddress().toString();
        quint16 remotePort = datagram.senderPort();
        memcpy(recvBuffer, datagram.data().data(), datagram.data().size());
        if (recvSize != -1)
        {
            MESSAGE *rawMsg = (MESSAGE *)recvBuffer;

            //-------------------------------
            // TODO: �ж���Ϣ��ʽ�Ƿ���ȷ����ʱֻ�Ǽ��ж�һ��
            if (ntohs(*(msg_code *)rawMsg) > 20)
                continue;

            if (CHAT_CLIENT_DEBUG)
            {
                TCHAR hexStr[1500 * 2 + 1];
                ::memset(hexStr, 0, sizeof(hexStr));
                bin_to_str((BYTE *)rawMsg, recvSize, hexStr);
                log_debug(0, _T("ChatClient::readPendingDatagrams �յ���Ϣ[%s:%u(%uB) raw:%s]\n"), remoteHost.toStdWString().c_str(), remotePort, recvSize, hexStr);
            }
            Message *message = new Message(rawMsg);
            if (message->GetCode() == MSG_REQUEST_FAILED)
            {
                TCHAR hexStr[1500 * 2 + 1];
                ::memset(hexStr, 0, sizeof(hexStr));
                bin_to_str((BYTE *)rawMsg, recvSize, hexStr);
                log_debug(6, _T("ChatClient::readPendingDatagrams ��Ϣ����ʧ��![%s:%u(%uB) raw:%s]\n"), remoteHost.toStdWString().c_str(), remotePort, recvSize, hexStr);
                continue;
            }

            if (fMessageTable.count(message->GetID()))
            {
                Message *sendedMsg = fMessageTable[message->GetID()];
                switch (sendedMsg->GetCode())
                {
                case MSG_SERVER_GET_INFO:
                {
                    // �ɹ���ȡ�˷������Ϣ
                    /*fSessionID = message->GetAttrAsInt64(ATTR_SESSION_ID);*/
                    fServerVersion = QString::fromWCharArray(message->GetAttrAsString(ATTR_SERVER_VERSION));
                    QMessageBox::information(nullptr, tr("����˰汾��Ϣ"), tr("�ɹ���ȡ����˰汾��Ϣ:%1").arg(fServerVersion));
                    break;
                }
                case MSG_SERVER_SET_INFO:                              // ���÷������Ϣ��1��
                {
                    break;
                }
                case MSG_USER_REGISTRATION_INFO:                        // ע���û���Ϣ
                {
                    /**
                     *	
                     */
                    fUserID = message->GetAttrAsInt32(ATTR_USER_ID);
                    fUserPP = message->GetAttrAsInt64(ATTR_USER_PP);
                    fRegisterDialog->SucceedRegister(fUserPP);
                    break;
                }
                case MSG_USER_UNREGISTRATION_INFO:                      // ע���û���Ϣ
                {
                    break;
                }
                case MSG_USER_GET_INFO:                                 // ��ȡ�û���Ϣ��2��
                {
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
                case MSG_USER_ONLINE:                                    // �û�������Ϣ��10��
                {
                    break;
                }
                case MSG_USER_OFFLINE:                                    // �û�������Ϣ��11��
                {
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            else
            {
                if (CHAT_CLIENT_DEBUG)
                {    
                    log_debug(6, _T("ChatClient::readPendingDatagrams �Ƿ���Ϣ!"));
                }
            }
            safe_delete(message);
        }
        // 
    }
}

P2_NAMESPACE_END