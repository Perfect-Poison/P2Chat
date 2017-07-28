#pragma once
#include "p2server_common.h"

P2_NAMESPACE_BEG

class Address : public sockaddr_in
{
public:
    Address();
    Address(const USHORT& inPort);
    Address(const std::string& inIP, const USHORT& inPort);
    Address(struct sockaddr_in inSockAddr);
    ~Address();
    string GetIP() const;
    void SetIP(const std::string& inIP);
    USHORT GetPort() const;
    void SetPort(const USHORT& inPort);
private:
    void _Address(const std::string& inIP, const USHORT& inPort);
};

P2_NAMESPACE_END