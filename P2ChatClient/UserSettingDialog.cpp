#include "UserSettingDialog.h"

UserSettingDialog::UserSettingDialog(QWidget *parent)
    : QDialog(nullptr)
{
    setWindowModality(Qt::ApplicationModal);
    fChatClient = reinterpret_cast<ChatClient*>(parent);
    ui.setupUi(this);
}

UserSettingDialog::~UserSettingDialog()
{

}
