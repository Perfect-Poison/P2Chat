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
    
    ~LogRecord() {}
    void SetType(const log_type& logType) { fLogType = logType; }
#ifdef UNICODE
    LogRecord(wstring message, const log_type logType) :
        fLogRecordQueueElem(this), fMessage(message), fLogType(logType) {}
    void SetMessage(const wstring& message) { fMessage = message; }
    wstring GetMessage() const { return fMessage; }
#else
    LogRecord(string message, const log_type logType) :
        fLogRecordQueueElem(this), fMessage(message), fLogType(logType) {}
    void SetMessage(const string& message) { fMessage = message; }
    string GetMessage() const { return fMessage; }
#endif
    log_type GetType() const { return fLogType; }
    QueueElem *GetQueueElem() { return &fLogRecordQueueElem; }
private:
    QueueElem fLogRecordQueueElem;
#ifdef UNICODE
    wstring fMessage;
#else
    string fMessage;
#endif
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
    bool LogOpen(const TCHAR *logName, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix);
    void LogClose();
    log_flags GetFlags() const { return fFlags; }
    void SetRotationPolicy(log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix);
    log_rotation_policy GetRotationPolicy() const { return fRotationPolicy; }
    int GetMaxLogSize() const { return fMaxLogSize; }
    TCHAR *GetDailySuffix() { return fDailyLogSuffix; }
    void EnQueueLogRecord(LogRecord *logRecord);
    void SetDebugLevel(int debugLevel) { fDebugLevel = debugLevel; }
    int GetDebugLevel() const { return fDebugLevel; }
private:
    LogThread();
    virtual void Entry();
    bool RotateLog();
    void SetDayStart();
    void WriteLogRecordToFile(LogRecord *logRecord);
private:
    static LogThread* sInstance;
    QueueBlocking fQueueB;
    TCHAR fLogFileName[MAX_PATH];
    FILE *fLogFileHandle;
    Mutex fMutex;
    log_flags fFlags;
    log_rotation_policy fRotationPolicy;
    TCHAR fDailyLogSuffix[64];   // 只在LOG_ROTATION_DAILY下有效
    int fMaxLogSize;    // 只在LOG_ROTATION_BY_SIZE下有效
    time_t fCurrentDayStart;
    int fLogHistorySize;
    int fDebugLevel;
};

P2_NAMESPACE_END