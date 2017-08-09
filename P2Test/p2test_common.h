#pragma once
#pragma execution_character_set("utf-8")

#include "p2cpapi.h"
#include <assert.h>
P2_NAMESPACE_USE

const int BIND_PORT_FOR_UDP = 5566;

typedef struct
{
    int    enumVal;
    TCHAR*  strVal;
}param;

typedef param msg_code_param;
typedef param attr_code_param;
typedef param attr_datatype_param;
typedef param msg_flags_param;
typedef param attr_flags_param;

msg_code_param sMsgCodeParam[];
attr_code_param sAttrCodeParam[];
attr_datatype_param sAttrDataTypeParam[];
msg_flags_param sMsgFlagsParam[];
attr_flags_param sAttrFlagsParam[];

constexpr int MSG_CODE_NUM = 14;
constexpr int ATTR_CODE_NUM = 8;
constexpr int ATTR_DATA_TYPE_NUM = 7;
constexpr int MSG_FLAGS_NUM = 3;
constexpr int ATTR_FLAGS_NUM = 3;


msg_code_param find_msg_code_param_by_enum(int enumVal);
msg_code_param find_msg_code_param_by_str(TCHAR *strVal);
attr_code_param find_attr_code_param_by_enum(int enumVal);
attr_code_param find_attr_code_param_by_str(TCHAR *strVal);
attr_datatype_param find_attr_datatype_param_by_enum(int enumVal);
attr_datatype_param find_attr_datatype_param_by_str(TCHAR *strVal);
msg_flags_param find_msg_flags_param_by_enum(int enumVal);
msg_flags_param find_msg_flags_param_by_str(TCHAR *strVal);
attr_flags_param find_attr_flags_param_by_enum(int enumVal); 
attr_flags_param find_attr_flags_param_by_str(TCHAR *strVal);