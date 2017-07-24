#include "EventContext.h"

P2_NAMESPACE_BEG

unsigned int EventContext::sEventID = WM_USER;
EventThread* EventThread::Instance = nullptr;

EventContext::EventContext(int inSocketID):
    CommonSocket(inSocketID),
    //fTask(notifyTask),
    fEventID(0),
    fWatchEventCalled(FALSE)
{
}


EventContext::~EventContext()
{
    EventThread::GetInstance()->UnRegisterEvent(fEventID);
}

void EventContext::RequestEvent(int theMask)
{
    MutexLocker locker(&fEventMutex);
    if (fWatchEventCalled)
    {
        fEventReq.er_eventbits = theMask;
        if (select_modwatch(&fEventReq, theMask) != 0)
            AssertV(false, Thread::GetErrno());
    }
    else
    {
        if (!compare_and_store(0x7FFF, WM_USER, &sEventID))
            fEventID = atomic_add(&sEventID, 1);
        else
            fEventID = WM_USER;

        if (!EventThread::GetInstance()->RegisterEvent(fEventID, this))
            printf("EventContext::RequestEvent 错误, EventID %d 已经存在\n", fEventID);

        ::memset(&fEventReq, 0, sizeof(fEventReq));
        fEventReq.er_handle = fSocketID;
        fEventReq.er_eventbits = theMask;
        fEventReq.er_eventid = fEventID;

        fWatchEventCalled = true;

        if (select_watchevent(&fEventReq, theMask) != 0)
            AssertV(false, Thread::GetErrno());
    }
}

void EventContext::AddRefTask(Task *task)
{
    MutexLocker locker(&fTaskSetMutex); 
    if (fTaskSet.count(task))
    {
        printf("[error]EventContext::AddInRelatedTask 引用任务已经存在\n");
        throw;
    }
    fTaskSet.insert(task);
}

void EventContext::RemoveRefTask(Task *task)
{
    MutexLocker locker(&fTaskSetMutex);
    if (!fTaskSet.count(task))
    {
        printf("[error]EventContext::RemoveOutRelatedTask 引用任务不存在\n");
        throw;
    }
    fTaskSet.erase(task);
}

uint32 EventContext::RefTaskCount()
{
    MutexLocker locker(&fTaskSetMutex);
    return fTaskSet.size();
}

// void EventContext::ProcessEvent(int eventBits)
// {
//     if (fTask != nullptr)
//         fTask->Signal(eventBits);
// }

void EventThread::Entry()
{
    struct eventreq theCurrentEvent;
    ::memset(&theCurrentEvent, 0, sizeof(theCurrentEvent));

    while (true) 
    {
        int theErrno = EINTR;
        while (theErrno == EINTR)
        {
            int theRetVal = select_waitevent(&theCurrentEvent);
            if (theRetVal >= 0)
                theErrno = theRetVal;
            else
                theErrno = Thread::GetErrno();
        }

        AssertV(theErrno == 0, theErrno);

        if (theCurrentEvent.er_eventid != 0) 
        {
            EventContext* theEvent = nullptr;
            {
                MutexLocker locker(&fEventTableMutex);
                if (fEventTable.count(theCurrentEvent.er_eventid))
                {
                    theEvent = fEventTable[theCurrentEvent.er_eventid];
                    //fEventTable.erase(theCurrentEvent.er_eventid);
                }
            }
            if (theEvent != nullptr)
                theEvent->ProcessEvent(theCurrentEvent.er_eventbits);
        }
    }
}

BOOL EventThread::RegisterEvent(uint32 eventID, EventContext *event)
{
    MutexLocker locker(&fEventTableMutex);
    if (fEventTable.count(eventID))
        return FALSE;
    else
    {
        fEventTable[eventID] = event;
        return TRUE;
    }
}

BOOL EventThread::UnRegisterEvent(uint32 eventID)
{
    MutexLocker locker(&fEventTableMutex);
    if (fEventTable.count(eventID)) 
    {
        EventContext *event = fEventTable[eventID];
        fEventTable.erase(eventID);
        //delete event;
        return TRUE;
    }
    else 
        return FALSE;
}


P2_NAMESPACE_END

