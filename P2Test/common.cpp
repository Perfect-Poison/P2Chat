#include "common.h"

param not_found_param = { 0, nullptr };

msg_code_param sMsgCodeParam[] =
{
    MSG_SERVER_GET_INFO,    "MSG_SERVER_GET_INFO",
    MSG_SERVER_SET_INFO,    "MSG_SERVER_SET_INFO",
    MSG_USER_GET_INFO,      "MSG_USER_GET_INFO",
    MSG_USER_SET_INFO,      "MSG_USER_SET_INFO",
    MSG_GROUP_GET_INFO,     "MSG_GROUP_GET_INFO",
    MSG_GROUP_SET_INFO,     "MSG_GROUP_SET_INFO",
    MSG_LOGIN,              "MSG_LOGIN",
    MSG_LOGOUT,             "MSG_LOGOUT",
    MSG_USER_MSG_PACKET,    "MSG_USER_MSG_PACKET",
    MSG_GROUP_MSG_PACKET,   "MSG_GROUP_MSG_PACKET",
    MSG_USER_ONLINE,        "MSG_USER_ONLINE",
    MSG_USER_OFFLINE,       "MSG_USER_OFFLINE",
    MSG_REQUEST_SUCCEED,    "MSG_REQUEST_SUCCEED",
    MSG_REQUEST_FAILED,     "MSG_REQUEST_FAILED"
};

extern attr_code_param sAttrCodeParam[] =
{
    ATTR_SERVER_INFO,   "ATTR_SERVER_INFO",

    ATTR_SESSION_ID,    "ATTR_SESSION_ID",

    ATTR_USER_ID,       "ATTR_USER_ID",
    ATTR_USER_NAME,     "ATTR_USER_NAME",
    ATTR_USER_PASSWORD, "ATTR_USER_PASSWORD",

    ATTR_GROUP_ID,      "ATTR_GROUP_ID",
    ATTR_GROUP_NAME,    "ATTR_GROUP_NAME",

    ATTR_MSG_PACKET,    "ATTR_MSG_PACKET"
};

extern attr_datatype_param sAttrDataTypeParam[] =
{
    dt_int16,   "dt_int16",
    dt_int32,   "dt_int32",
    dt_int64,   "dt_int64",
    dt_float32, "dt_float32",
    dt_float64, "dt_float64",
    dt_binary,  "dt_binary",
    dt_string,  "dt_string"
};

extern msg_flags_param sMsgFlagsParam[] =
{
    mf_binary,      "mf_binary",
    mf_end_of_file, "mf_end_of_file"
};

extern attr_flags_param sAttrFlagsParam[] =
{
    af_none,        "af_none",
    af_signed,      "af_signed",
    af_unsigned,    "af_unsigned"
};

msg_code_param find_msg_code_param_by_enum(int enumVal)
{
    for (size_t i = 0; i < MSG_CODE_NUM; i++)
    {
        if (sMsgCodeParam[i].enumVal == enumVal)
            return sMsgCodeParam[i];
    }
    return not_found_param;
}

msg_code_param find_msg_code_param_by_str(char *strVal)
{
    for (size_t i = 0; i < MSG_CODE_NUM; i++)
    {
        if (strcmp(sMsgCodeParam[i].strVal, strVal) == 0)
            return sMsgCodeParam[i];
    }
    return not_found_param;
}

attr_code_param find_attr_code_param_by_enum(int enumVal)
{
    for (size_t i = 0; i < ATTR_CODE_NUM; i++) 
    {
        if (sAttrCodeParam[i].enumVal == enumVal)
            return sAttrCodeParam[i];
    }
    return not_found_param;
}

attr_code_param find_attr_code_param_by_str(char *strVal)
{
    for (size_t i = 0; i < ATTR_CODE_NUM; i++)
    {
        if (strcmp(sAttrCodeParam[i].strVal, strVal) == 0)
            return sAttrCodeParam[i];
    }
    return not_found_param;
}

attr_datatype_param find_attr_datatype_param_by_enum(int enumVal)
{
    for (size_t i = 0; i < ATTR_DATA_TYPE_NUM; i++) 
    {
        if (sAttrDataTypeParam[i].enumVal == enumVal)
            return sAttrDataTypeParam[i];
    }
    return not_found_param;
}

attr_datatype_param find_attr_datatype_param_by_str(char *strVal)
{
    for (size_t i = 0; i < ATTR_DATA_TYPE_NUM; i++)
    {
        if (strcmp(sAttrDataTypeParam[i].strVal, strVal) == 0)
            return sAttrDataTypeParam[i];
    }
    return not_found_param;
}

msg_flags_param find_msg_flags_param_by_enum(int enumVal)
{
    for (size_t i = 0; i < MSG_FLAGS_NUM; i++)
    {
        if (sMsgFlagsParam[i].enumVal == enumVal)
            return sMsgFlagsParam[i];
    }
    return not_found_param;
}

msg_flags_param find_msg_flags_param_by_str(char *strVal)
{
    for (size_t i = 0; i < MSG_FLAGS_NUM; i++)
    {
        if (strcmp(sMsgFlagsParam[i].strVal, strVal) == 0)
            return sMsgFlagsParam[i];
    }
    return not_found_param;
}

attr_flags_param find_attr_flags_param_by_enum(int enumVal)
{
    for (size_t i = 0; i < ATTR_FLAGS_NUM; i++)
    {
        if (sAttrFlagsParam[i].enumVal == enumVal)
            return sAttrFlagsParam[i];
    }
    return not_found_param;
}

attr_flags_param find_attr_flags_param_by_str(char *strVal)
{
    for (size_t i = 0; i < ATTR_FLAGS_NUM; i++)
    {
        if (strcmp(sAttrFlagsParam[i].strVal, strVal) == 0)
            return sAttrFlagsParam[i];
    }
    return not_found_param;
}
