#include "Session.h"
P2_NAMESPACE_BEG

unordered_map<int64, Session*> SessionTable::fSessionTable;

Session::Session()
{
    fSessionId = -1;
    fCreationTime = time(0);
    fLastAccessTime = fCreationTime;
    fState = MSG_SERVER_GET_INFO;
    fIsLogined = FALSE;
}


Session::~Session()
{
}

Session* SessionTable::GetSession(int64 inSessionID)
{
    if (fSessionTable.count(inSessionID))
        return fSessionTable[inSessionID];
    return nullptr;
}

BOOL SessionTable::AddSession(Session* inSession)
{
    if (fSessionTable.count(inSession->GetSessionID()))
        return FALSE;
    fSessionTable.insert(make_pair(inSession->GetSessionID(), inSession));
    return TRUE;
}

BOOL SessionTable::DelSession(int64 inSessionID)
{
    if (fSessionTable.count(inSessionID))
    {
        Session *session = fSessionTable[inSessionID];
        safe_delete(session);
        fSessionTable.erase(inSessionID);
        return TRUE;
    }
    return FALSE;
}

P2_NAMESPACE_END

