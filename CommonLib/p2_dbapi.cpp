/*
** NetXMS - Network Management System
** Copyright (C) 2003-2014 Victor Kirhenshtein
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
** File: drivers.cpp
**
**/

#include "p2_dbapi.h"
P2_NAMESPACE_BEG

/**
* Check if statement handle is valid
*/
#define IS_VALID_STATEMENT_HANDLE(s) ((s != NULL) && (s->m_connection != NULL))

/**
* Log message codes
*/
UINT32 g_logMsgCode = 0;
UINT32 g_sqlErrorMsgCode = 0;

/**
* Long-running query threshold
*/
UINT32 g_sqlQueryExecTimeThreshold = 0xFFFFFFFF;

/**
* Options
*/
static bool s_writeLog = false;
static bool s_logSqlErrors = false;

/**
* Performance counters
*/
static UINT64 s_perfSelectQueries = 0;
static UINT64 s_perfNonSelectQueries = 0;
static UINT64 s_perfTotalQueries = 0;
static UINT64 s_perfLongRunningQueries = 0;
static UINT64 s_perfFailedQueries = 0;

/**
* Invalidate all prepared statements on connection
*/
static void InvalidatePreparedStatements(DB_HANDLE hConn)
{
    for (int i = 0; i < hConn->m_preparedStatements->size(); i++)
    {
        db_statement_t *stmt = hConn->m_preparedStatements->at(i);
        DrvFreeStatement(stmt->m_statement);
        stmt->m_statement = NULL;
        stmt->m_connection = NULL;
    }
    hConn->m_preparedStatements->clear();
}

bool DBInit(DWORD logMsgCode, DWORD sqlErrorMsgCode)
{
    g_logMsgCode = logMsgCode;
    s_writeLog = (logMsgCode > 0);
    g_sqlErrorMsgCode = sqlErrorMsgCode;
    s_logSqlErrors = (sqlErrorMsgCode > 0) && s_writeLog;

    return true;
}

// DB_DRIVER DBLoadDriver(bool dumpSQL)
// {
//     TCHAR szErrorText[256];
//     DB_DRIVER driver;
// 
//     driver = (DB_DRIVER)malloc(sizeof(db_driver_t));
//     memset(driver, 0, sizeof(db_driver_t));
// 
//     driver->m_logSqlErrors = s_logSqlErrors;
//     driver->m_dumpSql = dumpSQL;
//     driver->m_refCount = 1;
//     driver->m_defaultPrefetchLimit = 10;
//     if (s_writeLog)
//         __DBWriteLog(EVENTLOG_INFORMATION_TYPE, _T("Database driver loaded and initialized successfully"));
//     return driver;
// }
// 
// /**
// * Unload driver
// */
// void DBUnloadDriver(DB_DRIVER driver)
// {
//     if (driver == NULL)
//         return;
// 
//     DrvUnload();
// }
bool DBLoad()
{
    return DrvInit();
}

void DBUnload()
{
    DrvUnload();
}

DB_HANDLE DBConnect(const TCHAR *server, const TCHAR *dbName, const TCHAR *login, const TCHAR *password, bool dumpSql, TCHAR *errorText)
{
    MYSQL_CONN *hDrvConn;
    DB_HANDLE hConn = NULL;

    log_debug(8, "DBConnect: server=%s db=%s login=%s", server, dbName, login);
#ifdef UNICODE
    char *mbServer = (server == NULL) ? NULL : mb_from_wstr(server);
    char *mbDatabase = (dbName == NULL) ? NULL : mb_from_wstr(dbName);
    char *mbLogin = (login == NULL) ? NULL : mb_from_wstr(login);
    char *mbPassword = (password == NULL) ? NULL : mb_from_wstr(password);
    errorText[0] = 0;
    hDrvConn = DrvConnect(mbServer, mbLogin, mbPassword, mbDatabase, errorText);
#else
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
    hDrvConn = Connect(server, login, password, dbName, wcErrorText);
    WideCharToMultiByte(CP_ACP, WC_DEFAULTCHAR | WC_COMPOSITECHECK, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
    errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif
    if (hDrvConn != NULL)
    {
        hConn = (DB_HANDLE)malloc(sizeof(struct db_handle_t));
        if (hConn != NULL)
        {
            hConn->m_dumpSql = dumpSql;
            hConn->m_reconnectEnabled = true;
            hConn->m_connection = hDrvConn;
            hConn->m_transactionLevel = 0;
            hConn->m_preparedStatements = new vector<db_statement_t*>;
#ifdef UNICODE
            hConn->m_dbName = mbDatabase;
            hConn->m_login = mbLogin;
            hConn->m_password = mbPassword;
            hConn->m_server = mbServer;
#else
            hConn->m_dbName = (dbName == NULL) ? NULL : _tcsdup(dbName);
            hConn->m_login = (login == NULL) ? NULL : _tcsdup(login);
            hConn->m_password = (password == NULL) ? NULL : _tcsdup(password);
            hConn->m_server = (server == NULL) ? NULL : _tcsdup(server);
#endif
            log_debug(4, "New DB connection opened: handle=%p", hConn);

        }
        else
        {
            DrvDisconnect(hDrvConn);
        }
    }
#ifdef UNICODE
    if (hConn == NULL)
    {
        safe_free(mbServer);
        safe_free(mbDatabase);
        safe_free(mbLogin);
        safe_free(mbPassword);
    }
#endif
    return hConn;
}

void DBDisconnect(DB_HANDLE hConn)
{
    if (hConn == NULL)
        return;

    log_debug(4, "DB connection %p closed", hConn);

    InvalidatePreparedStatements(hConn);

    DrvDisconnect(hConn->m_connection);
    safe_free(hConn->m_dbName);
    safe_free(hConn->m_login);
    safe_free(hConn->m_password);
    safe_free(hConn->m_server);
    delete hConn->m_preparedStatements;
    free(hConn);
}

void DBEnableReconnect(DB_HANDLE hConn, bool enabled)
{
    if (hConn != NULL)
    {
        hConn->m_reconnectEnabled = enabled;
    }
}

/**
* Reconnect to database
*/
static void DBReconnect(DB_HANDLE hConn)
{
    int nCount;
    WCHAR errorText[DBDRV_MAX_ERROR_TEXT];

    log_debug(4, "DB reconnect: handle=%p", hConn);

    InvalidatePreparedStatements(hConn);
    DrvDisconnect(hConn->m_connection);
    for (nCount = 0; ; nCount++)
    {
        hConn->m_connection = DrvConnect(hConn->m_server, hConn->m_login,
            hConn->m_password, hConn->m_dbName, errorText);
        if (hConn->m_connection != NULL)
        {
            break;
        }
        Sleep(1000);
    }
}

/**
* Perform a non-SELECT SQL query
*/
bool DBQueryEx(DB_HANDLE hConn, TCHAR *szQuery, TCHAR *errorText)
{
    DWORD dwResult;
#ifdef UNICODE
#define pwszQuery szQuery
#define wcErrorText errorText
#else
    WCHAR *pwszQuery = WideStringFromMBString(szQuery);
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
#endif

    MutexLocker locker(&hConn->m_mutexTransLock);
    INT64 ms = GetCurrentTimeMilliS();

    dwResult = DrvQuery(hConn->m_connection, pwszQuery, wcErrorText);
    if ((dwResult == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
    {
        DBReconnect(hConn);
        dwResult = DrvQuery(hConn->m_connection, pwszQuery, wcErrorText);
    }

    s_perfNonSelectQueries++;
    s_perfTotalQueries++;

    ms = GetCurrentTimeMilliS() - ms;
    if (hConn->m_dumpSql)
    {
        log_debug(9, "%s sync query: \"%s\" [%d ms]", (dwResult == DBERR_SUCCESS) ? _T("Successful") : _T("Failed"), szQuery, ms);
    }
    if ((dwResult == DBERR_SUCCESS) && ((UINT32)ms > g_sqlQueryExecTimeThreshold))
    {
        log_debug(3, "Long running query: \"%s\" [%d ms]", szQuery, (int)ms);
        s_perfLongRunningQueries++;
    }


#ifndef UNICODE
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
    errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif

//     if (dwResult != DBERR_SUCCESS)
//     {
//         s_perfFailedQueries++;
//         if (hConn->m_driver->m_logSqlErrors)
//             nxlog_write(g_sqlErrorMsgCode, EVENTLOG_ERROR_TYPE, "ss", szQuery, errorText);
//         if (hConn->EventHandler != NULL)
//             hConn->EventHandler(DBEVENT_QUERY_FAILED, pwszQuery, wcErrorText, dwResult == DBERR_CONNECTION_LOST, hConn->m_driver->m_userArg);
//     }

#ifndef UNICODE
    free(pwszQuery);
#endif

    return dwResult == DBERR_SUCCESS;
#undef pwszQuery
#undef wcErrorText
}

bool DBQuery(DB_HANDLE hConn, const TCHAR *query)
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    return DBQueryEx(hConn, query, errorText);
}

/**
* Perform SELECT query
*/
DB_RESULT DBSelectEx(DB_HANDLE hConn, TCHAR *szQuery, TCHAR *errorText)
{
    MYSQL_RESULT *hResult;
    DB_RESULT result = NULL;
    DWORD dwError = DBERR_OTHER_ERROR;
#ifdef UNICODE
#define pwszQuery szQuery
#define wcErrorText errorText
#else
    WCHAR *pwszQuery = WideStringFromMBString(szQuery);
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
#endif

    MutexLocker locker(&hConn->m_mutexTransLock);
    INT64 ms = GetCurrentTimeMilliS();

    s_perfSelectQueries++;
    s_perfTotalQueries++;

    hResult = DrvSelect(hConn->m_connection, pwszQuery, &dwError, wcErrorText);
    if ((hResult == NULL) && (dwError == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
    {
        DBReconnect(hConn);
        hResult = DrvSelect(hConn->m_connection, pwszQuery, &dwError, wcErrorText);
    }

    ms = GetCurrentTimeMilliS() - ms;
    if (hConn->m_dumpSql)
    {
        log_debug(9, "%s sync query: \"%s\" [%d ms]", (hResult != NULL) ? "Successful" : "Failed", szQuery, (int)ms);
    }
    if ((hResult != NULL) && ((UINT32)ms > g_sqlQueryExecTimeThreshold))
    {
        log_debug(3, "Long running query: \"%s\" [%d ms]", szQuery, (int)ms);
        s_perfLongRunningQueries++;
    }

#ifndef UNICODE
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
    errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif

#ifndef UNICODE
    free(pwszQuery);
#endif

    if (hResult != NULL)
    {
        result = (DB_RESULT)malloc(sizeof(db_result_t));
        result->m_connection = hConn;
        result->m_data = hResult;
    }

    return result;
#undef pwszQuery
#undef wcErrorText
}

DB_RESULT DBSelect(DB_HANDLE hConn, const TCHAR *query)
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];

    return DBSelectEx(hConn, query, errorText);
}

/**
* Get number of columns
*/
int DBGetColumnCount(DB_RESULT hResult)
{
    return DrvGetColumnCount(hResult->m_data);
}

/**
* Get column name
*/
bool DBGetColumnName(DB_RESULT hResult, int column, TCHAR *buffer, int bufSize)
{
    const char *name;

    name = DrvGetColumnName(hResult->m_data, column);
    if (name != NULL)
    {
#ifdef UNICODE
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, -1, buffer, bufSize);
        buffer[bufSize - 1] = 0;
#else
        nx_strncpy(buffer, name, bufSize);
#endif
    }
    return name != NULL;
}

/**
* Get column count for unbuffered result set
*/
int DBGetColumnCount(DB_UNBUFFERED_RESULT hResult)
{
    return DrvGetColumnCountUnbuffered(hResult->m_data);
}

/**
* Get column name for unbuffered result set
*/
bool DBGetColumnName(DB_UNBUFFERED_RESULT hResult, int column, TCHAR *buffer, int bufSize)
{
    const char *name;

    name = DrvGetColumnNameUnbuffered(hResult->m_data, column);
    if (name != NULL)
    {
#ifdef UNICODE
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, -1, buffer, bufSize);
        buffer[bufSize - 1] = 0;
#else
        nx_strncpy(buffer, name, bufSize);
#endif
    }
    return name != NULL;
}

/**
* Get field's value. If buffer is NULL, dynamically allocated string will be returned.
* Caller is responsible for destroying it by calling free().
*/
TCHAR *DBGetField(DB_RESULT hResult, int iRow, int iColumn, TCHAR *pszBuffer, int nBufLen)
{
#ifdef UNICODE
    if (pszBuffer != NULL)
    {
        *pszBuffer = 0;
        return DrvGetField(hResult->m_data, iRow, iColumn, pszBuffer, nBufLen);
    }
    else
    {
        WCHAR *pszTemp;
        LONG nLen = DrvGetFieldLength(hResult->m_data, iRow, iColumn);
        if (nLen == -1)
        {
            pszTemp = NULL;
        }
        else
        {
            nLen++;
            pszTemp = (WCHAR *)malloc(nLen * sizeof(WCHAR));
            DrvGetField(hResult->m_data, iRow, iColumn, pszTemp, nLen);
        }
        return pszTemp;
    }
#else
    return DBGetFieldA(hResult, iRow, iColumn, pszBuffer, nBufLen);
#endif
}

/**
* Get field's value as UTF8 string. If buffer is NULL, dynamically allocated string will be returned.
* Caller is responsible for destroying it by calling free().
*/
char *DBGetFieldUTF8(DB_RESULT hResult, int iRow, int iColumn, char *pszBuffer, int nBufLen)
{
//     if (DrvGetFieldUTF8 != NULL)
//     {
        if (pszBuffer != NULL)
        {
            *pszBuffer = 0;
            return DrvGetFieldUTF8(hResult->m_data, iRow, iColumn, pszBuffer, nBufLen);
        }
        else
        {
            char *pszTemp;
            LONG nLen = DrvGetFieldLength(hResult->m_data, iRow, iColumn);
            if (nLen == -1)
            {
                pszTemp = NULL;
            }
            else
            {
                nLen = nLen * 2 + 1;  // increase buffer size because driver may return field length in characters
                pszTemp = (char *)malloc(nLen);
                DrvGetFieldUTF8(hResult->m_data, iRow, iColumn, pszTemp, nLen);
            }
            return pszTemp;
        }
//     }
//     else
//     {
//         LONG nLen = DrvGetFieldLength(hResult->m_data, iRow, iColumn);
//         if (nLen == -1)
//             return NULL;
//         nLen = nLen * 2 + 1;  // increase buffer size because driver may return field length in characters
// 
//         WCHAR *wtemp = (WCHAR *)malloc(nLen * sizeof(WCHAR));
//         DrvGetField(hResult->m_data, iRow, iColumn, wtemp, nLen);
//         char *value = (pszBuffer != NULL) ? pszBuffer : (char *)malloc(nLen);
//         WideCharToMultiByte(CP_UTF8, 0, wtemp, -1, value, (pszBuffer != NULL) ? nBufLen : nLen, NULL, NULL);
//         free(wtemp);
//         return value;
//     }
}

/**
* Get field's value as multibyte string. If buffer is NULL, dynamically allocated string will be returned.
* Caller is responsible for destroying it by calling free().
*/
char *DBGetFieldA(DB_RESULT hResult, int iRow, int iColumn, char *pszBuffer, int nBufLen)
{
    WCHAR *pwszData, *pwszBuffer;
    char *pszRet;
    int nLen;

    if (pszBuffer != NULL)
    {
        *pszBuffer = 0;
        pwszBuffer = (WCHAR *)malloc(nBufLen * sizeof(WCHAR));
        pwszData = DrvGetField(hResult->m_data, iRow, iColumn, pwszBuffer, nBufLen);
        if (pwszData != NULL)
        {
            WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, pwszData, -1, pszBuffer, nBufLen, NULL, NULL);
            pszRet = pszBuffer;
        }
        else
        {
            pszRet = NULL;
        }
        free(pwszBuffer);
    }
    else
    {
        nLen = DrvGetFieldLength(hResult->m_data, iRow, iColumn);
        if (nLen == -1)
        {
            pszRet = NULL;
        }
        else
        {
            nLen++;
            pwszBuffer = (WCHAR *)malloc(nLen * sizeof(WCHAR));
            pwszData = DrvGetField(hResult->m_data, iRow, iColumn, pwszBuffer, nLen);
            if (pwszData != NULL)
            {
                nLen = (int)wcslen(pwszData) + 1;
                pszRet = (char *)malloc(nLen);
                WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, pwszData, -1, pszRet, nLen, NULL, NULL);
            }
            else
            {
                pszRet = NULL;
            }
            free(pwszBuffer);
        }
    }
    return pszRet;
}

/**
* Get field's value as unsigned long
*/
UINT32 DBGetFieldULong(DB_RESULT hResult, int iRow, int iColumn)
{
    INT32 iVal;
    UINT32 dwVal;
    TCHAR *pszVal, szBuffer[256];

    pszVal = DBGetField(hResult, iRow, iColumn, szBuffer, 256);
    if (pszVal == NULL)
        return 0;
    str_strip(pszVal);
    if (*pszVal == _T('-'))
    {
        iVal = _tcstol(pszVal, NULL, 10);
        memcpy(&dwVal, &iVal, sizeof(INT32));   // To prevent possible conversion
    }
    else
    {
        dwVal = _tcstoul(pszVal, NULL, 10);
    }
    return dwVal;
}

/**
* Get field's value as unsigned 64-bit int
*/
UINT64 DBGetFieldUInt64(DB_RESULT hResult, int iRow, int iColumn)
{
    INT64 iVal;
    UINT64 qwVal;
    TCHAR *pszVal, szBuffer[256];

    pszVal = DBGetField(hResult, iRow, iColumn, szBuffer, 256);
    if (pszVal == NULL)
        return 0;
    str_strip(pszVal);
    if (*pszVal == _T('-'))
    {
        iVal = _tcstoll(pszVal, NULL, 10);
        memcpy(&qwVal, &iVal, sizeof(INT64));   // To prevent possible conversion
    }
    else
    {
        qwVal = _tcstoull(pszVal, NULL, 10);
    }
    return qwVal;
}

/**
* Get field's value as signed long
*/
INT32 DBGetFieldLong(DB_RESULT hResult, int iRow, int iColumn)
{
    TCHAR *pszVal, szBuffer[256];

    pszVal = DBGetField(hResult, iRow, iColumn, szBuffer, 256);
    return pszVal == NULL ? 0 : _tcstol(pszVal, NULL, 10);
}

/**
* Get field's value as signed 64-bit int
*/
INT64 DBGetFieldInt64(DB_RESULT hResult, int iRow, int iColumn)
{
    TCHAR *pszVal, szBuffer[256];

    pszVal = DBGetField(hResult, iRow, iColumn, szBuffer, 256);
    return pszVal == NULL ? 0 : _tcstoll(pszVal, NULL, 10);
}

/**
* Get field's value as double
*/
double DBGetFieldDouble(DB_RESULT hResult, int iRow, int iColumn)
{
    TCHAR *pszVal, szBuffer[256];

    pszVal = DBGetField(hResult, iRow, iColumn, szBuffer, 256);
    return pszVal == NULL ? 0 : _tcstod(pszVal, NULL);
}

/**
* Get field's value as integer array from byte array encoded in hex
*/
bool DBGetFieldByteArray(DB_RESULT hResult, int iRow, int iColumn,
    int *pnArray, int nSize, int nDefault)
{
    char pbBytes[128];
    bool bResult;
    int i, nLen;
    TCHAR *pszVal, szBuffer[256];

    pszVal = DBGetField(hResult, iRow, iColumn, szBuffer, 256);
    if (pszVal != NULL)
    {
        str_to_bin(pszVal, (BYTE *)pbBytes, 128);
        nLen = (int)_tcslen(pszVal) / 2;
        for (i = 0; (i < nSize) && (i < nLen); i++)
            pnArray[i] = pbBytes[i];
        for (; i < nSize; i++)
            pnArray[i] = nDefault;
        bResult = true;
    }
    else
    {
        for (i = 0; i < nSize; i++)
            pnArray[i] = nDefault;
        bResult = false;
    }
    return bResult;
}

bool DBGetFieldByteArray2(DB_RESULT hResult, int iRow, int iColumn,
    BYTE *data, int nSize, int nDefault)
{
    bool bResult;
    TCHAR *pszVal, szBuffer[256];

    pszVal = DBGetField(hResult, iRow, iColumn, szBuffer, 256);
    if (pszVal != NULL)
    {
        int bytes = (int)str_to_bin(pszVal, data, nSize);
        if (bytes < nSize)
            memset(&data[bytes], 0, nSize - bytes);
        bResult = true;
    }
    else
    {
        memset(data, nDefault, nSize);
        bResult = false;
    }
    return bResult;
}

/**
* Get number of rows in result
*/
int DBGetNumRows(DB_RESULT hResult)
{
    if (hResult == NULL)
        return 0;
    return DrvGetNumRows(hResult->m_data);
}

/**
* Free result
*/
void DBFreeResult(DB_RESULT hResult)
{
    if (hResult != NULL)
    {
        DrvFreeResult(hResult->m_data);
        free(hResult);
    }
}

/**
* Unbuffered SELECT query
*/
DB_UNBUFFERED_RESULT DBSelectUnbufferedEx(DB_HANDLE hConn, TCHAR *szQuery, TCHAR *errorText)
{
    MYSQL_UNBUFFERED_RESULT *hResult;
    DB_UNBUFFERED_RESULT result = NULL;
    DWORD dwError = DBERR_OTHER_ERROR;
#ifdef UNICODE
#define pwszQuery szQuery
#define wcErrorText errorText
#else
    WCHAR *pwszQuery = wstr_from_mb(szQuery);
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
#endif

    hConn->m_mutexTransLock.Lock();
    INT64 ms = GetCurrentTimeMilliS();

    s_perfSelectQueries++;
    s_perfTotalQueries++;

    hResult = DrvSelectUnbuffered(hConn->m_connection, pwszQuery, &dwError, wcErrorText);
    if ((hResult == NULL) && (dwError == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
    {
        DBReconnect(hConn);
        hResult = DrvSelectUnbuffered(hConn->m_connection, pwszQuery, &dwError, wcErrorText);
    }

    ms = GetCurrentTimeMilliS() - ms;
    if (hConn->m_dumpSql)
    {
        log_debug(9, _T("%s unbuffered query: \"%s\" [%d ms]"), (hResult != NULL) ? _T("Successful") : _T("Failed"), szQuery, (int)ms);
    }
    if ((hResult != NULL) && ((UINT32)ms > g_sqlQueryExecTimeThreshold))
    {
        log_debug(3, _T("Long running query: \"%s\" [%d ms]"), szQuery, (int)ms);
        s_perfLongRunningQueries++;
    }
    if (hResult == NULL)
    {
        s_perfFailedQueries++;
        hConn->m_mutexTransLock.Unlock();

#ifndef UNICODE
        WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
        errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif

    }

#ifndef UNICODE
    free(pwszQuery);
#endif

    if (hResult != NULL)
    {
        result = (DB_UNBUFFERED_RESULT)malloc(sizeof(db_unbuffered_result_t));
        result->m_connection = hConn;
        result->m_data = hResult;
    }

    return result;
#undef pwszQuery
#undef wcErrorText
}

DB_UNBUFFERED_RESULT DBSelectUnbuffered(DB_HANDLE hConn, const TCHAR *query)
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];

    return DBSelectUnbufferedEx(hConn, query, errorText);
}

/**
* Fetch next row from unbuffered SELECT result
*/
bool DBFetch(DB_UNBUFFERED_RESULT hResult)
{
    return DrvFetch(hResult->m_data);
}

/**
* Get field's value from unbuffered SELECT result
*/
TCHAR *DBGetField(DB_UNBUFFERED_RESULT hResult, int iColumn, TCHAR *pBuffer, int iBufSize)
{
#ifdef UNICODE
    if (pBuffer != NULL)
    {
        return DrvGetFieldUnbuffered(hResult->m_data, iColumn, pBuffer, iBufSize);
    }
    else
    {
        INT32 nLen;
        WCHAR *pszTemp;

        nLen = DrvGetFieldLengthUnbuffered(hResult->m_data, iColumn);
        if (nLen == -1)
        {
            pszTemp = NULL;
        }
        else
        {
            nLen++;
            pszTemp = (WCHAR *)malloc(nLen * sizeof(WCHAR));
            DrvGetFieldUnbuffered(hResult->m_data, iColumn, pszTemp, nLen);
        }
        return pszTemp;
    }
#else
    WCHAR *pwszData, *pwszBuffer;
    char *pszRet;
    int nLen;

    if (pBuffer != NULL)
    {
        pwszBuffer = (WCHAR *)malloc(iBufSize * sizeof(WCHAR));
        if (DrvGetFieldUnbuffered(hResult->m_data, iColumn, pwszBuffer, iBufSize) != NULL)
        {
            WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
                pwszBuffer, -1, pBuffer, iBufSize, NULL, NULL);
            pszRet = pBuffer;
        }
        else
        {
            pszRet = NULL;
        }
        free(pwszBuffer);
    }
    else
    {
        nLen = DrvGetFieldLengthUnbuffered(hResult->m_data, iColumn);
        if (nLen == -1)
        {
            pszRet = NULL;
        }
        else
        {
            nLen++;
            pwszBuffer = (WCHAR *)malloc(nLen * sizeof(WCHAR));
            pwszData = DrvGetFieldUnbuffered(hResult->m_data, iColumn, pwszBuffer, nLen);
            if (pwszData != NULL)
            {
                nLen = (int)wcslen(pwszData) + 1;
                pszRet = (char *)malloc(nLen);
                WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
                    pwszData, -1, pszRet, nLen, NULL, NULL);
            }
            else
            {
                pszRet = NULL;
            }
            free(pwszBuffer);
        }
    }
    return pszRet;
#endif
}

/**
* Get field's value as unsigned long from unbuffered SELECT result
*/
UINT32 DBGetFieldULong(DB_UNBUFFERED_RESULT hResult, int iColumn)
{
    INT32 iVal;
    UINT32 dwVal;
    TCHAR szBuffer[64];

    if (DBGetField(hResult, iColumn, szBuffer, 64) == NULL)
        return 0;
    str_strip(szBuffer);
    if (szBuffer[0] == _T('-'))
    {
        iVal = _tcstol(szBuffer, NULL, 10);
        memcpy(&dwVal, &iVal, sizeof(INT32));   // To prevent possible conversion
    }
    else
    {
        dwVal = _tcstoul(szBuffer, NULL, 10);
    }
    return dwVal;
}

/**
* Get field's value as unsigned 64-bit int from unbuffered SELECT result
*/
UINT64 DBGetFieldUInt64(DB_UNBUFFERED_RESULT hResult, int iColumn)
{
    INT64 iVal;
    UINT64 qwVal;
    TCHAR szBuffer[64];

    if (DBGetField(hResult, iColumn, szBuffer, 64) == NULL)
        return 0;
    str_strip(szBuffer);
    if (szBuffer[0] == _T('-'))
    {
        iVal = _tcstoll(szBuffer, NULL, 10);
        memcpy(&qwVal, &iVal, sizeof(INT64));   // To prevent possible conversion
    }
    else
    {
        qwVal = _tcstoull(szBuffer, NULL, 10);
    }
    return qwVal;
}

/**
* Get field's value as signed long from unbuffered SELECT result
*/
INT32 DBGetFieldLong(DB_UNBUFFERED_RESULT hResult, int iColumn)
{
    TCHAR szBuffer[64];
    return DBGetField(hResult, iColumn, szBuffer, 64) == NULL ? 0 : _tcstol(szBuffer, NULL, 10);
}

/**
* Get field's value as signed 64-bit int from unbuffered SELECT result
*/
INT64 DBGetFieldInt64(DB_UNBUFFERED_RESULT hResult, int iColumn)
{
    TCHAR szBuffer[64];
    return DBGetField(hResult, iColumn, szBuffer, 64) == NULL ? 0 : _tcstoll(szBuffer, NULL, 10);
}

/**
* Get field's value as signed long from unbuffered SELECT result
*/
double DBGetFieldDouble(DB_UNBUFFERED_RESULT hResult, int iColumn)
{
    TCHAR szBuffer[64];
    return DBGetField(hResult, iColumn, szBuffer, 64) == NULL ? 0 : _tcstod(szBuffer, NULL);
}

/**
* Free unbuffered SELECT result
*/
void DBFreeResult(DB_UNBUFFERED_RESULT hResult)
{
    DrvFreeUnbufferedResult(hResult->m_data);
    hResult->m_connection->m_mutexTransLock.Unlock();
    free(hResult);
}

/**
* Prepare statement
*/
DB_STATEMENT DBPrepareEx(DB_HANDLE hConn, TCHAR *query, TCHAR *errorText)
{
    DB_STATEMENT result = NULL;
    INT64 ms;

#ifdef UNICODE
#define pwszQuery query
#define wcErrorText errorText
#else
    WCHAR *pwszQuery = wstr_from_mb(query);
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
#endif

    hConn->m_mutexTransLock.Lock()

    if (hConn->m_dumpSql)
        ms = GetCurrentTimeMilliS();

    DWORD errorCode;
    MYSQL_STATEMENT *stmt = DrvPrepare(hConn->m_connection, pwszQuery, &errorCode, wcErrorText);
    if ((stmt == NULL) && (errorCode == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
    {
        DBReconnect(hConn);
        stmt = DrvPrepare(hConn->m_connection, pwszQuery, &errorCode, wcErrorText);
    }
    hConn->m_mutexTransLock.Unlock();

    if (stmt != NULL)
    {
        result = (DB_STATEMENT)malloc(sizeof(db_statement_t));
        result->m_connection = hConn;
        result->m_statement = stmt;
        result->m_query = _tcsdup(query);
    }
    else
    {
#ifndef UNICODE
        WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
        errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif

        s_perfFailedQueries++;
        s_perfTotalQueries++;
    }

    if (hConn->m_dumpSql)
    {
        ms = GetCurrentTimeMilliS() - ms;
        log_debug(9, _T("{%p} %s prepare: \"%s\" [%d ms]"), result, (result != NULL) ? _T("Successful") : _T("Failed"), query, ms);
    }

#ifndef UNICODE
    free(pwszQuery);
#endif

    if (result != NULL)
    {
        hConn->m_preparedStatements->push_back(result);
    }

    return result;
#undef pwszQuery
#undef wcErrorText
}

/**
* Prepare statement
*/
DB_STATEMENT DBPrepare(DB_HANDLE hConn, const TCHAR *query)
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    return DBPrepareEx(hConn, query, errorText);
}

/**
* Destroy prepared statement
*/
void DBFreeStatement(DB_STATEMENT hStmt)
{
    if (hStmt == NULL)
        return;

    if (hStmt->m_connection != NULL)
    {
        std::remove(hStmt->m_connection->m_preparedStatements->begin(),
            hStmt->m_connection->m_preparedStatements->end(),
            hStmt);
    }
    DrvFreeStatement(hStmt->m_statement);
    safe_free(hStmt->m_query);
    free(hStmt);
}

/**
* Get source query for prepared statement
*/
const TCHAR *DBGetStatementSource(DB_STATEMENT hStmt)
{
    return hStmt->m_query;
}


/**
* Bind parameter (generic)
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, int cType, void *buffer, int allocType)
{
    if ((pos <= 0) || !IS_VALID_STATEMENT_HANDLE(hStmt))
        return;

    if (hStmt->m_connection->m_dumpSql)
    {
        if (cType == DB_CTYPE_STRING)
        {
            log_debug(9, _T("{%p} bind at pos %d: \"%s\""), hStmt, pos, buffer);
        }
        else
        {
            TCHAR text[64];
            switch (cType)
            {
            case DB_CTYPE_INT32:
                _sntprintf(text, 64, _T("%d"), *((INT32 *)buffer));
                break;
            case DB_CTYPE_UINT32:
                _sntprintf(text, 64, _T("%u"), *((UINT32 *)buffer));
                break;
            case DB_CTYPE_INT64:
                _sntprintf(text, 64, _T("%I64d"), *((INT64 *)buffer));
                break;
            case DB_CTYPE_UINT64:
                _sntprintf(text, 64, _T("%I64u"), *((UINT64 *)buffer));
                break;
            case DB_CTYPE_DOUBLE:
                _sntprintf(text, 64, _T("%f"), *((double *)buffer));
                break;
            }
            log_debug(9, _T("{%p} bind at pos %d: \"%s\""), hStmt, pos, text);
        }
    }

#ifdef UNICODE
#define wBuffer buffer
#define realAllocType allocType
#else
    void *wBuffer;
    int realAllocType = allocType;
    if (cType == DB_CTYPE_STRING)
    {
        wBuffer = (void *)wstr_from_mb((char *)buffer);
        if (allocType == DB_BIND_DYNAMIC)
            free(buffer);
        realAllocType = DB_BIND_DYNAMIC;
    }
    else
    {
        wBuffer = buffer;
    }
#endif
    DrvBind(hStmt->m_statement, pos, sqlType, cType, wBuffer, realAllocType);
#undef wBuffer
#undef realAllocType
}

/**
* Bind string parameter
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, const TCHAR *value, int allocType)
{
    if (value != NULL)
        DBBind(hStmt, pos, sqlType, DB_CTYPE_STRING, (void *)value, allocType);
    else
        DBBind(hStmt, pos, sqlType, DB_CTYPE_STRING, (void *)_T(""), DB_BIND_STATIC);
}

/**
* Bind string parameter with length validation
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, const TCHAR *value, int allocType, int maxLen)
{
    if (value != NULL)
    {
        if ((int)_tcslen(value) <= maxLen)
        {
            DBBind(hStmt, pos, sqlType, DB_CTYPE_STRING, (void *)value, allocType);
        }
        else
        {
            if (allocType == DB_BIND_DYNAMIC)
            {
                ((TCHAR *)value)[maxLen] = 0;
                DBBind(hStmt, pos, sqlType, DB_CTYPE_STRING, (void *)value, DB_BIND_DYNAMIC);
            }
            else
            {
                TCHAR *temp = (TCHAR *)memdup(value, sizeof(TCHAR) * (maxLen + 1));
                temp[maxLen] = 0;
                DBBind(hStmt, pos, sqlType, DB_CTYPE_STRING, temp, DB_BIND_DYNAMIC);
            }
        }
    }
    else
    {
        DBBind(hStmt, pos, sqlType, DB_CTYPE_STRING, (void *)_T(""), DB_BIND_STATIC);
    }
}

/**
* Bind 32 bit integer parameter
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, INT32 value)
{
    DBBind(hStmt, pos, sqlType, DB_CTYPE_INT32, &value, DB_BIND_TRANSIENT);
}

/**
* Bind 32 bit unsigned integer parameter
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, UINT32 value)
{
    // C type intentionally set to INT32 - unsigned numbers will be written as negatives
    // and correctly parsed on read by DBGetFieldULong
    // Setting it to UINT32 may cause INSERT/UPDATE failures on some databases
    DBBind(hStmt, pos, sqlType, DB_CTYPE_INT32, &value, DB_BIND_TRANSIENT);
}

/**
* Bind 64 bit integer parameter
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, INT64 value)
{
    DBBind(hStmt, pos, sqlType, DB_CTYPE_INT64, &value, DB_BIND_TRANSIENT);
}

/**
* Bind 64 bit unsigned integer parameter
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, UINT64 value)
{
    DBBind(hStmt, pos, sqlType, DB_CTYPE_UINT64, &value, DB_BIND_TRANSIENT);
}

/**
* Bind floating point parameter
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, double value)
{
    DBBind(hStmt, pos, sqlType, DB_CTYPE_DOUBLE, &value, DB_BIND_TRANSIENT);
}

/**
* Bind UUID parameter
*/
void DBBind(DB_STATEMENT hStmt, int pos, int sqlType, const uuid& value)
{
    TCHAR buffer[64];
    DBBind(hStmt, pos, sqlType, DB_CTYPE_STRING, value.toString(buffer), DB_BIND_TRANSIENT);
}

/**
* Execute prepared statement (non-SELECT)
*/
bool DBExecuteEx(DB_STATEMENT hStmt, TCHAR *errorText)
{
    if (!IS_VALID_STATEMENT_HANDLE(hStmt))
    {
        _tcscpy(errorText, _T("Invalid statement handle"));
        return false;
    }

#ifdef UNICODE
#define wcErrorText errorText
#else
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
#endif

    DB_HANDLE hConn = hStmt->m_connection;
    MutexLocker locker(&hConn->m_mutexTransLock);
    UINT64 ms = GetCurrentTimeMilliS();

    s_perfNonSelectQueries++;
    s_perfTotalQueries++;

    DWORD dwResult = hConn->DrvExecute(hConn->m_connection, hStmt->m_statement, wcErrorText);
    ms = GetCurrentTimeMilliS() - ms;
    if (hConn->m_dumpSql)
    {
        log_debug(9, _T("%s prepared sync query: \"%s\" [%d ms]"), (dwResult == DBERR_SUCCESS) ? _T("Successful") : _T("Failed"), hStmt->m_query, (int)ms);
    }
    if ((dwResult == DBERR_SUCCESS) && ((UINT32)ms > g_sqlQueryExecTimeThreshold))
    {
        log_debug(3, _T("Long running query: \"%s\" [%d ms]"), hStmt->m_query, (int)ms);
        s_perfLongRunningQueries++;
    }

    // Do reconnect if needed, but don't retry statement execution
    // because it will fail anyway
    if ((dwResult == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
    {
        DBReconnect(hConn);
    }

#ifndef UNICODE
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
    errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif
    if (dwResult != DBERR_SUCCESS)
    {
        s_perfFailedQueries++;
    }

    return dwResult == DBERR_SUCCESS;
#undef wcErrorText
}

/**
* Execute prepared statement (non-SELECT)
*/
bool DBExecute(DB_STATEMENT hStmt)
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    return DBExecuteEx(hStmt, errorText);
}

/**
* Execute prepared SELECT statement
*/
DB_RESULT DBSelectPreparedEx(DB_STATEMENT hStmt, TCHAR *errorText)
{
    if (!IS_VALID_STATEMENT_HANDLE(hStmt))
    {
        _tcscpy(errorText, _T("Invalid statement handle"));
        return NULL;
    }

    DB_RESULT result = NULL;
#ifdef UNICODE
#define wcErrorText errorText
#else
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
#endif

    DB_HANDLE hConn = hStmt->m_connection;
    MutexLocker locker(&hConn->m_mutexTransLock);

    s_perfSelectQueries++;
    s_perfTotalQueries++;

    INT64 ms = GetCurrentTimeMilliS();
    DWORD dwError = DBERR_OTHER_ERROR;
    MYSQL_RESULT *hResult = DrvSelectPrepared(hConn->m_connection, hStmt->m_statement, &dwError, wcErrorText);

    ms = GetCurrentTimeMilliS() - ms;
    if (hConn->m_dumpSql)
    {
        log_debug(9, _T("%s prepared sync query: \"%s\" [%d ms]"),
            (hResult != NULL) ? _T("Successful") : _T("Failed"), hStmt->m_query, (int)ms);
    }
    if ((hResult != NULL) && ((UINT32)ms > g_sqlQueryExecTimeThreshold))
    {
        log_debug(3, _T("Long running query: \"%s\" [%d ms]"), hStmt->m_query, (int)ms);
        s_perfLongRunningQueries++;
    }

    // Do reconnect if needed, but don't retry statement execution
    // because it will fail anyway
    if ((hResult == NULL) && (dwError == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
    {
        DBReconnect(hConn);
    }

#ifndef UNICODE
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
    errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif

    if (hResult == NULL)
    {
        s_perfFailedQueries++;
    }

    if (hResult != NULL)
    {
        result = (DB_RESULT)malloc(sizeof(db_result_t));
        result->m_connection = hConn;
        result->m_data = hResult;
    }

    return result;
#undef wcErrorText
}

/**
* Execute prepared SELECT statement
*/
DB_RESULT DBSelectPrepared(DB_STATEMENT hStmt)
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    return DBSelectPreparedEx(hStmt, errorText);
}

/**
* Execute prepared SELECT statement without caching results
*/
DB_UNBUFFERED_RESULT DBSelectPreparedUnbufferedEx(DB_STATEMENT hStmt, TCHAR *errorText)
{
    if (!IS_VALID_STATEMENT_HANDLE(hStmt))
    {
        _tcscpy(errorText, _T("Invalid statement handle"));
        return NULL;
    }

    DB_UNBUFFERED_RESULT result = NULL;
#ifdef UNICODE
#define wcErrorText errorText
#else
    WCHAR wcErrorText[DBDRV_MAX_ERROR_TEXT] = L"";
#endif

    DB_HANDLE hConn = hStmt->m_connection;
    hConn->m_mutexTransLock.Lock();

    s_perfSelectQueries++;
    s_perfTotalQueries++;

    INT64 ms = GetCurrentTimeMilliS();
    DWORD dwError = DBERR_OTHER_ERROR;
    MYSQL_UNBUFFERED_RESULT *hResult = hConn->DrvSelectPreparedUnbuffered(hConn->m_connection, hStmt->m_statement, &dwError, wcErrorText);

    ms = GetCurrentTimeMilliS() - ms;
    if (hConn->m_dumpSql)
    {
        log_debug(9, _T("%s prepared sync query: \"%s\" [%d ms]"),
            (hResult != NULL) ? _T("Successful") : _T("Failed"), hStmt->m_query, (int)ms);
    }
    if ((hResult != NULL) && ((UINT32)ms > g_sqlQueryExecTimeThreshold))
    {
        log_debug(3, _T("Long running query: \"%s\" [%d ms]"), hStmt->m_query, (int)ms);
        s_perfLongRunningQueries++;
    }

    // Do reconnect if needed, but don't retry statement execution
    // because it will fail anyway
    if ((hResult == NULL) && (dwError == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
    {
        DBReconnect(hConn);
    }

#ifndef UNICODE
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, wcErrorText, -1, errorText, DBDRV_MAX_ERROR_TEXT, NULL, NULL);
    errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
#endif

    if (hResult == NULL)
    {
        hConn->m_mutexTransLock.Unlock();
        s_perfFailedQueries++;
    }

    if (hResult != NULL)
    {
        result = (DB_UNBUFFERED_RESULT)malloc(sizeof(db_unbuffered_result_t));
        result->m_connection = hConn;
        result->m_data = hResult;
    }

    return result;
#undef wcErrorText
}

/**
* Execute prepared SELECT statement
*/
DB_UNBUFFERED_RESULT DBSelectPreparedUnbuffered(DB_STATEMENT hStmt)
{
    TCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    return DBSelectPreparedUnbufferedEx(hStmt, errorText);
}

/**
* Begin transaction
*/
bool DBBegin(DB_HANDLE hConn)
{
    DWORD dwResult;
    bool bRet = false;

    hConn->m_mutexTransLock.Lock();
    if (hConn->m_transactionLevel == 0)
    {
        dwResult = DrvBegin(hConn->m_connection);
        if ((dwResult == DBERR_CONNECTION_LOST) && hConn->m_reconnectEnabled)
        {
            DBReconnect(hConn);
            dwResult = DrvBegin(hConn->m_connection);
        }
        if (dwResult == DBERR_SUCCESS)
        {
            hConn->m_transactionLevel++;
            bRet = true;
            log_debug(9, _T("BEGIN TRANSACTION successful (level %d)"), hConn->m_transactionLevel);
        }
        else
        {
            hConn->m_mutexTransLock.Unlock();
            log_debug(9, _T("BEGIN TRANSACTION failed"), hConn->m_transactionLevel);
        }
    }
    else
    {
        hConn->m_transactionLevel++;
        bRet = true;
        log_debug(9, _T("BEGIN TRANSACTION successful (level %d)"), hConn->m_transactionLevel);
    }
    return bRet;
}

/**
* Commit transaction
*/
bool DBCommit(DB_HANDLE hConn)
{
    bool bRet = false;

    MutexLocker locker(&hConn->m_mutexTransLock);
    if (hConn->m_transactionLevel > 0)
    {
        hConn->m_transactionLevel--;
        if (hConn->m_transactionLevel == 0)
            bRet = (DrvCommit(hConn->m_connection) == DBERR_SUCCESS);
        else
            bRet = true;
        log_debug(9, _T("COMMIT TRANSACTION %s (level %d)"), bRet ? _T("successful") : _T("failed"), hConn->m_transactionLevel);
    }
    return bRet;
}

/**
* Rollback transaction
*/
bool DBRollback(DB_HANDLE hConn)
{
    bool bRet = false;

    MutexLocker locker(&hConn->m_mutexTransLock);
    if (hConn->m_transactionLevel > 0)
    {
        hConn->m_transactionLevel--;
        if (hConn->m_transactionLevel == 0)
            bRet = (DrvRollback(hConn->m_connection) == DBERR_SUCCESS);
        else
            bRet = true;
        log_debug(9, _T("ROLLBACK TRANSACTION %s (level %d)"), bRet ? _T("successful") : _T("failed"), hConn->m_transactionLevel);
    }
    return bRet;
}

/**
* Prepare string for using in SQL statement
*/
wstring DBPrepareStringW(const WCHAR *str, int maxSize)
{
    wstring out;
    if ((maxSize > 0) && (str != NULL) && (maxSize < (int)wcslen(str)))
    {
        WCHAR *temp = (WCHAR *)malloc((maxSize + 1) * sizeof(WCHAR));
        wcsncpy(temp, str, maxSize + 1);
        out = DrvPrepareStringW(temp);
        free(temp);
    }
    else
    {
        out = DrvPrepareStringW(str);
    }
    return out;
}

/**
* Prepare string for using in SQL statement
*/
string DBPrepareStringA(const CHAR *str, int maxSize)
{
    string out;
    if ((maxSize > 0) && (str != NULL) && (maxSize < (int)strlen(str)))
    {
        CHAR *temp = (CHAR *)malloc((maxSize + 1) * sizeof(CHAR));
        strncpy(temp, str, maxSize + 1);
        out = DrvPrepareStringA(temp);
        free(temp);
    }
    else
    {
        out = DrvPrepareStringA(str);
    }
    return out;
}

/**
* Check if given table exist
*/
int DBIsTableExist(DB_HANDLE conn, const TCHAR *table)
{
#ifdef UNICODE
    return DrvIsTableExist(conn->m_connection, table);
#else
    WCHAR wname[256];
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, table, -1, wname, 256);
    return conn->DrvIsTableExist(conn->m_connection, wname);
#endif
}

/**
* Get performance counters
*/
void DBGetPerfCounters(LIBNXDB_PERF_COUNTERS *counters)
{
    counters->failedQueries = s_perfFailedQueries;
    counters->longRunningQueries = s_perfLongRunningQueries;
    counters->nonSelectQueries = s_perfNonSelectQueries;
    counters->selectQueries = s_perfSelectQueries;
    counters->totalQueries = s_perfTotalQueries;
}

P2_NAMESPACE_END