#pragma once
/*!
 * \file	p2cpapi.h
 *
 * \author	BrianYi
 * \date	2017/07/17
 *
 * \brief	传输协议相关的API和定义
 */

#include "Common/common.h"
#include "Common/uthash.h"
#include <map>

P2_NAMESPACE_BEG

typedef void* attr_data;
typedef uint32 msg_size;
typedef uint32 msg_id;
typedef uint32 msg_attrnum;

const uint16 SERVER_PORT_FOR_TCP = 6501;
const uint16 SERVER_PORT_FOR_UDP = 6551;
const uint16 MESSAGE_HEADER_SIZE = 16;
const uint16 ATTR_HEADER_SIZE = 6;

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
typedef uint16 msg_code;

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
typedef uint32 attr_code;

/**
 *	Data types
 */
enum
{
    dt_int16,
    dt_int32,
    dt_int64,
    dt_float32,
    dt_float64,
    dt_binary,
    dt_string,
    dt_num
};
typedef uint8 attr_datatype;

/**
 *	Message flags
 */
enum 
{
    mf_binary = 0x0001,
    mf_end_of_file = 0x0002,
};
typedef uint16 msg_flags;

/**
 *	Attribute flags
 */
enum
{
    af_none = 0x00,
    af_signed = 0x01,
    af_unsigned = 0x02,
};
typedef uint8 attr_flags;

/**
 *	Attribute Structure
 */
typedef struct  
{
    attr_code code;         // 4 byte
    attr_flags flags;       // 1 byte
    attr_datatype dataType; // 1 byte
    union 
    {
        int16 i16;
        int32 i32;
        int64 i64;
        float32 f32;
        float64 f64;
        struct
        {
            uint32 size;
            BYTE* data;
        }bin;
        struct  
        {
            uint32 size;
            WORD* data; // string类型全为宽字符，占2字节
        }str;
    };
}MESSAGE_ATTR;

/**
 *	Message Structure
 */
typedef struct  
{
    msg_code code;          // 2 byte
    msg_flags flags;        // 2 byte
    msg_size size;          // 4 byte
    msg_id msgID;           // 4 byte
    msg_attrnum attrNum;    // 4 byte
    void* attrs;
}MESSAGE;

/**
*	Attribute hash map entry
*/
typedef struct
{
    UT_hash_handle hh;
    attr_code code; // key
    size_t size;    // message attr size
    MESSAGE_ATTR data;
}MessageAttr;

/**
 *	消息处理类
 */
class Message
{
public:
    Message();
    Message(MESSAGE *msg);
    ~Message();
    MESSAGE *CreateMessage();
    msg_code GetCode() const { return fCode; }
    void SetCode(msg_code msgCode) { fCode = msgCode; }
    msg_flags GetFlags() const { return fFlags;  }
    void SetFlags(msg_flags msgFlags) { fFlags = msgFlags; }
    
    bool IsBinary() const { return (fFlags & mf_binary) ? true : false; }
    bool IsEndOfFile() const { return (fFlags & mf_end_of_file) ? true : false; }
    const BYTE *GetBinaryData() const { return IsBinary() ? fData : nullptr; }
    msg_size GetBinaryDataSize() const { return IsBinary() ? fAttrNum : 0; }
    
    bool IsAttrExist(attr_code attrCode) { return findAttr(attrCode) ? true : false; }
    attr_datatype GetAttrDataType(attr_code attrCode) { return findAttr(attrCode)->data.dataType; }

    void SetAttr(attr_code attrCode, int16 value) { set(attrCode, dt_int16, &value, af_signed); }
    void SetAttr(attr_code attrCode, uint16 value) { set(attrCode, dt_int16, &value, af_unsigned); }
    void SetAttr(attr_code attrCode, int32 value) { set(attrCode, dt_int32, &value, af_signed); }
    void SetAttr(attr_code attrCode, uint32 value) { set(attrCode, dt_int32, &value, af_unsigned); }
    void SetAttr(attr_code attrCode, int64 value) { set(attrCode, dt_int64, &value, af_signed); }
    void SetAttr(attr_code attrCode, uint64 value) { set(attrCode, dt_int64, &value, af_unsigned); }
    void SetAttr(attr_code attrCode, float32 value) { set(attrCode, dt_float32, &value); }
    void SetAttr(attr_code attrCode, float64 value) { set(attrCode, dt_float64, &value); }
    void SetAttr(attr_code attrCode, const BYTE* value, uint32 size) { set(attrCode, dt_binary, &value, af_none, size); }
    void SetAttr(attr_code attrCode, const TCHAR* value, uint32 size) { set(attrCode, dt_string, &value, af_none, size); }

    int16 GetAttrAsInt16(attr_code attrCode) const { return *((int16*)get(attrCode, dt_int16)); }
    uint16 GetAttrAsUInt16(attr_code attrCode) const { return *((uint16*)get(attrCode, dt_int16)); }
    int32 GetAttrAsInt32(attr_code attrCode) const { return *((int32*)get(attrCode, dt_int32)); };
    uint32 GetAttrAsUInt32(attr_code attrCode) const { return *((uint32*)get(attrCode, dt_int32)); }
    int64 GetAttrAsInt64(attr_code attrCode) const { return *((int64*)get(attrCode, dt_int64)); }
    uint64 GetAttrAsUInt64(attr_code attrCode) const { return *((uint64*)get(attrCode, dt_int64)); }
    float32 GetAttrAsFloat32(attr_code attrCode) const { return *((float32*)get(attrCode, dt_float32)); }
    float64 GetAttrAsFloat64(attr_code attrCode) const { return *((float64*)get(attrCode, dt_float64)); }
    uint32 GetAttrAsBinary(attr_code attrCode, BYTE* buffer, uint32 bufferSize) const { return *((uint32*)get(attrCode, dt_binary, buffer, bufferSize)); }
    WCHAR* GetAttrAsString(attr_code attrCode, WCHAR* strBufferW = nullptr, uint32 strBufferLenW = 0) const { return (WCHAR*)get(attrCode, dt_string, strBufferW, strBufferLenW); }
    static uint32 CalculateAttrSize(MESSAGE_ATTR *attr, bool networkByteOrder);
    static uint32 CalculateTotalAttrSize(MESSAGE *msg, bool networkByteOrder);
    static uint32 CalculateTotalAttrSize(MessageAttr *attrs, bool networkByteOrder);
private:
    void set(attr_code attrCode, attr_datatype dataType, const void *value, attr_flags flags = af_none, uint32 size = 0);
    void *get(attr_code attrCode, attr_datatype dataType, void *buffer = nullptr, uint32 bufferSize = 0) const;
    MessageAttr *findAttr(attr_code attrCode) const;
private:
    msg_code fCode;          // 2 byte
    msg_flags fFlags;        // 2 byte
    msg_size fSize;          // 4 byte
    msg_id fMsgID;           // 4 byte
    msg_attrnum fAttrNum;    // 4 byte
    MessageAttr *fAttrs;
    BYTE *fData;
};

P2_NAMESPACE_END