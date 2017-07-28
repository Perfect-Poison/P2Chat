#include "Address.h"

P2_NAMESPACE_BEG

Address::Address()
{
    _Address("0.0.0.0", 0);
}


Address::Address(const USHORT& inPort)
{
    _Address("0.0.0.0", inPort);
}

Address::Address(const string& inIP, const USHORT& inPort)
{
    _Address(inIP, inPort);
}

Address::Address(struct sockaddr_in inSockAddr)
{
    _Address(::inet_ntoa(inSockAddr.sin_addr), inSockAddr.sin_port);
}

Address::~Address()
{
}

string Address::GetIP() const
{
    return ::inet_ntoa(this->sin_addr);
}

void Address::SetIP(const string& inIP)
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

USHORT Address::GetPort() const
{
    return ntohs(this->sin_port);
}

void Address::SetPort(const USHORT& inPort)
{
    this->sin_port = htons(inPort);
}

void Address::_Address(const string& inIP, const USHORT& inPort)
{
    this->sin_family = AF_INET;
    this->SetIP(inIP);
    this->SetPort(inPort);
}

P2_NAMESPACE_END