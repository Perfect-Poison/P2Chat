#ifndef USERSETTINGDIALOG_H
#define USERSETTINGDIALOG_H

#include <QDialog>
#include "ui_UserSettingDialog.h"
#include "p2client_common.h"

P2_NAMESPACE_BEG
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
P2_NAMESPACE_END

#endif // USERSETTINGDIALOG_H
