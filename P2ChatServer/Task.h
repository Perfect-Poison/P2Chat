#pragma once

#include "p2server_common.h"
#include "Cond.h"
#include "Mutex.h"
#include "Queue.h"
#include <vector>

P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define TASK_DEBUG 1
#define TASKTHREAD_DEBUG 1
#define TASKTHREADPOOL_DEBUG 1
#else
#define TASK_DEBUG 0
#define TASKTHREAD_DEBUG 0
#define TASKTHREADPOOL_DEBUG 0
#endif

class Event;
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
    Task(Event *event);
    virtual ~Task();
    virtual int64 Run() = 0;
    void Signal(EventFlags eventFlags);
    void SetTaskName(const string& name) { fTaskName = name; }
    string GetTaskName() const { return fTaskName; }
    BOOL IsAlive() const { return fEventFlags & kAlive; }
    void SetAlive() { fEventFlags |= kAlive; }
    void SetDead() { fEventFlags &= kAliveOff; }
    EventFlags GetEventFlags() const { return fEventFlags & kAliveOff; }
    void SetDeleteEventWhenAllRefTasksFinished(BOOL deleteEvent) { fDeleteEvent = deleteEvent; }
    BOOL IsDeleteEventWhenAllRefTasksFinished() { return fDeleteEvent; }
    QueueElem* GetQueueElem() { return &fTaskQueueElem; }
private:
    string fTaskName;
    EventFlags fEventFlags;
    static unsigned int sTaskThreadPicker;
    BOOL fDeleteEvent;
    Event *fEvent;
    QueueElem fTaskQueueElem;
};

class TaskThread : public Thread
{
public:
    TaskThread() : Thread() 
    {
        if (TASKTHREAD_DEBUG)
            log_debug(1, _T("EventThread::EventThread 创建任务线程\n"));
    };
    virtual ~TaskThread() { this->StopAndWaitForThread(); };
    QueueBlocking* GetQueue() { return &fTaskQueueB; }
private:
    virtual void Entry();
    Task* WaitForTask();
private:
    QueueBlocking fTaskQueueB;
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
    static vector<TaskThread*> sTaskThreadArray;
    static Mutex sMutex;
};
P2_NAMESPACE_END

