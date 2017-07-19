#include "TCPSocket.h"

P2_NAMESPACE_BEG

TCPSocket::TCPSocket(int inSocketID, Task *notifytask):
    EventContext(inSocketID, notifytask)
{
}

TCPSocket::~TCPSocket()
{
    Close();
}

void TCPSocket::Listen(const USHORT& inPort)
{

    CommonSocket::_Bind(inPort);

    if (::listen(this->fSocketID, kListenQueueLength) != 0)
    {
        printf("[TCP] listen_on_port error!\n");
        return;
    }
}

int32 TCPSocket::Connect(const Address& inAddress)
{
    int32 ret = ::connect(this->fSocketID, (const sockaddr*)&inAddress, sizeof(struct sockaddr));
    if (ret == 0)
        this->fRemoteAddress = inAddress;
    return ret;
}

int32 TCPSocket::Connect(const string& inIP, const USHORT& inPort)
{
    Address address(inIP, inPort);
    int32 ret = ::connect(this->fSocketID, (const sockaddr*)&address, sizeof(struct sockaddr));
    if (ret == 0)
        this->fRemoteAddress = address;
    return ret;
}

TCPSocket* TCPSocket::AcceptClient()
{
    int size = sizeof(struct sockaddr);
    Address address;
    SOCKET socketID = ::accept(this->fSocketID, (struct sockaddr*)&address, &size);
    TCPSocket *tcpSocket = new TCPSocket(socketID);
    tcpSocket->SetRemoteAddress(address);
    return tcpSocket;
}

int32 TCPSocket::Send(const char* inContent, const size_t& inSize)
{
    int32 sentBytes = ::send(this->fSocketID, inContent, inSize, 0);
    if (sentBytes == SOCKET_ERROR)
    {
        printf("[send] with %s:%u cannot finish!\n", this->fRemoteAddress.GetIP().c_str(), this->fRemoteAddress.GetPort());
    }
    return sentBytes;
}

int32 TCPSocket::Recv(char* outContent, const size_t& inSize)
{
    int32 receivedBytes = ::recv(this->fSocketID, outContent, inSize, 0);
    return receivedBytes;
}

void TCPSocket::ProcessEvent(int eventBits)
{
    if (fTask != nullptr)
        fTask->Signal(eventBits);

    this->RequestEvent(EV_RE);
}

P2_NAMESPACE_END

