#include "LoginDiaglog.h"
#include "ChatClient.h"
#include "RegisterDialog.h"
P2_NAMESPACE_BEG

extern uint32 g_messageCounter;

LoginDiaglog::LoginDiaglog(QWidget *parent)
    : QDialog(nullptr)
{
    fChatClient = reinterpret_cast<ChatClient*>(parent);

    this->setFixedSize(300, 150);

    fUserNameLCombo = new QComboBox(this);
    fUserNameLCombo->setEditable(true);
    fUserNameLCombo->setFixedHeight(25);

    fPassWordLEdit = new QLineEdit(this);
    fPassWordLEdit->setEchoMode(QLineEdit::Password);
    fPassWordLEdit->setFixedHeight(25);

    fLoginBt = new QPushButton(tr(("µÇÂ¼")), this);
    fLoginBt->setFixedHeight(30);
    fRegisterBt = new QPushButton(tr(("×¢²áÕËºÅ")), this);
    fFindPasswordBt = new QPushButton(tr(("ÕÒ»ØÃÜÂë")), this);
    fRemPassword = new QCheckBox(tr(("¼Ç×¡ÃÜÂë")), this);
    fAutoLogin = new QCheckBox(tr(("×Ô¶¯µÇÂ¼")), this);

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(fUserNameLCombo, 2);
    h1->addWidget(fRegisterBt, 1);

    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(fPassWordLEdit, 2);
    h2->addWidget(fFindPasswordBt, 1);

    QHBoxLayout *h3 = new QHBoxLayout;
    h3->addWidget(fRemPassword, 1);
    h3->addWidget(fAutoLogin, 1);
    h3->addStretch(1);

    QHBoxLayout *h4 = new QHBoxLayout;
    h4->addWidget(fLoginBt, 2);
    h4->addStretch(1);

    QVBoxLayout *v1 = new QVBoxLayout;
    v1->addLayout(h1);
    v1->addLayout(h2);
    v1->addLayout(h3);
    v1->addLayout(h4);

    this->setLayout(v1);

    connect(fRegisterBt, SIGNAL(clicked()), fChatClient->fRegisterDialog, SLOT(show()));
	connect(fLoginBt, SIGNAL(clicked()), this, SLOT(login()));
}

LoginDiaglog::~LoginDiaglog()
{

}

void p2::LoginDiaglog::login()
{
	Message *message = new Message;
	int64 user_pp = fUserNameLCombo->currentText().toLongLong();
	wstring user_password = fPassWordLEdit->text().toStdWString();
	message->SetCode(MSG_LOGIN);
	message->SetFlags(mf_none);
	message->SetID(g_messageCounter++);
	message->SetAttr(ATTR_USER_PP, user_pp);
	message->SetAttr(ATTR_USER_PASSWORD, user_password.c_str(), (uint32)user_password.size());
	fChatClient->SendMessageWithKeepTrac(message, SERVER_IP, SERVER_PORT_FOR_UDP);
	//safe_delete(message);
}

P2_NAMESPACE_END

