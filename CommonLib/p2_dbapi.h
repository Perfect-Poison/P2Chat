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
#include "mysqldrv.h"
P2_NAMESPACE_BEG
//
// DB-related constants
//

#define MAX_DB_LOGIN          64
#define MAX_DB_PASSWORD       64
#define MAX_DB_NAME           256

/**
* DB events
*/
#define DBEVENT_CONNECTION_LOST        0
#define DBEVENT_CONNECTION_RESTORED    1
#define DBEVENT_QUERY_FAILED           2


/**
* Database syntax codes
*/
#define DB_SYNTAX_MYSQL    0
#define DB_SYNTAX_PGSQL    1
#define DB_SYNTAX_MSSQL    2
#define DB_SYNTAX_ORACLE   3
#define DB_SYNTAX_SQLITE   4
#define DB_SYNTAX_DB2      5
#define DB_SYNTAX_INFORMIX 6
#define DB_SYNTAX_UNKNOWN	-1

/**
* Database driver structure
*/
// struct db_driver_t
// {
//     //const char *m_name;
//     int m_refCount;
//     bool m_logSqlErrors;
//     bool m_dumpSql;
//     int m_reconnect;
//     int m_defaultPrefetchLimit;
//     Mutex m_mutexReconnect;
//     //HMODULE m_handle;
//     //void *m_userArg;
// };

/**
* Global variables
*/
// extern UINT32 g_logMsgCode;
// extern UINT32 g_sqlErrorMsgCode;

/**
* Database connection structures
*/

struct db_handle_t;
typedef db_handle_t * DB_HANDLE;

struct db_statement_t;
typedef db_statement_t * DB_STATEMENT;

struct db_result_t;
typedef db_result_t * DB_RESULT;

struct db_unbuffered_result_t;
typedef db_unbuffered_result_t * DB_UNBUFFERED_RESULT;

/**
* Prepared statement
*/
struct db_statement_t
{
    DB_HANDLE m_connection;
    MYSQL_STATEMENT *m_statement;
    TCHAR *m_query;
};

/**
* Database connection structure
*/
struct db_handle_t
{
    MYSQL_CONN *m_connection;
    //  DB_DRIVER m_driver;
    bool m_dumpSql;
    bool m_reconnectEnabled;
    Mutex *m_mutexTransLock;      // Transaction lock
    int m_transactionLevel;
    char *m_server;
    char *m_login;
    char *m_password;
    char *m_dbName;
    vector<db_statement_t*> *m_preparedStatements;
};

/**
* SELECT query result
*/
struct db_result_t
{
    //  DB_DRIVER m_driver;
    DB_HANDLE m_connection;
    MYSQL_RESULT *m_data;
};

/**
* Unbuffered SELECT query result
*/
struct db_unbuffered_result_t
{
    // DB_DRIVER m_driver;
    DB_HANDLE m_connection;
    MYSQL_UNBUFFERED_RESULT *m_data;
};

/**
* Pool connection information
*/
struct PoolConnectionInfo
{
    DB_HANDLE handle;
    bool inUse;
    time_t lastAccessTime;
    time_t connectTime;
    UINT32 usageCount;
    char srcFile[128];
    int srcLine;
};

/**
* DB library performance counters
*/
struct LIBNXDB_PERF_COUNTERS
{
    UINT64 selectQueries;
    UINT64 nonSelectQueries;
    UINT64 totalQueries;
    UINT64 longRunningQueries;
    UINT64 failedQueries;
};

/**
* Functions
*/
bool DBLoad(bool dumpSQL);
void DBUnload();

DB_HANDLE DBConnect(TCHAR *server, TCHAR *dbName, TCHAR *login, TCHAR *password, TCHAR *errorText);
void DBDisconnect(DB_HANDLE hConn);
void DBEnableReconnect(DB_HANDLE hConn, bool enabled);

DB_STATEMENT DBPrepare(DB_HANDLE hConn, TCHAR *szQuery);
DB_STATEMENT DBPrepareEx(DB_HANDLE hConn, TCHAR *szQuery, TCHAR *errorText);
void DBFreeStatement(DB_STATEMENT hStmt);
const TCHAR *DBGetStatementSource(DB_STATEMENT hStmt);
//bool DBOpenBatch(DB_STATEMENT hStmt);
//void DBNextBatchRow(DB_STATEMENT hStmt);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, int cType, void *buffer, int allocType);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, TCHAR *value, int allocType);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, TCHAR *value, int allocType, int maxLen);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, INT32 value);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, UINT32 value);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, INT64 value);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, UINT64 value);
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, double value);
//void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, uuid& value);
bool DBExecute(DB_STATEMENT hStmt);
bool DBExecuteEx(DB_STATEMENT hStmt, TCHAR *errorText);
DB_RESULT DBSelectPrepared(DB_STATEMENT hStmt);
DB_RESULT DBSelectPreparedEx(DB_STATEMENT hStmt, TCHAR *errorText);
DB_UNBUFFERED_RESULT DBSelectPreparedUnbuffered(DB_STATEMENT hStmt);
DB_UNBUFFERED_RESULT DBSelectPreparedUnbufferedEx(DB_STATEMENT hStmt, TCHAR *errorText);

bool DBQuery(DB_HANDLE hConn, TCHAR *szQuery);
bool DBQueryEx(DB_HANDLE hConn, TCHAR *szQuery, TCHAR *errorText);

DB_RESULT DBSelect(DB_HANDLE hConn, TCHAR *szQuery);
DB_RESULT DBSelectEx(DB_HANDLE hConn, TCHAR *szQuery, TCHAR *errorText);
int DBGetColumnCount(DB_RESULT hResult);
bool DBGetColumnName(DB_RESULT hResult, int column, TCHAR *buffer, int bufSize);
int DBGetNumRows(DB_RESULT hResult);
void DBFreeResult(DB_RESULT hResult);

TCHAR *DBGetField(DB_RESULT hResult, int row, int col, TCHAR *pszBuffer, int nBufLen);
char *DBGetFieldA(DB_RESULT hResult, int iRow, int iColumn, char *pszBuffer, int nBufLen);
char *DBGetFieldUTF8(DB_RESULT hResult, int iRow, int iColumn, char *pszBuffer, int nBufLen);
INT32 DBGetFieldLong(DB_RESULT hResult, int iRow, int iColumn);
UINT32 DBGetFieldULong(DB_RESULT hResult, int iRow, int iColumn);
INT64 DBGetFieldInt64(DB_RESULT hResult, int iRow, int iColumn);
UINT64 DBGetFieldUInt64(DB_RESULT hResult, int iRow, int iColumn);
double DBGetFieldDouble(DB_RESULT hResult, int iRow, int iColumn);
// UINT32 DBGetFieldIPAddr(DB_RESULT hResult, int iRow, int iColumn);
// InetAddress DBGetFieldInetAddr(DB_RESULT hResult, int iRow, int iColumn);
bool DBGetFieldByteArray(DB_RESULT hResult, int iRow, int iColumn,
    int *pnArray, int nSize, int nDefault);
bool DBGetFieldByteArray2(DB_RESULT hResult, int iRow, int iColumn,
    BYTE *data, int nSize, int nDefault);
//uuid DBGetFieldGUID(DB_RESULT hResult, int iRow, int iColumn);

DB_UNBUFFERED_RESULT DBSelectUnbuffered(DB_HANDLE hConn, TCHAR *szQuery);
DB_UNBUFFERED_RESULT DBSelectUnbufferedEx(DB_HANDLE hConn, TCHAR *szQuery, TCHAR *errorText);
bool DBFetch(DB_UNBUFFERED_RESULT hResult);
int DBGetColumnCount(DB_UNBUFFERED_RESULT hResult);
bool DBGetColumnName(DB_UNBUFFERED_RESULT hResult, int column, TCHAR *buffer, int bufSize);
void DBFreeResult(DB_UNBUFFERED_RESULT hResult);

TCHAR *DBGetField(DB_UNBUFFERED_RESULT hResult, int iColumn, TCHAR *pBuffer, int iBufSize);
INT32 DBGetFieldLong(DB_UNBUFFERED_RESULT hResult, int iColumn);
UINT32 DBGetFieldULong(DB_UNBUFFERED_RESULT hResult, int iColumn);
INT64 DBGetFieldInt64(DB_UNBUFFERED_RESULT hResult, int iColumn);
UINT64 DBGetFieldUInt64(DB_UNBUFFERED_RESULT hResult, int iColumn);
double DBGetFieldDouble(DB_UNBUFFERED_RESULT hResult, int iColumn);
// UINT32 DBGetFieldIPAddr(DB_UNBUFFERED_RESULT hResult, int iColumn);
// InetAddress DBGetFieldInetAddr(DB_UNBUFFERED_RESULT hResult, int iColumn);
//uuid DBGetFieldGUID(DB_UNBUFFERED_RESULT hResult, int iColumn);

bool DBBegin(DB_HANDLE hConn);
bool DBCommit(DB_HANDLE hConn);
bool DBRollback(DB_HANDLE hConn);

int DBIsTableExist(DB_HANDLE conn, TCHAR *table);

// int DBGetSchemaVersion(DB_HANDLE conn);
// int DBGetSyntax(DB_HANDLE conn);

// String DBPrepareString(DB_HANDLE conn, const TCHAR *str, int maxSize = -1);
// String DBPrepareString(const TCHAR *str, int maxSize = -1);
// #ifdef UNICODE
// String DBPrepareStringA(DB_HANDLE conn, const char *str, int maxSize = -1);
// String DBPrepareStringA(const char *str, int maxSize = -1);
// #else
// #define DBPrepareStringA DBPrepareString
// #endif
WCHAR* DBPrepareStringW(WCHAR *str, int maxSize = -1);
CHAR* DBPrepareStringA(CHAR *str, int maxSize = -1);
#ifdef UNICODE
#define DBPrepareString DBPrepareStringW
#else 
#define DBPrepareString DBPrepareStringA
#endif

// TCHAR *EncodeSQLString(const TCHAR *pszIn);
// void DecodeSQLString(TCHAR *pszStr);

// bool DBConnectionPoolStartup(DB_DRIVER driver, const TCHAR *server, const TCHAR *dbName,
//     const TCHAR *login, const TCHAR *password, const TCHAR *schema,
//     int basePoolSize, int maxPoolSize, int cooldownTime,
//     int connTTL);
// void DBConnectionPoolShutdown();
// DB_HANDLE __DBConnectionPoolAcquireConnection(const char *srcFile, int srcLine);
// #define DBConnectionPoolAcquireConnection() __DBConnectionPoolAcquireConnection(__FILE__, __LINE__)
// void DBConnectionPoolReleaseConnection(DB_HANDLE connection);
// int DBConnectionPoolGetSize();
// int DBConnectionPoolGetAcquiredCount();
// 
// void DBSetLongRunningThreshold(UINT32 threshold);
// ObjectArray<PoolConnectionInfo> *DBConnectionPoolGetConnectionList();

/**
 *	数据库连接池操作
 */
bool DBConnectionPoolPopulate();
void DBConnectionPoolShrink();
bool ResetConnection(PoolConnectionInfo *conn);
void ResetExpiredConnections();
void DBConnectionPoolShutdown();
DB_HANDLE __DBConnectionPoolAcquireConnection(const char *srcFile, int srcLine);
#define DBConnectionPoolAcquireConnection()   __DBConnectionPoolAcquireConnection(__FILE__, __LINE__)
void DBConnectionPoolReleaseConnection(DB_HANDLE handle);
int DBConnectionPoolGetSize();
int DBConnectionPoolGetAcquiredCount();
vector<PoolConnectionInfo *> *DBConnectionPoolGetConnectionList();

void DBGetPerfCounters(LIBNXDB_PERF_COUNTERS *counters);

bool IsDatabaseRecordExist(DB_HANDLE hdb, TCHAR *table, TCHAR *idColumn, UINT32 id);
//bool IsDatabaseRecordExist(DB_HANDLE hdb, TCHAR *table, TCHAR *idColumn, uuid& id);
bool IsDatabaseRecordExist(DB_HANDLE hdb, TCHAR *table, TCHAR *idColumn, TCHAR *id);


/**
 *	业务层处理
 */
bool user_registration_info(TCHAR* inUserPasswd, TCHAR* inNickName, int64 inBirthday, TCHAR* inSex, TCHAR* inICON, TCHAR* inProfile, int64 inQQ, TCHAR* inEmail, int64 inPhone, TCHAR* inWallpaper, int32* outID, int64* outPP);

P2_NAMESPACE_END