#include "p2cpapi.h"

P2_NAMESPACE_USE;

Message::Message()
{
    fMessage.code = 0;
    fMessage.flags = 0;
    fMessage.size = 0;
    fMessage.msgID = 0;
    fMessage.attrNum = 0;
    fMessage.attrs = nullptr;
}

Message::Message(MESSAGE *msg, bool networkByteOrder/* = true*/)
{
    fMessage.code = ntohs(msg->code);
    fMessage.flags = ntohs(msg->flags);
    fMessage.size = ntohl(msg->size);
    fMessage.msgID = ntohl(msg->msgID);
    fMessage.attrNum = ntohl(msg->attrNum);
    if (msg->flags & mf_binary) 
    {
        fMessage.attrs = malloc(msg->size);
        memcpy(fMessage.attrs, msg->attrs, fMessage.size);
    }
    else
    {
        uint32 totalAttrSize = CalculateTotalAttrSize(msg);
        fMessage.attrs = malloc(totalAttrSize);
        BYTE *p = (BYTE*)fMessage.attrs;
        for (int attr_i = 0; attr_i < fMessage.size; attr_i++) 
        {
            MESSAGE_ATTR *attrPtr = (MESSAGE_ATTR*)p;
            p += CalculateAttrSize(attrPtr);

        }
    }
}

Message::~Message()
{

}

MESSAGE* Message::CreateMessage()
{

}

void Message::set(attr_code attrCode, attr_datatype dataType, const void *value, attr_flags flags/* = af_none*/, uint32 size/* = 0*/)
{

}

void* Message::get(attr_code attrCode, attr_datatype dataType, attr_flags flags/* = af_none*/, void *buffer/* = nullptr*/, uint32 bufferSize/* = 0*/) const
{

}

/*static*/ uint32 Message::CalculateAttrSize(MESSAGE_ATTR *attr, bool networkByteOrder/* = true*/)
{
    uint32 attrSize = 0;
    switch (attr->dataType) 
    {
    case dt_int16:
        attrSize = ATTR_HEADER_SIZE + 2;
        break;
    case dt_int32:
    case dt_float32:
        attrSize = ATTR_HEADER_SIZE + 4;
        break;
    case dt_int64:
    case dt_float64:
        attrSize = ATTR_HEADER_SIZE + 8;
        break;
    case dt_binary:
    case dt_string:
        if (networkByteOrder)
            attrSize = ATTR_HEADER_SIZE + ntohl(attr->bin.size);
        else
            attrSize = ATTR_HEADER_SIZE + attr->bin.size;
        break;
    default:
        attrSize = ATTR_HEADER_SIZE;
        break;
    }
    return attrSize;
}

/*static */uint32 Message::CalculateTotalAttrSize(MESSAGE *msg, bool networkByteOrder/* = true*/)
{
    uint32 totalAttrSize = 0;
    msg_flags flags = networkByteOrder ? ntohs(msg->flags) : msg->flags;
    msg_attrnum attrnum = networkByteOrder ? ntohl(msg->attrNum) : msg->attrNum;
    if (flags & mf_binary)
        return attrnum;
    else
    {
        BYTE *p = (BYTE*)msg->attrs;
        for (auto attr_i = 0; attr_i < attrnum; attr_i++) 
        {
            totalAttrSize += CalculateAttrSize((MESSAGE_ATTR*)p, networkByteOrder);
            p += totalAttrSize;
        }
        return totalAttrSize;
    }
}