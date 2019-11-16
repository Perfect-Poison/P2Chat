#pragma once
/*!
 * \file	Iterator.h
 *
 * \author	BrianYi
 * \date	2017/07/31
 *
 * \brief	所有迭代器类
 */

#include "p2_common.h"
P2_NAMESPACE_BEG

class AbstractIterator
{
public:
    AbstractIterator() {}
    virtual ~AbstractIterator() {}
    virtual bool HasNext() = 0;
    virtual void* Next() = 0;
    virtual void Remove() = 0;
};


template <class T>
class Iterator
{
public:
    Iterator(AbstractIterator *absIter) { fAbsIter = absIter; }
    ~Iterator() { delete fAbsIter; }

    bool HasNext() { return fAbsIter->HasNext(); }
    T* Next() { return (T *)fAbsIter->Next(); }
    void Remove() { fAbsIter->Remove(); }
private:
    AbstractIterator *fAbsIter;
};


class HashMapBase;
struct HashMapEntry;
class HashMapIterator : public AbstractIterator
{
public:
    HashMapIterator(HashMapBase *hashMap) :fHashMap(hashMap), fCurr(nullptr) {}

    virtual bool HasNext();
    virtual void* Next();
    virtual void Remove();
private:
    HashMapBase* fHashMap;
    HashMapEntry* fCurr;
};

class Queue;
class QueueElem;
class QueueIterator : public AbstractIterator
{
public:
    QueueIterator(Queue *inQueue) : fQueue(inQueue), fCurr(nullptr) {}
    virtual bool HasNext();
    virtual void* Next();
    virtual void Remove();
private:
    Queue*      fQueue;
    QueueElem*  fCurr;
};

P2_NAMESPACE_END