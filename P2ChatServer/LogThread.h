#pragma once
#include "Thread.h"
P2_NAMESPACE_BEG

#ifdef P2CHAT_DEBUG
#define LOGTHREAD_DEBUG 1
#else
#define LOGTHREAD_DEBUG 0
#endif


class LogThread :
    public Thread
{
public:
    static LogThread* GetInstance()
    {
        if (sInstance == nullptr)
            sInstance = new LogThread;
        return sInstance;
    }
    virtual ~LogThread() {}
private:
    LogThread() : Thread()
    {
        if (LOGTHREAD_DEBUG)
            printf("EventThread::EventThread 创建日志线程\n");
    }
    virtual void Entry();
    static LogThread* sInstance;
    
};

P2_NAMESPACE_END