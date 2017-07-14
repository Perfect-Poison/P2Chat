/********************************************************************
	日期:	2016/12/14 11:06:05
	文件名:	CommonSocket.h
	作者:	BrianYi
	
	用途:	TCP和UDP类的公共基类，用于封装socket处理
*********************************************************************/
#pragma once
#include "common.h"
#include "Address.h"

P2_NAMESPACE_BEG

class CommonSocket
{
public:
    CommonSocket(INT32 inSocketType, INT32 inProtocol, IOType inIOType = Blocking);
    ~CommonSocket();
	void setIOType(IOType inIOType);
    void reuse_addr();
    void no_delay();
    void keep_alive();
    void set_socket_sndbuf_size(UINT32 inNewSize);
    void set_socket_rcvbuf_size(UINT32 inNewSize);
protected:
    void open();
    void close();
    void bind_to_port(const USHORT& inPort);
protected:
    SOCKET m_socketID;
    INT32 m_socketType;
    INT32 m_protocol;
    BOOL m_opened;
    BOOL m_binded;
    IOType m_ioType;
private:
    CommonSocket();
    static UINT32 s_num_sockets;
};

P2_NAMESPACE_END