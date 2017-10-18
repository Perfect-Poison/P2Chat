#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include <QtNetwork/QUdpSocket>
#include "ui_RegisterDialog.h"
#include "p2client_common.h"
P2_NAMESPACE_BEG

class ChatClient;
class RegisterDialog : public QWidget
{
    Q_OBJECT

public:
    RegisterDialog(QWidget *parent);
    ~RegisterDialog();
    void SucceedRegister(int64 inPP);
    void FailedRegister();
    public slots:
    void dealRegister();
private:
    Ui::RegisterDialog ui;
    ChatClient *fChatClient;
};
P2_NAMESPACE_END

#endif // REGISTERFORM_H
