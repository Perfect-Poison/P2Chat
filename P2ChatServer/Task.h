#pragma once

#include "Common/common.h"
#include "Common/Mutex.h"
#include "Task.h"
#include <queue>
#include <vector>

P2_NAMESPACE_BEG

#define TASK_DEBUG 1

class TaskThread;
class TaskThreadPool;
class Task
{
public:
    typedef unsigned int EventFlags;
    enum
    {
        KillEvent = 0x1 << 0x0,
        IdleEvent = 0x1 << 0x1,
        StartEvent = 0x1 << 0x2,
        TimeoutEvent = 0x1 << 0x3,

        ReadEvent = 0x1 << 0x4,
        WriteEvent = 0x1 << 0x5,

        UpdateEvent = 0x1 << 0x6
    };
    typedef unsigned int PriorityLevel;
    enum 
    {
        Urgent = 0x1 << 0x0
    };
    Task();
    virtual ~Task();

    virtual int64 Run() = 0;

    void Signal(EventFlags eventFlags);
    void SetTaskName(const string& name);
    string GetTaskName();
    void SetDefaultThread(TaskThread *defaultThread);
    void SetPriorityLevel(PriorityLevel priorityLevel);
    PriorityLevel GetPriorityLevel();
protected:
    EventFlags GetEvents();
    int64 CallLocked();
private:
    enum
    {
        Alive = 0x80000000, //EventFlags, again
        AliveOff = 0x7fffffff
    };
    void SetTaskThread(TaskThread *thread);

private:
    string fTaskName;
    EventFlags fEventFlags;
    //TaskThread *fUseThisThread;
    TaskThread *fDefaultThread;
    //BOOL fWriteLock;
    PriorityLevel fPriorityLevel;
    static unsigned int sTaskThreadPicker;
};

class TaskThread : public Thread
{
public:
    TaskThread();
    virtual ~TaskThread();
    void AddTask(Task *task);
    BOOL IsIdle();
private:
    virtual void Entry();
    Task* WaitForTask();
private:
    priority_queue<int32, vector<Task*>, greater<int32>> fPrioriTaskQueue;
    Mutex fMutex;
};

class TaskThreadPool
{
public:
    TaskThreadPool();
    virtual ~TaskThreadPool();
    static BOOL AddThreads(uint32 numToAdd);
    static void RemoveThreads();
    static TaskThread* GetThread(uint32 index);
    static uint32 GetNumThreads();
    static Mutex& GetMutex();
private:
    static uint32 sNumTaskThreads;
    static Mutex sMutexRW;
    static vector<TaskThread*> sTaskThreadArray;
    static Mutex sMutex;
};
P2_NAMESPACE_END

