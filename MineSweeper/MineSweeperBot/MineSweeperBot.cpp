// MineSweeperBot.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "Model.h"
#include "View.h"
#include "Controller.h"

#include <iostream>
#include "WinApp.h"
#include "Picture.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "Game.h"
#include "PicRecognition.h"



std::vector<cv::Mat> Sample0(11);


void PrintRes(int ind)
{
	Sample0[0] = cv::imread("pics//0.bmp", cv::IMREAD_UNCHANGED);
	Sample0[1] = cv::imread("pics//1.bmp", cv::IMREAD_UNCHANGED);
	Sample0[2] = cv::imread("pics//2.bmp", cv::IMREAD_UNCHANGED);
	Sample0[3] = cv::imread("pics//3.bmp", cv::IMREAD_UNCHANGED);
	Sample0[4] = cv::imread("pics//4.bmp", cv::IMREAD_UNCHANGED);
	Sample0[5] = cv::imread("pics//5.bmp", cv::IMREAD_UNCHANGED);
	Sample0[6] = cv::imread("pics//6.bmp", cv::IMREAD_UNCHANGED);
	Sample0[9] = cv::imread("pics//-1.bmp", cv::IMREAD_UNCHANGED);
	Sample0[10] = cv::imread("pics//bomb.bmp", cv::IMREAD_UNCHANGED);

	auto& ref = Sample0[ind];




	ref.convertTo(ref, CV_32FC4, 1.0 / 255.0);

	ref = ref.reshape(1, 1);

	cv::Mat Res;

	auto ANN = PicRecognition::GetANN();

	auto a = ANN->predict(ref, Res);
	for (int i = 0; i < Res.size().height; i++)
	{
		std::cout << "[";
		for (int j = 0; j < Res.size().width; j++)
		{
			std::cout << std::fixed << std::setw(2) << std::setprecision(3) << Res.at<float>(i, j);
			if (j != Res.size().width - 1)
				std::cout << ", ";
			else
				std::cout << "]" << std::endl;
		}
	}
}



int main()
{


	//std::cout << "PicRecognition::ReadSamples()" << std::endl << std::endl;
	//PicRecognition::ReadSamples();

	//std::cout << "PicRecognition::CreateANN()" << std::endl << std::endl;

	//PicRecognition::CreateANN();
	//std::cout << "PicRecognition::SaveANN()" << std::endl << std::endl;
	//PicRecognition::SaveANN();

	//system("pause");

	//return 0;


	//PicRecognition::OpenANN();

	Model		model	= Model();
	View		view	= View(&model);
	Controller	controller	= Controller(&model);

	controller.Run();

	return 0;






//	while (1)
//	{
//		WinApp Miner("MineSweeper.exe");
//
//		if (!Miner.isActive())
//		{
//			continue;
//		}
//
//		auto img = Miner.GetScreenShot();
//
//		cv::imshow("pic.GetCVImg()", img);
//		cv::waitKey();
//		Game game(img);
//
////		game.SaveExamples();
//
//		game.Print();
//
//		game.doTurn();
//
//
//	}
//
//	return 0;
}




// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
