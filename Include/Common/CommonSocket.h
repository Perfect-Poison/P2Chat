/********************************************************************
	����:	2016/12/14 11:06:05
	�ļ���:	CommonSocket.h
	����:	BrianYi
	
	��;:	TCP��UDP��Ĺ������࣬���ڷ�װsocket����
*********************************************************************/
#pragma once
#include "common.h"
#include "Address.h"

P2_NAMESPACE_BEG

class CommonSocket
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
public:
    CommonSocket(int32 inSocketType, int32 inProtocol, IOType inIOType = kBlocking);
    ~CommonSocket();
	void setIOType(IOType inIOType);
    void reuse_addr();
    void no_delay();
    void keep_alive();
    void set_socket_sndbuf_size(uint32 inNewSize);
    void set_socket_rcvbuf_size(uint32 inNewSize);
protected:
    void open();
    void close();
    void bind_to_port(const USHORT& inPort);
protected:
    SOCKET m_socketID;
    int32 m_socketType;
    int32 m_protocol;
    BOOL m_opened;
    BOOL m_binded;
    IOType m_ioType;
private:
    CommonSocket();
    static uint32 s_num_sockets;
};

P2_NAMESPACE_END