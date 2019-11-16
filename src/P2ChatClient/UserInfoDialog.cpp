#include "UserInfoDialog.h"
#include "ChatClient.h"
#include "UserSettingDialog.h"
#include <QtGui/QPixmap>
#include <QDateTime>
P2_NAMESPACE_BEG

UserInfoDialog::UserInfoDialog(QWidget *parent)
    : QDialog(nullptr)
{
    setWindowModality(Qt::ApplicationModal);
    fChatClient = reinterpret_cast<ChatClient*>(parent);
    ui.setupUi(this);

    // ������
    connect(ui.editToolBtn, SIGNAL(clicked()), fChatClient->fUserSettingDialog, SLOT(show()));
}

UserInfoDialog::~UserInfoDialog()
{

}

void p2::UserInfoDialog::dealShow()
{
	fUserInfo = &fChatClient->fUserInfo;
	QPixmap pixmap(QString::fromWCharArray(fUserInfo->wallpaper));
	ui.wallpaperLble->setText("wallpaper");
	ui.wallpaperLble->setPixmap(pixmap);
	ui.iconToolBtn->setIconSize(QSize(50, 50));
	ui.iconToolBtn->setIcon(QIcon(QString::fromWCharArray(fUserInfo->icon)));
	ui.nicknameLble->setText(QString::fromWCharArray(fUserInfo->nickname));
	ui.signatureLEdit->setText(QString::fromWCharArray(fUserInfo->signature));
	ui.ppNumberLble->setText(QString::number(fUserInfo->pp));
	ui.sexLble->setText(QString::fromWCharArray(fUserInfo->sex) == "m" ? "��" : "Ů");
	ui.ageLble->setText(QString::number(21)); // ����Ӧ��������
	QDate date = QDateTime::fromTime_t(fUserInfo->birthday).date();
	ui.birthdayLbel->setText(date.toString());
	ui.constellLble->setText(QString::fromStdString(caculateConstell(date.month(), date.day())));
	ui.zodiacLble->setText("��"); // ����Ӧ������
	ui.emailLble->setText(QString::fromWCharArray(fUserInfo->email));
	int diffDay = QDateTime::fromTime_t(fUserInfo->date_joined).date().daysTo(QDate::currentDate());
	ui.pAgeLble->setText(QString::number(diffDay) + " ��");
	ui.profileTEdit->setText(QString::fromWCharArray(fUserInfo->profile));
	this->show();
}

std::string p2::UserInfoDialog::caculateConstell(int value_birth_month, int value_birth_date)
{
	string constells[12][2] = {
	{ "Ħ����","ˮƿ��" },//һ��
	{ "ˮƿ��","˫����" },//����
	{ "˫����","������" },//����
	{ "������","��ţ��" },//����
	{ "��ţ��","˫����" },//����
	{ "˫����","��з��" },//����
	{ "��з��","ʨ����" },//����
	{ "ʨ����","��Ů��" },//����
	{ "��Ů��","�����" },//����
	{ "�����","��Ы��" },//ʮ��
	{ "��Ы��","������" },//ʮһ��
	{ "������","Ħ����" },//ʮһ��
	};
	//ÿ���������������������е�ֵ��Ӧÿ���������������ķָ�����
	int constell_dates[]{ 20,19,21,20,21,22,23,23,23,24,23,22 };
	return constells[value_birth_month - 1][value_birth_date / constell_dates[value_birth_month]];
}

P2_NAMESPACE_END

