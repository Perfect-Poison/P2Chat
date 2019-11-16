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
    SetTaskName(_T("unknown"));

    fTaskQueueElem.SetEnclosingObject(this);

    if (fEvent != nullptr)
        fEvent->AddRefTask(this);
}


Task::~Task()
{
    if (fEvent != nullptr)
        fEvent->RemoveRefTask(this);

    if (fEvent->RefTaskCount() == 0 && fDeleteEvent)
        safe_delete(fEvent);
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
            log_debug(0, _T("Task::Signal ������(alive)������TaskName=%s, �Ѽ��������߳�(Thread ID:%u)���������\n"), fTaskName, taskThread->GetThreadID());
    }
    else 
    {
        if (TASK_DEBUG)
            log_debug(5, _T("Task::Signal sent to dead TaskName=%s\n"), fTaskName);
    }
}

void TaskThread::Entry()
{
    Task* theTask = nullptr;

    log_debug(1, _T("TaskThread::Entry ���������߳�\n"));

    while (true) 
    {
        /**
         *	��ȡһ��Alive��task
         */
        theTask = WaitForTask();

        if (theTask == nullptr)
            break;

        BOOL doneProcessingEvent = false;
        while (!doneProcessingEvent) 
        {
            int64 theTimeout = 0;
            MutexLocker locker(&TaskThreadPool::GetMutex());
            if (TASKTHREAD_DEBUG)
                log_debug(0, _T("[�����߳�%u]TaskThread::Entry ʹ��ȫ��������ǰTaskName=%s CurTime=%I64d\n"), GetThreadID(), theTask->GetTaskName(), time(0));
            
            theTimeout = theTask->Run();
            if (theTimeout < 0) 
            {
                if (TASKTHREAD_DEBUG)
                    log_debug(0, _T("[�����߳�%u]TaskThread::Entry ɾ������ TaskName=%s CurTime=%I64d\n"), GetThreadID(), theTask->GetTaskName(), time(0));
                theTask->SetTaskName(_tcscat(theTask->GetTaskName(), _T(" deleted")));
                theTask->SetDead();
                safe_delete(theTask);

                doneProcessingEvent = TRUE;
            }
            else // theTimeout >= 0
            {
                //theTask->SetTaskName(theTask->GetTaskName() + " reagain");
                if (TASKTHREAD_DEBUG)
                    log_debug(0, _T("[�����߳�%u]TaskThread::Entry �ظ����� TaskName=%s CurTime=%I64d\n"), GetThreadID(), theTask->GetTaskName(), time(0));
                fTaskQueueB.EnQueue(theTask->GetQueueElem());
                doneProcessingEvent = TRUE;
            }
        }
    }

    log_debug(1, _T("TaskThread::Entry ֹͣ�����߳�\n"));

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
                    log_debug(0, _T("[�����߳�%u]TaskThread::WaitForTask ����alive��TaskName=%s, ��ǰ������г���Ϊ%d\n"), GetThreadID(), theTask->GetTaskName(), fTaskQueueB.GetLength());
                return theTask;
            }
            else
            {
                if (TASKTHREAD_DEBUG)
                    log_debug(0, _T("[�����߳�%u]TaskThread::WaitForTask ����dead��TaskName=%s, ��ǰ������г���Ϊ%d\n"), GetThreadID(), theTask->GetTaskName(), fTaskQueueB.GetLength());
                safe_delete(theTask);
            }
        }

        // �Ƿ�����ֹͣ�߳�����
        if (IsStopRequested())
            return nullptr;
    }
}

// uint32 TaskThread::GetQueueLength()
// {
//     MutexLocker locker(&fQueueMutex);
//     return fTaskQueue.size();
// }

bool TaskThreadPool::Initialize(uint32 inNumTaskThreads)
{
    Assert(sTaskThreadArray.empty());
    return AddThreads(inNumTaskThreads);
}

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

bool TaskThreadPool::AddThreads(uint32 numToAdd)
{
    for (uint32 x = 0; x < numToAdd; x++) 
    {
        sTaskThreadArray.push_back(new TaskThread);
        sTaskThreadArray.at(x)->Start();
    }
    sNumTaskThreads += numToAdd;
    return true;
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

