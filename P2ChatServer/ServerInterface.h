#pragma once
#include "p2server_common.h"

P2_NAMESPACE_BEG

class ServerInterface
{
public:
    ServerInterface();
    virtual ~ServerInterface();
    static uint32 GetNumProcessors();
protected:
private:
    enum
    {
        kMaxServerHeaderLen = 1000
    };


};

P2_NAMESPACE_END