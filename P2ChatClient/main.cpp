#include <QtWidgets/QApplication>
#include "ChatClient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChatClient chatClient;
    chatClient.show();
    return a.exec();
}
