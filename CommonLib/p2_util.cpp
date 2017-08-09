#include "p2_util.h"
#include "Mutex.h"
#include "LogThread.h"
P2_NAMESPACE_BEG

static Mutex sAtomicMutex;
static HWND sMsgWindow = nullptr;
static Mutex sLogMutex;

#define EPOCHFILETIME (116444736000000000i64)


char *bin_to_strA(const BYTE* pData, size_t size, char *pStr)
{
    size_t i;
    char *pCurr;

    for (i = 0, pCurr = pStr; i < size; i++)
    {
        *pCurr++ = bin2hex(pData[i] >> 4);
        *pCurr++ = bin2hex(pData[i] & 15);
    }
    *pCurr = 0;
    return pStr;
}
WCHAR *bin_to_strW(const BYTE* pData, size_t size, WCHAR *pStr)
{
    size_t i;
    WCHAR *pCurr;

    for (i = 0, pCurr = pStr; i < size; i++)
    {
        *pCurr++ = bin2hex(pData[i] >> 4);
        *pCurr++ = bin2hex(pData[i] & 15);
    }
    *pCurr = 0;
    return pStr;
}
size_t str_to_binA(const char* pStr, BYTE *pData, size_t size)
{
    size_t i;
    const char *pCurr;

    memset(pData, 0, size);
    for (i = 0, pCurr = pStr; (i < size) && (*pCurr != 0); i++)
    {
        pData[i] = hex2bin(*pCurr) << 4;
        pCurr++;
        if (*pCurr != 0)
        {
            pData[i] |= hex2bin(*pCurr);
            pCurr++;
        }
    }
    return i;
}
size_t str_to_binW(const WCHAR* pStr, BYTE *pData, size_t size)
{
    size_t i;
    const WCHAR *pCurr;

    memset(pData, 0, size);
    for (i = 0, pCurr = pStr; (i < size) && (*pCurr != 0); i++)
    {
        pData[i] = hex2bin(*pCurr) << 4;
        pCurr++;
        if (*pCurr != 0)
        {
            pData[i] |= hex2bin(*pCurr);
            pCurr++;
        }
    }
    return i;
}

void str_stripA(char *pszStr)
{
    int i;

    for (i = 0; (str[i] != 0) && ((str[i] == ' ') || (str[i] == '\t')); i++);
    if (i > 0)
        memmove(str, &str[i], strlen(&str[i]) + 1);
    for (i = (int)strlen(str) - 1; (i >= 0) && ((str[i] == ' ') || (str[i] == '\t')); i--);
    str[i + 1] = 0;
}

void str_stripW(WCHAR *pszStr)
{
    int i;

    for (i = 0; (str[i] != 0) && ((str[i] == L' ') || (str[i] == L'\t')); i++);
    if (i > 0)
        memmove(str, &str[i], (wcslen(&str[i]) + 1) * sizeof(WCHAR));
    for (i = (int)wcslen(str) - 1; (i >= 0) && ((str[i] == L' ') || (str[i] == L'\t')); i--);
    str[i + 1] = 0;
}

void *memdup(const void *data, size_t size)
{
    void *p = malloc(size);
    memcpy(p, data, size);
    return p;
}


unsigned int atomic_add(unsigned int *area, int val)
{
    MutexLocker locker(&sAtomicMutex);
    *area += val;
    return *area;
}

unsigned int atomic_or(unsigned int *area, unsigned int val)
{
    unsigned int oldVal;
    MutexLocker locker(&sAtomicMutex);
    oldVal = *area;
    *area = oldVal | val;
    return oldVal;
}

unsigned int atomic_sub(unsigned int *area, int val)
{
    return atomic_add(area, -val);
}

unsigned int compare_and_store(unsigned int oval, unsigned int nval, unsigned int *area)
{
    int rv;
    MutexLocker locker(&sAtomicMutex);
    if (oval == *area)
    {
        rv = 1;
        *area = nval;
    }
    else
        rv = 0;
    return rv;
}

int select_watchevent(struct eventreq *req, int which)
{
    return select_modwatch(req, which);
}

int select_modwatch(struct eventreq *req, int which)
{
    while (sMsgWindow == nullptr)
        Thread::Sleep(10);

    uint32 theEvent = 0;

    if (which & EV_RE)
        theEvent |= FD_READ | FD_ACCEPT | FD_CLOSE;
    if (which & EV_WR)
        theEvent |= FD_WRITE | FD_CONNECT;

    unsigned int message = req->er_eventid;

    // 开始监听socket发生的事件
    return ::WSAAsyncSelect(req->er_handle, sMsgWindow, message, theEvent);
}

LRESULT CALLBACK select_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
        return TRUE;
    return 0;
}

int select_waitevent(struct eventreq *req)
{
    if (sMsgWindow == nullptr)
    {
        WNDCLASSEX theWndClass;
        theWndClass.cbSize = sizeof(theWndClass);
        theWndClass.style = 0;
        theWndClass.lpfnWndProc = &select_wndproc;
        theWndClass.cbClsExtra = 0;
        theWndClass.cbWndExtra = 0;
        theWndClass.hInstance = nullptr;
        theWndClass.hIcon = nullptr;
        theWndClass.hCursor = nullptr;
        theWndClass.hbrBackground = nullptr;
        theWndClass.lpszMenuName = nullptr;
        theWndClass.lpszClassName = TEXT("P2Chat");
        theWndClass.hIconSm = nullptr;

        ::RegisterClassEx(&theWndClass);

        sMsgWindow = ::CreateWindow(TEXT("P2Chat"),
            TEXT("P2Chat"),
            WS_POPUP,
            0,
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            nullptr,
            nullptr);
        if (sMsgWindow == nullptr)
            ::exit(-1);
    }

    MSG msg;
    uint32 theErr = ::GetMessage(&msg, sMsgWindow, 0, 0);
    if (theErr > 0) 
    {
        uint32 theSelectErr = WSAGETSELECTERROR(msg.lParam);
        uint32 theEvent = WSAGETSELECTEVENT(msg.lParam);

        req->er_handle = msg.wParam;            // 存储socket
        req->er_eventbits = theEvent;
        req->er_eventid = msg.message;    // 存储message id(本项目中为事件独有的UniqueID)

        // 使该socket不再获取事件
        ::WSAAsyncSelect(req->er_handle, sMsgWindow, 0, 0);
        return 0;
    }
    else 
    {
        Assert(0);
        return EINTR;
    }
}

inline int64 GetCurrentTimeMicroS()
{
    FILETIME ft;
    LARGE_INTEGER li;
    __int64 t;

    ::GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    t = li.QuadPart;
    t -= EPOCHFILETIME;
    t /= 10;
    return t;
}

inline int64 GetCurrentTimeMilliS()
{
    return GetCurrentTimeMicroS() / 1000;
}

inline int64 GetCurrentTimeS()
{
    return GetCurrentTimeMilliS() / 1000;
}

TCHAR* FormatCalendarTime(TCHAR *buffer)
{
    time_t now = GetCurrentTimeS();
    struct tm *loc = localtime(&now);
    _tcsftime(buffer, 32, _T("%d-%b-%Y %H:%M:%S"), loc);
    return buffer;
}

TCHAR* FormatLogCalendarTime(TCHAR *buffer)
{
    int64 now = GetCurrentTimeMilliS();
    time_t t = now / 1000;
    struct tm *loc = localtime(&t);
    _tcsftime(buffer, 32, _T("[%d-%b-%Y %H:%M:%S"), loc);
    _sntprintf(&buffer[21], 8, _T(".%03d]"), (int)(now % 1000));
    return buffer;
}

bool log_open(const TCHAR *logName, log_flags flags, log_rotation_policy rotationPolicy, int maxLogSize, int historySize, const TCHAR *dailySuffix)
{
    LogThread *logThread = LogThread::GetInstance();
    log_flags logFlags = logThread->GetFlags();
    if (logFlags & LOG_IS_OPEN)
    {
        _tprintf("日志文件已经打开，请先关闭\n");
        return false;
    }
    if (logThread->LogOpen(logName, flags, rotationPolicy, maxLogSize, historySize, dailySuffix))
        logThread->Start();
    else
        return false;
    return true;
}

void log_close()
{
    LogThread *logThread = LogThread::GetInstance();
    logThread->LogClose();
}

void log_write(log_type logType, const TCHAR *format, ...)
{
    LogThread *logThread = LogThread::GetInstance();
    log_flags flags = logThread->GetFlags();
    if (!(flags & LOG_IS_OPEN))
    {
        _tprintf(_T("[error]log_write 未启动日志线程\n"));
        return;
    }

    TCHAR buffer[4096];
    va_list args;
    va_start(args, format);
    _vstprintf_s(buffer, sizeof(buffer), format, args);
    LogRecord *logRecord = new LogRecord(buffer, logType);
    logThread->EnQueueLogRecord(logRecord);
    va_end(args);
}

void log_debug(int level, const TCHAR *format, ...)
{
    LogThread *logThread = LogThread::GetInstance();
    if (level < logThread->GetDebugLevel())
        return;

    log_flags flags = logThread->GetFlags();
    if (!(flags & LOG_IS_OPEN))
    {
        _tprintf(_T("[error]log_write 未启动日志线程\n"));
        return;
    }

    TCHAR buffer[4096];
    va_list args;
    va_start(args, format);
    _vstprintf_s(buffer, sizeof(buffer), format, args);
    LogRecord *logRecord = new LogRecord(buffer, LOG_DEBUG);
    logThread->EnQueueLogRecord(logRecord);
    va_end(args);
}

void log_set_debug_level(int level)
{
    if ((level >= 0) && (level <= 9))
        LogThread::GetInstance()->SetDebugLevel(level);
}

int log_get_debug_level()
{
    return LogThread::GetInstance()->GetDebugLevel();
}

size_t utf8_to_mb(const char *src, int srcLen, char *dst, int dstLen)
{
    int len = (int)strlen(src) + 1;
    WCHAR *buffer = (len <= 32768) ? (WCHAR *)alloca(len) : (WCHAR *)malloc(len);
    MultiByteToWideChar(CP_UTF8, 0, src, -1, buffer, len);
    return WideCharToMultiByte(CP_ACP, WC_DEFAULTCHAR | WC_COMPOSITECHECK, buffer, -1, dst, dstLen, nullptr, nullptr);
}

size_t mb_to_utf8(const char *src, int srcLen, char *dst, int dstLen)
{
    int len = (int)strlen(src) + 1;
    WCHAR *buffer = (len < 32768) ? (WCHAR *)alloca(len) : (WCHAR *)malloc(len);
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, buffer, len);
    return WideCharToMultiByte(CP_UTF8, 0, buffer, -1, dst, dstLen, nullptr, nullptr);
}

WCHAR *wstr_from_mb(const char *pszString)
{
    if (pszString == nullptr)
        return nullptr;
    int nLen = (int)strlen(pszString) + 1;
    WCHAR *pwszOut = (WCHAR *)malloc(nLen * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszString, -1, pwszOut, nLen);
    return pwszOut;
}

WCHAR *wstr_from_utf8(const char *pszString)
{
    if (pszString == nullptr)
        return nullptr;
    int nLen = (int)strlen(pszString) + 1;
    WCHAR *pwszOut = (WCHAR *)malloc(nLen * sizeof(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, pszString, -1, pwszOut, nLen);
    return pwszOut;
}

char *mb_from_wstr(const WCHAR *pwszString)
{
    if (pwszString == nullptr)
        return nullptr;
    int nLen = (int)wcslen(pwszString) + 1;
    char *pszOut = (char *)malloc(nLen);
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, pwszString, -1, pszOut, nLen, nullptr, nullptr);
    return pszOut;
}

char *utf8_from_wstr(const WCHAR *pwszString)
{
    int nLen = WideCharToMultiByte(CP_UTF8, 0, pwszString, -1, nullptr, 0, nullptr, nullptr);
    char *pszOut = (char *)malloc(nLen);
    WideCharToMultiByte(CP_UTF8, 0, pwszString, -1, pszOut, nLen, nullptr, nullptr);
    return pszOut;
}


P2_NAMESPACE_END