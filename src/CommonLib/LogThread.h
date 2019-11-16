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
    LogRecord(TCHAR* message, const log_type logType) :
        fLogRecordQueueElem(this), fLogType(logType) { _tcsncpy_s(fMessage, message, _tcslen(message)); }
    void SetMessage(const TCHAR* message) { _tcsncpy_s(fMessage, message, _tcslen(message)); }
    TCHAR* GetMessage() { return fMessage; }
    log_type GetType() { return fLogType; }
    QueueElem *GetQueueElem() { return &fLogRecordQueueElem; }
private:
    QueueElem fLogRecordQueueElem;
    TCHAR fMessage[MAX_LOG_RECORD_MESSAGE_SIZE];
    log_type fLogType;
};


class LogThread :
    public Thread
{
public:
    static bool Initialize(const TCHAR *logName, int debugLevel, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix);
    static LogThread* GetInstance()
    {
        if (sInstance == nullptr)
            sInstance = new LogThread;
        return sInstance;
    }
    virtual ~LogThread() { LogClose(); }
    static bool LogOpen();
    static void LogClose();
    static log_flags GetFlags() { return fFlags; }
    static void SetRotationPolicy(log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix);
    static log_rotation_policy GetRotationPolicy() { return fRotationPolicy; }
    static int GetMaxLogSize() { return fMaxLogSize; }
    static TCHAR *GetDailySuffix() { return fDailyLogSuffix; }
    void EnQueueLogRecord(LogRecord *logRecord);
    static void SetDebugLevel(int debugLevel) { fDebugLevel = debugLevel; }
    static int GetDebugLevel() { return fDebugLevel; }
private:
    LogThread();
    virtual void Entry();
    static bool RotateLog();
    static void SetDayStart();
    static void WriteLogRecordToFile(LogRecord *logRecord);
private:
    static LogThread* sInstance;
    QueueBlocking fQueueB;
    static TCHAR fLogFileName[MAX_PATH];
    static FILE *fLogFileHandle;
    static Mutex fMutex;
    static log_flags fFlags;
    static log_rotation_policy fRotationPolicy;
    static TCHAR fDailyLogSuffix[64];   // 只在LOG_ROTATION_DAILY下有效
    static int fMaxLogSize;    // 只在LOG_ROTATION_BY_SIZE下有效
    static time_t fCurrentDayStart;
    static int fLogHistorySize;
    static int fDebugLevel;
};

P2_NAMESPACE_END