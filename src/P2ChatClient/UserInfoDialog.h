#ifndef USERINFODIALOG_H
#define USERINFODIALOG_H

#include <QDialog>
#include "ui_UserInfoDialog.h"
#include "p2client_common.h"
#include "p2_cpapi.h"

P2_NAMESPACE_BEG
class ChatClient;
class UserSettingDialog;
class UserInfoDialog : public QDialog
{
    Q_OBJECT

public:
    UserInfoDialog(QWidget *parent);
    ~UserInfoDialog();
	string caculateConstell(int month, int day);
public:
    Ui::UserInfoDialog ui;
    ChatClient  *fChatClient;
	UserInfo *fUserInfo;
public slots:
	void dealShow();
};
P2_NAMESPACE_END

#endif // USERINFODIALOG_H
