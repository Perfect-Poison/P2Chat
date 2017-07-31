#include "Iterator.h"
#include "HashMap.h"
#include "Queue.h"
P2_NAMESPACE_BEG

bool HashMapIterator::HasNext()
{
    if (fHashMap->Size() == 0)
        return false;
    return fCurr->hh.next != nullptr;
}

void* HashMapIterator::Next()
{
    if (fHashMap->Size() == 0)
        return false;

    if (fCurr == nullptr)
        fCurr = fHashMap->fHashData;
    else 
        fCurr = (HashMapEntry *)fCurr->hh.next;
    return &fCurr->pair;
}

void HashMapIterator::Remove()
{
    if (fCurr == nullptr)
        return;

    HASH_DEL(fHashMap->fHashData, fCurr);
    if (fHashMap->fKeyLength > 16)
        safe_free(fCurr->key.p);
    safe_free(fCurr);
}

bool QueueIterator::HasNext()
{
    if (fQueue->GetLength() == 0)
        return false;
    return fCurr->Next() != nullptr;
}

void* QueueIterator::Next()
{
    if (fQueue->GetLength() == 0)
        return false;

    if (fCurr == nullptr)
        fCurr = fQueue->GetHead();
    else
        fCurr = fCurr->Next();
    return fCurr->GetEnclosingObject();
}

void QueueIterator::Remove()
{
    if (fCurr == nullptr)
        return;

    fCurr->Remove();
}

P2_NAMESPACE_END