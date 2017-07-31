#pragma once
#include "Thread.h"
#include "Queue.h"
#include "p2server_common.h"
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
    enum
    {
        kMaxLogSize = 4096 * 1024
    };
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
    bool LogOpen(const char *logName, log_flags flags);
    void LogClose();
    void LogWrite(log_type logType, const char *format, ...);
    void LogDebug(int level, const char *format, ...);
    void SetDebugLevel(int level);
    int GetDebugLevel();
private:
    void WriteLogToFile(char *message, const log_type logType);
    static LogThread* sInstance;
    QueueBlocking fQueueB;
    char fLogFileName[MAX_PATH];
    FILE *fLogFileHandle;
    Mutex fMutex;
    int fDebugLevel;
    log_flags fFlags;
};

P2_NAMESPACE_END