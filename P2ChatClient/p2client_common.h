#pragma once
#pragma execution_character_set("utf-8")

/*!
* \file	p2client_common.h
*
* \author	BrianYi
* \date	    2017/10/11
*
* \brief	P2Chat客户端项目的公共头文件
*/

#include "p2_common.h"
#include "p2_util.h"
P2_NAMESPACE_BEG

const uint16 CLIENT_PORT_FOR_TCP = 5001;    // 客户端TCP端口
const uint16 CLIENT_PORT_FOR_UDP = 5051;    // 客户端UDP端口
const uint16 SERVER_PORT_FOR_TCP = 6001;
const uint16 SERVER_PORT_FOR_UDP = 6051;

const TCHAR SERVER_IP[] = _T("127.0.0.1");

const int32 MAX_UDP_PACKET = 1500;
enum
{
    p2UserStatusOnline,         // 在线
    p2UserStatusAFK,            // 离开
    p2UserStatusBusy,           // 忙碌
    p2UserStatusInvisible,      // 隐身
    p2UserStatusNoDisturbing,   // 请勿打扰
    p2UserStatusOffline,        // 离线
    p2UserStatusNum
};
typedef uint16 user_status;

static uint32 g_messageCounter = 0;

P2_NAMESPACE_END