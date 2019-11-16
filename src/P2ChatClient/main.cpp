#include <QtWidgets/QApplication>
#include "ChatClient.h"
#include "LogThread.h"
P2_NAMESPACE_USE


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //-------------------------------
    // 日志线程
    LogThread::Initialize(_T("p2clog"), 1, 0, LOG_ROTATION_DAILY, 1024 * 1024, 4, _T(""));
    LogThread* logThread = LogThread::GetInstance();
    logThread->Start();
    Sleep(10);

    ChatClient chatClient;
    /*chatClient.show();*/
    return a.exec();
}
