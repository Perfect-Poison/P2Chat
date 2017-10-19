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

    // 创建所有窗口
    fRegisterDialog = new RegisterDialog(this);
    fLoginDialog = new LoginDiaglog(this);
    fUserSettingDialog = new UserSettingDialog(this);
    fUserInfoDialog = new UserInfoDialog(this);

    // 布局界面
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
    fTabWidget->addTab(new QWidget, tr("联系人"));
    fTabWidget->addTab(new QWidget, tr("群聊"));
    fTabWidget->addTab(new QWidget, tr("会话"));
    fTabWidget->addTab(new QWidget, tr("我的地盘"));
    v2->addWidget(fTabWidget);

    fToolBar = new QToolBar(this);
    fSettingAct = new QAction(tr("主菜单"), this);
    fAddAct = new QAction(tr("加好友"), this);
    fToolBar->addAction(fSettingAct);
    fToolBar->addAction(fAddAct);
    v2->addWidget(fToolBar);

    fUserStatusCBox->addItem(QIcon(""), tr("在线"), p2UserStatusOnline);
    fUserStatusCBox->addItem(QIcon(""), tr("离开"), p2UserStatusAFK);
    fUserStatusCBox->addItem(QIcon(""), tr("忙碌"), p2UserStatusBusy);
    fUserStatusCBox->addItem(QIcon(""), tr("隐身"), p2UserStatusInvisible);
    fUserStatusCBox->addItem(QIcon(""), tr("请勿打扰"), p2UserStatusNoDisturbing);
    fUserStatusCBox->addItem(QIcon(""), tr("离线"), p2UserStatusOffline);
    fUserStatusCBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    fState = MSG_SERVER_GET_INFO;
    fSessionID = 0;
    fUserID = 0;
    fUserPP = 0;
    setLayout(v2);
    setFixedSize(250, 600);
    
    // 槽函数
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
            log_debug(0, _T("ChatClient::SendMessage 已向%s:%u发送%uB消息[raw:%s]\n"), inIP, inPort, sendSize, hexStr);
        }
        else
            log_debug(7, _T("[error]ChatClient::SendMessage 发送消息失败\n"));
    }

    safe_free(rawMsg);
}

void ChatClient::SendMessageWithKeepTrac(Message *inMessage, const TCHAR* inIP, uint16 inPort)
{
    if (fMessageTable.count(inMessage->GetID()))
    {
        log_debug(6, _T("[error]ChatClient::SendMessageWithKeepTrac 发送消息失败, 消息已经在消息表中\n"));
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
        // 解析包数据
        qint64 recvSize = fUdpSocket->pendingDatagramSize();
        QNetworkDatagram datagram = fUdpSocket->receiveDatagram();
        QString remoteHost = datagram.senderAddress().toString();
        quint16 remotePort = datagram.senderPort();
        memcpy(recvBuffer, datagram.data().data(), datagram.data().size());
        if (recvSize != -1)
        {
            MESSAGE *rawMsg = (MESSAGE *)recvBuffer;

            //-------------------------------
            // TODO: 判断消息格式是否正确，暂时只是简单判断一下
            if (ntohs(*(msg_code *)rawMsg) > 20)
                continue;

            if (CHAT_CLIENT_DEBUG)
            {
                TCHAR hexStr[1500 * 2 + 1];
                ::memset(hexStr, 0, sizeof(hexStr));
                bin_to_str((BYTE *)rawMsg, recvSize, hexStr);
                log_debug(0, _T("ChatClient::readPendingDatagrams 收到消息[%s:%u(%uB) raw:%s]\n"), remoteHost.toStdWString().c_str(), remotePort, recvSize, hexStr);
            }
            Message *message = new Message(rawMsg);
            if (message->GetCode() == MSG_REQUEST_FAILED)
            {
                TCHAR hexStr[1500 * 2 + 1];
                ::memset(hexStr, 0, sizeof(hexStr));
                bin_to_str((BYTE *)rawMsg, recvSize, hexStr);
                log_debug(6, _T("ChatClient::readPendingDatagrams 消息请求失败![%s:%u(%uB) raw:%s]\n"), remoteHost.toStdWString().c_str(), remotePort, recvSize, hexStr);
                continue;
            }

            if (fMessageTable.count(message->GetID()))
            {
                Message *sendedMsg = fMessageTable[message->GetID()];
                switch (sendedMsg->GetCode())
                {
                case MSG_SERVER_GET_INFO:
                {
                    // 成功获取了服务端信息
                    /*fSessionID = message->GetAttrAsInt64(ATTR_SESSION_ID);*/
                    fServerVersion = QString::fromWCharArray(message->GetAttrAsString(ATTR_SERVER_VERSION));
                    QMessageBox::information(nullptr, tr("服务端版本信息"), tr("成功获取服务端版本信息:%1").arg(fServerVersion));
                    break;
                }
                case MSG_SERVER_SET_INFO:                              // 设置服务端信息（1）
                {
                    break;
                }
                case MSG_USER_REGISTRATION_INFO:                        // 注册用户信息
                {
                    /**
                     *	
                     */
                    fUserID = message->GetAttrAsInt32(ATTR_USER_ID);
                    fUserPP = message->GetAttrAsInt64(ATTR_USER_PP);
                    fRegisterDialog->SucceedRegister(fUserPP);
                    break;
                }
                case MSG_USER_UNREGISTRATION_INFO:                      // 注销用户信息
                {
                    break;
                }
                case MSG_USER_GET_INFO:                                 // 获取用户信息（2）
                {
                    break;
                }
                case MSG_USER_SET_INFO:                                 // 设置用户信息（3）
                {
                    break;
                }
                case MSG_GROUP_GET_INFO:                                // 获取群组信息（4）
                {
                    break;
                }
                case MSG_GROUP_SET_INFO:                                // 设置群组信息（5）
                {
                    break;
                }
                case MSG_GROUP_REGISTRATION_INFO:                       // 注册群组信息
                {
                    break;
                }
                case MSG_GROUP_UNREGISTRATION_INFO:                     // 注销群组信息
                {
                    break;
                }
                case MSG_LOGIN:                                         // 用户登录消息（6）
                {
                    break;
                }
                case MSG_LOGOUT:                                        // 用户退出消息（7）
                {
                    break;
                }
                case MSG_USER_MSG_PACKET:                               // 用户消息包（8）
                {
                    break;
                }
                case MSG_GROUP_MSG_PACKET:                               // 群组消息包（9）
                {
                    break;
                }
                case MSG_USER_ONLINE:                                    // 用户在线消息（10）
                {
                    break;
                }
                case MSG_USER_OFFLINE:                                    // 用户离线消息（11）
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
                    log_debug(6, _T("ChatClient::readPendingDatagrams 非法消息!"));
                }
            }
            safe_delete(message);
        }
        // 
    }
}

P2_NAMESPACE_END