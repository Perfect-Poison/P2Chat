#include "mysqldrv.h"
/*
** MySQL Database Driver
** Copyright (C) 2003-2015 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: mysqldrv.cpp
**
**/
P2_NAMESPACE_BEG

void RemoveTrailingCRLFW(WCHAR *str)
{
    if (*str == 0)
        return;

    WCHAR *p = str + wcslen(str) - 1;
    if (*p == L'\n')
        p--;
    if (*p == L'\r')
        p--;
    *(p + 1) = 0;
}

void FormatErrorMessage(const char *source, WCHAR *errorText)
{
    if (errorText != NULL)
    {
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, source, -1, errorText, 1024);
        errorText[1023] = 0;
        RemoveTrailingCRLFW(errorText);
    }
}

bool DrvInit()
{
    return mysql_library_init(0, nullptr, nullptr) == 0;
}

void DrvUnload()
{
    mysql_library_end();
}

MYSQL_CONN* DrvConnect(const char *szHost, const char *szLogin, const char *szPassword, const char *szDatabase, WCHAR *errorText)
{
    MYSQL *pMySQL;
    MYSQL_CONN *pConn;
    const char *pHost = szHost;
    
    pMySQL = mysql_init(nullptr);
    if (pMySQL == nullptr)
    {
        wcscpy(errorText, L"Insufficient memory to allocate connection handle");
        return nullptr;
    }

    if (!mysql_real_connect(
        pMySQL,
        pHost,
        szLogin[0] == 0 ? nullptr : szLogin,
        (szPassword[0] == 0 || szLogin[0] == 0) ? nullptr : szPassword,
        szDatabase,
        0,
        nullptr,
        0))
    {
        FormatErrorMessage(mysql_error(pMySQL), errorText);
        mysql_close(pMySQL);
        return nullptr;
    }

    pConn = (MYSQL_CONN *)malloc(sizeof(MYSQL_CONN));
    pConn->pMySQL = pMySQL;

    mysql_set_character_set(pMySQL, "utf8");

    return pConn;
}

void DrvDisconnect(MYSQL_CONN *pConn)
{
    if (pConn != nullptr)
    {
        mysql_close(pConn->pMySQL);
        free(pConn);
    }
}

MYSQL_STATEMENT* DrvPrepare(MYSQL_CONN *pConn, WCHAR *pwszQuery, DWORD *pdwError, WCHAR *errorText)
{
    MYSQL_STATEMENT *result = nullptr;

    MutexLocker locker(&pConn->mutexQueryLock);
    MYSQL_STMT *stmt = mysql_stmt_init(pConn->pMySQL);
    if (stmt != nullptr)
    {
        char *pszQueryUTF8 = utf8_from_wstr(pwszQuery);
        int rc = mysql_stmt_prepare(stmt, pszQueryUTF8, (unsigned long)strlen(pszQueryUTF8));
        if (rc == 0)
        {
            result = (MYSQL_STATEMENT *)malloc(sizeof(MYSQL_STATEMENT));
            result->connection = pConn;
            result->statement = stmt;
            result->paramCount = (int)mysql_stmt_param_count(stmt);
            result->bindings = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * result->paramCount);
            memset(result->bindings, 0, sizeof(MYSQL_BIND) * result->paramCount);
            result->lengthFields = (unsigned long *)malloc(sizeof(unsigned long) * result->paramCount);
            memset(result->lengthFields, 0, sizeof(unsigned long) * result->paramCount);
            result->buffers = new vector<void*>(result->paramCount);
            *pdwError = DBERR_SUCCESS;
        }
        else
        {
            int nErr = mysql_errno(pConn->pMySQL);
            if (nErr == CR_SERVER_LOST || nErr == CR_CONNECTION_ERROR || nErr == CR_SERVER_GONE_ERROR)
            {
                *pdwError = DBERR_CONNECTION_LOST;
            }
            else
            {
                *pdwError = DBERR_OTHER_ERROR;
            }
            FormatErrorMessage(mysql_stmt_error(stmt), errorText);
            mysql_stmt_close(stmt);
        }
        free(pszQueryUTF8);
    }
    else 
    {
        *pdwError = DBERR_OTHER_ERROR;
        FormatErrorMessage("Call to mysql_stmt_init failed", errorText);
    }
    return result;
}

void DrvBind(MYSQL_STATEMENT *hStmt, int pos, int sqlType, int cType, void *buffer, int allocType)
{
    static size_t bufferSize[] = { 0, sizeof(INT32), sizeof(UINT32), sizeof(INT64), sizeof(UINT64), sizeof(double) };
    
    if ((pos < 1) || (pos > hStmt->paramCount))
        return;
    MYSQL_BIND *b = &hStmt->bindings[pos - 1];

    if (cType == DB_CTYPE_STRING)
    {
        b->buffer = utf8_from_wstr((WCHAR *)buffer);
        hStmt->buffers->push_back(b->buffer);
        if (allocType == DB_BIND_DYNAMIC)
            free(buffer);
        b->buffer_length = (unsigned long)strlen((char *)b->buffer) + 1;
        hStmt->lengthFields[pos - 1] = b->buffer_length - 1;
        b->length = &hStmt->lengthFields[pos - 1];
        b->buffer_type = MYSQL_TYPE_STRING;
    }
    else
    {
        switch (allocType)
        {
        case DB_BIND_STATIC:
            b->buffer = buffer;
            break;
        case DB_BIND_DYNAMIC:
            b->buffer = buffer;
            hStmt->buffers->push_back(buffer);
            break;
        case DB_BIND_TRANSIENT:
            b->buffer = memdup(buffer, bufferSize[cType]);
            hStmt->buffers->push_back(b->buffer);
            break;
        default:
            return;	// Invalid call
        }

        switch (cType)
        {
        case DB_CTYPE_UINT32:
            b->is_unsigned = true;
            /* no break */
        case DB_CTYPE_INT32:
            b->buffer_type = MYSQL_TYPE_LONG;
            break;
        case DB_CTYPE_UINT64:
            b->is_unsigned = true;
            /* no break */
        case DB_CTYPE_INT64:
            b->buffer_type = MYSQL_TYPE_LONGLONG;
            break;
        case DB_CTYPE_DOUBLE:
            b->buffer_type = MYSQL_TYPE_DOUBLE;
            break;
        }
    }
}

DWORD DrvExecute(MYSQL_CONN *pConn, MYSQL_STATEMENT *hStmt, WCHAR *errorText)
{
    DWORD dwResult;

    MutexLocker locker(&pConn->mutexQueryLock);

    if (mysql_stmt_bind_param(hStmt->statement, hStmt->bindings) == 0)
    {
        if (mysql_stmt_execute(hStmt->statement) == 0)
        {
            dwResult = DBERR_SUCCESS;
        }
        else
        {
            int nErr = mysql_errno(pConn->pMySQL);
            if (nErr == CR_SERVER_LOST || nErr == CR_CONNECTION_ERROR || nErr == CR_SERVER_GONE_ERROR)
            {
                dwResult = DBERR_CONNECTION_LOST;
            }
            else
            {
                dwResult = DBERR_OTHER_ERROR;
            }
            FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
        }
    }
    else
    {
        FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
        dwResult = DBERR_OTHER_ERROR;
    }

    return dwResult;
}

void DrvFreeStatement(MYSQL_STATEMENT *hStmt)
{
    if (hStmt == NULL)
        return;

    hStmt->connection->mutexQueryLock.Lock();
    mysql_stmt_close(hStmt->statement);
    hStmt->connection->mutexQueryLock.Unlock();
    delete hStmt->buffers;
    safe_free(hStmt->bindings);
    safe_free(hStmt->lengthFields);
    free(hStmt);
}

/**
* Perform actual non-SELECT query
*/
static DWORD DrvQueryInternal(MYSQL_CONN *pConn, const char *pszQuery, WCHAR *errorText)
{
    DWORD dwRet = DBERR_INVALID_HANDLE;

    MutexLocker locker(&pConn->mutexQueryLock);
    if (mysql_query(pConn->pMySQL, pszQuery) == 0)
    {
        dwRet = DBERR_SUCCESS;
        if (errorText != NULL)
            *errorText = 0;
    }
    else
    {
        int nErr = mysql_errno(pConn->pMySQL);
        if (nErr == CR_SERVER_LOST || nErr == CR_CONNECTION_ERROR || nErr == CR_SERVER_GONE_ERROR) // CR_SERVER_GONE_ERROR - ???
        {
            dwRet = DBERR_CONNECTION_LOST;
        }
        else
        {
            dwRet = DBERR_OTHER_ERROR;
        }
        FormatErrorMessage(mysql_error(pConn->pMySQL), errorText);
    }

    return dwRet;
}

/**
* Perform non-SELECT query
*/
DWORD DrvQuery(MYSQL_CONN *pConn, WCHAR *pwszQuery, WCHAR *errorText)
{
    DWORD dwRet;
    char *pszQueryUTF8;

    pszQueryUTF8 = utf8_from_wstr(pwszQuery);
    dwRet = DrvQueryInternal(pConn, pszQueryUTF8, errorText);
    free(pszQueryUTF8);
    return dwRet;
}

/**
* Perform SELECT query
*/
MYSQL_RESULT* DrvSelect(MYSQL_CONN *pConn, WCHAR *pwszQuery, DWORD *pdwError, WCHAR *errorText)
{
    MYSQL_RESULT *result = NULL;
    char *pszQueryUTF8;

    if (pConn == NULL)
    {
        *pdwError = DBERR_INVALID_HANDLE;
        return NULL;
    }

    pszQueryUTF8 = utf8_from_wstr(pwszQuery);
    MutexLocker locker(&pConn->mutexQueryLock);
    if (mysql_query(pConn->pMySQL, pszQueryUTF8) == 0)
    {
        result = (MYSQL_RESULT *)malloc(sizeof(MYSQL_RESULT));
        result->connection = pConn;
        result->isPreparedStatement = false;
        result->resultSet = mysql_store_result(pConn->pMySQL);
        *pdwError = DBERR_SUCCESS;
        if (errorText != NULL)
            *errorText = 0;
    }
    else
    {
        int nErr = mysql_errno(pConn->pMySQL);
        if (nErr == CR_SERVER_LOST || nErr == CR_CONNECTION_ERROR || nErr == CR_SERVER_GONE_ERROR) // CR_SERVER_GONE_ERROR - ???
        {
            *pdwError = DBERR_CONNECTION_LOST;
        }
        else
        {
            *pdwError = DBERR_OTHER_ERROR;
        }
        FormatErrorMessage(mysql_error(pConn->pMySQL), errorText);
    }

    free(pszQueryUTF8);
    return result;
}

/**
* Perform SELECT query using prepared statement
*/
MYSQL_RESULT* DrvSelectPrepared(MYSQL_CONN *pConn, MYSQL_STATEMENT *hStmt, DWORD *pdwError, WCHAR *errorText)
{
    MYSQL_RESULT *result = NULL;

    if (pConn == NULL)
    {
        *pdwError = DBERR_INVALID_HANDLE;
        return NULL;
    }

    MutexLocker locker(&pConn->mutexQueryLock);

    if (mysql_stmt_bind_param(hStmt->statement, hStmt->bindings) == 0)
    {
        if (mysql_stmt_execute(hStmt->statement) == 0)
        {
            result = (MYSQL_RESULT *)malloc(sizeof(MYSQL_RESULT));
            result->connection = pConn;
            result->isPreparedStatement = true;
            result->statement = hStmt->statement;
            result->resultSet = mysql_stmt_result_metadata(hStmt->statement);
            if (result->resultSet != NULL)
            {
                result->numColumns = mysql_num_fields(result->resultSet);

                result->lengthFields = (unsigned long *)malloc(sizeof(unsigned long) * result->numColumns);
                memset(result->lengthFields, 0, sizeof(unsigned long) * result->numColumns);

                result->bindings = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * result->numColumns);
                memset(result->bindings, 0, sizeof(MYSQL_BIND) * result->numColumns);
                for (int i = 0; i < result->numColumns; i++)
                {
                    result->bindings[i].buffer_type = MYSQL_TYPE_STRING;
                    result->bindings[i].length = &result->lengthFields[i];
                }

                mysql_stmt_bind_result(hStmt->statement, result->bindings);

                if (mysql_stmt_store_result(hStmt->statement) == 0)
                {
                    result->numRows = (int)mysql_stmt_num_rows(hStmt->statement);
                    result->currentRow = -1;
                    *pdwError = DBERR_SUCCESS;
                }
                else
                {
                    FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
                    *pdwError = DBERR_OTHER_ERROR;
                    mysql_free_result(result->resultSet);
                    free(result->bindings);
                    free(result->lengthFields);
                    free(result);
                    result = NULL;
                }
            }
            else
            {
                FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
                *pdwError = DBERR_OTHER_ERROR;
                free(result);
                result = NULL;
            }
        }
        else
        {
            int nErr = mysql_errno(pConn->pMySQL);
            if (nErr == CR_SERVER_LOST || nErr == CR_CONNECTION_ERROR || nErr == CR_SERVER_GONE_ERROR)
            {
                *pdwError = DBERR_CONNECTION_LOST;
            }
            else
            {
                *pdwError = DBERR_OTHER_ERROR;
            }
            FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
        }
    }
    else
    {
        FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
        *pdwError = DBERR_OTHER_ERROR;
    }

    return result;
}


LONG DrvGetFieldLength(MYSQL_RESULT *hResult, int iRow, int iColumn)
{
    if (hResult->isPreparedStatement)
    {
        if ((iRow < 0) || (iRow >= hResult->numRows) ||
            (iColumn < 0) || (iColumn >= hResult->numColumns))
            return -1;

        if (hResult->currentRow != iRow)
        {
            MutexLocker locker(&hResult->connection->mutexQueryLock);
            mysql_stmt_data_seek(hResult->statement, iRow);
            mysql_stmt_fetch(hResult->statement);
            hResult->currentRow = iRow;
        }
        return (LONG)hResult->lengthFields[iColumn];
    }
    else
    {
        mysql_data_seek(hResult->resultSet, iRow);
        MYSQL_ROW row = mysql_fetch_row(hResult->resultSet);
        return (row == NULL) ? (LONG)-1 : ((row[iColumn] == NULL) ? -1 : (LONG)strlen(row[iColumn]));
    }
}

/**
* Get field value from result - UNICODE and UTF8 implementation
*/
static void *GetFieldInternal(MYSQL_RESULT *hResult, int iRow, int iColumn, void *pBuffer, int nBufSize, bool utf8)
{
    void *pRet = NULL;

    if (hResult->isPreparedStatement)
    {
        if ((iRow < 0) || (iRow >= hResult->numRows) ||
            (iColumn < 0) || (iColumn >= hResult->numColumns))
            return NULL;

        MutexLocker locker(&hResult->connection->mutexQueryLock);
        if (hResult->currentRow != iRow)
        {
            mysql_stmt_data_seek(hResult->statement, iRow);
            mysql_stmt_fetch(hResult->statement);
            hResult->currentRow = iRow;
        }

        MYSQL_BIND b;
        unsigned long l = 0;
        my_bool isNull;

        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer = alloca(hResult->lengthFields[iColumn] + 1);

        b.buffer_length = hResult->lengthFields[iColumn] + 1;
        b.buffer_type = MYSQL_TYPE_STRING;
        b.length = &l;
        b.is_null = &isNull;
        int rc = mysql_stmt_fetch_column(hResult->statement, &b, iColumn, 0);
        if (rc == 0)
        {
            if (!isNull)
            {
                ((char *)b.buffer)[l] = 0;
                if (utf8)
                {
                    strncpy((char *)pBuffer, (char *)b.buffer, nBufSize);
                    ((char *)pBuffer)[nBufSize - 1] = 0;
                }
                else
                {
                    MultiByteToWideChar(CP_UTF8, 0, (char *)b.buffer, -1, (WCHAR *)pBuffer, nBufSize);
                    ((WCHAR *)pBuffer)[nBufSize - 1] = 0;
                }
            }
            else
            {
                if (utf8)
                    *((char *)pBuffer) = 0;
                else
                    *((WCHAR *)pBuffer) = 0;
            }
            pRet = pBuffer;
        }
    }
    else
    {
        mysql_data_seek(hResult->resultSet, iRow);
        MYSQL_ROW row = mysql_fetch_row(hResult->resultSet);
        if (row != NULL)
        {
            if (row[iColumn] != NULL)
            {
                if (utf8)
                {
                    strncpy((char *)pBuffer, row[iColumn], nBufSize);
                    ((char *)pBuffer)[nBufSize - 1] = 0;
                }
                else
                {
                    MultiByteToWideChar(CP_UTF8, 0, row[iColumn], -1, (WCHAR *)pBuffer, nBufSize);
                    ((WCHAR *)pBuffer)[nBufSize - 1] = 0;
                }
                pRet = pBuffer;
            }
        }
    }
    return pRet;
}

/**
* Get field value from result
*/
WCHAR *DrvGetField(MYSQL_RESULT *hResult, int iRow, int iColumn, WCHAR *pBuffer, int nBufSize)
{
    return (WCHAR *)GetFieldInternal(hResult, iRow, iColumn, pBuffer, nBufSize, false);
}

/**
* Get field value from result as UTF8 string
*/
char *DrvGetFieldUTF8(MYSQL_RESULT *hResult, int iRow, int iColumn, char *pBuffer, int nBufSize)
{
    return (char *)GetFieldInternal(hResult, iRow, iColumn, pBuffer, nBufSize, true);
}

/**
* Get number of rows in result
*/
int DrvGetNumRows(MYSQL_RESULT *hResult)
{
    return (hResult != NULL) ? (int)(hResult->isPreparedStatement ? hResult->numRows : mysql_num_rows(hResult->resultSet)) : 0;
}

/**
* Get column count in query result
*/
int DrvGetColumnCount(MYSQL_RESULT *hResult)
{
    return (hResult != NULL) ? (int)mysql_num_fields(hResult->resultSet) : 0;
}

/**
* Get column name in query result
*/
const char *DrvGetColumnName(MYSQL_RESULT *hResult, int column)
{
    MYSQL_FIELD *field;

    if (hResult == NULL)
        return NULL;

    field = mysql_fetch_field_direct(hResult->resultSet, column);
    return (field != NULL) ? field->name : NULL;
}

/**
* Free SELECT results
*/
void DrvFreeResult(MYSQL_RESULT *hResult)
{
    if (hResult == NULL)
        return;

    if (hResult->isPreparedStatement)
    {
        safe_free(hResult->bindings);
        safe_free(hResult->lengthFields);
    }

    mysql_free_result(hResult->resultSet);
    free(hResult);
}

/**
* Perform unbuffered SELECT query
*/
MYSQL_UNBUFFERED_RESULT* DrvSelectUnbuffered(MYSQL_CONN *pConn, WCHAR *pwszQuery, DWORD *pdwError, WCHAR *errorText)
{
    MYSQL_UNBUFFERED_RESULT *pResult = NULL;
    char *pszQueryUTF8;

    if (pConn == NULL)
    {
        *pdwError = DBERR_INVALID_HANDLE;
        return NULL;
    }

    pszQueryUTF8 = utf8_from_wstr(pwszQuery);
    pConn->mutexQueryLock.Lock();
    if (mysql_query(pConn->pMySQL, pszQueryUTF8) == 0)
    {
        pResult = (MYSQL_UNBUFFERED_RESULT *)malloc(sizeof(MYSQL_UNBUFFERED_RESULT));
        pResult->connection = pConn;
        pResult->isPreparedStatement = false;
        pResult->resultSet = mysql_use_result(pConn->pMySQL);
        if (pResult->resultSet != NULL)
        {
            pResult->noMoreRows = false;
            pResult->numColumns = mysql_num_fields(pResult->resultSet);
            pResult->pCurrRow = NULL;
            pResult->lengthFields = (unsigned long *)malloc(sizeof(unsigned long) * pResult->numColumns);
            pResult->bindings = NULL;
        }
        else
        {
            free(pResult);
            pResult = NULL;
        }

        *pdwError = DBERR_SUCCESS;
        if (errorText != NULL)
            *errorText = 0;
    }
    else
    {
        int nErr = mysql_errno(pConn->pMySQL);
        if (nErr == CR_SERVER_LOST || nErr == CR_CONNECTION_ERROR || nErr == CR_SERVER_GONE_ERROR) // CR_SERVER_GONE_ERROR - ???
        {
            *pdwError = DBERR_CONNECTION_LOST;
        }
        else
        {
            *pdwError = DBERR_OTHER_ERROR;
        }

        if (errorText != NULL)
        {
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mysql_error(pConn->pMySQL), -1, errorText, DBDRV_MAX_ERROR_TEXT);
            errorText[DBDRV_MAX_ERROR_TEXT - 1] = 0;
            RemoveTrailingCRLFW(errorText);
        }
    }

    if (pResult == NULL)
    {
        pConn->mutexQueryLock.Unlock();
    }
    free(pszQueryUTF8);

    return pResult;
}

/**
* Perform unbuffered SELECT query using prepared statement
*/
MYSQL_UNBUFFERED_RESULT* DrvSelectPreparedUnbuffered(MYSQL_CONN *pConn, MYSQL_STATEMENT *hStmt, DWORD *pdwError, WCHAR *errorText)
{
    MYSQL_UNBUFFERED_RESULT *result = NULL;

    pConn->mutexQueryLock.Lock();

    if (mysql_stmt_bind_param(hStmt->statement, hStmt->bindings) == 0)
    {
        if (mysql_stmt_execute(hStmt->statement) == 0)
        {
            result = (MYSQL_UNBUFFERED_RESULT *)malloc(sizeof(MYSQL_UNBUFFERED_RESULT));
            result->connection = pConn;
            result->isPreparedStatement = true;
            result->statement = hStmt->statement;
            result->resultSet = mysql_stmt_result_metadata(hStmt->statement);
            if (result->resultSet != NULL)
            {
                result->noMoreRows = false;
                result->numColumns = mysql_num_fields(result->resultSet);
                result->pCurrRow = NULL;

                result->lengthFields = (unsigned long *)malloc(sizeof(unsigned long) * result->numColumns);
                memset(result->lengthFields, 0, sizeof(unsigned long) * result->numColumns);

                result->bindings = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * result->numColumns);
                memset(result->bindings, 0, sizeof(MYSQL_BIND) * result->numColumns);
                for (int i = 0; i < result->numColumns; i++)
                {
                    result->bindings[i].buffer_type = MYSQL_TYPE_STRING;
                    result->bindings[i].length = &result->lengthFields[i];
                }

                mysql_stmt_bind_result(hStmt->statement, result->bindings);
                *pdwError = DBERR_SUCCESS;
            }
            else
            {
                FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
                *pdwError = DBERR_OTHER_ERROR;
                free(result);
                result = NULL;
            }
        }
        else
        {
            int nErr = mysql_errno(pConn->pMySQL);
            if (nErr == CR_SERVER_LOST || nErr == CR_CONNECTION_ERROR || nErr == CR_SERVER_GONE_ERROR)
            {
                *pdwError = DBERR_CONNECTION_LOST;
            }
            else
            {
                *pdwError = DBERR_OTHER_ERROR;
            }
            FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
        }
    }
    else
    {
        FormatErrorMessage(mysql_stmt_error(hStmt->statement), errorText);
        *pdwError = DBERR_OTHER_ERROR;
    }

    if (result == NULL)
    {
        pConn->mutexQueryLock.Unlock();
    }
    return result;
}

/**
* Fetch next result line from asynchronous SELECT results
*/
bool DrvFetch(MYSQL_UNBUFFERED_RESULT *result)
{
    if ((result == NULL) || (result->noMoreRows))
        return false;

    bool success = true;

    if (result->isPreparedStatement)
    {
        int rc = mysql_stmt_fetch(result->statement);
        if ((rc != 0) && (rc != MYSQL_DATA_TRUNCATED))
        {
            result->noMoreRows = true;
            success = false;
            result->connection->mutexQueryLock.Unlock();
        }
    }
    else
    {
        // Try to fetch next row from server
        result->pCurrRow = mysql_fetch_row(result->resultSet);
        if (result->pCurrRow == NULL)
        {
            result->noMoreRows = true;
            success = false;
            result->connection->mutexQueryLock.Unlock();
        }
        else
        {
            unsigned long *pLen;

            // Get column lengths for current row
            pLen = mysql_fetch_lengths(result->resultSet);
            if (pLen != NULL)
            {
                memcpy(result->lengthFields, pLen, sizeof(unsigned long) * result->numColumns);
            }
            else
            {
                memset(result->lengthFields, 0, sizeof(unsigned long) * result->numColumns);
            }
        }
    }
    return success;
}

/**
* Get field length from async query result result
*/
LONG DrvGetFieldLengthUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult, int iColumn)
{
    // Check if we have valid result handle
    if (hResult == NULL)
        return 0;

    // Check if there are valid fetched row
    if (hResult->noMoreRows || ((hResult->pCurrRow == NULL) && !hResult->isPreparedStatement))
        return 0;

    // Check if column number is valid
    if ((iColumn < 0) || (iColumn >= hResult->numColumns))
        return 0;

    return hResult->lengthFields[iColumn];
}

/**
* Get field from current row in async query result
*/
WCHAR *DrvGetFieldUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult, int iColumn, WCHAR *pBuffer, int iBufSize)
{
    // Check if we have valid result handle
    if (hResult == NULL)
        return NULL;

    // Check if there are valid fetched row
    if ((hResult->noMoreRows) || ((hResult->pCurrRow == NULL) && !hResult->isPreparedStatement))
        return NULL;

    // Check if column number is valid
    if ((iColumn < 0) || (iColumn >= hResult->numColumns))
        return NULL;

    // Now get column data
    WCHAR *value = NULL;
    if (hResult->isPreparedStatement)
    {
        MYSQL_BIND b;
        unsigned long l = 0;
        my_bool isNull;

        memset(&b, 0, sizeof(MYSQL_BIND));

        b.buffer = alloca(hResult->lengthFields[iColumn] + 1);

        b.buffer_length = hResult->lengthFields[iColumn] + 1;
        b.buffer_type = MYSQL_TYPE_STRING;
        b.length = &l;
        b.is_null = &isNull;
        int rc = mysql_stmt_fetch_column(hResult->statement, &b, iColumn, 0);
        if (rc == 0)
        {
            if (!isNull)
            {
                ((char *)b.buffer)[l] = 0;
                MultiByteToWideChar(CP_UTF8, 0, (char *)b.buffer, -1, (WCHAR *)pBuffer, iBufSize);
                ((WCHAR *)pBuffer)[iBufSize - 1] = 0;
            }
            else
            {
                *((WCHAR *)pBuffer) = 0;
            }
            value = pBuffer;
        }
    }
    else
    {
        int iLen = min((int)hResult->lengthFields[iColumn], iBufSize - 1);
        if (iLen > 0)
        {
            MultiByteToWideChar(CP_UTF8, 0, hResult->pCurrRow[iColumn], iLen, pBuffer, iBufSize);
        }
        pBuffer[iLen] = 0;
        value = pBuffer;
    }
    return value;
}

/**
* Get column count in async query result
*/
int DrvGetColumnCountUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult)
{
    return (hResult != NULL) ? hResult->numColumns : 0;
}

/**
* Get column name in async query result
*/
const char *DrvGetColumnNameUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult, int column)
{
    MYSQL_FIELD *field;

    if ((hResult == NULL) || (hResult->resultSet == NULL))
        return NULL;

    field = mysql_fetch_field_direct(hResult->resultSet, column);
    return (field != NULL) ? field->name : NULL;
}

/**
* Destroy result of async query
*/
void DrvFreeUnbufferedResult(MYSQL_UNBUFFERED_RESULT *hResult)
{
    if (hResult == NULL)
        return;

    // Check if all result rows fetched
    if (!hResult->noMoreRows)
    {
        // Fetch remaining rows
        if (!hResult->isPreparedStatement)
        {
            while (mysql_fetch_row(hResult->resultSet) != NULL);
        }

        // Now we are ready for next query, so unlock query mutex
        hResult->connection->mutexQueryLock.Unlock();
    }

    // Free allocated memory
    mysql_free_result(hResult->resultSet);
    free(hResult->lengthFields);
    free(hResult->bindings);
    free(hResult);
}

/**
* Begin transaction
*/
DWORD DrvBegin(MYSQL_CONN *pConn)
{
    return DrvQueryInternal(pConn, "BEGIN", NULL);
}

/**
* Commit transaction
*/
DWORD DrvCommit(MYSQL_CONN *pConn)
{
    return DrvQueryInternal(pConn, "COMMIT", NULL);
}

/**
* Rollback transaction
*/
DWORD DrvRollback(MYSQL_CONN *pConn)
{
    return DrvQueryInternal(pConn, "ROLLBACK", NULL);
}

/**
* Check if table exist
*/
int DrvIsTableExist(MYSQL_CONN *pConn, const WCHAR *name)
{
    WCHAR query[256], lname[256];
    wcsncpy(lname, name, 256);
    wcslwr(lname);
    swprintf(query, 256, L"SHOW TABLES LIKE '%ls'", lname);
    DWORD error;
    WCHAR errorText[DBDRV_MAX_ERROR_TEXT];
    int rc = DBIsTableExist_Failure;
    MYSQL_RESULT *hResult = (MYSQL_RESULT *)DrvSelect(pConn, query, &error, errorText);
    if (hResult != NULL)
    {
        rc = (DrvGetNumRows(hResult) > 0) ? DBIsTableExist_Found : DBIsTableExist_NotFound;
        DrvFreeResult(hResult);
    }
    return rc;
}
P2_NAMESPACE_END

