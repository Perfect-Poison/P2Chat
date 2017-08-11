#include "ConnectionPoolThread.h"
P2_NAMESPACE_BEG

ConnectionPoolThread* ConnectionPoolThread::sInstance = nullptr;
bool ConnectionPoolThread::s_initialized = false;
TCHAR ConnectionPoolThread::m_server[256] = _T("");
TCHAR ConnectionPoolThread::m_login[256] = _T("");
TCHAR ConnectionPoolThread::m_password[256] = _T("");
TCHAR ConnectionPoolThread::m_dbName[256] = _T("");

unsigned int ConnectionPoolThread::m_basePoolSize = 10;
unsigned int ConnectionPoolThread::m_maxPoolSize = 30;
unsigned int ConnectionPoolThread::m_cooldownTime = 300;
unsigned int ConnectionPoolThread::m_connectionTTL = 14400;

Mutex ConnectionPoolThread::m_poolAccessMutex;
vector<PoolConnectionInfo *> ConnectionPoolThread::m_connections;
Cond ConnectionPoolThread::m_condShutdown;
Cond ConnectionPoolThread::m_condRelease;
bool ConnectionPoolThread::m_dumpSQL = false;

/**
* Callback for sorting reset list
*/
static int ResetListSortCallback(const void *e1, const void *e2)
{
    return ((PoolConnectionInfo *)e1)->usageCount > ((PoolConnectionInfo *)e2)->usageCount ? -1 :
        (((PoolConnectionInfo *)e1)->usageCount == ((PoolConnectionInfo *)e2)->usageCount ? 0 : 1);
}

bool ConnectionPoolThread::Initialize(const TCHAR *server, const TCHAR *dbName, const TCHAR *login, const TCHAR *password, int basePoolSize, int maxPoolSize, int cooldownTime, int connTTL, bool dumpSQL)
{
    if (s_initialized)
        return true;   // already initialized
    if (!DBLoad(dumpSQL))
    {
        log_debug(5, _T("ConnectionPoolThread::Initialize Database Load failed\n"));
        return false;
    }
    _tcsncpy(m_server, server, 256);
    _tcsncpy(m_dbName, dbName, 256);
    _tcsncpy(m_login, login, 256);
    _tcsncpy(m_password, password, 256);

    m_basePoolSize = basePoolSize;
    m_maxPoolSize = maxPoolSize;
    m_cooldownTime = cooldownTime;
    m_connectionTTL = connTTL;
    m_dumpSQL = dumpSQL;
    //m_poolAccessMutex = MutexCreate();
    //m_connections.setOwner(true);
   // m_condShutdown = ConditionCreate(TRUE);
    //m_condRelease = ConditionCreate(FALSE);

    if (!DBConnectionPoolPopulate())
    {
        return false;
    }

    s_initialized = true;
    log_debug(0, _T("ConnectionPoolThread::Initialize Database Connection Pool initialized\n"));

    return true;
}

bool ConnectionPoolThread::DBConnectionPoolPopulate()
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    bool success = false;

    MutexLocker locker(&m_poolAccessMutex);
    for (size_t i = 0; i < m_basePoolSize; i++)
    {
        PoolConnectionInfo *conn = new PoolConnectionInfo;
        conn->handle = DBConnect(m_server, m_dbName, m_login, m_password, errorText);
        if (conn->handle != NULL)
        {
            conn->inUse = false;
            conn->connectTime = time(NULL);
            conn->lastAccessTime = conn->connectTime;
            conn->usageCount = 0;
            conn->srcFile[0] = 0;
            conn->srcLine = 0;
            m_connections.push_back(conn);
            success = true;
        }
        else
        {
            log_debug(5, _T("ConnectionPoolThread::DBConnectionPoolPopulate Database Connection Pool: cannot create DB connection %d (%s)\n"), i, errorText);
            delete conn;
        }
    }
    return success;
}

void ConnectionPoolThread::DBConnectionPoolShrink()
{
    MutexLocker locker(&m_poolAccessMutex);
    time_t now = time(NULL);
    for (size_t i = m_basePoolSize; i < m_connections.size(); i++)
    {
        PoolConnectionInfo *conn = m_connections[i];
        if (!conn->inUse && (now - conn->lastAccessTime > m_cooldownTime))
        {
            DBDisconnect(conn->handle);
            auto it = remove(m_connections.begin(), m_connections.end(), m_connections[i]);
            delete *it;
            i--;
        }
    }
}

bool ConnectionPoolThread::ResetConnection(PoolConnectionInfo *conn)
{
    time_t now = time(NULL);
    DBDisconnect(conn->handle);

    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    conn->handle = DBConnect(m_server, m_dbName, m_login, m_password, errorText);
    if (conn->handle != NULL)
    {
        conn->connectTime = now;
        conn->lastAccessTime = now;
        conn->usageCount = 0;

        log_debug(1, _T("ConnectionPoolThread::ResetConnection Database Connection Pool: connection %p reconnected\n"), conn->handle);
        return true;
    }
    else
    {
        log_debug(5, _T("ConnectionPoolThread::ResetConnection Database Connection Pool: connection %p reconnect failure (%s)\n"), conn->handle, errorText);
        return false;
    }
}

void ConnectionPoolThread::ResetExpiredConnections()
{
    time_t now = time(NULL);

    m_poolAccessMutex.Lock();

    size_t i, availCount = 0;
    vector<PoolConnectionInfo *> reconnList(m_connections.size());
    for (i = 0; i < m_connections.size(); i++)
    {
        PoolConnectionInfo *conn = m_connections[i];
        if (!conn->inUse)
        {
            availCount++;
            if (now - conn->connectTime > m_connectionTTL)
            {
                reconnList.push_back(conn);
            }
        }
    }

    size_t count = min(availCount / 2 + 1, reconnList.size()); // reset no more than 50% of available connections
    if (count < reconnList.size())
    {
        sort(reconnList.begin(), reconnList.end(), ResetListSortCallback);
        //reconnList.sort();
        while (reconnList.size() > count)
        {
            remove(reconnList.begin(), reconnList.end(), reconnList[count]);
            //reconnList.remove(count);
        }
    }

    for (i = 0; i < count; i++)
        reconnList[i]->inUse = true;
    m_poolAccessMutex.Unlock();

    // do reconnects
    for (i = 0; i < count; i++)
    {
        PoolConnectionInfo *conn = reconnList[i];
        bool success = ResetConnection(conn);
        MutexLocker locker(&m_poolAccessMutex);
        if (success)
        {
            conn->inUse = false;
        }
        else
        {
            auto it = remove(m_connections.begin(), m_connections.end(), conn);
            delete *it;
            //m_connections.remove(conn);
        }
    }
}

void ConnectionPoolThread::Entry()
{
    log_debug(1, _T("ConnectionPoolThread::Entry 启动数据库连接池线程\n"));
    while (!m_condShutdown.Wait((m_connectionTTL > 0) ? m_connectionTTL * 750 : 300000))
    {
        DBConnectionPoolShrink();
        if (m_connectionTTL > 0)
        {
            ResetExpiredConnections();
        }
    }

    log_debug(1, _T("ConnectionPoolThread::Entry 停止数据库连接池线程\n"));
}

/**
* Shutdown connection pool
*/
void ConnectionPoolThread::DBConnectionPoolShutdown()
{
    if (!s_initialized)
        return;

    m_condShutdown.Broadcast();
    //ThreadJoin(m_maintThread);

    for (size_t i = 0; i < m_connections.size(); i++)
    {
        DBDisconnect(m_connections[i]->handle);
    }

    for (auto it = m_connections.begin();
        it != m_connections.end();
        it++)
    {
        delete *it;
    }
    m_connections.clear();

    s_initialized = false;
    log_debug(1, _T("ConnectionPoolThread::DBConnectionPoolShutdown Database Connection Pool terminated\n"));
    DBUnload();
}

/**
* Acquire connection from pool. This function never fails - if it's impossible to acquire
* pooled connection, calling thread will be suspended until there will be connection available.
*/
DB_HANDLE ConnectionPoolThread::__DBConnectionPoolAcquireConnection(const char *srcFile, int srcLine)
{
retry:
    m_poolAccessMutex.Lock();

    DB_HANDLE handle = NULL;

    // find less used connection
    UINT32 count = 0xFFFFFFFF;
    int index = -1;
    for (size_t i = 0; (i < m_connections.size()) && (count > 0); i++)
    {
        PoolConnectionInfo *conn = m_connections[i];
        if (!conn->inUse && (conn->usageCount < count))
        {
            count = conn->usageCount;
            index = i;
        }
    }

    if (index > -1)
    {
        PoolConnectionInfo *conn = m_connections[index];
        handle = conn->handle;
        conn->inUse = true;
        conn->lastAccessTime = time(NULL);
        conn->usageCount++;
        strncpy(conn->srcFile, srcFile, 128);
        conn->srcLine = srcLine;
    }
    else if (m_connections.size() < m_maxPoolSize)
    {
        TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
        PoolConnectionInfo *conn = new PoolConnectionInfo;
        conn->handle = DBConnect(m_server, m_dbName, m_login, m_password, errorText);
        if (conn->handle != NULL)
        {
            conn->inUse = true;
            conn->connectTime = time(NULL);
            conn->lastAccessTime = conn->connectTime;
            conn->usageCount = 0;
            strncpy(conn->srcFile, srcFile, 128);
            conn->srcLine = srcLine;
            m_connections.push_back(conn);
            handle = conn->handle;
        }
        else
        {
            log_debug(5, _T("ConnectionPoolThread::DBConnectionPoolAcquireConnection Database Connection Pool: cannot create additional DB connection (%s)\n"), errorText);
            delete conn;
        }
    }

    m_poolAccessMutex.Unlock();

    if (handle == NULL)
    {
        log_debug(5, _T("ConnectionPoolThread::DBConnectionPoolAcquireConnection Database Connection Pool exhausted (call from %hs:%d)\n"), srcFile, srcLine);
        m_condRelease.Wait(10000);
        log_debug(5, _T("ConnectionPoolThread::DBConnectionPoolAcquireConnection Database Connection Pool: retry acquire connection (call from %hs:%d)\n"), srcFile, srcLine);
        goto retry;
    }

    log_debug(0, _T("ConnectionPoolThread::DBConnectionPoolAcquireConnection Database Connection Pool: handle %p acquired (call from %hs:%d)\n"), handle, srcFile, srcLine);
    return handle;
}

/**
* Release acquired connection
*/
void ConnectionPoolThread::DBConnectionPoolReleaseConnection(DB_HANDLE handle)
{
    m_poolAccessMutex.Lock();

    for (size_t i = 0; i < m_connections.size(); i++)
    {
        PoolConnectionInfo *conn = m_connections[i];
        if (conn->handle == handle)
        {
            conn->inUse = false;
            conn->lastAccessTime = time(NULL);
            conn->srcFile[0] = 0;
            conn->srcLine = 0;
            break;
        }
    }

    m_poolAccessMutex.Unlock();

    log_debug(0, _T("ConnectionPoolThread::DBConnectionPoolReleaseConnection Database Connection Pool: handle %p released\n"), handle);
    m_condRelease.Signal();
}

/**
* Get current size of DB connection pool
*/
int ConnectionPoolThread::DBConnectionPoolGetSize()
{
    MutexLocker locker(&m_poolAccessMutex);
    int size = m_connections.size();
    return size;
}

/**
* Get number of acquired connections in DB connection pool
*/
int ConnectionPoolThread::DBConnectionPoolGetAcquiredCount()
{
    int count = 0;
    MutexLocker locker(&m_poolAccessMutex);
    for (size_t i = 0; i < m_connections.size(); i++)
    {
        if (m_connections[i]->inUse)
            count++;
    }
    return count;
}

/**
* Get copy of active DB connections.
* Returned list must be deleted by the caller.
*/
vector<PoolConnectionInfo *> * ConnectionPoolThread::DBConnectionPoolGetConnectionList()
{
    vector<PoolConnectionInfo *> *list = new vector<PoolConnectionInfo *>;
    MutexLocker locker(&m_poolAccessMutex);
    for (size_t i = 0; i < m_connections.size(); i++)
    {
        if (m_connections[i]->inUse)
        {
            list->push_back((PoolConnectionInfo *)memdup(m_connections[i], sizeof(PoolConnectionInfo)));
        }
    }
    return list;
}

P2_NAMESPACE_END

