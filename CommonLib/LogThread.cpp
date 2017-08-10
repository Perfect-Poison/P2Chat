#include "LogThread.h"
P2_NAMESPACE_BEG

LogThread* LogThread::sInstance = nullptr;

LogThread::LogThread() : Thread()
{
    if (LOGTHREAD_DEBUG)
        _tprintf(_T("EventThread::EventThread ������־�߳�\n"));

    memset(fLogFileName, 0, sizeof(fLogFileName));
    fLogFileHandle = nullptr;
    fFlags = 0;
    fRotationPolicy = LOG_ROTATION_DAILY;
    _tcscpy(fDailyLogSuffix, _T("%Y%m%d"));   // ֻ��LOG_ROTATION_DAILY����Ч
    fMaxLogSize = 4096 * 1024;    // ֻ��LOG_ROTATION_BY_SIZE����Ч
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

        // �Ƿ�����ֹͣ�߳�����
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
        _tprintf(_T("[error]LogThread::LogOpen δ֪��log rotation policy\n"));
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
        // ɾ�����ļ�
        for (i = MAX_LOG_HISTORY_SIZE; i >= fLogHistorySize; i--)
        {
            _sntprintf(oldName, MAX_PATH, _T("%s.%d"), fLogFileName, i);
            _tunlink(oldName);
        }

        // ������־�ļ����������
        for (; i >= 0; i--) 
        {
            _sntprintf(oldName, MAX_PATH, _T("%s.%d"), fLogFileName, i);
            _sntprintf(newName, MAX_PATH, _T("%s.%d"), fLogFileName, i + 1);
            _trename(oldName, newName);
        }

        // ����ǰ��־����Ϊxxx.0
        _sntprintf(newName, MAX_PATH, _T("%s.0"), fLogFileName);
        _trename(fLogFileName, newName);
    }
    else if (fRotationPolicy == LOG_ROTATION_DAILY)
    {
        struct tm *loc = localtime(&fCurrentDayStart);
        TCHAR buffer[64];
        _tcsftime(buffer, 64, fDailyLogSuffix, loc);

        // ��������ǰ��־�ļ�Ϊxxx.suffix
        _sntprintf(newName, MAX_PATH, _T("%s.%s"), fLogFileName, buffer);
        _trename(fLogFileName, newName);

        SetDayStart();
    }

    // ���´���־�ļ�
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

