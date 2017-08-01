#pragma once
#include "Thread.h"
#include "Queue.h"
#include "p2_common.h"
#include "p2_util.h"
P2_NAMESPACE_BEG

#ifdef P2CHAT_DEBUG
#define LOGRECORD_DEBUG 1
#else 
#define LOGRECORD_DEBUG 0
#endif

#ifdef P2CHAT_DEBUG
#define LOGTHREAD_DEBUG 1
#else
#define LOGTHREAD_DEBUG 0
#endif

class LogRecord
{
public:
    LogRecord(string message, const log_type logType) :
        fLogRecordQueueElem(this), fMessage(message), fLogType(logType) {}
    ~LogRecord() {}
    void SetMessage(const string& message) { fMessage = message; }
    void SetType(const log_type& logType) { fLogType = logType; }
    string GetMessage() const { return fMessage; }
    log_type GetType() const { return fLogType; }
    QueueElem *GetQueueElem() { return &fLogRecordQueueElem; }
private:
    QueueElem fLogRecordQueueElem;
    string fMessage;
    log_type fLogType;
};


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
    virtual ~LogThread() { LogClose(); }
    bool LogOpen(const char *logName, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const char *dailySuffix);
    void LogClose();
    log_flags GetFlags() const { return fFlags; }
    void SetRotationPolicy(log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const char *dailySuffix);
    log_rotation_policy GetRotationPolicy() const { return fRotationPolicy; }
    int GetMaxLogSize() const { return fMaxLogSize; }
    char *GetDailySuffix() { return fDailyLogSuffix; }
    void EnQueueLogRecord(LogRecord *logRecord);
private:
    LogThread();
    virtual void Entry();
    bool RotateLog();
    void SetDayStart();
    void WriteLogRecordToFile(LogRecord *logRecord);
private:
    static LogThread* sInstance;
    QueueBlocking fQueueB;
    char fLogFileName[MAX_PATH];
    FILE *fLogFileHandle;
    Mutex fMutex;
    log_flags fFlags;
    log_rotation_policy fRotationPolicy;
    char fDailyLogSuffix[64];   // 只在LOG_ROTATION_DAILY下有效
    int fMaxLogSize;    // 只在LOG_ROTATION_BY_SIZE下有效
    time_t fCurrentDayStart;
    int fLogHistorySize;
};

P2_NAMESPACE_END