#include "Cond.h"

P2_NAMESPACE_BEG

Cond::Cond()
{
    fCondition = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

Cond::~Cond()
{
    BOOL theErr = ::CloseHandle(fCondition);
    Assert(theErr == TRUE);
}

/** 
 * \brief �����ź�
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
 * \brief �ȴ������ź�
 *
 * \date 2017/07/18
 *
 * \author BrianYi
 *
 * \retval 
 **/
void Cond::Wait(Mutex* inMutex, int32 inTimeoutInMilSecs /*= 0*/)
{
    DWORD theTimeout = INFINITE;
    if (inTimeoutInMilSecs > 0)
        theTimeout = inTimeoutInMilSecs;
    inMutex->Unlock();
    fWaitCount++;
    DWORD theErr = ::WaitForSingleObject(fCondition, theTimeout);
    fWaitCount--;
    Assert((theErr == WAIT_OBJECT_0) || (theErr == WAIT_TIMEOUT));
    inMutex->Lock();
}

/** 
 * \brief �����еȴ��̷߳����ź�
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


