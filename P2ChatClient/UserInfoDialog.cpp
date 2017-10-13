#include "UserInfoDialog.h"
#include "ChatClient.h"
#include "UserSettingDialog.h"

UserInfoDialog::UserInfoDialog(QWidget *parent)
    : QDialog(nullptr)
{
    setWindowModality(Qt::ApplicationModal);
    fChatClient = reinterpret_cast<ChatClient*>(parent);
    ui.setupUi(this);


    // ²ÛÁ¬½Ó
    connect(ui.editToolBtn, SIGNAL(clicked()), fChatClient->fUserSettingDialog, SLOT(show()));
}

UserInfoDialog::~UserInfoDialog()
{

}
