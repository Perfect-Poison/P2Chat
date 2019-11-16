#pragma once
#include "p2_common.h"
#include "p2_util.h"
#include "p2_cpapi.h"
P2_NAMESPACE_BEG



class Session
{
    int64   fSessionId;
    time_t  fCreationTime;
    time_t  fLastAccessTime;
    uint16  fState;
    BOOL    fIsLogined;
	UserInfo fUserInfo;
public:
    Session();
    virtual ~Session();
    int64 GetSessionID() { return fSessionId; }
    time_t GetCreationTime() { return fCreationTime; }
    time_t GetLastAccessTime() { return fLastAccessTime; }
    uint16 GetState() { return fState; }
    BOOL Logined() { return fIsLogined; }
	UserInfo GetUserInfo() { return fUserInfo; }
    void SetSessionID(const int64& inSessionID) { fSessionId = inSessionID; }
    void SetLastAccessTime(const time_t& inLastAccessTime) { fLastAccessTime = inLastAccessTime; }
    void SetState(const uint16& inState) { fState = inState; }
    void SetLogined(const BOOL& inIsLogined) { fIsLogined = inIsLogined; }
	void SetUserInfo(const UserInfo& userinfo) { fUserInfo = userinfo; }
};

class SessionTable
{
    static unordered_map<int64, Session*> fSessionTable;
public:
    SessionTable() {}
    virtual ~SessionTable() {}
    static Session* GetSession(int64 inSessionID);
    static BOOL AddSession(Session* inSession);
    static BOOL DelSession(int64 inSessionID);
};

P2_NAMESPACE_END