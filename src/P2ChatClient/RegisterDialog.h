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
	QString fIconFile;
	QString fWallpaper;
	Ui::RegisterDialog ui;
	ChatClient *fChatClient;
    public slots:
    void dealRegister();
	void dealChooseIcon();
};
P2_NAMESPACE_END

#endif // REGISTERFORM_H
