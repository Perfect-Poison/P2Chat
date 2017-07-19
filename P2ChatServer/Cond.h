#pragma once
/*!
 * \file	Cond.h
 *
 * \author	BrianYi
 * \date	2017/07/18
 *
 * \brief	条件变量类
 */
#include "Common/common.h"
#include "Common/Mutex.h"
P2_NAMESPACE_BEG

class Cond
{
public:
    Cond();
    ~Cond();

    void Signal();
    void Wait(Mutex* inMutex, int32 inTimeoutInMilSecs = 0);
    void Broadcast();

private:
    HANDLE fCondition;
    uint32 fWaitCount;
};

P2_NAMESPACE_END