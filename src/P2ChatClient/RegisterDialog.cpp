#include "RegisterDialog.h"
#include "ChatClient.h"
#include <QMessageBox>
#include <QFileDialog>

P2_NAMESPACE_BEG

extern uint32 g_messageCounter;

RegisterDialog::RegisterDialog(QWidget *parent)
    : QWidget(nullptr)
{
    setWindowModality(Qt::ApplicationModal);
    fChatClient = reinterpret_cast<ChatClient*>(parent);
    ui.setupUi(this);
	fIconFile = QDir::currentPath() + "/" + ICON_DIR + "/default.jpg";
	fWallpaper = QDir::currentPath() + "/" + WALLPAPER_DIR + "/default.jpg";
	string c = fIconFile.toStdString();
	ui.fIconTBt->setIcon(QIcon(fIconFile));
	ui.fIconTBt->setFixedSize(QSize(80, 80));
	ui.fIconTBt->setIconSize(QSize(80, 80));

    connect(ui.fRegisterBt, SIGNAL(clicked()), this, SLOT(dealRegister()));
	connect(ui.fIconTBt, SIGNAL(clicked()), this, SLOT(dealChooseIcon()));
	connect(ui.fCancelBt, SIGNAL(clicked()), this, SLOT(close()));
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
	QString signature = "Õâ¸öÈËºÜÀÁ£¬Ê²Ã´Ò²Ã»ÁôÅ¶~";
    int64 birthday = ui.fBirthdayDateEdit->dateTime().toSecsSinceEpoch();
    QString icon = fIconFile;
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
	message->SetAttr(ATTR_USER_SIGNATURE, signature.toStdWString().c_str(), signature.length());
    message->SetAttr(ATTR_USER_BIRTHDAY, birthday);
    message->SetAttr(ATTR_USER_SEX, ui.fManRBt->isChecked() ? _T("m") : _T("f"), 1);
    message->SetAttr(ATTR_USER_ICON, icon.toStdWString().c_str(), icon.length());
    message->SetAttr(ATTR_USER_PROFILE, profile.toStdWString().c_str(), profile.length());
    message->SetAttr(ATTR_USER_QQ, qq);
    message->SetAttr(ATTR_USER_EMAIL, email.toStdWString().c_str(), email.length());
    message->SetAttr(ATTR_USER_PHONE, phone);
	message->SetAttr(ATTR_USER_WALLPAPER, fWallpaper.toStdWString().c_str(), fWallpaper.length());
    fChatClient->SendMessageWithKeepTrac(message, SERVER_IP, SERVER_PORT_FOR_UDP);
}


void p2::RegisterDialog::dealChooseIcon()
{
	QString iconFileSaved = fIconFile;
	fIconFile = QFileDialog::getOpenFileName(this, tr("ÇëÑ¡ÔñÍ·Ïñ"), "icon", tr("Í¼Æ¬ (*.png *.jpg *.icon *.jpeg)"));
	if (fIconFile.isNull())
		fIconFile = iconFileSaved;
	ui.fIconTBt->setIcon(QIcon(fIconFile));
}

P2_NAMESPACE_END

