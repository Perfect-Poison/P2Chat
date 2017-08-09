#include "UDPSocket.h"
#include "UDPSession.h"

P2_NAMESPACE_BEG

UDPSocket::UDPSocket(int inSocketID):
    Event(inSocketID)
{
    SetEventName("UDPSocket");
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
            log_debug(7, "UDPSocket::Open ÒÑ¾­ÓÐsocket %u\n", fSocketID);
    }

    Socket::Open(SOCK_DGRAM, IPPROTO_UDP);
}

void UDPSocket::Bind(const USHORT& inPort)
{
    _Bind(inPort);
}

int32 UDPSocket::SendTo(const Address& inAddress, const BYTE* inContent, const size_t& inSize)
{
    int32 sentBytes = ::sendto(this->fSocketID, (char *)inContent, inSize, 0, (const sockaddr*)&inAddress, sizeof(struct sockaddr));
    if (sentBytes == SOCKET_ERROR)
    {
        if (UDPSOCKET_DEBUG)
            log_debug(7, "UDPSocket::SendTo send to %s:%u cannot finish!\n", inAddress.GetIP().c_str(), inAddress.GetPort());
    }
    return sentBytes;
}

int32 UDPSocket::SendTo(const string& inIP, const USHORT& inPort, const BYTE* inContent, const size_t& inSize)
{
    Address address(inIP, inPort);
    int32 sentBytes = ::sendto(this->fSocketID, (char *)inContent, inSize, 0, (const sockaddr*)&address, sizeof(struct sockaddr));
    if (sentBytes == SOCKET_ERROR)
    {
        if (UDPSOCKET_DEBUG)
            log_debug(7, "UDPSocket::SendTo send to %s:%u cannot finish!\n", inIP.c_str(), inPort);
    }
    return sentBytes;
}

int32 UDPSocket::RecvFrom(BYTE* outContent, const size_t& inSize)
{
    int32 size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->fSocketID, (char *)outContent, inSize, 0, nullptr, nullptr);
    return receivedBytes;
}

int32 UDPSocket::RecvFrom(BYTE* outContent, const size_t& inSize, Address& outAddress)
{
    int size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->fSocketID, (char *)outContent, inSize, 0, (struct sockaddr*)&outAddress, &size);
    return receivedBytes;
}

int32 UDPSocket::RecvFrom(BYTE* outContent, const size_t& inSize, string& outIP, USHORT& outPort)
{
    Address address;
    int size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->fSocketID, (char *)outContent, inSize, 0, (struct sockaddr*)&address, &size);
    outIP = address.GetIP();
    outPort = address.GetPort();
    return receivedBytes;
}

void UDPSocket::ProcessEvent(int eventBits)
{
    UDPSession *udpSession = new UDPSession(this);
    udpSession->Signal(eventBits);
}

P2_NAMESPACE_END

