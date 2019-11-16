/********************************************************************
	����:	2016/12/14 11:06:05
	�ļ���:	Socket.h
	����:	BrianYi
	
	��;:	TCPSocket��UDPSocket��Ĺ������࣬���ڷ�װsocket����
*********************************************************************/
#pragma once
#include "Address.h"
P2_NAMESPACE_BEG

#if P2CHAT_DEBUG
#define SOCKET_DEBUG 1
#else
#define SOCKET_DEBUG 0
#endif

class Socket
{
public:
    enum
    {
        kBlocking = 0,
        kNonBlocking = 1,
    };
    typedef int32 IOType;
    enum
    {
        kMaxSendBufSize = 96u * 1024u,
        kMaxRecvBufSize = 96u * 1024u
    };
    enum
    {
        kListenQueueLength = 128u
    };
    enum
    {
        kInvalidSocketID = -1
    };
public:
    Socket(int inSocketID);
    ~Socket();
	void SetIOType(IOType inIOType);
    void ReuseAddr();
    void NoDelay();
    void KeepAlive();
    void SetSocketSendBufferSize(uint32 inNewSize);
    void SetSocketRecvBufferSize(uint32 inNewSize);
protected:
    void Open(int32 inSocketType, int32 inProtocol);
    void Close();
    void _Bind(const USHORT& inPort);
protected:
    SOCKET fSocketID;
};

P2_NAMESPACE_END