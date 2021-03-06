#include "Event.h"

P2_NAMESPACE_BEG

unsigned int Event::sEventID = WM_USER;
EventThread* EventThread::sInstance = nullptr;

Event::Event(int inSocketID):
    Socket(inSocketID),
    fEventID(0),
    fWatchEventCalled(FALSE)
{
    SetEventName(_T("unknown"));
}


Event::~Event()
{
    EventThread::GetInstance()->UnRegisterEvent(fEventID);
}

void Event::RequestEvent(int theMask)
{
    if (EVENT_DEBUG)
        log_debug(1, _T("Event::RequestEvent 请求事件%s, EventName=%s CurTime=%I64d\n"), theMask == EV_RE ? _T("EV_RE") : _T("EV_WR"), GetEventName(), time(0));
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
            log_debug(5, _T("[error]Event::RequestEvent EventID %d 已经存在\n"), fEventID);

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
        log_debug(5, _T("[error]Event::AddInRelatedTask 引用任务0x%x已经存在\n"), (uint32)task);
        throw;
    }
    fTaskSet.insert(task);
}

void Event::RemoveRefTask(Task *task)
{
    MutexLocker locker(&fTaskSetMutex);
    if (!fTaskSet.count(task))
    {
        log_debug(5, _T("[error]Event::RemoveOutRelatedTask 引用任务0x%x不存在\n"), (uint32)task);
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

    log_debug(1, _T("EventThread::Entry 启动事件线程\n"));

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
                    log_debug(1, _T("[事件线程%u]EventThread::Entry 触发事件，EventName=%s EventID=%u CurTime=%I64d\n"), GetThreadID(), theEvent->GetEventName(), theCurrentEvent.er_eventid, time(0));
                theEvent->ProcessEvent(theCurrentEvent.er_eventbits);
            }
        }

        // 是否请求停止线程运行
        if (IsStopRequested())
            break;
    }

    log_debug(1, _T("EventThread::Entry 停止事件线程\n"));
}

BOOL EventThread::RegisterEvent(uint32 eventID, Event *event)
{
    MutexLocker locker(&fEventTableMutex);
    if (fEventTable.count(eventID))
    {
        if (EVENTTHREAD_DEBUG)
            log_debug(5, _T("[error][事件线程%u]EventThread::RegisterEvent 注册事件失败，EventName=%s EventID=%u\n"), GetThreadID(), event->GetEventName(), eventID);
        return FALSE;
    }
    else
    {
        fEventTable[eventID] = event;
        if (EVENTTHREAD_DEBUG)
            log_debug(1, _T("[事件线程%u]EventThread::RegisterEvent 注册事件，EventName=%s EventID=%u\n"), GetThreadID(), event->GetEventName(), eventID);
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
        {
            log_debug(1, _T("[事件线程%u]EventThread::UnRegisterEvent 注销事件，EventName=%s EventID=%u\n"), GetThreadID(), event->GetEventName(), eventID);
        }
        //delete event;
        return TRUE;
    }
    else
    {
        if (EVENTTHREAD_DEBUG)
            log_debug(5, _T("[error][事件线程%u]EventThread::RegisterEvent 注销事件失败，EventID=%u\n"), GetThreadID(), eventID);
        return FALSE;
    }
}


P2_NAMESPACE_END

