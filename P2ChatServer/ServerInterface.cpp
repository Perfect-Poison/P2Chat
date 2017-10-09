#include "ServerInterface.h"
#include "Session.h"
P2_NAMESPACE_BEG


ServerInterface::ServerInterface()
{
    
}


ServerInterface::~ServerInterface()
{
}

uint32 ServerInterface::GetNumProcessors()
{
    SYSTEM_INFO theSystemInfo;
    ::GetSystemInfo(&theSystemInfo);
    return theSystemInfo.dwNumberOfProcessors;
}

P2_NAMESPACE_END