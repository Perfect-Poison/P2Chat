#pragma once

/*!
 * \file	p2_common.h
 *
 * \author	BrianYi
 * \date	2017/07/11
 *
 * \brief	此文件为整个项目的公共头文件
 */

//-------------------------------
// 标准头文件 区
//-------------------------------
#include <intsafe.h>
#include <winsock2.h>
#include <process.h>
#include <functional>
#include <time.h>
#include <stdio.h>
#include <tchar.h>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <unordered_map>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma pack(1)

#ifndef P2_NAMESPACE_BEG
#define P2_NAMESPACE_BEG    namespace p2 {
#endif
#ifndef P2_NAMESPACE_END
#define P2_NAMESPACE_END    };
#endif
#ifndef P2_NAMESPACE_USE
#define P2_NAMESPACE_USE    using namespace p2;
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define _CHECK_MEMORY_LEAKS_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new _CHECK_MEMORY_LEAKS_NEW
#endif

P2_NAMESPACE_BEG

#define P2CHAT_DEBUG 1

#define Assert(condition)    {                              \
                                                                \
        if (!(condition))                                       \
        {                                                       \
            TCHAR s[512];                            \
            s[511] = 0;                          \
            _sntprintf (s,sizeof(s), _T("_Assert: %s, %d"),_T(__FILE__), __LINE__ ); \
            _tprintf(_T("%s\n"), s); \
        }   }

#define AssertV(condition,errNo)    {                                   \
        if (!(condition))                                                   \
        {                                                                   \
            TCHAR s[512];                                        \
            s[511] = 0;                                      \
            _sntprintf ( s,sizeof(s), _T("_AssertV: %s, %d (%d)"),_T(__FILE__), __LINE__, errNo );    \
            _tprintf(_T("%s\n"), s); \
        }   }

#define safe_free(x)    { if (x) { free(x); x = nullptr; } }
#define safe_delete(x)  { if (x) { delete x; x = nullptr; } }

#define hex2bin(x)  ((((x) >= '0') && ((x) <= '9')) ? ((x) - '0') : (((toupper(x) >= 'A') && (toupper(x) <= 'F')) ? (toupper(x) - 'A' + 10) : 0))
#define bin2hex(x)  ((x) < 10 ? ((x) + '0') : ((x) + ('A' - 10)))


typedef unsigned char   uint8;
typedef signed char     int8;
typedef unsigned short  uint16;
typedef signed short    int16;
typedef unsigned long   uint32;
typedef signed long     int32;
typedef LONGLONG        int64;
typedef ULONGLONG       uint64;
typedef float           float32;
typedef double          float64;

/**
 *	事件结构体
 */
struct eventreq 
{
    int          er_handle;
    unsigned int er_eventid;
    int          er_eventbits;
};


enum
{
    EV_RE = FD_READ | FD_ACCEPT | FD_CLOSE,
    EV_WR = FD_WRITE | FD_CONNECT
};

/**
*	log flags
*/
enum
{
    LOG_PRINT_TO_CONSOLE    = (uint32)0x00000001,
    LOG_IS_OPEN             = (uint32)0x80000000
};
typedef uint32 log_flags;

/**
*	log types
*/
enum
{
    LOG_DEBUG   = (WORD)0x0001,
    LOG_INFO    = (WORD)0x0002,
    LOG_WARNING = (WORD)0x0003,
    LOG_FATAL   = (WORD)0x0004
};
typedef WORD log_type;

/**
*	log rotation policy
*/
enum
{
    LOG_ROTATION_DISABLED   = (WORD)0x0000,
    LOG_ROTATION_DAILY      = (WORD)0x0001,
    LOG_ROTATION_BY_SIZE    = (WORD)0x0002
};
typedef WORD log_rotation_policy;


#define MAX_LOG_HISTORY_SIZE 128
#define MAX_LOG_RECORD_MESSAGE_SIZE 4096

P2_NAMESPACE_END