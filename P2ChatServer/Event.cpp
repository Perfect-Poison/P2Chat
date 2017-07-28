#include "Event.h"

P2_NAMESPACE_BEG

unsigned int Event::sEventID = WM_USER;
EventThread* EventThread::Instance = nullptr;

Event::Event(int inSocketID):
    Socket(inSocketID),
    fEventID(0),
    fWatchEventCalled(FALSE)
{
    SetEventName("unknown");
}


Event::~Event()
{
    EventThread::GetInstance()->UnRegisterEvent(fEventID);
}

void Event::RequestEvent(int theMask)
{
    if (EVENT_DEBUG)
        printf("Event::RequestEvent �����¼�%s, EventName=%s CurTime=%I64d\n", theMask == EV_RE ? "EV_RE" : "EV_WR", GetEventName().c_str(), time(0));
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
            printf("[error]Event::RequestEvent EventID %d �Ѿ�����\n", fEventID);

        ::memset(&fEventReq, 0, sizeof(fEventReq));
        fEventReq.er_handle = fSocketID;
        fEventReq.er_eventbits = theMask;
        fEventReq.er_eventid = fEventID;

        fWatchEventCalled = true;

        if (select_watchevent(&fEventReq, theMask) != 0)
            AssertV(false, Thread::GetErrno());
    }
}

void Event::AddRefTask(Task *task)
{
    MutexLocker locker(&fTaskSetMutex); 
    if (fTaskSet.count(task))
    {
        printf("[error]Event::AddInRelatedTask ���������Ѿ�����\n");
        throw;
    }
    fTaskSet.insert(task);
}

void Event::RemoveRefTask(Task *task)
{
    MutexLocker locker(&fTaskSetMutex);
    if (!fTaskSet.count(task))
    {
        printf("[error]Event::RemoveOutRelatedTask �������񲻴���\n");
        throw;
    }
    fTaskSet.erase(task);
}

uint32 Event::RefTaskCount()
{
    MutexLocker locker(&fTaskSetMutex);
    return fTaskSet.size();
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
            Event* theEvent = nullptr;
            {
                MutexLocker locker(&fEventTableMutex);
                if (fEventTable.count(theCurrentEvent.er_eventid))
                {
                    theEvent = fEventTable[theCurrentEvent.er_eventid];
                    //fEventTable.erase(theCurrentEvent.er_eventid);
                }
            }
            if (theEvent != nullptr)
            {
                if (EVENTTHREAD_DEBUG)
                    printf("[�¼��߳�%u]EventThread::Entry �����¼���EventName=%s EventID=%u CurTime=%I64d\n", GetThreadID(), theEvent->GetEventName().c_str(), theCurrentEvent.er_eventid, time(0));
                theEvent->ProcessEvent(theCurrentEvent.er_eventbits);
            }
        }
    }
}

BOOL EventThread::RegisterEvent(uint32 eventID, Event *event)
{
    MutexLocker locker(&fEventTableMutex);
    if (fEventTable.count(eventID))
    {
        if (EVENTTHREAD_DEBUG)
            printf("[error][�¼��߳�%u]EventThread::RegisterEvent ע���¼�ʧ�ܣ�EventName=%s EventID=%u\n", GetThreadID(), event->GetEventName().c_str(), eventID);
        return FALSE;
    }
    else
    {
        fEventTable[eventID] = event;
        if (EVENTTHREAD_DEBUG)
            printf("[�¼��߳�%u]EventThread::RegisterEvent ע���¼���EventName=%s EventID=%u\n", GetThreadID(), event->GetEventName().c_str(), eventID);
        return TRUE;
    }
}

BOOL EventThread::UnRegisterEvent(uint32 eventID)
{
    MutexLocker locker(&fEventTableMutex);
    if (fEventTable.count(eventID)) 
    {
        Event *event = fEventTable[eventID];
        fEventTable.erase(eventID);
        if (EVENTTHREAD_DEBUG)
            printf("[�¼��߳�%u]EventThread::UnRegisterEvent ע���¼���EventName=%s EventID=%u\n", GetThreadID(), event->GetEventName().c_str(), eventID);
        //delete event;
        return TRUE;
    }
    else
    {
        if (EVENTTHREAD_DEBUG)
            printf("[error][�¼��߳�%u]EventThread::RegisterEvent ע���¼�ʧ�ܣ�EventID=%u\n", GetThreadID(), eventID);
        return FALSE;
    }
}


P2_NAMESPACE_END

