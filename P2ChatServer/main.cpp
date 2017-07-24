#include "Common/common.h"
#include "mysqlpp/mysql++.h"
#include "Common/ConnectionPool.h"
#include "Common/EventContext.h"
#include "Common/TCPListenerSocket.h"
#include "Common/Task.h"
#include "Common/UDPSocket.h"
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
