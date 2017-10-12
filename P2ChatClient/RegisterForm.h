#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include <QtNetwork/QUdpSocket>
#include "ui_RegisterForm.h"
#include "p2client_common.h"

class ChatClient;
class RegisterForm : public QWidget
{
    Q_OBJECT

public:
    RegisterForm(QWidget *parent);
    ~RegisterForm();
private:
    Ui::RegisterForm ui;
    ChatClient *fChatClient;
};

#endif // REGISTERFORM_H
