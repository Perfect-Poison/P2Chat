#include "UDPSocket.h"

P2_NAMESPACE_BEG

UDPSocket::UDPSocket(int inSocketID, Task *notifytask):
    EventContext(inSocketID, notifytask)
{
}


UDPSocket::~UDPSocket()
{
    Close();
}

void UDPSocket::Open()
{
    if (UDPSOCKET_DEBUG)
    {
        if (fSocketID != kInvalidSocketID)
            printf("UDPSocket::Open 已经有socket %u\n", fSocketID);
    }

    CommonSocket::Open(SOCK_DGRAM, IPPROTO_UDP);
}

void UDPSocket::Bind(const USHORT& inPort)
{
    _Bind(inPort);
}

int32 UDPSocket::SendTo(const Address& inAddress, const char* inContent, const size_t& inSize)
{
    int32 sentBytes = ::sendto(this->fSocketID, inContent, inSize, 0, (const sockaddr*)&inAddress, sizeof(struct sockaddr));
    if (sentBytes == SOCKET_ERROR)
    {
        printf("[send] with %s:%u cannot finish!\n", inAddress.GetIP().c_str(), inAddress.GetPort());
    }
    return sentBytes;
}

int32 UDPSocket::SendTo(const string& inIP, const USHORT& inPort, const char* inContent, const size_t& inSize)
{
    Address address(inIP, inPort);
    int32 sentBytes = ::sendto(this->fSocketID, inContent, inSize, 0, (const sockaddr*)&address, sizeof(struct sockaddr));
    if (sentBytes == SOCKET_ERROR)
    {
        printf("[send] with %s:%u cannot finish!\n", inIP.c_str(), inPort);
    }
    return sentBytes;
}

int32 UDPSocket::RecvFrom(char* outContent, const size_t& inSize)
{
    int32 size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->fSocketID, outContent, inSize, 0, nullptr, nullptr);
    return receivedBytes;
}

int32 UDPSocket::RecvFrom(char* outContent, const size_t& inSize, Address& outAddress)
{
    int size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->fSocketID, outContent, inSize, 0, (struct sockaddr*)&outAddress, &size);
    return receivedBytes;
}

int32 UDPSocket::RecvFrom(char* outContent, const size_t& inSize, string& outIP, USHORT& outPort)
{
    Address address;
    int size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->fSocketID, outContent, inSize, 0, (struct sockaddr*)&address, &size);
    outIP = address.GetIP();
    outPort = address.GetPort();
    return receivedBytes;
}

void UDPSocket::ProcessEvent(int eventBits)
{
    if (UDPSOCKET_DEBUG)
    {
        if (this->fTask != nullptr)
            printf("[error]: UDPSocket::ProcessEvent 已经有task了\n");
    }

    UDPTask *udpTask = new UDPTask(this);
    this->SetTask(udpTask);
    udpTask->Signal(eventBits);
}

P2_NAMESPACE_END

