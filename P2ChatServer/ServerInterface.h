#pragma once
#include "Common/common.h"

P2_NAMESPACE_BEG

class ServerInterface
{
public:
    ServerInterface();
    virtual ~ServerInterface();
    static uint32 GetNumProcessors();
};

P2_NAMESPACE_END