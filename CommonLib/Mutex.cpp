#include "Mutex.h"


p2::Mutex::Mutex()
{
	::InitializeCriticalSection(&fMutex);
	fHolder = 0;
	fHolderCount = 0;
}


p2::Mutex::~Mutex()
{
	::DeleteCriticalSection(&fMutex);
}


void p2::Mutex::Lock()
{
	this->RecursiveLock();
}

void p2::Mutex::Unlock()
{
	this->RecursiveUnlock();
}

void p2::Mutex::RecursiveLock()
{
	if (::GetCurrentThreadId() == fHolder)
	{
		fHolderCount++;
		return;
	}
	::EnterCriticalSection(&fMutex);
    Assert(fHolder == 0);
	fHolder = GetCurrentThreadId();
	fHolderCount++;
    Assert(fHolderCount == 1);
}

void p2::Mutex::RecursiveUnlock()
{
    if (GetCurrentThreadId() != fHolder)
        return;

    Assert(fHolderCount > 0);
    fHolderCount--;
    if (fHolderCount == 0) 
    {
        fHolder = 0;
        ::LeaveCriticalSection(&fMutex);
    }
}

BOOL p2::Mutex::RecursiveTryLock()
{
    if (GetCurrentThreadId() == fHolder) 
    {
        fHolderCount++;
        return true;
    }

    BOOL theErr = ::TryEnterCriticalSection(&fMutex);
    if (!theErr)
        return theErr;

    Assert(fHolder == 0);
    fHolder = GetCurrentThreadId();
    fHolderCount++;
    Assert(fHolderCount == 1);
    return true;
}

BOOL p2::Mutex::TryLock()
{
    return RecursiveTryLock();
}

p2::MutexLocker::MutexLocker(Mutex *inMutex):
    fMutex(inMutex)
{
    if (fMutex)
        fMutex->Lock();
}

p2::MutexLocker::~MutexLocker()
{
    if (fMutex)
        fMutex->Unlock();
}

void p2::MutexLocker::Lock()
{
    if (fMutex)
        fMutex->Lock();
}

void p2::MutexLocker::Unlock()
{
    if (fMutex)
        fMutex->Unlock();
}
