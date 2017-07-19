#include "TCPSocket.h"

P2_NAMESPACE_BEG

TCPSocket::TCPSocket(Task *notifytask, IOType inIOType):
    EventContext(EventContext::kInvalidFileDesc, notifytask),
    CommonSocket(SOCK_STREAM, IPPROTO_TCP, inIOType)
{

}

TCPSocket::~TCPSocket()
{
    Close();
}

void TCPSocket::Listen(const USHORT& inPort)
{
    if (!this->fOpened)
        this->Open();

    if (!this->fBinded)
    {
        CommonSocket::Bind(inPort);
    }

    if (::listen(this->fSocketID, kListenQueueLength) != 0)
    {
        printf("[TCP] listen_on_port error!\n");
        return;
    }
}

int32 TCPSocket::Connect(const Address& inAddress)
{
    if (this->fBinded)
    {
        printf("Socket %u already binded!\n", this->fSocketID);
        return -1;
    }
    if (!this->fOpened)
        this->Open();

    int32 ret = ::connect(this->fSocketID, (const sockaddr*)&inAddress, sizeof(struct sockaddr));
    if (ret == 0)
    {
        this->fRemoteAddress = inAddress;
        this->fBinded = TRUE;
    }
    return ret;
}

int32 TCPSocket::Connect(const std::string& inIP, const USHORT& inPort)
{
    if (this->fBinded)
    {
        printf("Socket %u already binded!\n", this->fSocketID);
        return -1;
    }
    if (!this->fOpened)
        this->Open();

    Address address(inIP, inPort);
    int32 ret = ::connect(this->fSocketID, (const sockaddr*)&address, sizeof(struct sockaddr));
    if (ret == 0)
    {
        this->fRemoteAddress = address;
        this->fBinded = TRUE;
    }
    return ret;
}

TCPSocket* TCPSocket::AcceptClient(IOType inIOType /*= kBlocking*/)
{
    if (this->fIOType != inIOType)
        this->SetIOType(inIOType);

    int size = sizeof(struct sockaddr);
    Address address;
    SOCKET socketID = ::accept(this->fSocketID, (struct sockaddr*)&address, &size);
    TCPSocket *tcpSocket = new TCPSocket;
    tcpSocket->fSocketID = socketID;
    tcpSocket->fBinded = TRUE;
    tcpSocket->fOpened = TRUE;
    tcpSocket->fRemoteAddress = address;
//     client.m_socketID = socketID;
//     client.m_binded = TRUE;
//     client.m_opened = TRUE;
//     client.m_address = address;
    return tcpSocket;
}

int32 TCPSocket::Send(const char* inContent, const size_t& inSize)
{
    if (!this->fOpened)
        this->Open();

    int32 sentBytes = ::send(this->fSocketID, inContent, inSize, 0);
    if (sentBytes == SOCKET_ERROR)
    {
        printf("[send] with %s:%u cannot finish!\n", this->fRemoteAddress.GetIP().c_str(), this->fRemoteAddress.GetPort());
    }
    return sentBytes;
}

int32 TCPSocket::Receive(char* outContent, const size_t& inSize, IOType inIOType /*= kBlocking*/)
{
    if (!this->fOpened)
        this->Open();
    if (!this->fBinded)
    {
        printf("Please first listen on port!\n");
        return -1;
    }

    if (this->fIOType != inIOType)
        this->SetIOType(inIOType);

    int32 receivedBytes = ::recv(this->fSocketID, outContent, inSize, 0);
    return receivedBytes;
}

P2_NAMESPACE_END