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
    // 日志线程
    log_set_debug_level(1);
    if (!log_open(_T("p2log"), LOG_PRINT_TO_CONSOLE, LOG_ROTATION_DAILY, 1024 * 1024, 4, _T("")))
        _tprintf(_T("[error] 日志文件打开失败\n"));
    Sleep(10);

    //-------------------------------
    // 数据库连接池线程
    ConnectionPoolThread::Initialize(_T("localhost"), _T("p2chatdb"), _T("root"), _T("123123"), 10, 30, 300, 14400, true);
    ConnectionPoolThread *connectionPoolThread = ConnectionPoolThread::GetInstance();
    DB_HANDLE hConn = connectionPoolThread->DBConnectionPoolAcquireConnection();
    DB_STATEMENT hStmt = DBPrepare(hConn, _T("INSERT INTO log_data (log_type_id,record_date,log_data) VALUES (?,?,?)"));
    if (hStmt != nullptr)
    {
        DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, 5);
        DBBind(hStmt, 2, DB_SQLTYPE_VARCHAR, _T("2000-12-12 23:11:11"), DB_BIND_STATIC);
        DBBind(hStmt, 3, DB_SQLTYPE_VARCHAR, _T("this is my first log"), DB_BIND_STATIC);
        UINT32 rcc = DBExecute(hStmt);
        connectionPoolThread->DBConnectionPoolReleaseConnection(hConn);
        DBFreeStatement(hStmt);
    }
    
    //-------------------------------
    // 事件监听线程
    EventThread *eventThread = EventThread::GetInstance();
    
    TCPListenerSocket *tcpListenerSocket = new TCPListenerSocket;
    tcpListenerSocket->Open();
    tcpListenerSocket->Listen(SERVER_PORT_FOR_TCP);
    
    UDPSocket *udpSocket = new UDPSocket;
    udpSocket->Open();
    udpSocket->Bind(SERVER_PORT_FOR_UDP);
    
    //-------------------------------
    // 4个任务线程
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
