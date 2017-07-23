#include "p2test.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	P2Test w;
	w.show();
	return a.exec();
}
