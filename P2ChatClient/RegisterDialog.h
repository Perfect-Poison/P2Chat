#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include <QtNetwork/QUdpSocket>
#include "ui_RegisterDialog.h"
#include "p2client_common.h"

class ChatClient;
class RegisterDialog : public QWidget
{
    Q_OBJECT

public:
    RegisterDialog(QWidget *parent);
    ~RegisterDialog();
private:
    Ui::RegisterDialog ui;
    ChatClient *fChatClient;
};

#endif // REGISTERFORM_H
