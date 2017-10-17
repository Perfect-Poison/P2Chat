#include "RegisterDialog.h"
#include "ChatClient.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QWidget(nullptr)
{
    setWindowModality(Qt::ApplicationModal);
    fChatClient = reinterpret_cast<ChatClient*>(parent);
    ui.setupUi(this);
}

RegisterDialog::~RegisterDialog()
{
    
}
