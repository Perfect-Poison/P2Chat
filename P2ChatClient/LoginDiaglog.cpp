#include "LoginDiaglog.h"

LoginDiaglog::LoginDiaglog(QWidget *parent)
    : QDialog(parent)
{
    Init();
}

LoginDiaglog::~LoginDiaglog()
{

}

void LoginDiaglog::Init()
{
    this->setFixedSize(300, 150);

    fUserNameLCombo = new QComboBox(this);
    fUserNameLCombo->setEditable(true);
    fUserNameLCombo->setFixedHeight(25);

    fPassWordLEdit = new QLineEdit(this);
    fPassWordLEdit->setEchoMode(QLineEdit::Password);
    fPassWordLEdit->setFixedHeight(25);

    fLoginBt = new QPushButton(tr(("��¼")), this);
    fLoginBt->setFixedHeight(30);
    fRegisterBt = new QPushButton(tr(("ע���˺�")), this);
    fFindPasswordBt = new QPushButton(tr(("�һ�����")), this);
    fRemPassword = new QCheckBox(tr(("��ס����")), this);
    fAutoLogin = new QCheckBox(tr(("�Զ���¼")), this);

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
}
