// Standard Library

#include "TCPClient.h"
#include "UTM_Device.h"
#include "Kinect2.h"
//#include "my_sql.h"

#include <iostream>   
#include <fstream>  
#include <sstream>  


int main(int argc, char** argv)
{
	//mySQL * mysql = new mySQL();

	//mysql->ConnectDatabase();//连接数据库
	//mysql->QueryDatabase();//查询数据
	CKinect2 * myKinect = new CKinect2();

	char buff[1024 * 4];
	memset(buff, 0, sizeof(buff));
	CUTM_Device * UTM = new CUTM_Device();
	UTM->init("192.168.0.10", 10940);

	int inStart = 0;
	int inEnd = 1080;
	UTM->GetDataByGD(inStart, inEnd, 0);
	
	//FILE *fp = NULL;
	//fp = fopen("C:\\Users\\Administrator\\Desktop\\WPR程序源代码\\my_demo\\depthlibrary_test_507\\data.xls", "w");
	// 写文件  
	ofstream outFile;  
	outFile.open("data.csv", ios::out); // 打开模式可省略  

	HRESULT hr;
	myKinect->Start();
	myKinect->DepthStart();
	// Enter main loop
	while (myKinect->bToExit == false)
	{
		hr = myKinect->GetDepthFrame();
		if (SUCCEEDED(hr))
		{
			UTM->GetDataByGD(inStart, inEnd, 0);
			for (int i = 0; i < (inEnd - inStart + 1); i++)
			{
				//printf("%d ", UTM->final_data[i]);
				//fprintf(fp, "%d\t%d\n", i, UTM->final_data[i]);
				outFile << UTM->final_data[i] << ',';
			}
			outFile << endl;
			// 4c. check keyboard input
			if (cv::waitKey(30) == VK_ESCAPE){
				break;
			}
		}
	}
	//fclose(fp);
	myKinect->DepthStop();

	outFile.close();
	delete UTM;
	delete myKinect;
	//delete mysql;
	


	system("pause");

	return 0;
}

