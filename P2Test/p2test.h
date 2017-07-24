#ifndef P2TEST_H
#define P2TEST_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QUdpSocket>
#include "ui_p2test.h"
#include "common.h"

class P2Test : public QMainWindow
{
	Q_OBJECT

public:
	P2Test(QWidget *parent = 0);
	~P2Test();
    void init();
private:
	Ui::P2TestClass ui;
    QUdpSocket *fUdpSocket;
};

#endif // P2TEST_H
