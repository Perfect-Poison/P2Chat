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
	// ����������Ϣ�б���
    void SendMessage(Message *inMessage, const TCHAR* inIP, uint16 inPort);
	// ��������Ϣ�б���,��Ҫ�յ��ظ����ɾ��(����ʹ��)
    void SendMessageWithKeepTrac(Message *inMessage, const TCHAR* inIP, uint16 inPort);
public slots:
void readPendingDatagrams();
public:
    friend LoginDiaglog;
    friend RegisterDialog;
    friend UserInfoDialog;
    friend UserSettingDialog;
    QUdpSocket      *fUdpSocket;			// ͨѶUDP
    LoginDiaglog    *fLoginDialog;			// ��¼����
    RegisterDialog  *fRegisterDialog;		// ע�����
    UserInfoDialog  *fUserInfoDialog;		// �û����Ͻ���
    UserSettingDialog *fUserSettingDialog;	// �û��趨����
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
