#ifndef USERINFODIALOG_H
#define USERINFODIALOG_H

#include <QDialog>
#include "ui_UserInfoDialog.h"

class ChatClient;
class UserSettingDialog;
class UserInfoDialog : public QDialog
{
    Q_OBJECT

public:
    UserInfoDialog(QWidget *parent);
    ~UserInfoDialog();

private:
    Ui::UserInfoDialog ui;
    ChatClient  *fChatClient;
};

#endif // USERINFODIALOG_H
