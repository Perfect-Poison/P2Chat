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
    void SendMessage(Message *inMessage, const TCHAR* inIP, uint16 inPort);
    void SendMessageWithKeepTrac(Message *inMessage, const TCHAR* inIP, uint16 inPort);
public slots:
void readPendingDatagrams();
private:
    friend LoginDiaglog;
    friend RegisterDialog;
    friend UserInfoDialog;
    friend UserSettingDialog;
    QUdpSocket      *fUdpSocket;
    LoginDiaglog    *fLoginDialog;
    RegisterDialog  *fRegisterDialog;
    UserInfoDialog  *fUserInfoDialog;
    UserSettingDialog *fUserSettingDialog;
private:
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
    int32   fUserID;
    int64   fUserPP;
};

P2_NAMESPACE_END
#endif // CHATCLIENT_H
