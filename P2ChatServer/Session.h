#pragma once
#include "p2_common.h"
#include "p2_util.h"
P2_NAMESPACE_BEG



class Session
{
public:
    Session();
    virtual ~Session();
private:
    int64   fSessionId;
    time_t  fCreationTime;
    time_t  fLastAccessTime;
};

P2_NAMESPACE_END