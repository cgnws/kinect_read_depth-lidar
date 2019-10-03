#pragma once
// Standard Library
#include <array>
#include <iostream>

// OpenCV Header
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


// Kinect for Windows SDK Header
#include <Kinect.h>
#include <cstring>

using namespace cv;
using namespace std;

class CKinect2
{
public:
	CKinect2();
	~CKinect2();

	IKinectSensor* pSensor;
	IColorFrameReader* pColorFrameReader;
	IDepthFrameReader* pDepthFrameReader;
	IBodyFrameReader* pBodyFrameReader;
	ICoordinateMapper* pCoordinateMapper;
	char depthstr[100];

	HRESULT CKinect2::Start();//启动摄像头
	HRESULT CKinect2::ColorStart();//启动彩色摄像头
	HRESULT CKinect2::DepthStart();//启动深度摄像头
	HRESULT CKinect2::BodyStart();//启动骨骼数据读取
	HRESULT CKinect2::CoordinateMapper();//坐标系转换函数
	void CKinect2::ColorStop();//结束
	void CKinect2::DepthStop();//结束
	void CKinect2::BodyStop();//结束
	void CKinect2::CoordinateMapperStop();
	void CKinect2::DrawLine(cv::Mat& rImg, const Joint& rJ1, const Joint& rJ2, ICoordinateMapper* pCMapper);//绘制骨骼点连线
	bool bToExit;//退出标志字
	HRESULT CKinect2::GetDepthFrame();//获取当前时刻深度图
	HRESULT CKinect2::GetColorFrame();//获取当前时刻彩色图
	HRESULT CKinect2::GetBodydata();//获取当前时刻骨骼数据并显示
	void CKinect2::WholeSteps();//完整实现一次三种数据读取

	//RGB彩色
	static const int        rColorWidth = 1920;//彩色图宽度像素点个数
	static const int        rColorHeight = 1080;//彩色图高度像素点个数
	UINT                    uColorPointNum = rColorWidth * rColorHeight;
	UINT                    uColorBufferSize = uColorPointNum * 4 * sizeof(BYTE);//彩色图所需内存
	cv::Mat	                mColorImg;//彩色图像
	//RGBQUAD*                m_pColorRGBX;
	//unsigned char*			m_pColorBuf;
	//DepthSpacePoint*		cColorToDepthPoints;

	//Depth深度
	Mat CKinect2::ConvertMat(const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth);
	static const int        rDepthWidth = 512;//深度图宽度像素点个数
	static const int        rDepthHeight = 424;//深度图高度像素点个数
	UINT                    uDepthPointNum = rDepthWidth * rDepthHeight;
	UINT16                  uDepthMin;//读到的深度图最小深度
	UINT16                  uDepthMax;//读到的深度图最大深度
	//DepthSpacePoint			*depthPoints;
	//CameraSpacePoint		*cameraPoints;

	//Body数据
	IBody**                 aBodyData;//人体数据
	INT32                   iBodyCount;//识别的人体个数
	cv::Mat                 mbodyImg;//骨骼所画图像
};

