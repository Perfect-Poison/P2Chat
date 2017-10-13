#ifndef LOGINDIAGLOG_H
#define LOGINDIAGLOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtNetwork/QUdpSocket>
#include "p2client_common.h"

class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;
class RegisterDialog;
class ChatClient;
class LoginDiaglog : public QDialog
{
    Q_OBJECT

public:
    LoginDiaglog(QWidget *parent);
    ~LoginDiaglog();
private:
    ChatClient  *fChatClient;
    QComboBox   *fUserNameLCombo;
    QLineEdit   *fPassWordLEdit;
    QPushButton *fLoginBt;
    QPushButton *fRegisterBt;
    QPushButton *fFindPasswordBt;
    QCheckBox   *fRemPassword;
    QCheckBox   *fAutoLogin;
};

#endif // LOGINDIAGLOG_H
