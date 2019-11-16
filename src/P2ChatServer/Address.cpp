#include "Address.h"

P2_NAMESPACE_BEG

Address::Address()
{
    _Address(_T("0.0.0.0"), 0);
}


Address::Address(const USHORT& inPort)
{
    _Address(_T("0.0.0.0"), inPort);
}

Address::Address(const TCHAR* inIP, const USHORT& inPort)
{
    _Address(inIP, inPort);
}

Address::Address(struct sockaddr_in inSockAddr)
{
    TCHAR *buffer = _t_inet_ntoa(inSockAddr.sin_addr);
    _Address(buffer, inSockAddr.sin_port);
    safe_free(buffer);
}

Address::~Address()
{
}

TCHAR* Address::GetIP() const
{
    return _t_inet_ntoa(this->sin_addr);
}

void Address::SetIP(const TCHAR* inIP)
{
    ULONG address = _t_inet_addr(inIP);
    if (address == INADDR_NONE)
    {
        _tprintf(_T("Invalid ip address!\n"));
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

bool operator==(const Address& a1, const Address& a2)
{
    if ((sizeof(a1) == sizeof(a2)) && (memcmp((const void *)&a1, (const void *)&a2, sizeof(a1)) == 0))
        return true;
    return false;
}

void Address::_Address(const TCHAR* inIP, const USHORT& inPort)
{
    this->sin_family = AF_INET;
    this->SetIP(inIP);
    this->SetPort(inPort);
}

P2_NAMESPACE_END