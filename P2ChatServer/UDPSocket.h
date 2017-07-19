#pragma once
#include "EventContext.h"
#include "Common/common.h"
#include "Common/CommonSocket.h"

P2_NAMESPACE_BEG

class UDPSocket :
    public EventContext, public CommonSocket
{
public:
    UDPSocket();
    virtual ~UDPSocket();
};

P2_NAMESPACE_END