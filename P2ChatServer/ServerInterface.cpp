#include "ServerInterface.h"
P2_NAMESPACE_USE


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
