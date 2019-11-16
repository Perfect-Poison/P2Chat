#pragma once
/*!
 * \file	Thread.h
 *
 * \author	BrianYi
 * \date	2017/07/28
 *
 * \brief	œﬂ≥Ã¿‡
 */

#include "p2_common.h"
P2_NAMESPACE_BEG

class Thread
{
public:
    Thread();
    virtual ~Thread();
    virtual void Entry() = 0;
    void Start();
    static void Sleep(uint32 inMsec);
    void Join();
    void SendStopRequest();
    BOOL IsStopRequested();
    void StopAndWaitForThread();
    void* GetThreadData();
    void SetThreadData(void* inThreadData);
    DWORD GetThreadID();
    static Thread* GetCurrent();
    static int          GetErrno();
private:
    static unsigned int WINAPI _Entry(LPVOID inThread);
private:
    HANDLE fThreadID;
    BOOL fStopRequested;
    BOOL fJoined;
    void *fThreadData;
    static DWORD sThreadStorageIndex;
    static void* sMainThreadData;
};

P2_NAMESPACE_END