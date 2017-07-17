#include "CommonSocket.h"

P2_NAMESPACE_BEG

uint32 CommonSocket::s_num_sockets = 0;

// CommonSocket::CommonSocket()
// {
//     this->s_num_sockets++;
//     if (this->s_num_sockets == 1)
//     {
//         WSADATA wsaData;
//         if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//             printf("WSAStartup error!\n");
//     }
// 
//     this->m_socketType = SOCK_STREAM;
//     this->m_protocol = IPPROTO_TCP;
//     this->m_opened = FALSE;
//     this->m_binded = FALSE;
//     this->m_ioType = Blocking;
//     this->open();
//     this->setIOType(this->m_ioType);
//     ::setsockopt(this->m_socketID, SOL_SOCKET, SO_SNDBUF, (char*)&SEND_BUF_SIZE, sizeof(int));
//     ::setsockopt(this->m_socketID, SOL_SOCKET, SO_RCVBUF, (char*)&RECV_BUF_SIZE, sizeof(int));
// }


CommonSocket::CommonSocket(int32 inSocketType, int32 inProtocol, IOType inIOType)
{
    this->s_num_sockets++;
    if (this->s_num_sockets == 1)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            printf("WSAStartup error!\n");
    }

    this->m_socketType = inSocketType;
    this->m_protocol = inProtocol;
    this->m_opened = FALSE;
    this->m_binded = FALSE;
    this->m_ioType = inIOType;
    this->open();
    this->setIOType(this->m_ioType);
    uint32 maxSendBufSize = kMaxSendBufSize;
    uint32 maxRecvBufSize = kMaxRecvBufSize;
    ::setsockopt(this->m_socketID, SOL_SOCKET, SO_SNDBUF, (char*)&maxSendBufSize, sizeof(int));
    ::setsockopt(this->m_socketID, SOL_SOCKET, SO_RCVBUF, (char*)&maxRecvBufSize, sizeof(int));
}

CommonSocket::~CommonSocket()
{
    this->s_num_sockets--;
    if (this->s_num_sockets == 0)
    {
        WSACleanup();
        printf("clean up all sockets!\n");
    }
}

void CommonSocket::open()
{
    if (this->m_opened)
    {
        printf("Already opened!\n");
        return;
    }
    
    if ((this->m_socketID = ::socket(AF_INET, this->m_socketType, this->m_protocol)) == SOCKET_ERROR)
    {
        printf("Create socket error!\n");
        return;
    }
    this->m_opened = TRUE;
}

void CommonSocket::close()
{
    if (this->m_opened)
    {
        ::closesocket(this->m_socketID);
        this->m_opened = FALSE;
        this->m_binded = FALSE;
    }
}

void CommonSocket::bind_to_port(const USHORT& inPort)
{
    if (!this->m_opened)
        this->open();
    else
    {
        if (this->m_binded)
        {
            printf("Socket %u already binded!\n", this->m_socketID);
            return;
        }

        Address address(inPort);
        if (::bind(this->m_socketID, (const sockaddr*)&address, sizeof(struct sockaddr)) == SOCKET_ERROR)
        {
            printf("Binded socket error!\n");
            return;
        }
        this->m_binded = TRUE;
    }
}

void CommonSocket::setIOType(IOType inIOType)
{
    ULONG iMode = 0;
    switch (inIOType)
    {
    case kBlocking:
        iMode = 0;
        if (::ioctlsocket(this->m_socketID, FIONBIO, &iMode) == SOCKET_ERROR)
        {
            printf("Blocking: ioctlsocket failed!\n");
        }
        break;
    case kNonBlocking:
        iMode = 1;
        if (::ioctlsocket(this->m_socketID, FIONBIO, &iMode) == SOCKET_ERROR)
        {
            printf("Non-Blocking: ioctlsocket failed!\n");
        }
        break;
    default:
        printf("ERROR: No such IO type!\n");
        break;
    }
    this->m_ioType = inIOType;
}

void CommonSocket::reuse_addr()
{
    int one = 1;
    if (::setsockopt(this->m_socketID, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[reuse_addr] error!\n");
        return;
    }
    return;
}

void CommonSocket::no_delay()
{
    int one = 1;
    if (::setsockopt(this->m_socketID, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[no_delay] error!\n");
        return;
    }
    return;
}

void CommonSocket::keep_alive()
{
    int one = 1;
    if (::setsockopt(this->m_socketID, SOL_SOCKET, SO_KEEPALIVE, (char*)&one, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[keep_alive] error!\n");
        return;
    }
    return;
}

void CommonSocket::set_socket_sndbuf_size(uint32 inNewSize)
{
    int bufSize = inNewSize;
    if (::setsockopt(this->m_socketID, SOL_SOCKET, SO_SNDBUF, (char*)&bufSize, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[set_socket_sndbuf_size] error!\n");
        return;
    }
    return;
}

void CommonSocket::set_socket_rcvbuf_size(uint32 inNewSize)
{
    int bufSize = inNewSize;
    if (::setsockopt(this->m_socketID, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize, sizeof(int)) == SOCKET_ERROR)
    {
        printf("[set_socket_rcvbuf_size] error!\n");
        return;
    }
    return;
}

P2_NAMESPACE_END