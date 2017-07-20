#pragma once
/*!
 * \file	common.h
 *
 * \author	BrianYi
 * \date	2017/07/11
 *
 * \brief	���ļ�Ϊ������Ŀ�Ĺ����ļ�
 */

//-------------------------------
// ��׼ͷ�ļ� ��
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
// �궨�� ��

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
// ���Ͷ��� ��
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
 *	�¼��ṹ��
 */
struct eventreq 
{
    int          er_handle;
    unsigned int er_eventid;
    int          er_eventbits;
};

//*******************************
// ö�� ��
enum
{
    EV_RE = FD_READ | FD_ACCEPT | FD_CLOSE,
    EV_WR = FD_WRITE | FD_CONNECT
};


//*******************************
// ���� ��
const uint16 SERVER_PORT_FOR_TCP = 6001;
const uint16 SERVER_PORT_FOR_UDP = 6051;


//*******************************
// ȫ�� ��

/**
 *	�ڴ������غ���
 */
void *memdup(const void *data, size_t size);

/**
*	ԭ�Ӳ�����غ���
*/
unsigned int atomic_add(unsigned int *area, int val);
unsigned int atomic_or(unsigned int *area, unsigned int val);
unsigned int atomic_sub(unsigned int *area, int val);

unsigned int compare_and_store(unsigned int oval, unsigned int nval, unsigned int *area);

/**
*	�¼�������غ���
*/
int select_watchevent(struct eventreq *req, int which);
int select_modwatch(struct eventreq *req, int which);
int select_waitevent(struct eventreq *req);

P2_NAMESPACE_END