#include "Queue.h"
P2_NAMESPACE_BEG


Queue::Queue() : fLength(0) 
{ 
    fSentinel.fNext = &fSentinel; 
    fSentinel.fPrev = &fSentinel; 
}

void Queue::EnQueue(QueueElem* elem)
{
    Assert(elem != nullptr);
    if (elem->fQueue == this)
        return;
    Assert(elem->fQueue == nullptr);
    elem->fNext = fSentinel.fNext;
    elem->fPrev = &fSentinel;
    elem->fQueue = this;
    fSentinel.fNext->fPrev = elem;
    fSentinel.fNext = elem;
    fLength++;
}

QueueElem* Queue::DeQueue()
{
    if (fLength > 0)
    {
        QueueElem* elem = fSentinel.fPrev;
        Assert(fSentinel.fPrev != &fSentinel);
        elem->fPrev->fNext = &fSentinel;
        fSentinel.fPrev = elem->fPrev;
        elem->fQueue = nullptr;
        fLength--;
        return elem;
    }
    else 
        return nullptr;
}

void Queue::Remove(QueueElem* elem)
{
    Assert(elem != nullptr);
    Assert(elem != &fSentinel);

    if (elem->fQueue == this)
    {
        elem->fNext->fPrev = elem->fPrev;
        elem->fPrev->fNext = elem->fNext;
        elem->fQueue = nullptr;
        fLength--;
    }
}

QueueElem* QueueBlocking::DeQueueBlocing(int32 inTimeoutMilSecs)
{
    MutexLocker locker(&fMutex);
    
    if (fLength == 0)
        fCond.Wait(inTimeoutMilSecs);

    QueueElem* elem = Queue::DeQueue();
    return elem;
}

QueueElem* QueueBlocking::DeQueue()
{
    MutexLocker locker(&fMutex);
    QueueElem* elem = Queue::DeQueue();
    return elem;
}

void QueueBlocking::EnQueue(QueueElem* elem)
{
    {
        MutexLocker locker(&fMutex);
        Queue::EnQueue(elem);
    }
    fCond.Signal();
}

void QueueElem::Remove()
{
    if (fQueue != nullptr)
        fQueue->Remove(this);
}

P2_NAMESPACE_END

