#pragma once

#include "Common/common.h"
#include "Common/Mutex.h"
#include "Task.h"
#include "Cond.h"
#include <queue>
#include <vector>

P2_NAMESPACE_BEG

#define TASK_DEBUG 1

class TaskThread;
class TaskThreadPool;
class Task
{
public:
    enum
    {
        /**
         *	以下为事件驱动的任务
         */
        kReadEvent = FD_READ,
        kWriteEvent = FD_WRITE,
        kAcceptEvent = FD_ACCEPT,
        kConnectEvent = FD_CONNECT,
        kCloseEvent = FD_CLOSE,

        /**
         *	以下为自定义的任务
         */
        kKillEvent = 0x00000040,
        kAlive = 0x80000000,
        kAliveOff = 0x7fffffff
    };
    typedef unsigned int EventFlags;
    Task();
    virtual ~Task();
    virtual int64 Run() = 0;
    void Signal(EventFlags eventFlags);
    void SetTaskName(const string& name) { fTaskName = name; }
    string GetTaskName() const { return fTaskName; }
    BOOL IsAlive() const { return fEventFlags & kAlive; }
    void SetAlive() { fEventFlags |= kAlive; }
    void SetDead() { fEventFlags &= kAliveOff; }
    EventFlags GetEvents() const { return fEventFlags & kAliveOff; }
private:
    string fTaskName;
    EventFlags fEventFlags;
    static unsigned int sTaskThreadPicker;
};

class TaskThread : public Thread
{
public:
    TaskThread() : Thread() {};
    virtual ~TaskThread() { this->StopAndWaitForThread(); };
    void EnQueue(Task *task);
    Cond* GetCond() { return &fQueueCond; }
private:
    virtual void Entry();
    Task* WaitForTask();
    Task* DeQueueBlocking(int32 inTimeoutInMilSecs);
    uint32 GetQueueLength();
private:
    queue<Task*> fTaskQueue;
    Mutex fQueueMutex;
    Cond fQueueCond;
};

class TaskThreadPool
{
public:
    static BOOL AddThreads(uint32 numToAdd);
    static void RemoveThreads();
    static TaskThread* GetThread(uint32 index);
    static uint32 GetNumThreads() { return sNumTaskThreads; }
    static Mutex& GetMutex() { return sMutex; };
private:
    static uint32 sNumTaskThreads;
/*    static Mutex sMutexRW;*/
    static vector<TaskThread*> sTaskThreadArray;
    static Mutex sMutex;
};
P2_NAMESPACE_END

