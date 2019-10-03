// TCPClient.cpp: implementation of the CTCPClient class.
//
//////////////////////////////////////////////////////////////////////  
#include "TCPClient.h"
#include <stdio.h> 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//THIS_FILE是一个char数组全局变量，字符串值为当前文件的全路径，这样在Debug版本中当程序出错时出错处理代码可用这个变量告诉你是哪个文件中的代码有问题。

CTCPClient::CTCPClient()
{
}

CTCPClient::~CTCPClient()
{
}


void CTCPClient::CloseSocket()
{
	if (cntskt != INVALID_SOCKET)//无效socket，值为十六进制0xFFFFFFFF
	{
		closesocket(cntskt);
		cntskt = INVALID_SOCKET;
	}
}

void CTCPClient::Send(char *inbuf, int inLen)
{
		send(cntskt, inbuf, inLen, 0);
}

int CTCPClient::Recv(char *inbuf, int inLen)
{
		int r = recv(cntskt, inbuf, inLen, 0);
		return r;
}

BOOL CTCPClient::ConnectTo(char * inDesIP, int inDesPort)
{
	//加载套接字  
	WSADATA wsaData;
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
		return FALSE;
	}

	SOCKADDR_IN serversocketaddr;
	serversocketaddr.sin_family = AF_INET;
	serversocketaddr.sin_addr.S_un.S_addr = inet_addr(inDesIP);
	serversocketaddr.sin_port = htons(inDesPort);

	cntskt = socket(AF_INET, SOCK_STREAM, 0);

	if (SOCKET_ERROR == cntskt){
		printf("Socket() error:%d", WSAGetLastError());
		return FALSE;
	}

	//连接服务器
	if (connect(cntskt, (struct sockaddr *)&serversocketaddr, sizeof(serversocketaddr)) != INVALID_SOCKET)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}
}