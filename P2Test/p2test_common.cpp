#include "p2test_common.h"


param not_found_param = { 0, nullptr };

msg_code_param sMsgCodeParam[] =
{
    MSG_SERVER_GET_INFO,    _T("MSG_SERVER_GET_INFO"),
    MSG_SERVER_SET_INFO,    _T("MSG_SERVER_SET_INFO"),
    MSG_USER_GET_INFO,      _T("MSG_USER_GET_INFO"),
    MSG_USER_SET_INFO,      _T("MSG_USER_SET_INFO"),
    MSG_GROUP_GET_INFO,     _T("MSG_GROUP_GET_INFO"),
    MSG_GROUP_SET_INFO,     _T("MSG_GROUP_SET_INFO"),
    MSG_LOGIN,              _T("MSG_LOGIN"),
    MSG_LOGOUT,             _T("MSG_LOGOUT"),
    MSG_USER_MSG_PACKET,    _T("MSG_USER_MSG_PACKET"),
    MSG_GROUP_MSG_PACKET,   _T("MSG_GROUP_MSG_PACKET"),
    MSG_USER_ONLINE,        _T("MSG_USER_ONLINE"),
    MSG_USER_OFFLINE,       _T("MSG_USER_OFFLINE"),
    MSG_REQUEST_SUCCEED,    _T("MSG_REQUEST_SUCCEED"),
    MSG_REQUEST_FAILED,     _T("MSG_REQUEST_FAILED")
};

extern attr_code_param sAttrCodeParam[] =
{
    ATTR_SERVER_INFO,   _T("ATTR_SERVER_INFO"),

    ATTR_SESSION_ID,    _T("ATTR_SESSION_ID"),

    ATTR_USER_ID,       _T("ATTR_USER_ID"),
    ATTR_USER_PP,     _T("ATTR_USER_PP"),
    ATTR_USER_PASSWORD, _T("ATTR_USER_PASSWORD"),

    ATTR_GROUP_ID,      _T("ATTR_GROUP_ID"),
    ATTR_GROUP_NAME,    _T("ATTR_GROUP_NAME"),

    ATTR_MSG_PACKET,    _T("ATTR_MSG_PACKET")
};

extern attr_datatype_param sAttrDataTypeParam[] =
{
    dt_int16,   _T("dt_int16"),
    dt_int32,   _T("dt_int32"),
    dt_int64,   _T("dt_int64"),
    dt_float32, _T("dt_float32"),
    dt_float64, _T("dt_float64"),
    dt_binary,  _T("dt_binary"),
    dt_string,  _T("dt_string")
};

extern msg_flags_param sMsgFlagsParam[] =
{
    mf_none,        _T("mf_none"),
    mf_binary,      _T("mf_binary"),
    mf_end_of_file, _T("mf_end_of_file")
};

extern attr_flags_param sAttrFlagsParam[] =
{
    af_none,        _T("af_none"),
    af_signed,      _T("af_signed"),
    af_unsigned,    _T("af_unsigned")
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

msg_code_param find_msg_code_param_by_str(const TCHAR *strVal)
{
    for (size_t i = 0; i < MSG_CODE_NUM; i++)
    {
        if (_tcscmp(sMsgCodeParam[i].strVal, strVal) == 0)
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

attr_code_param find_attr_code_param_by_str(const TCHAR *strVal)
{
    for (size_t i = 0; i < ATTR_CODE_NUM; i++)
    {
        if (_tcscmp(sAttrCodeParam[i].strVal, strVal) == 0)
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

attr_datatype_param find_attr_datatype_param_by_str(const TCHAR *strVal)
{
    for (size_t i = 0; i < ATTR_DATA_TYPE_NUM; i++)
    {
        if (_tcscmp(sAttrDataTypeParam[i].strVal, strVal) == 0)
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

msg_flags_param find_msg_flags_param_by_str(const TCHAR *strVal)
{
    for (size_t i = 0; i < MSG_FLAGS_NUM; i++)
    {
        if (_tcscmp(sMsgFlagsParam[i].strVal, strVal) == 0)
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

attr_flags_param find_attr_flags_param_by_str(const TCHAR *strVal)
{
    for (size_t i = 0; i < ATTR_FLAGS_NUM; i++)
    {
        if (_tcscmp(sAttrFlagsParam[i].strVal, strVal) == 0)
            return sAttrFlagsParam[i];
    }
    return not_found_param;
}
