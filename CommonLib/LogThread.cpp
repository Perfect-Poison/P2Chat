#include "LogThread.h"
P2_NAMESPACE_BEG

LogThread* LogThread::sInstance = nullptr;

LogThread::LogThread() : Thread()
{
    if (LOGTHREAD_DEBUG)
        printf("EventThread::EventThread 创建日志线程\n");

    memset(fLogFileName, 0, sizeof(fLogFileName));
    fLogFileHandle = nullptr;
    fFlags = 0;
    fRotationPolicy = LOG_ROTATION_DAILY;
    strcpy(fDailyLogSuffix, "%Y%m%d");   // 只在LOG_ROTATION_DAILY下有效
    fMaxLogSize = 4096 * 1024;    // 只在LOG_ROTATION_BY_SIZE下有效
    fCurrentDayStart = 0;
    fLogHistorySize = 4;
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

bool LogThread::LogOpen(const char *logName, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const char *dailySuffix)
{
    fFlags = flags & 0x7FFFFFFF;
    char buffer[32];
    strncpy(fLogFileName, logName, MAX_PATH);
    fLogFileHandle = fopen(logName, "a");
    if (fLogFileHandle != nullptr)
    {
        fFlags |= LOG_IS_OPEN;
        fprintf(fLogFileHandle, "%s Log file opened\n", FormatLogCalendarTime(buffer));
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

void LogThread::SetRotationPolicy(log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const char *dailySuffix)
{
    fRotationPolicy = rotationPolicy;
    switch (rotationPolicy)
    {
        //         case LOG_ROTATION_DISABLED:
        //             break;
    case LOG_ROTATION_DAILY:
        if (dailySuffix != nullptr)
            strncpy(fDailyLogSuffix, dailySuffix, strlen(dailySuffix) + 1);
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
        printf("[error]LogThread::LogOpen 未知的log rotation policy\n");
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
    char oldName[MAX_PATH], newName[MAX_PATH];

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
            snprintf(oldName, MAX_PATH, "%s.%d", fLogFileName, i);
            _unlink(oldName);
        }

        // 所有日志文件名向后推移
        for (; i >= 0; i--) 
        {
            snprintf(oldName, MAX_PATH, "%s.%d", fLogFileName, i);
            snprintf(newName, MAX_PATH, "%s.%d", fLogFileName, i + 1);
            rename(oldName, newName);
        }

        // 将当前日志命名为xxx.0
        snprintf(newName, MAX_PATH, "%s.0", fLogFileName);
        rename(fLogFileName, newName);
    }
    else if (fRotationPolicy == LOG_ROTATION_DAILY)
    {
        struct tm *loc = localtime(&fCurrentDayStart);
        char buffer[64];
        strftime(buffer, 64, fDailyLogSuffix, loc);

        // 重命名当前日志文件为xxx.suffix
        snprintf(newName, MAX_PATH, "%s.%s", fLogFileName, buffer);
        rename(fLogFileName, newName);

        SetDayStart();
    }

    // 重新打开日志文件
    fLogFileHandle = fopen(fLogFileName, "w");
    if (fLogFileHandle != nullptr)
    {
        fFlags |= LOG_IS_OPEN;
        char buffer[32];
        fprintf(fLogFileHandle, "%s Log file truncated.\n", FormatLogCalendarTime(buffer));
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
    char buffer[64];
    char logLevel[64];
    log_type logType = logRecord->GetType();
    const char *message = logRecord->GetMessage().c_str();
    switch (logType)
    {
    case LOG_DEBUG:
        snprintf(logLevel, 16, "[%s]", "DEBUG");
        break;
    case LOG_INFO:
        snprintf(logLevel, 16, "[%s]", "INFO ");
        break;
    case LOG_WARNING:
        snprintf(logLevel, 16, "[%s]", "WARN ");
        break;
    case LOG_ERROR:
        snprintf(logLevel, 16, "[%s]", "ERROR");
        break;
    default:
        strncpy(logLevel, "", 1);
        break;
    }

    time_t t = time(nullptr);
    if ((fRotationPolicy == LOG_ROTATION_DAILY) && (t >= fCurrentDayStart + 86400))
        RotateLog();

    FormatLogCalendarTime(buffer);
    if (fLogFileHandle != nullptr)
    {
        fprintf(fLogFileHandle, "%s %s%s", buffer, logLevel, message);
        fflush(fLogFileHandle);
    }
    if (fFlags & LOG_PRINT_TO_CONSOLE)
        printf("%s %s%s", buffer, logLevel, message);

    if ((fLogFileHandle != nullptr) && (fRotationPolicy == LOG_ROTATION_BY_SIZE) && (fMaxLogSize != 0))
    {
        struct stat st;

        fstat(_fileno(fLogFileHandle), &st);
        if (st.st_size >= fMaxLogSize)
            RotateLog();
    }
}

P2_NAMESPACE_END

