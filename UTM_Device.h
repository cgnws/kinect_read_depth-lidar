#define UTM_RECVBUF_LEN 1024*4
#include "TCPClient.h"
class CUTM_Device
{
public:
	CUTM_Device();
	virtual ~CUTM_Device();
	void CUTM_Device::init(char * IP, int port);
	int CUTM_Device::Recv(char *inbuf, int inLen);
	void CUTM_Device::Send(char *inbuf, int inLen);


protected:
	CTCPClient TCPClient;
	long CUTM_Device::urg_decode(const char* data, int data_byte);

public:
	int CUTM_Device::checkSum(char *buffer, int size, char actual_sum);//校验和
	bool CUTM_Device::checkDATA(char *buff, int inStart, int inEnd);//检验接收到的数据是否正确，分类存储并解析
	void CUTM_Device::GetDataByGD(int inStart, int inEnd, int inClusterCnt);
	int m_nStartIndex;		//扫描开始的序号
	int m_nEndIndex;		//
	long final_data[1081];//用于存储距离解码数据，柱坐标
};