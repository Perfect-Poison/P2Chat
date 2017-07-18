#include "Task.h"

p2::Task::Task():
    fEventFlags(0),
    fPriorityLevel(Urgent),
    fUseThisThread(nullptr),
    fDefaultThread(nullptr),
    fWriteLock(false)
{
    SetTaskName("unknown");
}


p2::Task::~Task()
{
}

void p2::Task::Signal(EventFlags eventFlags)
{
    eventFlags |= Alive;
    EventFlags oldEventFlags = atomic_or(&fEventFlags, eventFlags);
    if ((!(oldEventFlags & Alive)) && (TaskThreadPool::GetNumThreads() > 0))
    {
        if (fDefaultThread && !fUseThisThread)
            fUseThisThread = fDefaultThread;
        if (fUseThisThread) 
        {
            fUseThisThread->AddTask(this);
        }
        else 
        {
            unsigned int theThreadIndex = atomic_add((unsigned int *)sTaskThreadPicker, 1);
            theThreadIndex %= TaskThreadPool::GetNumThreads();
            if (TASK_DEBUG)
            {
                printf("Task::Signal enque TaskName=%s using Task::GetNumThreads(), task range=[0-%lu] thread index =%u\n",
                    fTaskName.c_str(), TaskThreadPool::GetNumThreads() - 1, theThreadIndex);
            }
            TaskThreadPool::GetThread(theThreadIndex)->AddTask(this);
        }

    }
}

p2::TaskThread::TaskThread()
{
}


p2::TaskThread::~TaskThread()
{
}

void p2::TaskThread::Entry()
{
    Task* theTask = nullptr;

    while (true) 
    {
        theTask = WaitForTask();

        if (theTask == nullptr)
            return;

        BOOL doneProcessingEvent = false;
        while (!doneProcessingEvent) 
        {
            int64 theTimeout = 0;
            MutexLocker locker(&TaskThreadPool::GetMutex());
            if (TASK_DEBUG)
                printf("TaskThread::Entry run global locked TaskName=%s CurTime=%I64d ThreadID=%d\n", theTask->GetTaskName(), time(0), Thread::GetCurrentThreadID());
            
            theTimeout = theTask->Run();
            if (theTimeout < 0) 
            {
                if (TASK_DEBUG)
                    printf("TaskThread::Entry delete TaskName=%s CurTime=%I64d ThreadID=%d\n", theTask->GetTaskName(), time(0), Thread::GetCurrentThreadID());

            }
        }
    }
}

p2::Task* p2::TaskThread::WaitForTask()
{

}

void p2::TaskThread::AddTask(Task *task)
{

}

BOOL p2::TaskThread::IsIdle()
{
    MutexLocker locker(&fMutex);
    if (fPrioriTaskQueue.empty())
        return true;
    return false;
}

p2::TaskThreadPool::TaskThreadPool()
{
}


p2::TaskThreadPool::~TaskThreadPool()
{
}

p2::TaskThread* p2::TaskThreadPool::GetThread(uint32 index)
{
    MutexLocker locker(&sMutex);
    Assert(!sTaskThreadArray.empty());
    if (index >= sTaskThreadArray.size())
        return nullptr;
    return sTaskThreadArray.at(index);
}
