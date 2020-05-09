#include "Game.h"

#include <sstream>
#include <iomanip>
#include <random>
#include <limits>
#include <chrono>


#include "PicRecognition.h"
#include <Windows.h>
#include "WinApp.h"



Game::Game(cv::Mat Screenshot):
	m_bGameEnd(false),
	m_isParsed(false),
	m_SrcImg(Screenshot),
	m_SrcSrc(Screenshot)
{
	for (auto& row : m_iFields)
	{
		for (auto& Field : row)
		{
			Field = -1;
		}
	}


	cv::Mat temp = m_SrcImg;

	cv::Size sz(16*18, 30*18);
	cv::resize(m_SrcImg, temp, sz);

	PicRecognition::OpenANN();


	if (m_SrcImg.rows == 192 && m_SrcImg.cols == 401)
	{
		m_bGameEnd = true;
		return;
	}

	m_isParsed = ParseImg(temp);

	if (m_isParsed)
	{
		m_WorkingArea = FindWorkinArea(m_SrcSrc);
	}
}



void Game::SaveExamples()
{
	auto WorkingArea = FindWorkinArea(m_SrcSrc);

	if (WorkingArea.height == 0 && WorkingArea.width == 0)
	{
		return;
	}

	std::default_random_engine generator;
	unsigned int Seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	generator.seed(Seed);
//	std::uniform_int_distribution<int> distribution(1, std::numeric_limits<int>::max());
	std::uniform_int_distribution<int> distribution(1, 2000000000);

	auto dx = WorkingArea.width / 30.0;
	auto dy = WorkingArea.height / 16.0;

	for (unsigned int i = 0; i < 16; i++)
	{
		for (unsigned int j = 0; j < 30; j++)
		{
			auto X = static_cast<int>(WorkingArea.x + j * dx + 1);
			auto Y = static_cast<int>(WorkingArea.y + i * dy + 1);
			auto dX = static_cast<int>(dx);
			auto dY = static_cast<int>(dy);

			cv::Rect ROI(X, Y, dX, dY);

			cv::Mat t = m_SrcSrc(ROI);

			std::stringstream filename;

			int dice_roll = distribution(generator);

			filename << "pics//" << dice_roll << ".bmp";

			cv::imwrite(filename.str(), t);

		}
	}
}








bool Game::ParseImg(cv::Mat Image)
{
	auto WorkingArea = FindWorkinArea(Image);

	if (WorkingArea.height == 0 && WorkingArea.width == 0)
	{
		return false;
	}

	auto dx = WorkingArea.width / 30.0;
	auto dy = WorkingArea.height / 16.0;

	for (unsigned int i = 0; i < 16; i++)
	{
		for (unsigned int j = 0; j < 30; j++)
		{
			auto X = static_cast<int>(WorkingArea.x + j * dx + 1);
			auto Y = static_cast<int>(WorkingArea.y + i * dy + 1);
			auto dX = static_cast<int>(dx);
			auto dY = static_cast<int>(dy);

			cv::Rect ROI(X, Y, dX, dY);

			cv::Mat t = Image(ROI);

			cv::resize(t, t, cv::Size(18, 18));

			cv::cvtColor(t, t, cv::COLOR_BGRA2BGR);

			m_iFields[i][j] = FieldIdentification(t);
		}
	}

	return true;
}





cv::Rect Game::FindWorkinArea(cv::Mat Image)
{
	cv::Mat src_gray;

	cv::cvtColor(Image, src_gray, cv::COLOR_BGR2GRAY);

	cv::Mat AdTPic;

	cv::adaptiveThreshold(src_gray, AdTPic, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 5, 10);



	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;


	cv::findContours(AdTPic, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	double MaxArea = 0;
	std::vector<cv::Point>* cntr = nullptr;
	for (auto& contour : contours)
	{
		auto Area = contourArea(contour, false);

		if (MaxArea < std::abs(Area))
		{
			MaxArea = Area;
			cntr = &contour;
		}
	}

	if (MaxArea < 1)
	{
		return cv::Rect(0, 0, 0, 0);
	}

	return cv::boundingRect(*cntr);
}



int Game::FieldIdentification(cv::Mat Image)
{
	cv::Mat Data;

	cv::Size sz(18,18);

	cv::resize(Image, Data, sz);



	Data.convertTo(Data, CV_32FC4, 1.0 / 255.0);

	Data = Data.reshape(1, 1);

	auto ANN = PicRecognition::GetANN();

	cv::Mat result;

	auto Res = ANN->predict(Data, result);

	int k = -1;
	float d = -100;

	for (int i = 0; i < 11; i++)
	{
		if (result.at<float>(0, i) > d)
		{
			d = result.at<float>(0, i);
			k = i;
		}
	}

	return k;

}

bool Game::isBeginning()
{
	for (auto& Fieldi : m_iFields)
	{
		for (auto& Field : Fieldi)
		{
			if (Field != 10)
			{
				return false;
			}
		}
	}

	return true;
}







void LeftClick()
{
	INPUT    Input = { 0 };
	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}


void RightClick()
{
	INPUT    Input = { 0 };
	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}


void MouseMove(int x, int y)
{
	double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
	double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
	double fx = x * (65535.0f / fScreenWidth);
	double fy = y * (65535.0f / fScreenHeight);
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	Input.mi.dx = (LONG)fx;
	Input.mi.dy = (LONG)fy;
	::SendInput(1, &Input, sizeof(INPUT));
}



void Game::AddFreeField(int row, int col)
{
	WinApp Miner("MineSweeper.exe");

	auto hWnd = Miner.GetHWND();

	// hWnd - дескриптор окна.
	SetForegroundWindow(hWnd);

	RECT wrect, winRect;
	GetClientRect(hWnd, &wrect);
	GetWindowRect(hWnd, &winRect);
	auto s = m_WorkingArea;

	POINT p = { 0 };
	MapWindowPoints(hWnd, GetDesktopWindow(), &p, 1);



	int d1 = s.width / 30;
	int d2 = s.height / 16;

	int x = p.x + s.x + col * s.width / 30 + d1 / 2;
	int y = p.y + s.y + (15 - row) * s.height / 16 + d2 / 2;

	MouseMove(x, y);
	Sleep(10);

	LeftClick();
}




void Game::AddBomb(int row, int col)
{
	WinApp Miner("MineSweeper.exe");

	auto hWnd = Miner.GetHWND();

	// hWnd - дескриптор окна.
	SetForegroundWindow(hWnd);

	RECT wrect, winRect;
	GetClientRect(hWnd, &wrect);
	GetWindowRect(hWnd, &winRect);
	auto s = m_WorkingArea;

	POINT p = { 0 };
	MapWindowPoints(hWnd, GetDesktopWindow(), &p, 1);



	int d1 = s.width / 30;
	int d2 = s.height / 16;

	int x = p.x + s.x + col * s.width / 30 + d1 / 2;
	int y = p.y + s.y + (15 - row) * s.height / 16 + d2 / 2;

	MouseMove(x, y);
	Sleep(10);

	RightClick();
}