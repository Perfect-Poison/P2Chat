#include "p2_util.h"
#include "Mutex.h"
P2_NAMESPACE_BEG

static Mutex sAtomicMutex;
static HWND sMsgWindow = nullptr;

#define EPOCHFILETIME (116444736000000000i64)


char *bin_to_str(const BYTE* pData, size_t size, char *pStr, bool hasSeparator/* = true*/)
{
    size_t i;
    char *pCurr;
    const char* kHEXChars = { "0123456789ABCDEF" };
    for (i = 0, pCurr = pStr; i < size; i++)
    {
        *pCurr++ = kHEXChars[pData[i] >> 4];
        *pCurr++ = kHEXChars[pData[i] & 0xF];
        if (hasSeparator)
            *pCurr++ = ' ';
    }
    *pCurr = 0;
    return pStr;
}

size_t str_to_bin(const char* pStr, BYTE *pData, size_t size)
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

    // ��ʼ����socket�������¼�
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

        req->er_handle = msg.wParam;            // �洢socket
        req->er_eventbits = theEvent;
        req->er_eventid = msg.message;    // �洢message id(����Ŀ��Ϊ�¼����е�UniqueID)

        // ʹ��socket���ٻ�ȡ�¼�
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

char* FormatCalendarTime(char *buffer)
{
    time_t now = GetCurrentTimeS();
    struct tm *loc = localtime(&now);
    strftime(buffer, 32, "%d-%b-%Y %H:%M:%S", loc);
    return buffer;
}

char* FormatLogCalendarTime(char *buffer)
{
    int64 now = GetCurrentTimeMilliS();
    time_t t = now / 1000;
    struct tm *loc = localtime(&t);
    strftime(buffer, 32, "[%d-%b-%Y %H:%M:%S", loc);
    snprintf(&buffer[21], 8, ".%03d]", (int)(now % 1000));
    return buffer;
}

P2_NAMESPACE_END