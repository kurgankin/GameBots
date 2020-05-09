#ifndef _WINAPLICATION_H
#define _WINAPLICATION_H



#include <Windows.h>
#include <TlHelp32.h>

#include <string>
#include <opencv2/opencv.hpp>


class WinApp
{
public:
	WinApp(const std::string &procName);
	~WinApp();

	bool isPlaying();
	bool isEndedGame();
	bool isSettings();
	bool isStartingNewGame();

	bool isClosed();


	void StartNewGame();

	std::string GetWindowName();


	cv::Mat GetScreenShot();


	DWORD getPID();
	HWND GetHWND();

	HBITMAP GetMainWindowScreenshot();


private:
	void Open(DWORD accessRights = PROCESS_ALL_ACCESS);
	void Close();
	void UpdatePID();


private:
	std::string m_pName;

	HANDLE m_hProc;
	DWORD m_pID;
};

#endif



