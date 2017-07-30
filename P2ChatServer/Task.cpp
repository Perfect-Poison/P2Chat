#include "Task.h"
#include "Event.h"

P2_NAMESPACE_BEG

unsigned int Task::sTaskThreadPicker = 0;
vector<TaskThread*> TaskThreadPool::sTaskThreadArray;
uint32 TaskThreadPool::sNumTaskThreads = 0;
Mutex TaskThreadPool::sMutex;


Task::Task(Event *event):
    fEventFlags(0),
    fEvent(event),
    fDeleteEvent(false),
    fTaskQueueElem()
{
    SetTaskName("unknown");

    fTaskQueueElem.SetEnclosingObject(this);

    if (fEvent != nullptr)
        fEvent->AddRefTask(this);
}


Task::~Task()
{
    if (fEvent != nullptr)
        fEvent->RemoveRefTask(this);

    if (fEvent->RefTaskCount() == 0 && fDeleteEvent)
        safe_free(fEvent);
}

void Task::Signal(EventFlags eventFlags)
{
    eventFlags |= kAlive;
    EventFlags oldEventFlags = atomic_or(&fEventFlags, eventFlags);
    if ((!(oldEventFlags & kAlive)) && (TaskThreadPool::GetNumThreads() > 0))
    {
        unsigned int theThreadIndex = atomic_add(&sTaskThreadPicker, 1);
        theThreadIndex %= TaskThreadPool::GetNumThreads();
        TaskThread *taskThread = TaskThreadPool::GetThread(theThreadIndex);
        taskThread->GetQueue()->EnQueue(&this->fTaskQueueElem);
        if (TASK_DEBUG)
        {
            printf("Task::Signal 被激活(alive)的任务TaskName=%s, 已加入任务线程(Thread ID:%u)的任务队列\n", fTaskName.c_str(), taskThread->GetThreadID());
        }
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
        /**
         *	获取一个Alive的task
         */
        theTask = WaitForTask();

        if (theTask == nullptr)
            return;

        BOOL doneProcessingEvent = false;
        while (!doneProcessingEvent) 
        {
            int64 theTimeout = 0;
            MutexLocker locker(&TaskThreadPool::GetMutex());
            if (TASKTHREAD_DEBUG)
                printf("[任务线程%u]TaskThread::Entry 使用全局锁，当前TaskName=%s CurTime=%I64d\n", GetThreadID(), theTask->GetTaskName().c_str(), time(0));
            
            theTimeout = theTask->Run();
            if (theTimeout < 0) 
            {
                if (TASKTHREAD_DEBUG)
                    printf("[任务线程%u]TaskThread::Entry 删除任务 TaskName=%s CurTime=%I64d\n", GetThreadID(), theTask->GetTaskName().c_str(), time(0));
                theTask->SetTaskName(theTask->GetTaskName() + " deleted");
                theTask->SetDead();
                safe_free(theTask);

                doneProcessingEvent = TRUE;
            }
            else // theTimeout >= 0
            {
                //theTask->SetTaskName(theTask->GetTaskName() + " reagain");
                if (TASKTHREAD_DEBUG)
                    printf("[任务线程%u]TaskThread::Entry 重复任务 TaskName=%s CurTime=%I64d\n", GetThreadID(), theTask->GetTaskName().c_str(), time(0));
                fTaskQueueB.EnQueue(theTask->GetQueueElem());
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
        QueueElem* theElem = fTaskQueueB.DeQueueBlocing(theTimeout);
        if (theElem != nullptr)
        {
            Task *theTask = (Task *)theElem->GetEnclosingObject();
            if (theTask->IsAlive())
            {
                if (TASKTHREAD_DEBUG)
                    printf("[任务线程%u]TaskThread::WaitForTask 发现alive的TaskName=%s, 当前任务队列长度为%d\n", GetThreadID(), theTask->GetTaskName().c_str(), fTaskQueueB.GetQueue()->GetLength());
                return theTask;
            }
            else
            {
                if (TASKTHREAD_DEBUG)
                    printf("[任务线程%u]TaskThread::WaitForTask 发现dead的TaskName=%s, 当前任务队列长度为%d\n", GetThreadID(), theTask->GetTaskName().c_str(), fTaskQueueB.GetQueue()->GetLength());
                safe_free(theTask);
            }
        }

        // 是否请求停止线程运行
        if (Thread::GetCurrent()->IsStopRequested())
            return nullptr;
    }
}

// uint32 TaskThread::GetQueueLength()
// {
//     MutexLocker locker(&fQueueMutex);
//     return fTaskQueue.size();
// }

// void TaskThread::EnQueue(Task *task)
// {
// //     {
// //         MutexLocker locker(&fQueueMutex);
// //         fTaskQueue.push(task);
// //     }
// //     fQueueCond.Signal();
//     QueueElem *elem = (QueueElem *)task->;
//     fTaskQueueB.EnQueue(elem);
// }

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
        sTaskThreadArray.at(x)->GetQueue()->GetCond()->Signal();
    
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

