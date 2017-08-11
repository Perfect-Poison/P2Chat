#pragma once
#include "p2server_common.h"
P2_NAMESPACE_BEG

class Address : public sockaddr_in
{
public:
    Address();
    Address(const USHORT& inPort);
    Address(const TCHAR* inIP, const USHORT& inPort);
    Address(struct sockaddr_in inSockAddr);
    ~Address();
    TCHAR* GetIP() const;
    void SetIP(const TCHAR* inIP);
    USHORT GetPort() const;
    void SetPort(const USHORT& inPort);
private:
    void _Address(const TCHAR* inIP, const USHORT& inPort);
};

P2_NAMESPACE_END