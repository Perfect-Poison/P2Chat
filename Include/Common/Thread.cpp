#include "Common/Thread.h"

DWORD p2::Thread::sThreadStorageIndex = 0;
void* p2::Thread::sMainThreadData = nullptr;

p2::Thread::Thread(): 
    fStopRequested(false),
    fJoined(false),
    fThreadData(nullptr)
{
    if (!sThreadStorageIndex) 
    {
        sThreadStorageIndex = ::TlsAlloc();
        Assert(sThreadStorageIndex >= 0);
    }
}


p2::Thread::~Thread()
{
    this->StopAndWaitForThread();
}

void p2::Thread::Start()
{
    unsigned int theId = 0;
    fThreadID = (HANDLE)_beginthreadex(NULL, 0, _Entry, (void*)this, 0, &theId);
    Assert(fThreadID != nullptr);
}

unsigned int WINAPI p2::Thread::_Entry(LPVOID inThread)
{
    Thread *theThread = (Thread*)inThread;
    BOOL theErr = ::TlsSetValue(sThreadStorageIndex, theThread);
    Assert(theErr == TRUE);
    theThread->Entry();
    return NULL;
}

void p2::Thread::Sleep(UINT32 inMsec)
{
    ::Sleep(inMsec);
}

void p2::Thread::Join()
{
    Assert(!fJoined);
    fJoined = true;
    DWORD theErr = ::WaitForSingleObject(fThreadID, INFINITE);

    // WAIT_OBJECT_0 表示等待的对象（比如线程、互斥锁）已经正常执行完成或完成释放
    Assert(theErr == WAIT_OBJECT_0);
}

void p2::Thread::SendStopRequest()
{
    fStopRequested = true;
}

BOOL p2::Thread::IsStopRequested()
{
    return fStopRequested;
}

void p2::Thread::StopAndWaitForThread()
{
    fStopRequested = true;
    if (!fJoined) 
        Join();
}

void* p2::Thread::GetThreadData()
{
    return fThreadData;
}

void p2::Thread::SetThreadData(void* inThreadData)
{
    fThreadData = inThreadData;
}

DWORD p2::Thread::GetCurrentThreadID()
{
    return ::GetCurrentThreadId();
}

p2::Thread* p2::Thread::GetCurrent()
{
    return (Thread*)::TlsGetValue(sThreadStorageIndex);
}


