#include "Common/common.h"
#include "mysqlpp/mysql++.h"
#include "ConnectionPool.h"
#include "Common/UDP.h"
using namespace mysqlpp;

LRESULT CALLBACK select_wndproc(HWND inWIndow, UINT inMsg, WPARAM inParam, LPARAM inOtherParam);

int main()
{
    WNDCLASSEX theWndClass;
    theWndClass.cbSize = sizeof(theWndClass);
    theWndClass.style = 0;
    theWndClass.lpfnWndProc = &select_wndproc;
    theWndClass.cbClsExtra = 0;
    theWndClass.cbWndExtra = 0;
    theWndClass.hInstance = NULL;
    theWndClass.hIcon = NULL;
    theWndClass.hCursor = NULL;
    theWndClass.hbrBackground = NULL;
    theWndClass.lpszMenuName = NULL;
    theWndClass.lpszClassName = "DarwinStreamingServerWindow";
    theWndClass.hIconSm = NULL;

    return 0;
}
