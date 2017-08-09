#pragma once
#include "Thread.h"
#include "p2_dbapi.h"
#include "Mutex.h"
#include "Cond.h"
P2_NAMESPACE_BEG

class ConnectionPoolThread :
    public Thread
{
public:
    ConnectionPoolThread();
    virtual ~ConnectionPoolThread();
private:
    bool s_initialized = false;
    TCHAR m_server[256];
    TCHAR m_login[256];
    TCHAR m_password[256];
    TCHAR m_dbName[256];
    TCHAR m_schema[256];

    int m_basePoolSize;
    int m_maxPoolSize;
    int m_cooldownTime;
    int m_connectionTTL;

    Mutex m_poolAccessMutex;
    vector<PoolConnectionInfo> m_connections;
    Cond m_condShutdown;
    Cond m_condRelease;
};

P2_NAMESPACE_END