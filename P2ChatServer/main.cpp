#include "p2_common.h"
#include "Event.h"
#include "TCPListenerSocket.h"
#include "Task.h"
#include "UDPSocket.h"
#include "LogThread.h"
#include "ConnectionPoolThread.h"
P2_NAMESPACE_USE

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        _tprintf(_T("[error] WSAStartup error!\n"));

    //-------------------------------
    // ��־�߳�
    log_set_debug_level(1);
    if (!log_open(_T("p2log"), LOG_PRINT_TO_CONSOLE, LOG_ROTATION_DAILY, 1024 * 1024, 4, _T("")))
        _tprintf(_T("[error] ��־�ļ���ʧ��\n"));
    Sleep(10);

    //-------------------------------
    // ���ݿ����ӳ��߳�
    ConnectionPoolThread::Initialize(_T("localhost"), _T("p2chatdb"), _T("root"), _T("123123"), 10, 30, 300, 14400, true);
    ConnectionPoolThread *connectionPoolThread = ConnectionPoolThread::GetInstance();
    DB_HANDLE dbHandle = connectionPoolThread->DBConnectionPoolAcquireConnection();
    
    //-------------------------------
    // �¼������߳�
    EventThread *eventThread = EventThread::GetInstance();
    
    TCPListenerSocket *tcpListenerSocket = new TCPListenerSocket;
    tcpListenerSocket->Open();
    tcpListenerSocket->Listen(SERVER_PORT_FOR_TCP);
    
    UDPSocket *udpSocket = new UDPSocket;
    udpSocket->Open();
    udpSocket->Bind(SERVER_PORT_FOR_UDP);
    
    //-------------------------------
    // 4�������߳�
    TaskThreadPool::AddThreads(4);
    
    eventThread->Start();
    
    tcpListenerSocket->RequestEvent(EV_RE);
    
    udpSocket->RequestEvent(EV_RE);
    
    while (true)
    {
        Sleep(1000);
    }

    WSACleanup();
    _tprintf(_T("clean up all sockets!\n"));
    return 0;
}
