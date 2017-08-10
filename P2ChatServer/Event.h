#pragma once
#include "Thread.h"
#include "Socket.h"
#include "Task.h"
#include <map>
#include <set>
P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define EVENT_DEBUG 1
#define EVENTTHREAD_DEBUG 1
#else
#define EVENT_DEBUG 0
#define EVENTTHREAD_DEBUG 0
#endif

/*!
 * \class	Event
 *
 * \brief	Event不会携带任何任务，每个Event触发后，会生成一个新的任务，并将任务加入到任务线程的任务队列
 *
 * \author	BrianYi
 * \date	2017/07/21
 */
class Event : public Socket
{
public:
    Event(int inSocketID);
    virtual ~Event();
    virtual void RequestEvent(int theMask);
    //void SetTask(Task *task) { fTask = task; };
    virtual void ProcessEvent(int eventBits) = 0;
    void AddRefTask(Task *task);
    void RemoveRefTask(Task *task);
    uint32 RefTaskCount();
#ifdef UNICODE
    void SetEventName(const wstring& eventName) { fEventName = eventName; }
    wstring GetEventName() const { return fEventName; }
#else 
    void SetEventName(const string& eventName) { fEventName = eventName; }
    string GetEventName() const { return fEventName; }
#endif
protected:
    //Task *fTask;
    unsigned int fEventID;
    BOOL fWatchEventCalled;
    struct eventreq fEventReq;
    static unsigned int sEventID;
private:
#ifdef UNICODE
    wstring fEventName;
#else 
    string fEventName;
#endif
    Mutex fEventMutex;
    Mutex fTaskSetMutex;
    set<Task*> fTaskSet;
};

class EventThread :public Thread
{
public:
    static EventThread* GetInstance()
    {
        if (sInstance == nullptr)
            sInstance = new EventThread;
        return sInstance;
    }
    virtual ~EventThread() {}
    BOOL RegisterEvent(uint32 eventID, Event *event);
    BOOL UnRegisterEvent(uint32 eventID);
private:
    EventThread() : Thread()
    {
        if (EVENTTHREAD_DEBUG)
            log_debug(1, _T("EventThread::EventThread 创建事件(监听)线程\n"));
    }
    virtual void Entry();
    map<uint32, Event*> fEventTable;
    Mutex fEventTableMutex;
    static EventThread *sInstance;
};

P2_NAMESPACE_END

