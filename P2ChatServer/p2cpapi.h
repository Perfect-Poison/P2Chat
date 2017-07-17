#pragma once
/*!
 * \file	p2cpapi.h
 *
 * \author	BrianYi
 * \date	2017/07/17
 *
 * \brief	传输协议相关的API和类型定义
 */

#include "Common/common.h"

typedef void* attr_data;
typedef UINT32 msg_size;
typedef UINT32 msg_id;
typedef UINT32 msg_attrnum;

/**
 *	Message Code
 */
enum
{
    MSG_SERVER_GET_INFO,
    MSG_SERVER_SET_INFO,
    MSG_USER_GET_INFO,
    MSG_USER_SET_INFO,
    MSG_GROUP_GET_INFO,
    MSG_GROUP_SET_INFO,
    MSG_LOGIN,
    MSG_LOGOUT,
    MSG_USER_MSG_PACKET,
    MSG_GROUP_MSG_PACKET,
    MSG_USER_ONLINE,
    MSG_USER_OFFLINE,
    MSG_REQUEST_SUCCEED,
    MSG_REQUEST_FAILED,
    MSG_NUM
};
typedef UINT16 msg_code;

/**
 *	Attribute Code
 */
enum
{
    ATTR_SERVER_INFO,

    ATTR_SESSION_ID,

    ATTR_USER_ID,
    ATTR_USER_NAME,
    ATTR_USER_PASSWORD,

    ATTR_GROUP_ID,
    ATTR_GROUP_NAME,

    ATTR_MSG_PACKET,

    ATTR_NUM
};
typedef UINT32 attr_id;

/**
 *	Data types
 */
enum
{
    dt_int16 = 0,
    dt_int32 = 1,
    dt_int64 = 2,
    dt_float32 = 3,
    dt_float64 = 4,
    dt_binary = 5,
    dt_string = 6
};
typedef UINT8 attr_datatype;

/**
 *	Message flags
 */
enum 
{
    mf_binary = 1,
    mf_end_of_file = 2
};
typedef UINT16 msg_flags;

/**
 *	Message Attribute Structure
 */
typedef struct  
{
    attr_id attrID;
    attr_datatype dataType;
    attr_data data;
}P2_MESSAGE_ATTR;

/**
 *	Message Structure
 */
typedef struct  
{
    msg_code msgCode;
    msg_flags flags;
    msg_size size;
    msg_id msgID;
    msg_attrnum attrNum;
    P2_MESSAGE_ATTR *attrs;
}P2_MESSAGE;

