#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#define CHAT_CLIENT_DEBUG   1

#include <QWidget>
#include <QMap>
#include "p2client_common.h"

class LoginDiaglog;
class RegisterDialog;
class UserInfoDialog;
class UserSettingDialog;
class QUdpSocket;
class QToolButton;
class QComboBox;
class QLineEdit;
class QTabWidget;
class QToolBar;
class QSplitter;
class ChatClient : public QWidget
{
    Q_OBJECT
public:
    ChatClient(QWidget *parent = 0);
    ~ChatClient();
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
    QMap<quint32, p2::Message*>  fMessageTable;
};

#endif // CHATCLIENT_H
