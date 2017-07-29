#pragma once

#include "Common/Mutex.h"
#include "Common/Cond.h"
#include "p2server_common.h"
#include <queue>
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
         *	����Ϊ�¼�����������
         */
        kReadEvent = FD_READ,
        kWriteEvent = FD_WRITE,
        kAcceptEvent = FD_ACCEPT,
        kConnectEvent = FD_CONNECT,
        kCloseEvent = FD_CLOSE,

        /**
         *	����Ϊ�Զ��������
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
private:
    string fTaskName;
    EventFlags fEventFlags;
    static unsigned int sTaskThreadPicker;
    BOOL fDeleteEvent;
    Event *fEvent;
};

class TaskThread : public Thread
{
public:
    TaskThread() : Thread() 
    {
        if (TASKTHREAD_DEBUG)
            printf("EventThread::EventThread ���������߳�\n");
    };
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

