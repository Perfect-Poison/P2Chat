/********************************************************************
	日期:	2016/12/14 11:05:16
	文件名:	TCP.h
	作者:	BrianYi
	
	用途:	TCP连接的实现
*********************************************************************/
#pragma once
#include "CommonSocket.h"

P2_NAMESPACE_BEG

class TCP :
	public CommonSocket
{
public:
    TCP();
    TCP(const TCP& inTCP);
	~TCP();
	void listen_on_port(const USHORT& inPort);
	INT32 connect_to(const Address& inAddress);
	INT32 connect_to(const std::string& inIP, const USHORT& inPort);
	TCP accept_client(IOType inIOType = Blocking);
	INT32 send(const char* inContent, const size_t& inSize);
	INT32 receive(char* outContent, const size_t& inSize, IOType inIOType = Blocking);
	Address getAddress(void);
	std::string getIP(void);
	USHORT getPort(void);
protected:
	Address m_address;
};

P2_NAMESPACE_END