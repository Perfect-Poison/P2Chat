#pragma once
#include "p2client_common.h"
P2_NAMESPACE_BEG

class ClientSession
{
    int64   fSessionId;
    time_t  fCreationTime;
    time_t  fLastAccessTime;
    uint16  fState;
public:
    ClientSession();
    ~ClientSession();
    int64 GetSessionID() { return fSessionId; }
    time_t GetCreationTime() { return fCreationTime; }
    time_t GetLastAccessTime() { return fLastAccessTime; }
    uint16 GetState() { return fState; }
    void SetSessionID(const int64& inSessionID) { fSessionId = inSessionID; }
    void SetLastAccessTime(const time_t& inLastAccessTime) { fLastAccessTime = inLastAccessTime; }
    void SetState(const uint16& inState) { fState = inState; }
};

class ClientSessionTable
{
    static unordered_map<int64, ClientSession*> fSessionTable;
public:
    ClientSessionTable() {}
    virtual ~ClientSessionTable() {}
    static ClientSession* GetSession(int64 inSessionID);
    static BOOL AddSession(ClientSession* inSession);
    static BOOL DelSession(int64 inSessionID);
};

P2_NAMESPACE_END