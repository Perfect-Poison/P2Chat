#include "RegisterDialog.h"
#include "ChatClient.h"
#include <QMessageBox>

P2_NAMESPACE_BEG

RegisterDialog::RegisterDialog(QWidget *parent)
    : QWidget(nullptr)
{
    setWindowModality(Qt::ApplicationModal);
    fChatClient = reinterpret_cast<ChatClient*>(parent);
    ui.setupUi(this);

    connect(ui.fRegisterBt, SIGNAL(clicked()), this, SLOT(dealRegister()));
}

RegisterDialog::~RegisterDialog()
{
    
}

void RegisterDialog::SucceedRegister(int64 inPP)
{
    QMessageBox::information(nullptr, tr("×¢²á³É¹¦"), tr("¹§Ï²Äú! ×¢²á³É¹¦, PPºÅÎª:%1").arg(inPP));
}

void RegisterDialog::FailedRegister()
{
    QMessageBox::information(nullptr, tr("×¢²áÊ§°Ü"), tr("×¢²áÊ§°Ü!"));
}

void RegisterDialog::dealRegister()
{
    QString passwd = ui.fPasswordLEdit->text();
    QString nickName = ui.fNickNameLEdit->text();
    int64 birthday = ui.fBirthdayDateEdit->dateTime().toSecsSinceEpoch();
    QString icon = "default";
    QString profile = ui.fProfileTextEdit->toPlainText();
    int64 qq = atoll(ui.fQQLEdit->text().toLocal8Bit());
    QString email = ui.fEmailLEdit->text();
    int64 phone = atoll(ui.fPhoneLEdit->text().toLocal8Bit());

    Message *message = new Message;
    message->SetCode(MSG_USER_REGISTRATION_INFO);
    message->SetFlags(mf_none);
    message->SetID(g_messageCounter++);
    
    message->SetAttr(ATTR_USER_PASSWORD, passwd.toStdWString().c_str(), passwd.length());
    message->SetAttr(ATTR_USER_NICKNAME, nickName.toStdWString().c_str(), nickName.length());
    message->SetAttr(ATTR_USER_BIRTHDAY, birthday);
    message->SetAttr(ATTR_USER_SEX, ui.fManRBt->isChecked() ? _T("m") : _T("f"), 1);
    message->SetAttr(ATTR_USER_ICON, icon.toStdWString().c_str(), icon.length());
    message->SetAttr(ATTR_USER_PROFILE, profile.toStdWString().c_str(), profile.length());
    message->SetAttr(ATTR_USER_QQ, qq);
    message->SetAttr(ATTR_USER_EMAIL, email.toStdWString().c_str(), email.length());
    message->SetAttr(ATTR_USER_PHONE, phone);
    fChatClient->SendMessageWithKeepTrac(message, SERVER_IP, SERVER_PORT_FOR_UDP);
}
P2_NAMESPACE_END

