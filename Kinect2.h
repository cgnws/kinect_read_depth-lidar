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

	HRESULT CKinect2::Start();//��������ͷ
	HRESULT CKinect2::ColorStart();//������ɫ����ͷ
	HRESULT CKinect2::DepthStart();//�����������ͷ
	HRESULT CKinect2::BodyStart();//�����������ݶ�ȡ
	HRESULT CKinect2::CoordinateMapper();//����ϵת������
	void CKinect2::ColorStop();//����
	void CKinect2::DepthStop();//����
	void CKinect2::BodyStop();//����
	void CKinect2::CoordinateMapperStop();
	void CKinect2::DrawLine(cv::Mat& rImg, const Joint& rJ1, const Joint& rJ2, ICoordinateMapper* pCMapper);//���ƹ���������
	bool bToExit;//�˳���־��
	HRESULT CKinect2::GetDepthFrame();//��ȡ��ǰʱ�����ͼ
	HRESULT CKinect2::GetColorFrame();//��ȡ��ǰʱ�̲�ɫͼ
	HRESULT CKinect2::GetBodydata();//��ȡ��ǰʱ�̹������ݲ���ʾ
	void CKinect2::WholeSteps();//����ʵ��һ���������ݶ�ȡ

	//RGB��ɫ
	static const int        rColorWidth = 1920;//��ɫͼ������ص����
	static const int        rColorHeight = 1080;//��ɫͼ�߶����ص����
	UINT                    uColorPointNum = rColorWidth * rColorHeight;
	UINT                    uColorBufferSize = uColorPointNum * 4 * sizeof(BYTE);//��ɫͼ�����ڴ�
	cv::Mat	                mColorImg;//��ɫͼ��
	//RGBQUAD*                m_pColorRGBX;
	//unsigned char*			m_pColorBuf;
	//DepthSpacePoint*		cColorToDepthPoints;

	//Depth���
	Mat CKinect2::ConvertMat(const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth);
	static const int        rDepthWidth = 512;//���ͼ������ص����
	static const int        rDepthHeight = 424;//���ͼ�߶����ص����
	UINT                    uDepthPointNum = rDepthWidth * rDepthHeight;
	UINT16                  uDepthMin;//���������ͼ��С���
	UINT16                  uDepthMax;//���������ͼ������
	//DepthSpacePoint			*depthPoints;
	//CameraSpacePoint		*cameraPoints;

	//Body����
	IBody**                 aBodyData;//��������
	INT32                   iBodyCount;//ʶ����������
	cv::Mat                 mbodyImg;//��������ͼ��
};

