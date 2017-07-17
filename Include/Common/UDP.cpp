#include "UDP.h"

P2_NAMESPACE_BEG

UDP::UDP():CommonSocket(SOCK_DGRAM, IPPROTO_UDP)
{
}


// UDP::UDP(IOType inIOType): CommonSocket(SOCK_DGRAM, inIOType)
// {
// }

UDP::UDP(const UDP& inUDP) :CommonSocket(SOCK_DGRAM, IPPROTO_UDP)
{
    *this = inUDP;
}

UDP::~UDP()
{
}

void UDP::bind_port( const USHORT& inPort )
{
	if (!this->m_opened)
		this->open();

	if (!this->m_binded)
	{
		CommonSocket::bind_to_port(inPort);
	}
}

int32 UDP::send(const Address& inAddress, const char* inContent, const size_t& inSize)
{
	if (!this->m_opened)
		this->open();

// 	if (inSize > kMaxSendBufSize)
// 	{
// 		printf("Send buffer overflow!\n");
// 		return -1;
// 	}

	int32 sentBytes = ::sendto(this->m_socketID, inContent, inSize, 0, (const sockaddr*)&inAddress, sizeof(struct sockaddr));
	if (sentBytes == SOCKET_ERROR)
	{
		printf("[send] with %s:%u cannot finish!\n", inAddress.getIP().c_str(), inAddress.getPort());
	}
	return sentBytes;
}

int32 UDP::send(const std::string& inIP, const USHORT& inPort, const char* inContent, const size_t& inSize)
{
    if (!this->m_opened)
        this->open();

//     if (inSize > kMaxSendBufSize)
//     {
//         printf("Send buffer overflow!\n");
//         return -1;
//     }

    Address address(inIP, inPort);
    int32 sentBytes = ::sendto(this->m_socketID, inContent, inSize, 0, (const sockaddr*)&address, sizeof(struct sockaddr));
    if (sentBytes == SOCKET_ERROR)
    {
        printf("[send] with %s:%u cannot finish!\n", inIP.c_str(), inPort);
    }
    return sentBytes;
}

int32 UDP::receive(char* outContent, const size_t& inSize, IOType inIOType)
{
    if (!this->m_opened)
        this->open();
    if (!this->m_binded)
    {
        printf("Please first listen on port!\n");
        return -1;
    }

    if (this->m_ioType != inIOType)
        this->setIOType(inIOType);

    int32 size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->m_socketID, outContent, inSize, 0, NULL, NULL);
    return receivedBytes;
}

int32 UDP::receive(char* outContent, const size_t& inSize, Address& outAddress, IOType inIOType)
{
    if (!this->m_opened)
        this->open();
    if (!this->m_binded)
    {
        printf("Please first listen on port!\n");
        return -1;
    }

    if (this->m_ioType != inIOType)
        this->setIOType(inIOType);

    int32 size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->m_socketID, outContent, inSize, 0, (struct sockaddr*)&outAddress, &size);
    return receivedBytes;
}

int32 UDP::receive(char* outContent, const size_t& inSize, std::string& outIP, USHORT& outPort, IOType inIOType)
{
    if (!this->m_opened)
        this->open();
    if (!this->m_binded)
    {
        printf("Please first listen on port!\n");
        return -1;
    }

    if (this->m_ioType != inIOType)
        this->setIOType(inIOType);

    Address address;
    int32 size = sizeof(struct sockaddr);
    int32 receivedBytes = ::recvfrom(this->m_socketID, outContent, inSize, 0, (struct sockaddr*)&address, &size);
    outIP = address.getIP();
    outPort = address.getPort();
    return receivedBytes;
}

P2_NAMESPACE_END