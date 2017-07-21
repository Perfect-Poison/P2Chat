#pragma once
#include "Common/common.h"
#include "Task.h"
#include "Common/Thread.h"
#include "Common/CommonSocket.h"
#include <map>

P2_NAMESPACE_BEG

class EventContext : public CommonSocket
{
public:
    EventContext(int inSocketID, Task *notifyTask);
    virtual ~EventContext();
    virtual void RequestEvent(int theMask);
    void SetTask(Task *task) { fTask = task; };
    virtual void ProcessEvent(int eventBits);
protected:
    Task *fTask;
    unsigned int fEventID;
    BOOL fWatchEventCalled;
    struct eventreq fEventReq;
    static unsigned int sEventID;
private:
    Mutex fEventMutext;
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

