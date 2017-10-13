#ifndef USERSETTINGDIALOG_H
#define USERSETTINGDIALOG_H

#include <QDialog>
#include "ui_UserSettingDialog.h"

class ChatClient;
class UserSettingDialog : public QDialog
{
    Q_OBJECT

public:
    UserSettingDialog(QWidget *parent);
    ~UserSettingDialog();

private:
    Ui::UserSettingDialog ui;
    ChatClient  *fChatClient;
};

#endif // USERSETTINGDIALOG_H
