#include "Common/common.h"
#include "Common/Mutex.h"
P2_NAMESPACE_BEG

static Mutex sAtomicMutex;
static HWND sMsgWindow = nullptr;

void * memdup(const void *data, size_t size)
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
        theWndClass.lpszClassName = "P2Chat";
        theWndClass.hIconSm = nullptr;

        ::RegisterClassEx(&theWndClass);

        sMsgWindow = ::CreateWindow("P2Chat",
            "P2Chat",
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


P2_NAMESPACE_END