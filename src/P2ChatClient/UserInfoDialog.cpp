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

    // 槽连接
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
	ui.sexLble->setText(QString::fromWCharArray(fUserInfo->sex) == "m" ? "男" : "女");
	ui.ageLble->setText(QString::number(21)); // 年龄应当做减法
	QDate date = QDateTime::fromTime_t(fUserInfo->birthday).date();
	ui.birthdayLbel->setText(date.toString());
	ui.constellLble->setText(QString::fromStdString(caculateConstell(date.month(), date.day())));
	ui.zodiacLble->setText("狗"); // 属相应当计算
	ui.emailLble->setText(QString::fromWCharArray(fUserInfo->email));
	int diffDay = QDateTime::fromTime_t(fUserInfo->date_joined).date().daysTo(QDate::currentDate());
	ui.pAgeLble->setText(QString::number(diffDay) + " 天");
	ui.profileTEdit->setText(QString::fromWCharArray(fUserInfo->profile));
	this->show();
}

std::string p2::UserInfoDialog::caculateConstell(int value_birth_month, int value_birth_date)
{
	string constells[12][2] = {
	{ "摩羯座","水瓶座" },//一月
	{ "水瓶座","双鱼座" },//二月
	{ "双鱼座","白羊座" },//三月
	{ "白羊座","金牛座" },//四月
	{ "金牛座","双子座" },//五月
	{ "双子座","巨蟹座" },//六月
	{ "巨蟹座","狮子座" },//七月
	{ "狮子座","处女座" },//八月
	{ "处女座","天秤座" },//九月
	{ "天秤座","天蝎座" },//十月
	{ "天蝎座","射手座" },//十一月
	{ "射手座","摩羯座" },//十一月
	};
	//每个月有两个星座，数组中的值对应每个月中两个星座的分割日期
	int constell_dates[]{ 20,19,21,20,21,22,23,23,23,24,23,22 };
	return constells[value_birth_month - 1][value_birth_date / constell_dates[value_birth_month]];
}

P2_NAMESPACE_END

