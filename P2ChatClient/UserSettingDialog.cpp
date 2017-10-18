#include "UserSettingDialog.h"
P2_NAMESPACE_BEG

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
P2_NAMESPACE_END
