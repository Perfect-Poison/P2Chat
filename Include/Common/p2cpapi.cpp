#include "p2cpapi.h"

P2_NAMESPACE_BEG

/**
 *	Create new hash entry with given attribute size
 */
inline MessageAttr *CreateMessageAttr(size_t attrSize)
{
    size_t entrySize = sizeof(MessageAttr) - sizeof(MESSAGE_ATTR) + attrSize;
    MessageAttr *entry = (MessageAttr *)calloc(1, entrySize);
    entry->size = entrySize;
    return entry;
}

Message::Message()
{
    fCode = 0;
    fFlags = 0;
    fSize = 0;
    fMsgID = 0;
    fAttrNum = 0;
    fAttrs = nullptr;
    fData = nullptr;
}

Message::Message(MESSAGE *msg)
{
    fCode = ntohs(msg->code);
    fFlags = ntohs(msg->flags);
    fSize = ntohl(msg->size);
    fMsgID = ntohl(msg->msgID);
    fAttrNum = ntohl(msg->attrNum);
    fAttrs = nullptr;
    fData = nullptr;

    if (msg->flags & mf_binary) 
        fData = (BYTE *)memdup(msg->attrs, fSize);
    else
    {
//         uint32 totalAttrSize = CalculateTotalAttrSize(msg);
//         fMessage.attrs = malloc(totalAttrSize);
        BYTE *pDest = nullptr/*(BYTE *)fMessage.attrs*/;
        BYTE *pSour = (BYTE *)msg->attrs;
        uint32 attrSize = 0;
        for (uint32 attr_i = 0; attr_i < fAttrNum; attr_i++)
        {
            //MESSAGE_ATTR *attrDest = (MESSAGE_ATTR *)pDest;
            MESSAGE_ATTR *attrSour = (MESSAGE_ATTR *)pSour;
            attrSize = CalculateAttrSize(attrSour, true);

            MessageAttr *entry = CreateMessageAttr(attrSize);
            entry->code = ntohl(attrSour->code);
            memcpy(&entry->data, attrSour, attrSize);

            entry->data.code = ntohl(entry->data.code);
            switch (entry->data.dataType)
            {
            case dt_int16:
                entry->data.i16 = ntohs(entry->data.i16);
                break;
            case dt_int32:
                entry->data.i32 = ntohl(entry->data.i32);
                break;
            case dt_int64:
                entry->data.i64 = ntohll(entry->data.i64);
                break;
            case dt_float32:
                entry->data.f32 = ntohf(*((uint32*)&entry->data.f32));
                break;
            case dt_float64:
                entry->data.f64 = ntohd(*((uint64*)&entry->data.f64));
                break;
            case dt_binary:
                entry->data.bin.size = ntohl(entry->data.bin.size);
                break;
            case dt_string:
                entry->data.str.size = ntohl(entry->data.str.size);
                for (uint32 i = 0; i < entry->data.str.size / 2; i++)
                    entry->data.str.data[i] = ntohs(entry->data.str.data[i]);
                break;
            default:
                printf("Message::Message no such data type!\n");
                break;
            }
            HASH_ADD_INT(fAttrs, code, entry);

            pSour += attrSize;
            //pDest += attrSize;
        }
    }
}

Message::~Message()
{
    MessageAttr *entry, *tmp;
    HASH_ITER(hh, fAttrs, entry, tmp)
    {
        HASH_DEL(fAttrs, entry);
        safe_free(entry);
    }
}

MESSAGE* Message::CreateMessage()
{
    fSize = MESSAGE_HEADER_SIZE + CalculateTotalAttrSize(fAttrs, false);
    MESSAGE *msg = (MESSAGE *)malloc(fSize);
    msg->code = htons(fCode);
    msg->flags = htons(fFlags);
    msg->size = htonl(fSize);
    msg->msgID = htonl(fMsgID);
    msg->attrNum = htonl(fAttrNum);

    if (fFlags & mf_binary)
    {
        memcpy(msg->attrs, fData, fAttrNum);
    }
    else
    {
        BYTE *pDest = (BYTE *)msg->attrs;
        size_t attrSize = 0;
        MessageAttr *entry, *tmp;
        HASH_ITER(hh, fAttrs, entry, tmp)
        {
            MESSAGE_ATTR *attrDest = (MESSAGE_ATTR *)pDest;

            attrSize = CalculateAttrSize(&entry->data, false);
            memcpy(attrDest, &entry->data, attrSize);

            attrDest->code = htonl(attrDest->code);
            switch (attrDest->dataType)
            {
            case dt_int16:
                attrDest->i16 = htons(attrDest->i16);
                break;
            case dt_int32:
                attrDest->i32 = htonl(attrDest->i32);
                break;
            case dt_int64:
                attrDest->i64 = htonll(attrDest->i64);
                break;
            case dt_float32:
                attrDest->f32 = htonf(attrDest->f32);
                break;
            case dt_float64:
                attrDest->f64 = htond(attrDest->f64);
                break;
            case dt_binary:
                attrDest->bin.size = htonl(attrDest->bin.size);
                break;
            case dt_string:
                for (uint32 i = 0; i < attrDest->str.size / 2; i++)
                    attrDest->str.data[i] = htons(attrDest->str.data[i]);
                attrDest->str.size = htonl(attrDest->str.size);
                break;
            default:
                printf("Message::CreateMessage no such data type!\n");
                break;
            }
            pDest += attrSize;
        }
    }
    return msg;
}

void Message::set(attr_code attrCode, attr_datatype dataType, const void *value, attr_flags flags/* = af_none*/, uint32 size/* = 0*/)
{
    if (fFlags & mf_binary)
        return ;

    MessageAttr *entry;
    uint32 length;
    switch (dataType) 
    {
    case dt_int16:
        entry = CreateMessageAttr(ATTR_HEADER_SIZE + 2);
        entry->data.i16 = *((uint16*)value);
        break;
    case dt_int32:
        entry = CreateMessageAttr(ATTR_HEADER_SIZE + 4);
        entry->data.i32 = *((uint32*)value);
        break;
    case dt_int64:
        entry = CreateMessageAttr(ATTR_HEADER_SIZE + 8);
        entry->data.i64 = *((uint64*)value);
        break;
    case dt_float32:
        entry = CreateMessageAttr(ATTR_HEADER_SIZE + 4);
        entry->data.f32 = *((float32*)value);
        break;
    case dt_float64:
        entry = CreateMessageAttr(ATTR_HEADER_SIZE + 8);
        entry->data.f64 = *((float64*)value);
        break;
    case dt_binary:
        entry = CreateMessageAttr(ATTR_HEADER_SIZE + 4 + size);
        entry->data.bin.size = size;
        if (size > 0 && value)
            memcpy(entry->data.bin.data, value, size);
        break;
    case dt_string:
        // string类型都定义为宽字符
        length = size;
        entry = CreateMessageAttr(ATTR_HEADER_SIZE + 4 + length * 2);
        entry->data.str.size = length * 2;
        if (length > 0 && value)
            memcpy(entry->data.str.data, value, length * 2);
        break;
    default:
        printf("Message::set no such data type!\n");
        break;
    }
    entry->code = attrCode;
    entry->data.code = attrCode;
    entry->data.dataType = dataType;
    entry->data.flags = flags;

    // add or replace attribute
    MessageAttr *curr = findAttr(attrCode);
    if (curr) 
    {
        HASH_DEL(fAttrs, curr);
        safe_free(curr);
    }
    HASH_ADD_INT(fAttrs, code, entry);
}

void* Message::get(attr_code attrCode, attr_datatype dataType, void *buffer/* = nullptr*/, uint32 bufferSize/* = 0*/) const
{
    if (buffer != nullptr && bufferSize == 0)
        return nullptr;

    MessageAttr *msgAttr = findAttr(attrCode);
    if (msgAttr == nullptr)
        return nullptr;

    MESSAGE_ATTR *attr = &msgAttr->data;
    WCHAR *wstr = nullptr;
    uint32 length = 0;
    switch (dataType)
    {
    case dt_int16:
    case dt_int32:
    case dt_int64:
    case dt_float32:
    case dt_float64:
        return (void*)((BYTE*)attr + ATTR_HEADER_SIZE);
        break;
    case dt_binary:
    case dt_string:
        if (attr->str.data != nullptr)
        {
            if (buffer == nullptr)
                wstr = (WCHAR *)malloc(attr->str.size + 2);
            else 
                wstr = (WCHAR *)buffer;
            length = (buffer == nullptr) ? (attr->str.size / 2) : min(attr->str.size / 2, bufferSize - 1);
            memcpy(wstr, (BYTE *)attr->str.data, length * 2);
            wstr[length] = 0;
        }
        else 
        {
            if (buffer != nullptr)
            {
                wstr = (WCHAR *)buffer;
                wstr[0] = 0;
            }
        }
        return (void*)wstr;
        break;
    default:
        printf("Message::get no such data type!\n");
        break;
    }
    return nullptr;
}

/*static*/ uint32 Message::CalculateAttrSize(MESSAGE_ATTR *attr, bool networkByteOrder)
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

/*static */uint32 Message::CalculateTotalAttrSize(MESSAGE *msg, bool networkByteOrder)
{
    uint32 totalAttrSize = 0;
    msg_flags flags = networkByteOrder ? ntohs(msg->flags) : msg->flags;
    msg_attrnum attrnum = networkByteOrder ? ntohl(msg->attrNum) : msg->attrNum;
    if (flags & mf_binary)
        return attrnum;
    else
    {
        BYTE *p = (BYTE*)msg->attrs;
        for (uint32 attr_i = 0; attr_i < attrnum; attr_i++) 
        {
            totalAttrSize += CalculateAttrSize((MESSAGE_ATTR*)p, networkByteOrder);
            p += totalAttrSize;
        }
        return totalAttrSize;
    }
}

/*static */uint32 Message::CalculateTotalAttrSize(MessageAttr *attrs, bool networkByteOrder)
{
    MessageAttr *entry, *tmp;
    uint32 totalAttrSize = 0;
    HASH_ITER(hh, attrs, entry, tmp)
    {
        totalAttrSize += CalculateAttrSize(&entry->data, networkByteOrder);
    }
    return totalAttrSize;
}

MessageAttr * Message::findAttr(attr_code attrCode) const
{
    MessageAttr *entry;
    HASH_FIND_INT(fAttrs, &attrCode, entry);
    return entry;
}

P2_NAMESPACE_END