#include "Task.h"

P2_NAMESPACE_BEG

Task::Task():
    fEventFlags(0),
    fPriorityLevel(Urgent)
{
    SetTaskName("unknown");
}


Task::~Task()
{
}

void Task::Signal(EventFlags eventFlags)
{
    eventFlags |= Alive;
    EventFlags oldEventFlags = atomic_or(&fEventFlags, eventFlags);
    if ((!(oldEventFlags & Alive)) && (TaskThreadPool::GetNumThreads() > 0))
    {
        unsigned int theThreadIndex = atomic_add((unsigned int *)sTaskThreadPicker, 1);
        theThreadIndex %= TaskThreadPool::GetNumThreads();
        if (TASK_DEBUG)
        {
            printf("Task::Signal enque TaskName=%s using Task::GetNumThreads(), task range=[0-%lu] thread index =%u\n",
                fTaskName.c_str(), TaskThreadPool::GetNumThreads() - 1, theThreadIndex);
        }
        TaskThreadPool::GetThread(theThreadIndex)->EnQueue(this);
    }
    else 
    {
        if (TASK_DEBUG)
            printf("Task::Signal sent to dead TaskName=%s\n", fTaskName.c_str());
    }
}

void TaskThread::Entry()
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
                printf("TaskThread::Entry run global locked TaskName=%s CurTime=%I64d ThreadID=%d\n", theTask->GetTaskName().c_str(), time(0), Thread::GetCurrentThreadID());
            
            theTimeout = theTask->Run();
            if (theTimeout < 0) 
            {
                if (TASK_DEBUG)
                    printf("TaskThread::Entry delete TaskName=%s CurTime=%I64d ThreadID=%d\n", theTask->GetTaskName().c_str(), time(0), Thread::GetCurrentThreadID());
                theTask->SetTaskName(theTask->GetTaskName() + " deleted");
                theTask->SetDead();
                doneProcessingEvent = TRUE;
            }
            else // theTimeout >= 0
            {
                theTask->SetTaskName(theTask->GetTaskName() + " reagain");
                EnQueue(theTask);
                doneProcessingEvent = TRUE;
            }
        }
    }
}

Task* TaskThread::WaitForTask()
{
    while (true)
    {
        int32 theTimeout = 10;
        Task * task = DeQueueBlocking(theTimeout);
        if (task != nullptr)
        {
            if (task->IsAlive())
            {
                if (TASK_DEBUG)
                    printf("TaskThread::WaitForTask found alive signal-task=%s, threadID=%d, queue length=%d\n", task->GetTaskName().c_str(), GetCurrentThreadID(), GetQueueLength());
                return task;
            }
            else
            {
                if (TASK_DEBUG)
                    printf("TaskThread::WaitForTask found dead task=%s, threadID=%d, queue length=%d\n", task->GetTaskName().c_str(), GetCurrentThreadID(), GetQueueLength());
                delete task;
            }
        }

        // 是否请求停止线程运行
        if (Thread::GetCurrent()->IsStopRequested())
            return nullptr;
    }
}

Task* TaskThread::DeQueueBlocking(int32 inTimeoutInMilSecs)
{
    MutexLocker locker(&fQueueMutex);
    if (fPrioriTaskQueue.empty())
        fQueueCond.Wait(&fQueueMutex, inTimeoutInMilSecs);
    Task *task = fPrioriTaskQueue.top();
    fPrioriTaskQueue.pop();
    return task;
}

uint32 TaskThread::GetQueueLength()
{
    MutexLocker locker(&fQueueMutex);
    return fPrioriTaskQueue.size();
}

void TaskThread::EnQueue(Task *task)
{
    {
        MutexLocker locker(&fQueueMutex);
        fPrioriTaskQueue.push(task);
    }
    fQueueCond.Signal();
}

uint32 TaskThreadPool::sNumTaskThreads = 0;

BOOL TaskThreadPool::AddThreads(uint32 numToAdd)
{
    Assert(sTaskThreadArray.empty());
    for (uint32 x = 0; x < numToAdd; x++) 
    {
        sTaskThreadArray.push_back(new TaskThread);
        sTaskThreadArray.at(x)->Start();
    }
    sNumTaskThreads = numToAdd;
    return TRUE;
}

void TaskThreadPool::RemoveThreads()
{
    for (uint32 x = 0; x < sNumTaskThreads; x++)
        sTaskThreadArray.at(x)->SendStopRequest();

    for (uint32 x = 0; x < sNumTaskThreads; x++)
        sTaskThreadArray.at(x)->GetCond()->Signal();
    
    for (uint32 x = 0; x < sNumTaskThreads; x++)
        delete sTaskThreadArray.at(x);

    sTaskThreadArray.clear();
}

TaskThread* TaskThreadPool::GetThread(uint32 index)
{
    MutexLocker locker(&sMutex);
    Assert(!sTaskThreadArray.empty());
    if (index >= sTaskThreadArray.size())
        return nullptr;
    return sTaskThreadArray.at(index);
}

P2_NAMESPACE_END