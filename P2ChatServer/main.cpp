#include "Common/common.h"
#include "mysqlpp/mysql++.h"
#include "ConnectionPool.h"
#include "EventContext.h"
#include "TCPListenerSocket.h"
#include "Task.h"
using namespace mysqlpp;

P2_NAMESPACE_USE

int main()
{
    EventThread *eventThread = EventThread::GetInstance();
    TCPListenerSocket *tcpListenerSocket = new TCPListenerSocket;
    tcpListenerSocket->Initialize(SERVER_PORT_FOR_TCP);
    TaskThreadPool::AddThreads(1);
    eventThread->Start();
    tcpListenerSocket->RequestEvent(EV_RE);
    while (true)
    {
        Sleep(1000);
    }
    return 0;
}
