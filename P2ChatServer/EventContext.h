#pragma once
#include "Common/common.h"
#include "Task.h"
#include "Common/Thread.h"
#include "Common/CommonSocket.h"
#include <map>
#include <set>
P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define EVENTCONTEXT_DEBUG 1
#else
#define EVENTCONTEXT_DEBUG 0
#endif

/*!
 * \class	EventContext
 *
 * \brief	Event����Я���κ�����ÿ��Event�����󣬻�����һ���µ����񣬲���������뵽�����̵߳��������
 *
 * \author	BrianYi
 * \date	2017/07/21
 */
class EventContext : public CommonSocket
{
public:
    EventContext(int inSocketID);
    virtual ~EventContext();
    virtual void RequestEvent(int theMask);
    //void SetTask(Task *task) { fTask = task; };
    virtual void ProcessEvent(int eventBits) = 0;
    void AddRefTask(Task *task);
    void RemoveRefTask(Task *task);
    uint32 RefTaskCount();
protected:
    //Task *fTask;
    unsigned int fEventID;
    BOOL fWatchEventCalled;
    struct eventreq fEventReq;
    static unsigned int sEventID;
private:
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
    BOOL RegisterEvent(uint32 eventID, EventContext *event);
    BOOL UnRegisterEvent(uint32 eventID);
private:
    EventThread() : Thread() {}
    virtual void Entry();
    map<uint32, EventContext*> fEventTable;
    Mutex fEventTableMutex;
    static EventThread *Instance;
};

P2_NAMESPACE_END

