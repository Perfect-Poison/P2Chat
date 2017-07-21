#include "CommonSocket.h"

P2_NAMESPACE_BEG

CommonSocket::CommonSocket(int inSocketID):
    fSocketID(inSocketID)
{
}

CommonSocket::~CommonSocket()
{
}

void CommonSocket::Open(int32 inSocketType, int32 inProtocol)
{
    if ((this->fSocketID = ::socket(AF_INET, inSocketType, inProtocol)) == SOCKET_ERROR)
    {
        printf("Create socket error!\n");
        return;
    }
}

void CommonSocket::Close()
{
    if (this->fSocketID != kInvalidSocketID)
    {
        ::closesocket(this->fSocketID);
        this->fSocketID = kInvalidSocketID;
    }
}

void CommonSocket::_Bind(const USHORT& inPort)
{
    Address address(inPort);
    if (::bind(this->fSocketID, (const sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        printf("Binded socket error!\n");
        return;
    }
}

void CommonSocket::SetIOType(IOType inIOType)
{
    ULONG iMode = 0;
    switch (inIOType)
    {
    case kBlocking:
        iMode = 0;
        if (::ioctlsocket(this->fSocketID, FIONBIO, &iMode) == SOCKET_ERROR)
        {
            printf("Blocking: ioctlsocket failed!\n");
        }
        break;
    case kNonBlocking:
        iMode = 1;
        if (::ioctlsocket(this->fSocketID, FIONBIO, &iMode) == SOCKET_ERROR)
        {
            printf("Non-Blocking: ioctlsocket failed!\n");
        }
        break;
    default:
        printf("ERROR: No such IO type!\n");
        break;
    }
}

void CommonSocket::ReuseAddr()
{
    int one = 1;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[reuse_addr] error!\n");
        return;
    }
    return;
}

void CommonSocket::NoDelay()
{
    int one = 1;
    if (::setsockopt(this->fSocketID, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[no_delay] error!\n");
        return;
    }
    return;
}

void CommonSocket::KeepAlive()
{
    int one = 1;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_KEEPALIVE, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[keep_alive] error!\n");
        return;
    }
    return;
}

void CommonSocket::SetSocketSendBufferSize(uint32 inNewSize)
{
    int bufSize = inNewSize;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_SNDBUF, (char*)&bufSize, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[set_socket_sndbuf_size] error!\n");
        return;
    }
    return;
}

void CommonSocket::SetSocketRecvBufferSize(uint32 inNewSize)
{
    int bufSize = inNewSize;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[set_socket_rcvbuf_size] error!\n");
        return;
    }
    return;
}

P2_NAMESPACE_END