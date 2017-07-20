#include "Common/common.h"
#include "mysqlpp/mysql++.h"
#include "ConnectionPool.h"
#include "EventContext.h"
#include "TCPListenerSocket.h"
#include "Task.h"
#include "UDPSocket.h"
using namespace mysqlpp;

P2_NAMESPACE_USE

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        printf("WSAStartup error!\n");

    EventThread *eventThread = EventThread::GetInstance();
    TCPListenerSocket *tcpListenerSocket = new TCPListenerSocket;
    tcpListenerSocket->Initialize(SERVER_PORT_FOR_TCP);
    UDPSocket *udpSocket = new UDPSocket;
    udpSocket->Open();
    udpSocket->Bind(SERVER_PORT_FOR_UDP);
    udpSocket->RequestEvent(EV_RE);

    TaskThreadPool::AddThreads(4);
    eventThread->Start();
    tcpListenerSocket->RequestEvent(EV_RE);
    while (true)
    {
        Sleep(1000);
    }

    WSACleanup();
    printf("clean up all sockets!\n");
    return 0;
}
