#pragma once
#include "Common/p2cpapi.h"
P2_NAMESPACE_USE;

typedef struct
{
    int    enumVal;
    char*  strVal;
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
constexpr int MSG_FLAGS_NUM = 2;
constexpr int ATTR_FLAGS_NUM = 3;


msg_code_param find_msg_code_param_by_enum(int enumVal);
msg_code_param find_msg_code_param_by_str(char *strVal);
attr_code_param find_attr_code_param_by_enum(int enumVal);
attr_code_param find_attr_code_param_by_str(char *strVal);
attr_datatype_param find_attr_datatype_param_by_enum(int enumVal);
attr_datatype_param find_attr_datatype_param_by_str(char *strVal);
msg_flags_param find_msg_flags_param_by_enum(int enumVal);
msg_flags_param find_msg_flags_param_by_str(char *strVal);
attr_flags_param find_attr_flags_param_by_enum(int enumVal); 
attr_flags_param find_attr_flags_param_by_str(char *strVal);