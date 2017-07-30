#pragma once
/*!
 * \file	Mutex.h
 *
 * \author	BrianYi
 * \date	2017/07/28
 *
 * \brief	ª•≥‚À¯¿‡
 */

#include "p2_common.h"
#include "Thread.h"
P2_NAMESPACE_BEG

class Mutex
{
public:
	Mutex();
	~Mutex();
	void Lock();
	void Unlock();
    BOOL TryLock();
private:
    void RecursiveLock();
    void RecursiveUnlock();
    BOOL RecursiveTryLock();
private:
	CRITICAL_SECTION fMutex;
	uint32 fHolderCount;
	DWORD fHolder;
};

class MutexLocker
{
public:
    MutexLocker(Mutex *inMutex);
    ~MutexLocker();
    void Lock();
    void Unlock();
private:
    Mutex *fMutex;
};

P2_NAMESPACE_END