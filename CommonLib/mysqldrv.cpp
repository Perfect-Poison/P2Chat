#include "mysqldrv.h"
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


P2_NAMESPACE_END

