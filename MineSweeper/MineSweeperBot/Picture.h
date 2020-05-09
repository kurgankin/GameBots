#pragma once

#include <Windows.h>
#include <string>
#include <opencv2/opencv.hpp>


class Picture
{
public:
	Picture(HBITMAP hBitmap);
	~Picture();

	cv::Mat GetCVImg();

	void SaveFile(const std::string& FileName);


protected:
	PBITMAPINFO CreateBitmapInfoStruct();

protected:
	HBITMAP m_hBitmap;
};

