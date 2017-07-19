#include "CommonSocket.h"

P2_NAMESPACE_BEG

uint32 CommonSocket::sNumSockets = 0;

// CommonSocket::CommonSocket()
// {
//     this->sNumSockets++;
//     if (this->sNumSockets == 1)
//     {
//         WSADATA wsaData;
//         if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//             printf("WSAStartup error!\n");
//     }
// 
//     this->fSocketType = SOCK_STREAM;
//     this->fProtocol = IPPROTO_TCP;
//     this->fOpened = FALSE;
//     this->fBinded = FALSE;
//     this->fIOType = Blocking;
//     this->open();
//     this->setIOType(this->fIOType);
//     ::setsockopt(this->fSocketID, SOL_SOCKET, SO_SNDBUF, (char*)&SEND_BUF_SIZE, sizeof(int));
//     ::setsockopt(this->fSocketID, SOL_SOCKET, SO_RCVBUF, (char*)&RECV_BUF_SIZE, sizeof(int));
// }


CommonSocket::CommonSocket(int32 inSocketType, int32 inProtocol, IOType inIOType)
{
    this->sNumSockets++;
    if (this->sNumSockets == 1)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            printf("WSAStartup error!\n");
    }

    this->fSocketType = inSocketType;
    this->fProtocol = inProtocol;
    this->fOpened = FALSE;
    this->fBinded = FALSE;
    this->fIOType = inIOType;
    this->Open();
    this->SetIOType(this->fIOType);
    uint32 maxSendBufSize = kMaxSendBufSize;
    uint32 maxRecvBufSize = kMaxRecvBufSize;
    ::setsockopt(this->fSocketID, SOL_SOCKET, SO_SNDBUF, (char*)&maxSendBufSize, sizeof(int));
    ::setsockopt(this->fSocketID, SOL_SOCKET, SO_RCVBUF, (char*)&maxRecvBufSize, sizeof(int));
}

CommonSocket::~CommonSocket()
{
    this->sNumSockets--;
    if (this->sNumSockets == 0)
    {
        WSACleanup();
        printf("clean up all sockets!\n");
    }
}

void CommonSocket::Open()
{
    if (this->fOpened)
    {
        printf("Already opened!\n");
        return;
    }
    
    if ((this->fSocketID = ::socket(AF_INET, this->fSocketType, this->fProtocol)) == SOCKET_ERROR)
    {
        printf("Create socket error!\n");
        return;
    }
    this->fOpened = TRUE;
}

void CommonSocket::Close()
{
    if (this->fOpened)
    {
        ::closesocket(this->fSocketID);
        this->fOpened = FALSE;
        this->fBinded = FALSE;
    }
}

void CommonSocket::Bind(const USHORT& inPort)
{
    if (!this->fOpened)
        this->open();
    else
    {
        if (this->fBinded)
        {
            printf("Socket %u already binded!\n", this->fSocketID);
            return;
        }

        Address address(inPort);
        if (::bind(this->fSocketID, (const sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR)
        {
            printf("Binded socket error!\n");
            return;
        }
        this->fBinded = TRUE;
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
    this->fIOType = inIOType;
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