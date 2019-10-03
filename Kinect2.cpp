#include "direct.h"
#include "Kinect2.h"



int i1;

CKinect2::CKinect2() :
pSensor(NULL),
pCoordinateMapper(NULL),
pBodyFrameReader(NULL),
pColorFrameReader(NULL),
pDepthFrameReader(NULL)
{
	bToExit = false;
}

CKinect2::~CKinect2()
{
	// Close Sensor
	cout << "close sensor" << endl;
	pSensor->Close();

	// Release Sensor
	cout << "Release sensor" << endl;
	pSensor->Release();
	pSensor = nullptr;

}

HRESULT CKinect2::Start()
{
	i1 = 0;
	HRESULT hr;

	// 1a. Sensor related code
	cout << "Try to get default sensor" << endl;
	hr = GetDefaultKinectSensor(&pSensor);
	if (FAILED(hr))
	{
		cerr << "Get Sensor failed" << endl;
		return hr;
	}

	// 1b. Open sensor
	hr = pSensor->Open();
	cout << "Try to open sensor" << endl;
	if (FAILED(hr))
	{
		cerr << "Can't open sensor" << endl;
		return hr;
	}
}

	
HRESULT CKinect2::CoordinateMapper()
{
	HRESULT hr;
	// 6. Coordinate Mapper
	hr = pSensor->get_CoordinateMapper(&pCoordinateMapper);
	if (FAILED(hr))
	{
		cerr << "get_CoordinateMapper failed" << endl;
		return hr;
	}
}

void CKinect2::ColorStop()
{
	i1 = 0;
	bToExit = true;
	// release frame reader
	cout << "Release Colorframe reader" << endl;
	pColorFrameReader->Release();
	pColorFrameReader = nullptr;
}

void CKinect2::DepthStop()
{
	i1 = 0;
	bToExit = true;
	// release frame reader
	cout << "Release Depthframe reader" << endl;
	pDepthFrameReader->Release();
	pDepthFrameReader = nullptr;
}

void CKinect2::BodyStop()
{
	i1 = 0;
	bToExit = true;
	delete aBodyData;
	// release frame reader
	cout << "Release Bodyframe reader" << endl;
	pBodyFrameReader->Release();
	pBodyFrameReader = nullptr;
}

void CKinect2::CoordinateMapperStop()
{
	// release coordinate mapper
	cout << "Release coordinate mapper" << endl;
	pCoordinateMapper->Release();
	pCoordinateMapper = nullptr;
}

void CKinect2::DrawLine(cv::Mat& rImg, const Joint& rJ1, const Joint& rJ2, ICoordinateMapper* pCMapper)
{
	if (rJ1.TrackingState == TrackingState_NotTracked || rJ2.TrackingState == TrackingState_NotTracked)
		return;

	ColorSpacePoint ptJ1, ptJ2;
	pCMapper->MapCameraPointToColorSpace(rJ1.Position, &ptJ1);
	pCMapper->MapCameraPointToColorSpace(rJ2.Position, &ptJ2);

	cv::line(rImg, cv::Point(ptJ1.X, ptJ1.Y), cv::Point(ptJ2.X, ptJ2.Y), cv::Vec3b(0, 0, 255), 5);
}

HRESULT CKinect2::GetDepthFrame()
{
	HRESULT hr;
	// 1a. Get last frame
	IDepthFrame* pDepthFrame = nullptr;
	hr = pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);
	if (SUCCEEDED(hr))
	{
		// 4c. Get image buffer
		UINT    uBufferSize = 0;
		UINT16*    pBuffer = nullptr;
		pDepthFrame->AccessUnderlyingBuffer(&uBufferSize, &pBuffer);

		Mat mDepthImg = ConvertMat(pBuffer, rDepthWidth, rDepthHeight, uDepthMin, uDepthMax);

		vector<int>compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);
		//sprintf(depthstr, "C:\\Users\\Administrator\\Desktop\\WPR程序源代码\\my_demo\\depthlibrary_test_507\\depthlibrary_test_507\\Depth%d.png", i1);
		//sprintf(depthstr, "C:/Users/Administrator/Desktop/WPR程序源代码/my_demo/depthlibrary_test_507/depthlibrary_test_507/Depth%d.png", i1);
		sprintf(depthstr, "./DepthImage/Depth%d.png", i1);
		imwrite(depthstr, mDepthImg, compression_params);
		imshow("Color Map", mDepthImg);
		memset(depthstr, 0, sizeof(depthstr));
		i1++;
		// 1d. release frame
		pDepthFrame->Release();
		pDepthFrame = nullptr;
		bToExit = false;
	}
	return hr;
}

HRESULT CKinect2::GetColorFrame()
{
	HRESULT hr;
	// Prepare OpenCV data
	
	mColorImg = cv::Mat(rColorHeight, rColorWidth, CV_8UC4);
	// 1a. Get last frame
	IColorFrame* pColorFrame = nullptr;
	hr = pColorFrameReader->AcquireLatestFrame(&pColorFrame);
	if (SUCCEEDED(hr))
	{
		// 1b. Copy to OpenCV image
		hr = pColorFrame->CopyConvertedFrameDataToArray(uColorBufferSize, mColorImg.data, ColorImageFormat_Bgra);
		if (FAILED(hr))
		{
			cerr << "Data copy error" << endl;
		}
		cv::imshow("Color Map", mColorImg);
		// 1c. release frame
		pColorFrame->Release();
	}
	return hr;
}

HRESULT CKinect2::GetBodydata()
{
	HRESULT hr;
	mbodyImg = mColorImg.clone();
	// 4b. Get body data
	IBodyFrame* pBodyFrame = nullptr;
	hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr))
	{
		// 4b. get Body data
		hr = pBodyFrame->GetAndRefreshBodyData(iBodyCount, aBodyData);
		if (SUCCEEDED(hr))
		{
			// 4c. for each body
			for (int i = 0; i < iBodyCount; ++i)
			{
				IBody* pBody = aBodyData[i];

				// check if is tracked
				BOOLEAN bTracked = false;
				if ((pBody->get_IsTracked(&bTracked) == S_OK) && bTracked)
				{
					// get joint position
					Joint aJoints[JointType::JointType_Count];
					if (pBody->GetJoints(JointType::JointType_Count, aJoints) == S_OK)
					{
						DrawLine(mbodyImg, aJoints[JointType_SpineBase], aJoints[JointType_SpineMid], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_SpineShoulder], aJoints[JointType_Neck], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_Neck], aJoints[JointType_Head], pCoordinateMapper);

						DrawLine(mbodyImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_WristLeft], aJoints[JointType_HandLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_HandLeft], aJoints[JointType_HandTipLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_HandLeft], aJoints[JointType_ThumbLeft], pCoordinateMapper);

						DrawLine(mbodyImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_ElbowRight], aJoints[JointType_WristRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_WristRight], aJoints[JointType_HandRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_HandRight], aJoints[JointType_HandTipRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_HandRight], aJoints[JointType_ThumbRight], pCoordinateMapper);

						DrawLine(mbodyImg, aJoints[JointType_SpineBase], aJoints[JointType_HipLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_AnkleLeft], aJoints[JointType_FootLeft], pCoordinateMapper);

						DrawLine(mbodyImg, aJoints[JointType_SpineBase], aJoints[JointType_HipRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_HipRight], aJoints[JointType_KneeRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight], pCoordinateMapper);
						DrawLine(mbodyImg, aJoints[JointType_AnkleRight], aJoints[JointType_FootRight], pCoordinateMapper);
					}
				}
			}
		}
		else
		{
			cerr << "Can't read body data" << endl;
		}

		// 4e. release frame
		pBodyFrame->Release();
	}

	// show image
	cv::imshow("Body Image", mbodyImg);
	return hr;
}

HRESULT CKinect2::ColorStart()
{
	HRESULT hr;
	// 2. Color related code
	int iColorWidth = 0, iColorHeight = 0;

		cout << "Try to get color source" << endl;
		// 2a.Get frame source
		IColorFrameSource* pcFrameSource = nullptr;
		hr = pSensor->get_ColorFrameSource(&pcFrameSource);
		if (FAILED(hr))
		{
			cerr << "Can't get color frame source" << endl;
			return hr;
		}

		// 2b.Get frame description
		cout << "get color frame description" << endl;
		IFrameDescription* pcFrameDescription = nullptr;
		hr = pcFrameSource->get_FrameDescription(&pcFrameDescription);

		if (SUCCEEDED(hr))
		{
			pcFrameDescription->get_Width(&iColorWidth);
		}

		if (SUCCEEDED(hr))
		{
			pcFrameDescription->get_Height(&iColorHeight);
		}

		pcFrameDescription->Release();
		pcFrameDescription = nullptr;

		// 2c.get frame reader
		cout << "Try to get color frame reader" << endl;
		if (SUCCEEDED(hr))
		{
			hr = pcFrameSource->OpenReader(&pColorFrameReader);
		}
		if (FAILED(hr))
		{
			cerr << "Can't get color frame reader" << endl;
			return hr;
		}

		// 2d.release Frame source
		cout << "Release frame source" << endl;
		pcFrameSource->Release();
		pcFrameSource = nullptr;
}
HRESULT CKinect2::DepthStart()
{
	HRESULT hr;
	// 3. Depth related code
	uDepthMin = 0;
	uDepthMax = 0;
	
		cout << "Try to get depth source" << endl;
		// 3a.Get frame source
		IDepthFrameSource* pdFrameSource = nullptr;
		hr = pSensor->get_DepthFrameSource(&pdFrameSource);
		if (FAILED(hr))
		{
			cerr << "Can't get depth frame source" << endl;
			return hr;
		}

		// 3b. get some dpeth only meta
		pdFrameSource->get_DepthMinReliableDistance(&uDepthMin);
		pdFrameSource->get_DepthMaxReliableDistance(&uDepthMax);
		cout << "Reliable Distance: "
			<< uDepthMin << " – " << uDepthMax << endl;

		// 3c.Get frame description
		cout << "get depth frame description" << endl;
		IFrameDescription* pdFrameDescription = nullptr;
		int iDepthWidth = 0, iDepthHeight = 0;
		hr = pdFrameSource->get_FrameDescription(&pdFrameDescription);

		if (SUCCEEDED(hr))
		{
			hr = pdFrameDescription->get_Width(&iDepthWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pdFrameDescription->get_Height(&iDepthHeight);
		}

		pdFrameDescription->Release();
		pdFrameDescription = nullptr;

		// 3d.get frame reader
		cout << "Try to get depth frame reader" << endl;
		hr = pdFrameSource->OpenReader(&pDepthFrameReader);
		if (FAILED(hr))
		{
			cerr << "Can't get depth frame reader" << endl;
			return hr;
		}

		// 3e.release Frame source
		cout << "Release frame source" << endl;
		pdFrameSource->Release();
		pdFrameSource = nullptr;
	
}
HRESULT CKinect2::BodyStart()
{
	HRESULT hr;
	// 4. Body related code
	aBodyData = nullptr;
	iBodyCount = 0;
	
		// 4a. Get frame source
		cout << "Try to get body source" << endl;
		IBodyFrameSource* pbFrameSource = nullptr;
		hr = pSensor->get_BodyFrameSource(&pbFrameSource);
		if (FAILED(hr))
		{
			cerr << "Can't get body frame source" << endl;
			return hr;
		}

		// 4b. Get the number of body
		hr = pbFrameSource->get_BodyCount(&iBodyCount);
		if (FAILED(hr))
		{
			cerr << "Can't get body count" << endl;
			return hr;
		}
		cout << " > Can trace " << iBodyCount << " bodies" << endl;
		aBodyData = new IBody*[iBodyCount];
		for (int i = 0; i < iBodyCount; ++i)
			aBodyData[i] = nullptr;

		// 4c. get frame reader
		cout << "Try to get body frame reader" << endl;
		hr = pbFrameSource->OpenReader(&pBodyFrameReader);
		if (FAILED(hr))
		{
			cerr << "Can't get body frame reader" << endl;
			return hr;
		}

		// 4d. release Frame source
		cout << "Release frame source" << endl;
		pbFrameSource->Release();
		pbFrameSource = nullptr;
	
}

void CKinect2::WholeSteps()
{
	HRESULT hr;
	Start();
	ColorStart();
	DepthStart();
	BodyStart();

	CoordinateMapper();

	// Enter main loop
	while (bToExit == false)
	{
		hr=GetDepthFrame();
		//if (i1 > 10) break;

		hr=GetColorFrame();
		hr=GetBodydata();

		// 4c. check keyboard input
		if (cv::waitKey(30) == VK_ESCAPE){
			break;
		}
	}
	CoordinateMapperStop();
	ColorStop();
	DepthStop();
	BodyStop();
}

/// PNG: 把 16位深度数据 从 UINT16型的数组 转换到 CV_16UC1的Mat，即16位无符号整型单通道Mat中
Mat CKinect2::ConvertMat(const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth)
{
	Mat img(nHeight, nWidth, CV_16UC1);
	UINT16* p_mat = (UINT16*)img.data;

	const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

	while (pBuffer < pBufferEnd)
	{
		USHORT depth = *pBuffer;
		*p_mat = (depth >= nMinDepth) && (depth <= nMaxDepth) ? depth << 3 : 0;
		p_mat++;
		++pBuffer;
	}
	return img;
}