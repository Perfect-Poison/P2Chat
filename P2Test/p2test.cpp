#include "p2test.h"

P2Test::P2Test(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
    init();
}

P2Test::~P2Test()
{

}

void P2Test::init()
{
    /**
     *	消息头
     */
    for (size_t i = 0; i < MSG_CODE_NUM; i++) 
        ui.msgCodeCombo->addItem(sMsgCodeParam[i].strVal, sMsgCodeParam[i].enumVal);

    for (size_t i = 0; i < MSG_FLAGS_NUM; i++)
        ui.msgFlagsCombo->addItem(sMsgFlagsParam[i].strVal, sMsgFlagsParam[i].enumVal);

    ui.msgIDLineEdit->setText("0");

    ui.attrNumLineEdit->setText("0");

    /**
     *	消息载荷
     */
    
    /**
     *	
     */
    ui.ipLineEdit->setText("127.0.0.1");
    ui.portLineEdit->setText(QString::number(SERVER_PORT_FOR_UDP));
}
