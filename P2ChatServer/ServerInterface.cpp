#include "ServerInterface.h"



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
