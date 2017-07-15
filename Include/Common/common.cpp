#include "Common/common.h"
#include "Common/Mutex.h"

static p2::Mutex sAtomicMutex;

unsigned int p2::atomic_add(unsigned int *area, int val)
{
    MutexLocker locker(&sAtomicMutex);
    *area += val;
    return *area;
}

unsigned int p2::atomic_or(unsigned int *area, unsigned int val)
{
    unsigned int oldVal;
    MutexLocker locker(&sAtomicMutex);
    oldVal = *area;
    *area = oldVal | val;
    return oldVal;
}

unsigned int p2::atomic_sub(unsigned int *area, int val)
{
    return atomic_add(area, -val);
}
