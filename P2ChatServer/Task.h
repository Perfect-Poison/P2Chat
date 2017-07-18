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
        KillEvent = 0x1 << 0x0,
        IdleEvent = 0x1 << 0x1,
        StartEvent = 0x1 << 0x2,
        TimeoutEvent = 0x1 << 0x3,

        ReadEvent = 0x1 << 0x4,
        WriteEvent = 0x1 << 0x5,

        UpdateEvent = 0x1 << 0x6,

        Alive = 0x80000000,
        AliveOff = 0x7fffffff
    };
    typedef unsigned int EventFlags;
    enum
    {
        Urgent
    };
    typedef uint32 PriorityLevel;
    Task();
    virtual ~Task();
    virtual int64 Run() = 0;
    void Signal(EventFlags eventFlags);
    void SetTaskName(const string& name) { fTaskName = name; }
    string GetTaskName() const { return fTaskName; }
    void SetPriorityLevel(PriorityLevel priorityLevel) { fPriorityLevel = priorityLevel; }
    PriorityLevel GetPriorityLevel() const { return fPriorityLevel; }
    BOOL IsAlive() const { return fEventFlags & Alive; }
    void SetAlive() { fEventFlags |= Alive; }
    void SetDead() { fEventFlags &= AliveOff; }
    EventFlags GetEvents() const { return fEventFlags & AliveOff; }
protected:
    int64 CallLocked();
private:
    string fTaskName;
    EventFlags fEventFlags;
    PriorityLevel fPriorityLevel;
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
    priority_queue<int32, vector<Task*>, greater<int32>> fPrioriTaskQueue;
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

