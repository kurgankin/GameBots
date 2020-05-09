#pragma once

#include <opencv2/opencv.hpp>


namespace PicRecognition
{
	void ReadSamples(void);

	void CreateANN(void);

	void SaveANN();

	void OpenANN();

	cv::Ptr<cv::ml::ANN_MLP> GetANN();
};

