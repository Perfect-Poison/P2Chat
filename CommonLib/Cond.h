#pragma once
/*!
 * \file	Cond.h
 *
 * \author	BrianYi
 * \date	2017/07/18
 *
 * \brief	����������
 */

#include "p2_common.h"
#include "Mutex.h"
P2_NAMESPACE_BEG

class Cond
{
public:
    Cond();
    ~Cond();

    void Signal();
    bool Wait(int32 inTimeoutInMilSecs = 0);
    void Broadcast();
private:
    HANDLE fCondition;
    uint32 fWaitCount;
};

P2_NAMESPACE_END