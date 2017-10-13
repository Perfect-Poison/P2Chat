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

    // �������д���
    fRegisterDialog = new RegisterDialog(this);
    fLoginDialog = new LoginDiaglog(this);
    fUserSettingDialog = new UserSettingDialog(this);
    fUserInfoDialog = new UserInfoDialog(this);

    // ���ֽ���
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
    fTabWidget->addTab(new QWidget, tr("��ϵ��"));
    fTabWidget->addTab(new QWidget, tr("Ⱥ��"));
    fTabWidget->addTab(new QWidget, tr("�Ự"));
    fTabWidget->addTab(new QWidget, tr("�ҵĵ���"));
    v2->addWidget(fTabWidget);

    fToolBar = new QToolBar(this);
    fSettingAct = new QAction(tr("���˵�"), this);
    fAddAct = new QAction(tr("�Ӻ���"), this);
    fToolBar->addAction(fSettingAct);
    fToolBar->addAction(fAddAct);
    v2->addWidget(fToolBar);

    fUserStatusCBox->addItem(QIcon(""), tr("����"), p2UserStatusOnline);
    fUserStatusCBox->addItem(QIcon(""), tr("�뿪"), p2UserStatusAFK);
    fUserStatusCBox->addItem(QIcon(""), tr("æµ"), p2UserStatusBusy);
    fUserStatusCBox->addItem(QIcon(""), tr("����"), p2UserStatusInvisible);
    fUserStatusCBox->addItem(QIcon(""), tr("�������"), p2UserStatusNoDisturbing);
    fUserStatusCBox->addItem(QIcon(""), tr("����"), p2UserStatusOffline);
    fUserStatusCBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    setLayout(v2);
    setFixedSize(250, 600);
    // �ۺ���

    connect(fIconToolBt, SIGNAL(clicked()), fUserInfoDialog, SLOT(show()));
}

ChatClient::~ChatClient()
{

}
