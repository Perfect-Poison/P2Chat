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


//-------------------------------
// �궨�� ��
//-------------------------------

//-------------------------------
// ���Ͷ��� ��
//-------------------------------


//-------------------------------
// ö�� ��
//-------------------------------
enum IOType
{
    Blocking = 0,
    NonBlocking = 1
};

//-------------------------------
// ���� ��
//-------------------------------
const UINT32 RECV_BUF_SIZE = 10 * 1024u;
const UINT32 SEND_BUF_SIZE = 10 * 1024u;
const INT32 MAX_CONNECTION_NUM = 32;

//-------------------------------
// ȫ�� ��
//-------------------------------


P2_NAMESPACE_END