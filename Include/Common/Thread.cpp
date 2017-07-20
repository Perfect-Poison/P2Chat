#include "Common/Thread.h"

P2_NAMESPACE_BEG

DWORD Thread::sThreadStorageIndex = 0;
void* Thread::sMainThreadData = nullptr;

Thread::Thread(): 
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


Thread::~Thread()
{
    this->StopAndWaitForThread();
}

void Thread::Start()
{
    unsigned int theId = 0;
    fThreadID = (HANDLE)_beginthreadex(NULL, 0, _Entry, (void*)this, 0, &theId);
    Assert(fThreadID != nullptr);
}

int Thread::GetErrno()
{
    int winErr = ::GetLastError();
    switch (winErr)
    {

    case ERROR_FILE_NOT_FOUND: return ENOENT;
    case ERROR_PATH_NOT_FOUND: return ENOENT;

    case WSAEINTR:      return EINTR;
    case WSAENETRESET:  return EPIPE;
    case WSAENOTCONN:   return ENOTCONN;
    case WSAEWOULDBLOCK:return EAGAIN;
    case WSAECONNRESET: return EPIPE;
    case WSAEADDRINUSE: return EADDRINUSE;
    case WSAEMFILE:     return EMFILE;
    case WSAEINPROGRESS:return EINPROGRESS;
    case WSAEADDRNOTAVAIL: return EADDRNOTAVAIL;
    case WSAECONNABORTED: return EPIPE;
    case 0:             return 0;

    default:            return ENOTCONN;
    }
}

unsigned int WINAPI Thread::_Entry(LPVOID inThread)
{
    Thread *theThread = (Thread*)inThread;
    BOOL theErr = ::TlsSetValue(sThreadStorageIndex, theThread);
    Assert(theErr == TRUE);
    theThread->Entry();
    return NULL;
}

void Thread::Sleep(uint32 inMsec)
{
    ::Sleep(inMsec);
}

void Thread::Join()
{
    Assert(!fJoined);
    fJoined = true;
    DWORD theErr = ::WaitForSingleObject(fThreadID, INFINITE);

    // WAIT_OBJECT_0 表示等待的对象（比如线程、互斥锁）已经正常执行完成或完成释放
    Assert(theErr == WAIT_OBJECT_0);
}

void Thread::SendStopRequest()
{
    fStopRequested = true;
}

BOOL Thread::IsStopRequested()
{
    return fStopRequested;
}

void Thread::StopAndWaitForThread()
{
    fStopRequested = true;
    if (!fJoined) 
        Join();
}

void* Thread::GetThreadData()
{
    return fThreadData;
}

void Thread::SetThreadData(void* inThreadData)
{
    fThreadData = inThreadData;
}

DWORD Thread::GetThreadID()
{
    return (DWORD)fThreadID;
}

Thread* Thread::GetCurrent()
{
    return (Thread*)::TlsGetValue(sThreadStorageIndex);
}


P2_NAMESPACE_END