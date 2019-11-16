/*
** NetXMS - Network Management System
** DB Library
** Copyright (C) 2003-2011 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: nxdbapi.h
**
**/
#pragma once
#include "p2_common.h"
#include "Thread.h"
#include "p2_dbapi.h"
#include "Mutex.h"
#include "Cond.h"
P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define CONNECTIONPOOLTHREAD_DEBUG 1
#else
#define CONNECTIONPOOLTHREAD_DEBUG 0
#endif

class ConnectionPoolThread :
    public Thread
{
public:
    static ConnectionPoolThread* GetInstance()
    {
        if (!s_initialized)
            return nullptr;
        if (sInstance == nullptr)
            sInstance = new ConnectionPoolThread;
        return sInstance;
    }
    static bool Initialize(const TCHAR *server, const TCHAR *dbName,
        const TCHAR *login, const TCHAR *password, int basePoolSize, int maxPoolSize, int cooldownTime,
        int connTTL, bool dumpSQL);
    static bool DBConnectionPoolPopulate();
    static void DBConnectionPoolShrink();
    static bool ResetConnection(PoolConnectionInfo *conn);
    static void ResetExpiredConnections();
    
    virtual void Entry();
    static void DBConnectionPoolShutdown();
    static DB_HANDLE __DBConnectionPoolAcquireConnection(const char *srcFile, int srcLine);
    static void DBConnectionPoolReleaseConnection(DB_HANDLE handle);
    static int DBConnectionPoolGetSize();
    static int DBConnectionPoolGetAcquiredCount();
    static vector<PoolConnectionInfo *> *DBConnectionPoolGetConnectionList();

private:
    ConnectionPoolThread() :Thread()
    {
        if (!s_initialized)
            log_debug(5, _T("[error] ConnectionPoolThread::ConnectionPoolThread 未进行初始化\n"));
        
//         if (CONNECTIONPOOLTHREAD_DEBUG)
//             log_debug(1, _T("ConnectionPoolThread::ConnectionPoolThread 创建数据库连接池线程\n"));
    }
    virtual ~ConnectionPoolThread() {};
    static ConnectionPoolThread *sInstance;
    static bool s_initialized;
    static TCHAR m_server[256];
    static TCHAR m_login[256];
    static TCHAR m_password[256];
    static TCHAR m_dbName[256];

    static unsigned int m_basePoolSize;
    static unsigned int m_maxPoolSize;
    static unsigned int m_cooldownTime;
    static unsigned int m_connectionTTL;

    static Mutex m_poolAccessMutex;
    static vector<PoolConnectionInfo *> m_connections;
    static Cond m_condShutdown;
    static Cond m_condRelease;
    static bool m_dumpSQL;
};

P2_NAMESPACE_END