#pragma once
#include "common.h"
#include <string>

P2_NAMESPACE_BEG

class Address : public sockaddr_in
{
public:
    Address();
    Address(const USHORT& inPort);
    Address(const std::string& inIP, const USHORT& inPort);
    Address(struct sockaddr_in inSockAddr);
    ~Address();
    std::string getIP() const;
    void setIP(const std::string& inIP);
    USHORT getPort() const;
    void setPort(const USHORT& inPort);
private:
    void _address(const std::string& inIP, const USHORT& inPort);
};

P2_NAMESPACE_END