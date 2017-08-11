#include "LogThread.h"
P2_NAMESPACE_BEG

LogThread* LogThread::sInstance = nullptr;
TCHAR LogThread::fLogFileName[MAX_PATH];
FILE* LogThread::fLogFileHandle = nullptr;
Mutex LogThread::fMutex;
log_flags LogThread::fFlags = 0;
log_rotation_policy LogThread::fRotationPolicy = LOG_ROTATION_DAILY;
TCHAR LogThread::fDailyLogSuffix[64] = _T("%Y%m%d");   // 只在LOG_ROTATION_DAILY下有效
int LogThread::fMaxLogSize = 4096 * 1024;    // 只在LOG_ROTATION_BY_SIZE下有效
time_t LogThread::fCurrentDayStart = 0;
int LogThread::fLogHistorySize = 4;
int LogThread::fDebugLevel = 0;

bool LogThread::Initialize(const TCHAR *logName, int debugLevel, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix)
{
    _tcsncpy(fLogFileName, logName, MAX_PATH);
    fDebugLevel = debugLevel;
    fFlags = flags & 0x7FFFFFFF;
    SetRotationPolicy(rotationPolicy, maxLogSize, historySize, dailySuffix);
    return true;
}

bool LogThread::LogOpen()
{
    //fFlags = flags & 0x7FFFFFFF;
    TCHAR buffer[32];
    //_tcsncpy(fLogFileName, logName, MAX_PATH);
    fLogFileHandle = _tfopen(fLogFileName, _T("a"));
    if (fLogFileHandle != nullptr)
    {
        fFlags |= LOG_IS_OPEN;
        TCHAR bufferDate[64];
        _ftprintf(fLogFileHandle, _T("%s ======================================================================================\n"), FormatLogCalendarTime(buffer));
        _ftprintf(fLogFileHandle, _T("%s | log date:            %s\n"), FormatLogCalendarTime(buffer), FormatCalendarTime(bufferDate));
        _ftprintf(fLogFileHandle, _T("%s | log file name:       %s\n"), FormatLogCalendarTime(buffer), fLogFileName);
        if (fRotationPolicy == LOG_ROTATION_DISABLED)
        {
            _ftprintf(fLogFileHandle, _T("%s | log rotation policy: %s\n"), FormatLogCalendarTime(buffer), _T("LOG_ROTATION_DISABLED"));
        }
        else if (fRotationPolicy == LOG_ROTATION_DAILY)
        {
            _ftprintf(fLogFileHandle, _T("%s | log rotation policy: %s\n"), FormatLogCalendarTime(buffer), _T("LOG_ROTATION_DAILY"));
        }
        else if (fRotationPolicy == LOG_ROTATION_BY_SIZE)
        {
            _ftprintf(fLogFileHandle, _T("%s | log rotation policy: %s\n"), FormatLogCalendarTime(buffer), _T("LOG_ROTATION_BY_SIZE"));
            _ftprintf(fLogFileHandle, _T("%s | history size(files): %d\n"), FormatLogCalendarTime(buffer), fLogHistorySize);
            _ftprintf(fLogFileHandle, _T("%s | max file size(B):    %d\n"), FormatLogCalendarTime(buffer), fMaxLogSize);
        }
        _ftprintf(fLogFileHandle, _T("%s | log debug level:     %d\n"), FormatLogCalendarTime(buffer), fDebugLevel);
        _ftprintf(fLogFileHandle, _T("%s ======================================================================================\n"), FormatLogCalendarTime(buffer));
        fflush(fLogFileHandle);

        //SetRotationPolicy(rotationPolicy, maxLogSize, historySize, dailySuffix);
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

LogThread::LogThread() : Thread()
{
//     //     if (LOGTHREAD_DEBUG)
//     //         _tprintf(_T("EventThread::EventThread 创建日志线程\n"));
// 
//     memset(fLogFileName, 0, sizeof(fLogFileName));
//     fLogFileHandle = nullptr;
//     fFlags = 0;
//     fRotationPolicy = LOG_ROTATION_DAILY;
//     _tcscpy(fDailyLogSuffix, _T("%Y%m%d"));   // 只在LOG_ROTATION_DAILY下有效
//     fMaxLogSize = 4096 * 1024;    // 只在LOG_ROTATION_BY_SIZE下有效
//     fCurrentDayStart = 0;
//     fLogHistorySize = 4;
//     fDebugLevel = 0;
}

void LogThread::Entry()
{
    if (!LogOpen())
    {
        _tprintf(_T("[error] 日志文件打开失败\n"));
        return;
    }

    log_debug(1, _T("LogThread::Entry 启动日志线程\n"));

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
        if (IsStopRequested())
            break;
    }

    log_debug(1, _T("LogThread::Entry 停止日志线程\n"));
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
    log_type logType = logRecord->GetType();
#ifdef UNICODE
    wstring msg = logRecord->GetMessage();
    char *msgUTF8 = utf8_from_wstr(msg.c_str());
#else
    string msg = logRecord->GetMessage();
    char *msgUTF8 = memdup(msg.c_str(), msg.length() + 1);
    msgUTF8[msg.length()] = '\0';
#endif
    char buffer[64];
    char logLevel[64];
    switch (logType)
    {
    case LOG_DEBUG:
        _snprintf(logLevel, 16, "[%s]", "DEBUG");
        break;
    case LOG_INFO:
        _snprintf(logLevel, 16, "[%s]", "INFO ");
        break;
    case LOG_WARNING:
        _snprintf(logLevel, 16, "[%s]", "WARN ");
        break;
    case LOG_ERROR:
        _snprintf(logLevel, 16, "[%s]", "ERROR");
        break;
    default:
        strncpy(logLevel, "", 1);
        break;
    }

    time_t t = time(nullptr);
    if ((fRotationPolicy == LOG_ROTATION_DAILY) && (t >= fCurrentDayStart + 86400))
        RotateLog();

    FormatLogCalendarTimeA(buffer);
    if (fLogFileHandle != nullptr)
    {
        fprintf(fLogFileHandle, "%s %s %s", buffer, logLevel, msgUTF8);
        //fprintf(fLogFileHandle, "%s", msgUTF8);
        fflush(fLogFileHandle);
    }
    if (fFlags & LOG_PRINT_TO_CONSOLE)
        printf("%s %s %s", buffer, logLevel, msgUTF8);

    free(msgUTF8);
    if ((fLogFileHandle != nullptr) && (fRotationPolicy == LOG_ROTATION_BY_SIZE) && (fMaxLogSize != 0))
    {
        struct stat st;

        fstat(_fileno(fLogFileHandle), &st);
        if (st.st_size >= fMaxLogSize)
            RotateLog();
    }
}

P2_NAMESPACE_END

