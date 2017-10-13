#include "ChatClient.h"
#include "LoginDiaglog.h"
#include "RegisterDialog.h"
#include "UserInfoDialog.h"
#include "UserSettingDialog.h"
#include <QtWidgets/QToolBar>
#include <QtWidgets/QSplitter>
#include <QtNetwork/QUdpSocket>
#include <QtGui/QList>

ChatClient::ChatClient(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("P2Chat"));
    fUdpSocket = new QUdpSocket(this);
    fUdpSocket->bind(QHostAddress::LocalHost, CLIENT_PORT_FOR_UDP);

    // 创建所有窗口
    fRegisterDialog = new RegisterDialog(this);
    fLoginDialog = new LoginDiaglog(this);
    fUserSettingDialog = new UserSettingDialog(this);
    fUserInfoDialog = new UserInfoDialog(this);

    // 布局界面
    fIconToolBt = new QToolButton(this);
    fUserStatusCBox = new QComboBox(this);
    fNickNameLEdit = new QLineEdit(this);
    fSignatureLEdit = new QLineEdit(this);
    fIconToolBt->setFixedSize(50, 50);

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(fUserStatusCBox, 1);
    h1->addWidget(fNickNameLEdit, 1);
    QVBoxLayout *v1 = new QVBoxLayout;
    v1->addLayout(h1);
    v1->addWidget(fSignatureLEdit, 2);
    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(fIconToolBt, 1);
    h2->addLayout(v1, 2);

    fSearchLEdit = new QLineEdit(this);
    QVBoxLayout *v2 = new QVBoxLayout;
    v2->addLayout(h2);
    v2->addWidget(fSearchLEdit);

    fTabWidget = new QTabWidget(this);
    fTabWidget->addTab(new QWidget, tr("联系人"));
    fTabWidget->addTab(new QWidget, tr("群聊"));
    fTabWidget->addTab(new QWidget, tr("会话"));
    fTabWidget->addTab(new QWidget, tr("我的地盘"));
    v2->addWidget(fTabWidget);

    fToolBar = new QToolBar(this);
    fSettingAct = new QAction(tr("主菜单"), this);
    fAddAct = new QAction(tr("加好友"), this);
    fToolBar->addAction(fSettingAct);
    fToolBar->addAction(fAddAct);
    v2->addWidget(fToolBar);

    fUserStatusCBox->addItem(QIcon(""), tr("在线"), p2UserStatusOnline);
    fUserStatusCBox->addItem(QIcon(""), tr("离开"), p2UserStatusAFK);
    fUserStatusCBox->addItem(QIcon(""), tr("忙碌"), p2UserStatusBusy);
    fUserStatusCBox->addItem(QIcon(""), tr("隐身"), p2UserStatusInvisible);
    fUserStatusCBox->addItem(QIcon(""), tr("请勿打扰"), p2UserStatusNoDisturbing);
    fUserStatusCBox->addItem(QIcon(""), tr("离线"), p2UserStatusOffline);
    fUserStatusCBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    setLayout(v2);
    setFixedSize(250, 600);
    // 槽函数

    connect(fIconToolBt, SIGNAL(clicked()), fUserInfoDialog, SLOT(show()));
}

ChatClient::~ChatClient()
{

}
