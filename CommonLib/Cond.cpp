#include "Cond.h"

P2_NAMESPACE_BEG

Cond::Cond():
    fWaitCount(0)
{
    fCondition = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

Cond::~Cond()
{
    BOOL theErr = ::CloseHandle(fCondition);
    Assert(theErr == TRUE);
}

/** 
 * \brief 发送信号
 *
 * \date 2017/07/18
 *
 * \author BrianYi
 *
 * \retval 
 **/
void Cond::Signal()
{
    BOOL theErr = ::SetEvent(fCondition);
    Assert(theErr == TRUE);
}

/** 
 * \brief 等待接收信号
 *
 * \date 2017/07/18
 *
 * \author BrianYi
 *
 * \retval 
 **/
bool Cond::Wait(int32 inTimeoutInMilSecs /*= 0*/)
{
    DWORD theTimeout = INFINITE;
    if (inTimeoutInMilSecs > 0)
        theTimeout = inTimeoutInMilSecs;
    //inMutex->Unlock();
    fWaitCount++;
    DWORD retVal = ::WaitForSingleObject(fCondition, theTimeout);
    fWaitCount--;
    Assert((retVal == WAIT_OBJECT_0) || (retVal == WAIT_TIMEOUT));
    //inMutex->Lock();
    return retVal == WAIT_OBJECT_0;
}

/** 
 * \brief 向所有等待线程发送信号
 *
 * \date 2017/07/18
 *
 * \author BrianYi
 *
 * \retval 
 **/
void Cond::Broadcast()
{
    uint32 waitCount = fWaitCount;
    for (uint32 x = 0; x < waitCount; x++) 
    {
        BOOL theErr = ::SetEvent(fCondition);
        Assert(theErr == TRUE);
    }
}

P2_NAMESPACE_END


