#include <QtWidgets/QApplication>
#include "LoginDiaglog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDiaglog w;
    w.show();
    return a.exec();
}
