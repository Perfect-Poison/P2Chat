#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QWidget>

class LoginDiaglog;
class RegisterForm;
class QUdpSocket;
class QToolButton;
class QComboBox;
class QLineEdit;
class QTabWidget;
class QToolBar;
class ChatClient : public QWidget
{
    Q_OBJECT
public:
    ChatClient(QWidget *parent = 0);
    ~ChatClient();
private:
    friend LoginDiaglog;
    friend RegisterForm;
    QUdpSocket      *fUdpSocket;
    LoginDiaglog    *fLoginDialog;
    RegisterForm    *fRegisterForm;
private:
    QToolButton     *fIconToolBt;
    QComboBox       *fStatusCBox;
    QLineEdit       *fNickNameLEdit;
    QLineEdit       *fSignatureLEdit;
    QLineEdit       *fSearchLEdit;
    QTabWidget      *fTabWidget;
    QToolBar        *fToolBar;
};

#endif // CHATCLIENT_H
