#include "RegisterForm.h"
#include "ChatClient.h"

RegisterForm::RegisterForm(QWidget *parent)
    : QWidget(parent)
{
    setWindowModality(Qt::ApplicationModal);
    fChatClient = reinterpret_cast<ChatClient*>(parent);
    ui.setupUi(this);
}

RegisterForm::~RegisterForm()
{

}
