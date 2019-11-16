#include "ClientSession.h"
#include "p2_cpapi.h"
P2_NAMESPACE_BEG

unordered_map<int64, ClientSession*> ClientSessionTable::fSessionTable;

ClientSession::ClientSession()
{
    fSessionId = -1;
    fCreationTime = time(0);
    fLastAccessTime = fCreationTime;
    fState = MSG_SERVER_GET_INFO;
}


ClientSession::~ClientSession()
{
}

ClientSession* ClientSessionTable::GetSession(int64 inSessionID)
{
    if (fSessionTable.count(inSessionID))
        return fSessionTable[inSessionID];
    return nullptr;
}

BOOL ClientSessionTable::AddSession(ClientSession* inSession)
{
    if (fSessionTable.count(inSession->GetSessionID()))
        return FALSE;
    fSessionTable.insert(make_pair(inSession->GetSessionID(), inSession));
    return TRUE;
}

BOOL ClientSessionTable::DelSession(int64 inSessionID)
{
    if (fSessionTable.count(inSessionID))
    {
        ClientSession *session = fSessionTable[inSessionID];
        safe_delete(session);
        fSessionTable.erase(inSessionID);
        return TRUE;
    }
    return FALSE;
}

P2_NAMESPACE_END