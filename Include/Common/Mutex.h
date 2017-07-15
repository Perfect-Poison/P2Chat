#pragma once
#include "Common/common.h"
#include "Common/Thread.h"

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
	UINT32 fHolderCount;
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