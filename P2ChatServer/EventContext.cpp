#include "EventContext.h"

P2_NAMESPACE_BEG

unsigned int EventContext::sEventID = WM_USER;

EventContext::EventContext(int inFileDesc, Task *notifyTask):
    fFileDesc(inFileDesc),
    fTask(notifyTask),
    fEventID(0),
    fWatchEventCalled(FALSE)
{
}


EventContext::~EventContext()
{
    EventThread::GetInstance()->UnRegisterEvent(fEventID);
}

void EventContext::RequestEvent(int theMask/* = EV_RE*/)
{
    if (fWatchEventCalled)
    {
        fEventReq.er_eventbits = theMask;
        if (select_modwatch(&fEventReq, theMask) != 0)
            AssertV(false, Thread::GetError());
    }
    else
    {
        if (!compare_and_store(8192, WM_USER, &sEventID))
            fEventID = atomic_add(&sEventID, 1);
        else
            fEventID = WM_USER;

        if (!EventThread::GetInstance()->RegisterEvent(fEventID, this))
            printf("EventContext::RequestEvent error, event id %d is already there\n", fEventID);

        ::memset(&fEventReq, 0, sizeof(fEventReq));
        fEventReq.er_handle = fFileDesc;
        fEventReq.er_eventbits = theMask;
        fEventReq.er_eventid = fEventID;

        fWatchEventCalled = true;

        if (select_watchevent(&fEventReq, theMask) != 0)
            AssertV(false, Thread::GetErrno());
    }
}

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
            if (fEventTable.count(theCurrentEvent.er_eventid))
            {
                EventContext* theEvent = fEventTable.at(theCurrentEvent.er_eventid);
                theEvent->ProcessEvent(theCurrentEvent.er_eventbits);
            }
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
        delete event;
        return TRUE;
    }
    else 
        return FALSE;
}


P2_NAMESPACE_END

