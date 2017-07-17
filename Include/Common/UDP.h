/********************************************************************
	日期:	2016/12/14 11:05:46
	文件名:	UDP.h
	作者:	BrianYi
	
	用途:	UDP连接的实现
*********************************************************************/
#pragma once
#include "CommonSocket.h"

P2_NAMESPACE_BEG

class UDP :
    public CommonSocket
{
public:
    UDP();
    UDP(const UDP& inUDP);
    ~UDP();
	void bind_port(const USHORT& inPort);
	int32 send(const Address& inAddress, const char* inContent, const size_t& inSize);
    int32 send(const std::string& inIP, const USHORT& inPort, const char* inContent, const size_t& inSize);
    int32 receive(char* outContent, const size_t& inSize, IOType inIOType = kBlocking);
    int32 receive(char* outContent, const size_t& inSize, Address& outAddress, IOType inIOType = kBlocking);
    int32 receive(char* outContent, const size_t& inSize, std::string& outIP, USHORT& outPort, IOType inIOType = kBlocking);
};

P2_NAMESPACE_END