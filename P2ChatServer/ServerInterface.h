#pragma once
#include "Common/common.h"

class ServerInterface
{
public:
    ServerInterface();
    virtual ~ServerInterface();
    static UINT32 GetNumProcessors();
};

