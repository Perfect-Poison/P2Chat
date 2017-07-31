#pragma once
#include "p2_common.h"
#include "Cond.h"
#include "Iterator.h"
P2_NAMESPACE_BEG

class Queue;

/*!
 * \class	QueueElem
 *
 * \brief	队列元素类
 *
 * \author	BrianYi
 * \date 	2017/07/30
*/
class QueueElem
{
public:
    QueueElem(void* enclosingObject = nullptr) : fNext(nullptr), fPrev(nullptr), fQueue(nullptr),
        fEnclosingObject(enclosingObject) {}
    virtual ~QueueElem() {}

    BOOL IsMemberOf(const Queue& queue) { return (&queue == fQueue); }
    BOOL IsMemberOfAnyQueue() { return fQueue != nullptr; }
    void* GetEnclosingObject() { return fEnclosingObject; }
    void SetEnclosingObject(void* obj) { fEnclosingObject = obj; }

    QueueElem* Next() { return fNext; }
    QueueElem* Prev() { return fPrev; }
    Queue*     GetQueue() { return fQueue; }
    void Remove();
private:
    QueueElem*  fNext;
    QueueElem*  fPrev;
    Queue*      fQueue;
    void*       fEnclosingObject;
    friend class Queue;
};

/*!
 * \class	Queue
 *
 * \brief	队列类
 *
 * \author	BrianYi
 * \date 	2017/07/30
*/
class Queue
{
public:
    Queue();
    ~Queue() {}

    void EnQueue(QueueElem* elem);
    QueueElem* DeQueue();

    QueueElem* GetHead() { if (fLength > 0) return fSentinel.fPrev; return nullptr; }
    QueueElem* GetTail() { if (fLength > 0) return fSentinel.fNext; return nullptr; }
    uint32 GetLength() { return fLength; }
    void Remove(QueueElem* elem);
    Iterator<QueueElem>* iterator() { return new Iterator<QueueElem>(new QueueIterator(this)); }
protected:
    QueueElem   fSentinel;
    uint32      fLength;
};

/*!
 * \class	QueueBlocking
 *
 * \brief	阻塞队列类
 *
 * \author	BrianYi
 * \date 	2017/07/30
*/
class QueueBlocking : public Queue
{
public:
    QueueBlocking() {}
    ~QueueBlocking() {}

    QueueElem* DeQueueBlocing(int32 inTimeoutMilSecs);
    QueueElem* DeQueue();
    void EnQueue(QueueElem* elem);

    Cond*   GetCond() { return &fCond; }
private:
    Cond    fCond;
    Mutex   fMutex;
};

// /*!
//  * \class	QueueIter
//  *
//  * \brief	迭代器类，用于遍历Queue中的元素
//  *
//  * \author	BrianYi
//  * \date 	2017/07/30
// */
// class QueueIter
// {
// public:
//     QueueIter(Queue* inQueue) : fQueueP(inQueue), fCurrentElemP(inQueue->GetHead()) {}
//     QueueIter(Queue* inQueue, QueueElem* startElemP) : fQueueP(inQueue)
//     {
//         if (startElemP)
//         {
//             Assert(startElemP->IsMemberOf(*inQueue));
//             fCurrentElemP = startElemP;
//         }
//         else
//             fCurrentElemP = nullptr;
//     }
//     ~QueueIter() {}
// 
//     void Reset() { fCurrentElemP = fQueueP->GetHead(); }
//     QueueElem* GetCurrent() { return fCurrentElemP; }
//     void Next()
//     {
//         if (fCurrentElemP == fQueueP->GetTail())
//             fCurrentElemP = nullptr;
//         else
//             fCurrentElemP = fCurrentElemP->Prev();
//     }
//     BOOL IsDone() { return fCurrentElemP == nullptr; }
// private:
//     Queue*      fQueueP;
//     QueueElem*  fCurrentElemP;
// };
P2_NAMESPACE_END