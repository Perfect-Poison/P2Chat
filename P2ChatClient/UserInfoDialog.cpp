#include "UserInfoDialog.h"
#include "ChatClient.h"
#include "UserSettingDialog.h"
P2_NAMESPACE_BEG

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
P2_NAMESPACE_END
