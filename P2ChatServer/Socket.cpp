#include "Socket.h"

P2_NAMESPACE_BEG

Socket::Socket(int inSocketID):
    fSocketID(inSocketID)
{
}

Socket::~Socket()
{
}

void Socket::Open(int32 inSocketType, int32 inProtocol)
{
    if ((this->fSocketID = ::socket(AF_INET, inSocketType, inProtocol)) == SOCKET_ERROR)
    {
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::Open Create socket error!\n"));
        return;
    }
}

void Socket::Close()
{
    if (this->fSocketID != kInvalidSocketID)
    {
        ::closesocket(this->fSocketID);
        this->fSocketID = kInvalidSocketID;
    }
}

void Socket::_Bind(const USHORT& inPort)
{
    Address address(inPort);
    if (::bind(this->fSocketID, (const sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::_Bind Binded socket error!\n"));
        return;
    }
}

void Socket::SetIOType(IOType inIOType)
{
    ULONG iMode = 0;
    switch (inIOType)
    {
    case kBlocking:
        iMode = 0;
        if (::ioctlsocket(this->fSocketID, FIONBIO, &iMode) == SOCKET_ERROR)
        {
            if (SOCKET_DEBUG)
                log_debug(7, _T("[error]Socket::SetIOType Blocking: ioctlsocket failed!\n"));
        }
        break;
    case kNonBlocking:
        iMode = 1;
        if (::ioctlsocket(this->fSocketID, FIONBIO, &iMode) == SOCKET_ERROR)
        {
            if (SOCKET_DEBUG)
                log_debug(7, _T("[error]Socket::SetIOType Non-Blocking: ioctlsocket failed!\n"));
        }
        break;
    default:
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::SetIOType No such IO type!\n"));
        break;
    }
}

void Socket::ReuseAddr()
{
    int one = 1;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::ReuseAddr error!\n"));
        return;
    }
    return;
}

void Socket::NoDelay()
{
    int one = 1;
    if (::setsockopt(this->fSocketID, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::NoDelay error!\n"));
        return;
    }
    return;
}

void Socket::KeepAlive()
{
    int one = 1;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_KEEPALIVE, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::KeepAlive error!\n"));
        return;
    }
    return;
}

void Socket::SetSocketSendBufferSize(uint32 inNewSize)
{
    int bufSize = inNewSize;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_SNDBUF, (char*)&bufSize, sizeof(int)) == SOCKET_ERROR)
    {
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::SetSocketSendBufferSize error!\n"));
        return;
    }
    return;
}

void Socket::SetSocketRecvBufferSize(uint32 inNewSize)
{
    int bufSize = inNewSize;
    if (::setsockopt(this->fSocketID, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize, sizeof(int)) == SOCKET_ERROR)
    {
        if (SOCKET_DEBUG)
            log_debug(7, _T("[error]Socket::SetSocketRecvBufferSize error!\n"));
        return;
    }
    return;
}

P2_NAMESPACE_END