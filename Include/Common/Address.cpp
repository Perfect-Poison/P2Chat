#include "Address.h"

P2_NAMESPACE_BEG

Address::Address()
{
    _address("0.0.0.0", 0);
}


Address::Address(const USHORT& inPort)
{
    _address("0.0.0.0", inPort);
}

Address::Address(const std::string& inIP, const USHORT& inPort)
{
    _address(inIP, inPort);
}

Address::Address(struct sockaddr_in inSockAddr)
{
    _address(::inet_ntoa(inSockAddr.sin_addr), inSockAddr.sin_port);
}

Address::~Address()
{
}

std::string Address::getIP() const
{
    return ::inet_ntoa(this->sin_addr);
}

void Address::setIP(const std::string& inIP)
{
    ULONG address = ::inet_addr(inIP.c_str());
    if (address == INADDR_NONE)
    {
        printf("Invalid ip address!\n");
    }
    else
    {
        this->sin_addr.s_addr = address;
    }
}

USHORT Address::getPort() const
{
    return ntohs(this->sin_port);
}

void Address::setPort(const USHORT& inPort)
{
    this->sin_port = htons(inPort);
}

void Address::_address(const std::string& inIP, const USHORT& inPort)
{
    this->sin_family = AF_INET;
    this->setIP(inIP);
    this->setPort(inPort);
}

P2_NAMESPACE_END