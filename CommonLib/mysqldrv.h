#pragma once
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
** File: mysqldrv.h
**
**/

#include "p2_util.h"
#include "mysql.h"
#include "errmsg.h"
#include "Mutex.h"
#include <vector>
P2_NAMESPACE_BEG

//
// Constants
//

#define DBDRV_MAX_ERROR_TEXT        1024

//
// Error codes
//

#define DBERR_SUCCESS               0
#define DBERR_CONNECTION_LOST       1
#define DBERR_INVALID_HANDLE        2
#define DBERR_OTHER_ERROR           255


//
// DB binding buffer allocation types
//

#define DB_BIND_STATIC     0 // buffer is managed by caller and will be valid until after the query is executed
#define DB_BIND_TRANSIENT  1 // buffer will be duplicated by DB driver in DBBind()
#define DB_BIND_DYNAMIC    2 // DB Driver will call free() on buffer


//
// C and SQL types for parameter binding
//

#define DB_CTYPE_STRING    0
#define DB_CTYPE_INT32     1
#define DB_CTYPE_UINT32    2
#define DB_CTYPE_INT64     3
#define DB_CTYPE_UINT64    4
#define DB_CTYPE_DOUBLE    5

#define DB_SQLTYPE_VARCHAR 0
#define DB_SQLTYPE_INTEGER 1
#define DB_SQLTYPE_BIGINT  2
#define DB_SQLTYPE_DOUBLE  3
#define DB_SQLTYPE_TEXT    4

/**
* DBIsTableExist return codes
*/
enum
{
    DBIsTableExist_Failure = -1,
    DBIsTableExist_NotFound = 0,
    DBIsTableExist_Found = 1
};

/**
* Structure of DB connection handle
*/
typedef struct
{
    MYSQL *pMySQL;
    Mutex mutexQueryLock;
} MYSQL_CONN;

/**
* Structure of prepared statement
*/
typedef struct
{
    MYSQL_CONN *connection;
    MYSQL_STMT *statement;
    MYSQL_BIND *bindings;
    unsigned long *lengthFields;
    vector<void*> *buffers;
    int paramCount;
} MYSQL_STATEMENT;

/**
* Structure of synchronous SELECT result
*/
typedef struct
{
    MYSQL_CONN *connection;
    MYSQL_RES *resultSet;
    bool isPreparedStatement;
    MYSQL_STMT *statement;
    int numColumns;
    int numRows;
    int currentRow;
    MYSQL_BIND *bindings;
    unsigned long *lengthFields;
} MYSQL_RESULT;

/**
* Structure of asynchronous SELECT result
*/
typedef struct
{
    MYSQL_CONN *connection;
    MYSQL_RES *resultSet;
    MYSQL_ROW pCurrRow;
    bool noMoreRows;
    int numColumns;
    MYSQL_BIND *bindings;
    unsigned long *lengthFields;
    bool isPreparedStatement;
    MYSQL_STMT *statement;
} MYSQL_UNBUFFERED_RESULT;

WCHAR *DrvPrepareStringW(const WCHAR *str);
char *DrvPrepareStringA(const char *str);
#ifdef UNICODE
#define DrvPrepareString    DrvPrepareStringW
#else
#define DrvPrepareString    DrvPrepareStringA
#endif

bool DrvInit();
void DrvUnload();
MYSQL_CONN* DrvConnect(const char *szHost, const char *szLogin, const char *szPassword, const char *szDatabase, WCHAR *errorText);
void DrvDisconnect(MYSQL_CONN *pConn);
MYSQL_STATEMENT* DrvPrepare(MYSQL_CONN *pConn, WCHAR *pwszQuery, DWORD *pdwError, WCHAR *errorText);
void DrvBind(MYSQL_STATEMENT *hStmt, int pos, int sqlType, int cType, void *buffer, int allocType);
DWORD DrvExecute(MYSQL_CONN *pConn, MYSQL_STATEMENT *hStmt, WCHAR *errorText);
void DrvFreeStatement(MYSQL_STATEMENT *hStmt);
static DWORD DrvQueryInternal(MYSQL_CONN *pConn, const char *pszQuery, WCHAR *errorText);
DWORD DrvQuery(MYSQL_CONN *pConn, WCHAR *pwszQuery, WCHAR *errorText);
MYSQL_RESULT* DrvSelect(MYSQL_CONN *pConn, WCHAR *pwszQuery, DWORD *pdwError, WCHAR *errorText);
MYSQL_RESULT* DrvSelectPrepared(MYSQL_CONN *pConn, MYSQL_STATEMENT *hStmt, DWORD *pdwError, WCHAR *errorText);
LONG DrvGetFieldLength(MYSQL_RESULT *hResult, int iRow, int iColumn);
static void *GetFieldInternal(MYSQL_RESULT *hResult, int iRow, int iColumn, void *pBuffer, int nBufSize, bool utf8);
WCHAR *DrvGetField(MYSQL_RESULT *hResult, int iRow, int iColumn, WCHAR *pBuffer, int nBufSize);
char *DrvGetFieldUTF8(MYSQL_RESULT *hResult, int iRow, int iColumn, char *pBuffer, int nBufSize);
int DrvGetNumRows(MYSQL_RESULT *hResult);
int DrvGetColumnCount(MYSQL_RESULT *hResult);
const char *DrvGetColumnName(MYSQL_RESULT *hResult, int column);
void DrvFreeResult(MYSQL_RESULT *hResult);
MYSQL_UNBUFFERED_RESULT* DrvSelectUnbuffered(MYSQL_CONN *pConn, WCHAR *pwszQuery, DWORD *pdwError, WCHAR *errorText);
MYSQL_UNBUFFERED_RESULT* DrvSelectPreparedUnbuffered(MYSQL_CONN *pConn, MYSQL_STATEMENT *hStmt, DWORD *pdwError, WCHAR *errorText);
bool DrvFetch(MYSQL_UNBUFFERED_RESULT *result);
LONG DrvGetFieldLengthUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult, int iColumn);
WCHAR *DrvGetFieldUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult, int iColumn, WCHAR *pBuffer, int iBufSize);
int DrvGetColumnCountUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult);
const char *DrvGetColumnNameUnbuffered(MYSQL_UNBUFFERED_RESULT *hResult, int column);
void DrvFreeUnbufferedResult(MYSQL_UNBUFFERED_RESULT *hResult);
DWORD DrvBegin(MYSQL_CONN *pConn);
DWORD DrvCommit(MYSQL_CONN *pConn);
DWORD DrvRollback(MYSQL_CONN *pConn);
int DrvIsTableExist(MYSQL_CONN *pConn, const WCHAR *name);

P2_NAMESPACE_END