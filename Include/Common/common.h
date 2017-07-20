#pragma once
/*!
 * \file	common.h
 *
 * \author	BrianYi
 * \date	2017/07/11
 *
 * \brief	此文件为整个项目的公共文件
 */

//-------------------------------
// 标准头文件 区
//-------------------------------
#include <iostream>
#include <intsafe.h>
#include <winsock2.h>
#include <process.h>
#include <string>
#include <functional>
#include <time.h>
#include <stdio.h>
using namespace std;

#ifndef P2_NAMESPACE_BEG
#define P2_NAMESPACE_BEG    namespace p2 {
#endif
#ifndef P2_NAMESPACE_END
#define P2_NAMESPACE_END    };
#endif
#ifndef P2_NAMESPACE_USE
#define P2_NAMESPACE_USE    using namespace p2;
#endif

P2_NAMESPACE_BEG



//*******************************
// 宏定义 区

#define P2CHAT_DEBUG 0


constexpr size_t kAssertBuffSize = 512;

#define Assert(condition)    {                              \
                                                                \
        if (!(condition))                                       \
        {                                                       \
            char s[kAssertBuffSize];                            \
            s[kAssertBuffSize -1] = 0;                          \
            _snprintf_s (s,kAssertBuffSize -1, "_Assert: %s, %d",__FILE__, __LINE__ ); \
            printf("%s\n", s); \
        }   }

#define AssertV(condition,errNo)    {                                   \
        if (!(condition))                                                   \
        {                                                                   \
            char s[kAssertBuffSize];                                        \
            s[kAssertBuffSize -1] = 0;                                      \
            _snprintf_s ( s,kAssertBuffSize -1, "_AssertV: %s, %d (%d)",__FILE__, __LINE__, errNo );    \
            printf("%s\n", s); \
        }   }

#define safe_free(x)    { if (x) { free(x); x = nullptr; } }

//*******************************
// 类型定义 区
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

//*******************************
// 枚举 区
enum
{
    EV_RE = FD_READ | FD_ACCEPT | FD_CLOSE,
    EV_WR = FD_WRITE | FD_CONNECT
};


//*******************************
// 常量 区
const uint16 SERVER_PORT_FOR_TCP = 6001;
const uint16 SERVER_PORT_FOR_UDP = 6051;


//*******************************
// 全局 区

/**
 *	内存操作相关函数
 */
void *memdup(const void *data, size_t size);

/**
*	原子操作相关函数
*/
unsigned int atomic_add(unsigned int *area, int val);
unsigned int atomic_or(unsigned int *area, unsigned int val);
unsigned int atomic_sub(unsigned int *area, int val);

unsigned int compare_and_store(unsigned int oval, unsigned int nval, unsigned int *area);

/**
*	事件操作相关函数
*/
int select_watchevent(struct eventreq *req, int which);
int select_modwatch(struct eventreq *req, int which);
int select_waitevent(struct eventreq *req);

P2_NAMESPACE_END