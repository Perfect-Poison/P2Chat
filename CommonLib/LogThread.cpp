#include "LogThread.h"
P2_NAMESPACE_BEG

LogThread* LogThread::sInstance = nullptr;

LogThread::LogThread() : Thread()
{
    if (LOGTHREAD_DEBUG)
        _tprintf(_T("EventThread::EventThread 创建日志线程\n"));

    memset(fLogFileName, 0, sizeof(fLogFileName));
    fLogFileHandle = nullptr;
    fFlags = 0;
    fRotationPolicy = LOG_ROTATION_DAILY;
    _tcscpy(fDailyLogSuffix, _T("%Y%m%d"));   // 只在LOG_ROTATION_DAILY下有效
    fMaxLogSize = 4096 * 1024;    // 只在LOG_ROTATION_BY_SIZE下有效
    fCurrentDayStart = 0;
    fLogHistorySize = 4;
    fDebugLevel = 0;
}

void LogThread::Entry()
{
    if (!(fFlags & LOG_IS_OPEN))
        return;

    while (true) 
    {
        int32 theTimeout = 10;
        QueueElem *theElem = fQueueB.DeQueueBlocing(theTimeout);
        if (theElem != nullptr)
        {
            LogRecord *theLogRecord = (LogRecord *)theElem->GetEnclosingObject();
            WriteLogRecordToFile(theLogRecord);
            delete theLogRecord;
        }

        // 是否请求停止线程运行
        if (Thread::GetCurrent()->IsStopRequested())
            return ;
    }
}

bool LogThread::LogOpen(const TCHAR *logName, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix)
{
    fFlags = flags & 0x7FFFFFFF;
    TCHAR buffer[32];
    _tcsncpy(fLogFileName, logName, MAX_PATH);
    fLogFileHandle = _tfopen(logName, _T("a"));
    if (fLogFileHandle != nullptr)
    {
        fFlags |= LOG_IS_OPEN;
        _ftprintf(fLogFileHandle, _T("%s Log file opened\n"), FormatLogCalendarTime(buffer));
        fflush(fLogFileHandle);

        SetRotationPolicy(rotationPolicy, maxLogSize, historySize, dailySuffix);
    }
    return (fFlags & LOG_IS_OPEN) ? true : false;
}

void LogThread::LogClose()
{
    if (fFlags & LOG_IS_OPEN)
    {
        if (fLogFileHandle != nullptr)
            fclose(fLogFileHandle);
        fFlags &= ~LOG_IS_OPEN;
    }
}

void LogThread::SetRotationPolicy(log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix)
{
    fRotationPolicy = rotationPolicy;
    switch (rotationPolicy)
    {
        //         case LOG_ROTATION_DISABLED:
        //             break;
    case LOG_ROTATION_DAILY:
        if ((dailySuffix != nullptr) && (dailySuffix[0] != 0))
            _tcsncpy(fDailyLogSuffix, dailySuffix, _tcsclen(dailySuffix) + 1);
        SetDayStart();
        break;
    case LOG_ROTATION_BY_SIZE:
        if (maxLogSize >= 1024)
            fMaxLogSize = maxLogSize;
        else
            fMaxLogSize = 1024;

        if ((historySize >= 0) && (historySize <= MAX_LOG_HISTORY_SIZE))
            fLogHistorySize = historySize;
        else
            fLogHistorySize = MAX_LOG_HISTORY_SIZE;
        break;
    default:
        _tprintf(_T("[error]LogThread::LogOpen 未知的log rotation policy\n"));
        break;
    }
}

void LogThread::EnQueueLogRecord(LogRecord *logRecord)
{
    fQueueB.EnQueue(logRecord->GetQueueElem());
}

bool LogThread::RotateLog()
{
    int i;
    TCHAR oldName[MAX_PATH], newName[MAX_PATH];

    if ((fLogFileHandle != nullptr) && (fFlags & LOG_IS_OPEN))
    {
        fclose(fLogFileHandle);
        fFlags &= ~LOG_IS_OPEN;
    }

    if (fRotationPolicy == LOG_ROTATION_BY_SIZE)
    {
        // 删除旧文件
        for (i = MAX_LOG_HISTORY_SIZE; i >= fLogHistorySize; i--)
        {
            _sntprintf(oldName, MAX_PATH, _T("%s.%d"), fLogFileName, i);
            _tunlink(oldName);
        }

        // 所有日志文件名向后推移
        for (; i >= 0; i--) 
        {
            _sntprintf(oldName, MAX_PATH, _T("%s.%d"), fLogFileName, i);
            _sntprintf(newName, MAX_PATH, _T("%s.%d"), fLogFileName, i + 1);
            _trename(oldName, newName);
        }

        // 将当前日志命名为xxx.0
        _sntprintf(newName, MAX_PATH, _T("%s.0"), fLogFileName);
        _trename(fLogFileName, newName);
    }
    else if (fRotationPolicy == LOG_ROTATION_DAILY)
    {
        struct tm *loc = localtime(&fCurrentDayStart);
        TCHAR buffer[64];
        _tcsftime(buffer, 64, fDailyLogSuffix, loc);

        // 重命名当前日志文件为xxx.suffix
        _sntprintf(newName, MAX_PATH, _T("%s.%s"), fLogFileName, buffer);
        _trename(fLogFileName, newName);

        SetDayStart();
    }

    // 重新打开日志文件
    fLogFileHandle = _tfopen(fLogFileName, _T("w"));
    if (fLogFileHandle != nullptr)
    {
        fFlags |= LOG_IS_OPEN;
        TCHAR buffer[32];
        _ftprintf(fLogFileHandle, _T("%s Log file truncated.\n"), FormatLogCalendarTime(buffer));
        fflush(fLogFileHandle);
    }

    return (fFlags & LOG_IS_OPEN) ? true : false;
}

void LogThread::SetDayStart()
{
    time_t now = time(nullptr);
    struct tm dayStart;
    struct tm *ltm = localtime(&now);
    memcpy(&dayStart, ltm, sizeof(struct tm));
    dayStart.tm_hour = 0;
    dayStart.tm_min = 0;
    dayStart.tm_sec = 0;
    fCurrentDayStart = mktime(&dayStart);
}

void LogThread::WriteLogRecordToFile(LogRecord *logRecord)
{
    TCHAR buffer[64];
    TCHAR logLevel[64];
    log_type logType = logRecord->GetType();
#ifdef UNICODE
    wstring msg = logRecord->GetMessage();
#else
    string msg = logRecord->GetMessage();
#endif
    switch (logType)
    {
    case LOG_DEBUG:
        _sntprintf(logLevel, 16, _T("[%s]"), _T("DEBUG"));
        break;
    case LOG_INFO:
        _sntprintf(logLevel, 16, _T("[%s]"), _T("INFO "));
        break;
    case LOG_WARNING:
        _sntprintf(logLevel, 16, _T("[%s]"), _T("WARN "));
        break;
    case LOG_ERROR:
        _sntprintf(logLevel, 16, _T("[%s]"), _T("ERROR"));
        break;
    default:
        _tcsncpy(logLevel, _T(""), 1);
        break;
    }

    time_t t = time(nullptr);
    if ((fRotationPolicy == LOG_ROTATION_DAILY) && (t >= fCurrentDayStart + 86400))
        RotateLog();

    FormatLogCalendarTime(buffer);
    if (fLogFileHandle != nullptr)
    {
        _ftprintf(fLogFileHandle, _T("%s %s %s"), buffer, logLevel, msg.c_str());
        fflush(fLogFileHandle);
    }
    if (fFlags & LOG_PRINT_TO_CONSOLE)
        _tprintf(_T("%s %s %s"), buffer, logLevel, msg.c_str());

    if ((fLogFileHandle != nullptr) && (fRotationPolicy == LOG_ROTATION_BY_SIZE) && (fMaxLogSize != 0))
    {
        struct stat st;

        fstat(_fileno(fLogFileHandle), &st);
        if (st.st_size >= fMaxLogSize)
            RotateLog();
    }
}

P2_NAMESPACE_END

