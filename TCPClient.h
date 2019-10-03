#pragma once

#include <WinSock2.h>   
#pragma comment(lib, "ws2_32.lib")  


class CTCPClient
{
public:
	CTCPClient();
	virtual ~CTCPClient();

public:
	void CloseSocket();
	void Send(char *inbuf, int inLen);
	int Recv(char *inbuf, int inLen);

	BOOL ConnectTo(char * inDesIP, int inDesPort);
	SOCKET cntskt;
};


