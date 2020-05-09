#include "PicRecognition.h"

#include <vector>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <tchar.h>




cv::Size TmplSize(18, 18);

static cv::Mat InputTrainingData;
static cv::Mat OutputTrainingData;

static cv::Ptr<cv::ml::ANN_MLP> ann;


void ReadSample00(std::string Dir, int num)
{
	std::vector<std::string> Files;

	WIN32_FIND_DATA f;

	std::stringstream sstr;

	sstr << "./pics/" << Dir << "/*";

	std::cout << sstr.str() << std::endl;

	HANDLE h = FindFirstFile(sstr.str().c_str(), &f);

	if (h == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		std::string str(f.cFileName);
		if (str.find(".bmp") != std::string::npos)
		{
			std::stringstream ss;

			ss << "pics/"<< Dir <<  "/" << str;

			auto Pic = cv::imread(ss.str(), cv::IMREAD_COLOR);

			cv::resize(Pic, Pic, TmplSize);

			Pic.convertTo(Pic, CV_32FC4, 1.0 / 255.0);

			InputTrainingData.push_back(Pic.reshape(1, 1));

			float output[1][11] = { { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 } };
			output[0][num] = 1.0;

			OutputTrainingData.push_back(cv::Mat(1, 11, CV_32F, output));
		}
	} while (FindNextFile(h, &f));
}



void PicRecognition::ReadSamples(void)
{
	ReadSample00("00", 0);
	ReadSample00("01", 1);
	ReadSample00("02", 2);
	ReadSample00("03", 3);
	ReadSample00("04", 4);
	ReadSample00("05", 5);
	ReadSample00("06", 6);
	ReadSample00("07", 7);
	ReadSample00("08", 8);
	ReadSample00("bomb", 9);
	ReadSample00("unknown", 10);

	std::cout << "count = " << OutputTrainingData.size().height << std::endl;
}

void PicRecognition::CreateANN(void)
{
	ann = cv::ml::ANN_MLP::create();

	cv::Mat_<int> layers(1, 5);

	layers(0, 0) = InputTrainingData.cols;		// input
	layers(0, 1) = 18*18;						// hidden
	layers(0, 2) = 100;							// hidden
	layers(0, 3) = 50;							// hidden
	layers(0, 4) = OutputTrainingData.cols;		// output

	ann->setLayerSizes(layers);

	ann->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);

	ann->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 900, 0.000001));

	ann->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.00001);

	cv::Ptr<cv::ml::TrainData> trainData = 
		cv::ml::TrainData::create(InputTrainingData, cv::ml::SampleTypes::ROW_SAMPLE, OutputTrainingData);
	ann->train(trainData);

	//ann->train(trainData
	//	, cv::ml::ANN_MLP::TrainFlags::UPDATE_WEIGHTS
	//	| cv::ml::ANN_MLP::TrainFlags::NO_INPUT_SCALE
	//	| cv::ml::ANN_MLP::TrainFlags::NO_OUTPUT_SCALE
	//);
}



void PicRecognition::SaveANN()
{
	if (ann.empty())
	{
		return;
	}

	cv::FileStorage fs;
	fs.open("mlp.xml", cv::FileStorage::WRITE);
	ann->write(fs);
	fs.release();
}



void PicRecognition::OpenANN()
{
	if (!ann.empty())
	{
		return;
	}

	cv::FileStorage fs;
	ann = cv::ml::ANN_MLP::create();
	fs.open("mlp.xml", cv::FileStorage::READ);
	ann->read(fs.root());
	fs.release();
}



cv::Ptr<cv::ml::ANN_MLP> PicRecognition::GetANN()
{
	return ann;
}