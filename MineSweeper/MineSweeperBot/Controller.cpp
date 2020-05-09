#include "Controller.h"
#include "WinApp.h"
#include "PicRecognition.h"


void LeftClick	();
void RightClick	();
void MouseMove	(int x, int y);




Controller::Controller(Model* model):
	m_Model(model)
{
	PicRecognition::OpenANN();
}


void Controller::Run()
{
	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			std::cout << "ESCAPE-PRESSED" << std::endl;
			break;
		}

		if (isWinningGame())
		{
			WinApp("MineSweeper.exe").StartNewGame();
			continue;
		}

		if (isPlaying())
		{
			auto img = MakeScreenShot();

			auto Fields = Recognise(img);

			if (!Fields)
			{
				break;
			}

			m_WorkingArea = FindWorkinArea(img);

			m_Model->UpdateFields(Fields);

			auto Bombs = m_Model->FindBombs();

			PlaceBombs(Bombs);

			auto FreeFields = m_Model->FindFreeFields();

			PlaceFreeFields(FreeFields);


			if (FreeFields->empty() && Bombs->empty())
			{
				auto l = m_Model->FindAnything();

				PlaceFreeFields(l);
				std::cerr << "";

				Sleep(10);
			}
		}
	}
}



cv::Mat Controller::MakeScreenShot()
{
	return WinApp("MineSweeper.exe").GetScreenShot();
}


PtrFields Controller::Recognise(cv::Mat img)
{
	cv::Mat temp;

	cv::resize(img, temp, cv::Size(RowsCount * FieldSize, ColsCount * FieldSize));

	return ParseImg(temp);
}



bool Controller::isPlaying()
{
	return WinApp("MineSweeper.exe").isPlaying();
}

bool Controller::isWinningGame()
{
	return WinApp("MineSweeper.exe").isEndedGame();
}



PtrFields Controller::ParseImg(cv::Mat Image)
{
	PtrFields Fields = std::make_shared<std::array<std::array<int, COLS_COUNT>, ROWS_COUNT>>();

	auto WorkingArea = FindWorkinArea(Image);

	if (WorkingArea.height == 0 && WorkingArea.width == 0)
	{
		return false;
	}

	auto dx = 1.0 * WorkingArea.width / ColsCount;
	auto dy = 1.0 * WorkingArea.height / RowsCount;

	for (std::remove_const<decltype(RowsCount)>::type i{ 0 }; i < RowsCount; i++)
	{
		for (std::remove_const<decltype(RowsCount)>::type j{ 0 }; j < ColsCount; j++)
		{
			auto X = static_cast<int>(WorkingArea.x + j * dx + 1);
			auto Y = static_cast<int>(WorkingArea.y + i * dy + 1);
			auto dX = static_cast<int>(dx);
			auto dY = static_cast<int>(dy);

			cv::Rect ROI(X, Y, dX, dY);

			cv::Mat Field = Image(ROI);

			cv::resize(Field, Field, cv::Size(FieldSize, FieldSize));

			cv::cvtColor(Field, Field, cv::COLOR_BGRA2BGR);

			(*Fields)[i][j] = FieldIdentification(Field);
		}
	}

	return Fields;
}




cv::Rect Controller::FindWorkinArea(cv::Mat Image)
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



int Controller::FieldIdentification(cv::Mat Image)
{
	cv::Mat Data;

	cv::Size sz(18, 18);

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

void Controller::PlaceBombs(std::shared_ptr<std::list<FieldCoords>> BombsList)
{
	if (!BombsList || BombsList->empty())
	{
		return;
	}

	for (auto &BombCoords : *BombsList)
	{
		PlaceBomb(BombCoords);
	}
}

void Controller::PlaceFreeFields(std::shared_ptr<std::list<FieldCoords>> FreeFieldsList)
{
	if (!FreeFieldsList || FreeFieldsList->empty())
	{
		return;
	}

	for (auto& FieldCoords : *FreeFieldsList)
	{
		PlaceFreeField(FieldCoords);
	}
}



void Controller::PlaceBomb(FieldCoords Bomb)
{
	if (!isPlaying())
	{
		return;
	}

	auto hWnd = WinApp("MineSweeper.exe").GetHWND();

	// hWnd - дескриптор окна.
	SetForegroundWindow(hWnd);

	RECT wrect, winRect;
	GetClientRect(hWnd, &wrect);
	GetWindowRect(hWnd, &winRect);

	POINT p = { 0 };
	MapWindowPoints(hWnd, GetDesktopWindow(), &p, 1);



	int d1 = m_WorkingArea.width / 30;
	int d2 = m_WorkingArea.height / 16;

	int x = p.x + m_WorkingArea.x + Bomb.second * m_WorkingArea.width / 30 + d1 / 2;
	int y = p.y + m_WorkingArea.y + (15 - Bomb.first) * m_WorkingArea.height / 16 + d2 / 2;

	MouseMove(x, y);

	POINT p1;
	if (GetCursorPos(&p1))
	{
		long int cnt = 2000000;
		while (cnt--)
		{
			if (p1.x == x && p1.y == y)
			{
				break;
			}
		}

	}

	RightClick();


	Sleep(20);
}

void Controller::PlaceFreeField(FieldCoords Field)
{
	if (!isPlaying())
	{
		return;
	}

	auto hWnd = WinApp("MineSweeper.exe").GetHWND();

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

	int x = p.x + s.x + Field.second * s.width / 30 + d1 / 2;
	int y = p.y + s.y + (15 - Field.first) * s.height / 16 + d2 / 2;


	MouseMove(x, y);

	POINT p1;
	if (GetCursorPos(&p1))
	{
		long int cnt = 2000000;
		while (cnt--)
		{
			if (p1.x == x && p1.y == y)
			{
				break;
			}
		}

	}



	LeftClick();


}



void LeftClick()
{
	auto hWnd = WinApp("MineSweeper.exe").GetHWND();
	SetForegroundWindow(hWnd);
	BringWindowToTop(hWnd);

	INPUT    Input = { 0 };
	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	::SendInput(1, &Input, sizeof(INPUT));

	Sleep(50);
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}


void RightClick()
{
	auto hWnd = WinApp("MineSweeper.exe").GetHWND();
	SetForegroundWindow(hWnd);
	BringWindowToTop(hWnd);


	INPUT    Input = { 0 };
	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	Sleep(50);
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