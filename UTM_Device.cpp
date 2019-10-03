#include "UTM_Device.h"
#include <stdio.h>  
#include <string.h>  


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define PI	3.1415926535897

//send
static char m_cmdToSend[512];

//recv
static char m_recvbuf[UTM_RECVBUF_LEN] = { 0 };	//接收到的原始数据包
static char m_dataBuf[UTM_RECVBUF_LEN] = { 0 };	//解包后的数据

char origin_data[3250];//用于存储距离原始数据
char recvbuff[3400];//用于存储接收到的数据
char recvbuff1[2200];

long coordinate_data[1081];//用于存储直角坐标系数据
char check_data[64];//用于存储一行数据，方便校验和
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUTM_Device::CUTM_Device()
{
	
}

CUTM_Device::~CUTM_Device()
{
	TCPClient.CloseSocket();
}

int CUTM_Device::checkSum(char *buffer, int size, char actual_sum) //校验和
{
	char expected_sum = 0x00;
	int i;

	for (i = 0; i < size; ++i) {
		expected_sum += buffer[i];
	}
	expected_sum = (expected_sum & 0x3f) + 0x30;

	return (expected_sum == actual_sum) ? 0 : -1;
}

// Decode 6bit data  
long CUTM_Device::urg_decode(const char data[], int data_byte)//距离解码之-30h，data是数据，data_byte是长度
{
	long value = 0;
	for (int i = 0; i < data_byte; ++i) {
		value <<= 6;
		value &= ~0x3f;
		value |= data[i] - 0x30;
	}
	return value;
}

void CUTM_Device::init(char * IP, int port)
{
	BOOL b = TCPClient.ConnectTo(IP, port);
	char buff1[] = "BM\n";
	char buff2[20];
	Send(buff1, sizeof(buff1));
	Recv(buff2, sizeof(buff2));
}

void CUTM_Device::GetDataByGD(int inStart, int inEnd, int inClusterCnt)
{
	sprintf_s(m_cmdToSend, "GD%04d%04d%02d\n", inStart, inEnd, inClusterCnt);

	Send(m_cmdToSend, 13);
	memset(recvbuff, 0, sizeof(recvbuff));
	int maxnum = Recv(recvbuff, sizeof(recvbuff));
	if (recvbuff[maxnum - 2] != '\n'&& recvbuff[maxnum - 1] != '\n')//数据一次接受不完，最多接受两次就够，阈值点数为396
	{
		memset(recvbuff1, 0, sizeof(recvbuff1));
		maxnum += Recv(recvbuff1, sizeof(recvbuff1));
		strcat_s(recvbuff, recvbuff1);
	}
	//checkDATA(recvbuff, inStart, inEnd);
	m_nStartIndex = inStart;
	m_nEndIndex = inEnd;
	memset(origin_data, 0, sizeof(origin_data));
	char occ[66];
	strncpy_s(occ, recvbuff, 12);
	if (strncmp(occ, m_cmdToSend, 12) == 0)//校验返回值第一行，命令
	{
		strncpy_s(occ, recvbuff+13, 3);
		if (checkSum(occ, 2, occ[2]) == 0)//校验返回值第二行，状态
		{
			strncpy_s(occ, recvbuff + 17, 5);
			if (checkSum(occ, 4, occ[4]) == 0)//校验返回值第三行，时间戳
			{
				for (int i = 0; i < 60; i++)//校验返回值数据行，提取原始数据
				{
					if (maxnum < 23 + (i+1) * 66)
					{
						strncpy_s(occ, recvbuff + 23 + i * 66, maxnum - 23 - i * 66 - 2);//最后一行
						if (checkSum(occ, maxnum - 23 - i * 66 - 2 - 1, recvbuff[maxnum - 2 - 1]) == 0)
							strcat_s(origin_data, occ);//距离数据链接成一串
						break;
					}
					strncpy_s(occ, recvbuff + 23 + i * 66, 64);
					if (checkSum(occ, 64, recvbuff[23 + i * 66 + 64]) == 0)
						strcat_s(origin_data, occ);//距离数据链接成一串
				}

				memset(final_data, 0, sizeof(final_data));
				int v = sizeof(origin_data);
				for (int j = 0; j < (inEnd - inStart + 1); j++)//原始数据转为距离数据
				{
					final_data[j] = urg_decode(origin_data + j * 3, 3);
				}
			}
			else printf("error:time stamp recieve");
		}
		else printf("error:status recieve");
	}
	else printf("error:command recieve");
}

void CUTM_Device::Send(char *inbuf, int inLen)
{
	TCPClient.Send(inbuf, inLen);
}

int CUTM_Device::Recv(char *inbuf, int inLen)
{
	int r = TCPClient.Recv(inbuf, inLen);
	return r;
}