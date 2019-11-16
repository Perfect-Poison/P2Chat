#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#define CHAT_CLIENT_DEBUG   1

#include <QWidget>
#include <QMap>
#include "p2client_common.h"
#include "p2_cpapi.h"
class QUdpSocket;
class QToolButton;
class QComboBox;
class QLineEdit;
class QTabWidget;
class QToolBar;
class QSplitter;

P2_NAMESPACE_BEG
class LoginDiaglog;
class RegisterDialog;
class UserInfoDialog;
class UserSettingDialog;
class ChatClient : public QWidget
{
    Q_OBJECT
public:
    ChatClient(QWidget *parent = 0);
    ~ChatClient();
	// 不保存在消息列表中
    void SendMessage(Message *inMessage, const TCHAR* inIP, uint16 inPort);
	// 保存在消息列表中,需要收到回复后才删除(优先使用)
    void SendMessageWithKeepTrac(Message *inMessage, const TCHAR* inIP, uint16 inPort);
public slots:
void readPendingDatagrams();
public:
    friend LoginDiaglog;
    friend RegisterDialog;
    friend UserInfoDialog;
    friend UserSettingDialog;
    QUdpSocket      *fUdpSocket;			// 通讯UDP
    LoginDiaglog    *fLoginDialog;			// 登录界面
    RegisterDialog  *fRegisterDialog;		// 注册界面
    UserInfoDialog  *fUserInfoDialog;		// 用户资料界面
    UserSettingDialog *fUserSettingDialog;	// 用户设定界面
    QToolButton     *fIconToolBt;
    QComboBox       *fUserStatusCBox;
    QLineEdit       *fNickNameLEdit;
    QLineEdit       *fSignatureLEdit;
    QLineEdit       *fSearchLEdit;
    QTabWidget      *fTabWidget;
    QToolBar        *fToolBar;
    QAction         *fSettingAct;
    QAction         *fAddAct;
    QMap<msg_id, Message*>  fMessageTable;
    uint16  fState;
    int64   fSessionID;
    QString fServerVersion;
	UserInfo fUserInfo;
};

P2_NAMESPACE_END
#endif // CHATCLIENT_H
