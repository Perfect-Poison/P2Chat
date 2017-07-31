#include "LogThread.h"
P2_NAMESPACE_BEG

LogThread* LogThread::sInstance = nullptr;

void LogThread::Entry()
{

}

bool LogThread::LogOpen(const char *logName, log_flags flags)
{
    fFlags = flags & 0x7FFFFFFF;
    char buffer[32];
    strncpy(fLogFileName, logName, MAX_PATH);
    fLogFileHandle = fopen(logName, "a");
    if (fLogFileHandle != nullptr)
    {
        fFlags |= LOG_IS_OPEN;
        fprintf(fLogFileHandle, "\n%s Log file opened\n", FormatLogCalendarTime(buffer));
        fflush(fLogFileHandle);
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

void LogThread::LogWrite(log_type logType, const char *format, ...)
{
    va_list args;
    char szBuffer[4096];

    if (!(fFlags & LOG_IS_OPEN))
        return;
    
    va_start(args, format);
    vsprintf_s(szBuffer, format, args);
    WriteLogToFile(szBuffer, logType);
    va_end(args);
}

void LogThread::WriteLogToFile(char *message, const log_type logType)
{
    char buffer[64];
    char logLevel[64];

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

    // todo...
}

P2_NAMESPACE_END

