#include "p2_common.h"
#include "mysqlpp/mysql++.h"
#include "ConnectionPool.h"
#include "Event.h"
#include "TCPListenerSocket.h"
#include "Task.h"
#include "UDPSocket.h"
#include "LogThread.h"
using namespace mysqlpp;

P2_NAMESPACE_USE

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        printf("WSAStartup error!\n");

    LogThread *logThread = LogThread::GetInstance();
    if (logThread->LogOpen("P2Chat", LOG_PRINT_TO_CONSOLE, LOG_ROTATION_DAILY, 1024 * 1024, 4, ""))
        logThread->Start();
    else
        printf("[error] 日志文件打开失败\n");


    EventThread *eventThread = EventThread::GetInstance();
    
    TCPListenerSocket *tcpListenerSocket = new TCPListenerSocket;
    tcpListenerSocket->Open();
    tcpListenerSocket->Listen(SERVER_PORT_FOR_TCP);
    
    UDPSocket *udpSocket = new UDPSocket;
    udpSocket->Open();
    udpSocket->Bind(SERVER_PORT_FOR_UDP);
    
    TaskThreadPool::AddThreads(4);
    
    eventThread->Start();
    
    tcpListenerSocket->RequestEvent(EV_RE);
    
    udpSocket->RequestEvent(EV_RE);
    
    while (true)
    {
        Sleep(1000);
    }

    WSACleanup();
    printf("clean up all sockets!\n");
    return 0;
}
