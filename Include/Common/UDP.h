/********************************************************************
	����:	2016/12/14 11:05:46
	�ļ���:	UDP.h
	����:	BrianYi
	
	��;:	UDP���ӵ�ʵ��
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
	void listen_on_port(const USHORT& inPort);
	INT32 send(const Address& inAddress, const char* inContent, const size_t& inSize);
    INT32 send(const std::string& inIP, const USHORT& inPort, const char* inContent, const size_t& inSize);
    INT32 receive(char* outContent, const size_t& inSize, IOType inIOType = Blocking);
    INT32 receive(char* outContent, const size_t& inSize, Address& outAddress, IOType inIOType = Blocking);
    INT32 receive(char* outContent, const size_t& inSize, std::string& outIP, USHORT& outPort, IOType inIOType = Blocking);
};

P2_NAMESPACE_END