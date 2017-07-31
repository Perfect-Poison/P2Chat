#pragma once
/*!
 * \file	p2server_common.h
 *
 * \author	BrianYi
 * \date	2017/07/28
 *
 * \brief	P2Chat服务端项目的公共头文件
 */

#include "p2_common.h"
#include "p2_util.h"

#define P2CHAT_DEBUG 0

P2_NAMESPACE_BEG


const uint16 SERVER_PORT_FOR_TCP = 6001;    // 服务端TCP端口
const uint16 SERVER_PORT_FOR_UDP = 6051;    // 服务端UDP端口

/**
 *	Log flags
 */
enum
{
    LOG_TO_CONSOLE  = (uint32)0x00000001,
    LOG_IS_OPEN     = (uint32)0x80000000
};
typedef uint32 log_flags;

enum
{
    LOG_DEBUG   = (WORD)0x0001,
    LOG_INFO    = (WORD)0x0002,
    LOG_WARNING = (WORD)0x0003,
    LOG_ERROR   = (WORD)0x0004
};
typedef WORD log_type;

P2_NAMESPACE_END