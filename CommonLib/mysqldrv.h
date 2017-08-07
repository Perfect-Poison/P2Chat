#pragma once
#include "p2_util.h"
#include "mysql.h"
#include "errmsg.h"
#include "Mutex.h"
#include <vector>
P2_NAMESPACE_BEG
typedef struct
{
    MYSQL *pMySQL;
    Mutex mutexQueryLock;
} MYSQL_CONN;

typedef struct
{
    MYSQL_CONN *connection;
    MYSQL_STMT *statement;
    MYSQL_BIND *bindings;
    unsigned long *lengthFields;
    vector<void*> *buffers;
    int paramCount;
} MYSQL_STATEMENT;

bool DrvInit();
void DrvUnload();
MYSQL_CONN* DrvConnect(const char *szHost, const char *szLogin, const char *szPassword, const char *szDatabase, WCHAR *errorText);
void DrvDisconnect(MYSQL_CONN *pConn);
MYSQL_STATEMENT* DrvPrepare(MYSQL_CONN *pConn, WCHAR *pwszQuery, DWORD *pdwError, WCHAR *errorText);
P2_NAMESPACE_END