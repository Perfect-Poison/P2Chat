#pragma once
#pragma execution_character_set("utf-8")

/*!
* \file	p2client_common.h
*
* \author	BrianYi
* \date	    2017/10/11
*
* \brief	P2Chat�ͻ�����Ŀ�Ĺ���ͷ�ļ�
*/

#include "p2_common.h"
#include "p2_util.h"
P2_NAMESPACE_BEG

const uint16 CLIENT_PORT_FOR_TCP = 5001;    // �ͻ���TCP�˿�
const uint16 CLIENT_PORT_FOR_UDP = 5051;    // �ͻ���UDP�˿�
const uint16 SERVER_PORT_FOR_TCP = 6001;
const uint16 SERVER_PORT_FOR_UDP = 6051;

const TCHAR SERVER_IP[] = _T("127.0.0.1");

const int32 MAX_UDP_PACKET = 1500;
enum
{
    p2UserStatusOnline,         // ����
    p2UserStatusAFK,            // �뿪
    p2UserStatusBusy,           // æµ
    p2UserStatusInvisible,      // ����
    p2UserStatusNoDisturbing,   // �������
    p2UserStatusOffline,        // ����
    p2UserStatusNum
};
typedef uint16 user_status;

static uint32 g_messageCounter = 0;

P2_NAMESPACE_END