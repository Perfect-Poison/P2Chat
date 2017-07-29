#pragma once
#include "Common/Thread.h"
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
 * \brief	Event����Я���κ�����ÿ��Event�����󣬻�����һ���µ����񣬲���������뵽�����̵߳��������
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
    void SetEventName(const string& eventName) { fEventName = eventName; }
    string GetEventName() const { return fEventName; }
protected:
    //Task *fTask;
    unsigned int fEventID;
    BOOL fWatchEventCalled;
    struct eventreq fEventReq;
    static unsigned int sEventID;
private:
    string fEventName;
    Mutex fEventMutex;
    Mutex fTaskSetMutex;
    set<Task*> fTaskSet;
};

class EventThread :public Thread
{
public:
    static EventThread* GetInstance()
    {
        if (Instance == nullptr)
            Instance = new EventThread;
        return Instance;
    }
    virtual ~EventThread() {}
    BOOL RegisterEvent(uint32 eventID, Event *event);
    BOOL UnRegisterEvent(uint32 eventID);
private:
    EventThread() : Thread()
    {
        if (EVENTTHREAD_DEBUG)
            printf("EventThread::EventThread �����¼�(����)�߳�\n");
    }
    virtual void Entry();
    map<uint32, Event*> fEventTable;
    Mutex fEventTableMutex;
    static EventThread *Instance;
};

P2_NAMESPACE_END

