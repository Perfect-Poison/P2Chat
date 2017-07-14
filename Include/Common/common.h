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
// 宏定义 区
//-------------------------------

//-------------------------------
// 类型定义 区
//-------------------------------


//-------------------------------
// 枚举 区
//-------------------------------
enum IOType
{
    Blocking = 0,
    NonBlocking = 1
};

//-------------------------------
// 常量 区
//-------------------------------
const UINT32 RECV_BUF_SIZE = 10 * 1024u;
const UINT32 SEND_BUF_SIZE = 10 * 1024u;
const INT32 MAX_CONNECTION_NUM = 32;

//-------------------------------
// 全局 区
//-------------------------------


P2_NAMESPACE_END