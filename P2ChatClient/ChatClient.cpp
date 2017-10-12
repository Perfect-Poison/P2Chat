#include "ChatClient.h"
#include "LoginDiaglog.h"
#include "RegisterForm.h"
#include <QtWidgets/QToolBar>
#include <QtNetwork/QUdpSocket>

ChatClient::ChatClient(QWidget *parent)
    : QWidget(parent)
{
    fUdpSocket = new QUdpSocket(this);
    fUdpSocket->bind(QHostAddress::LocalHost, CLIENT_PORT_FOR_UDP);

    // �������д���
    fLoginDialog = new LoginDiaglog(this);
    fRegisterForm = new RegisterForm(this);
    
    // ���ֽ���
    fIconToolBt = new QToolButton(this);
    fStatusCBox = new QComboBox(this);
    fNickNameLEdit = new QLineEdit(this);
    fSignatureLEdit = new QLineEdit(this);

    fSearchLEdit = new QLineEdit(this);

    fTabWidget = new QTabWidget(this);

    fToolBar = new QToolBar(this);

    // �ۺ���
}

ChatClient::~ChatClient()
{

}
