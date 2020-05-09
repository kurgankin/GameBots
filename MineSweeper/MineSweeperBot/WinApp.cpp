#include "WinApp.h"

#include "Picture.h"



HWND GetMainWindow(DWORD PId);

WinApp::WinApp(const std::string &procName)
{
	m_pName = procName;

	UpdatePID();
}



WinApp::~WinApp()
{

}



bool WinApp::isPlaying()
{
	return GetWindowName() == "Сапер";
}

bool WinApp::isEndedGame()
{
	return GetWindowName() == "Игра проиграна" || GetWindowName() == "Игра выиграна";
}

bool WinApp::isSettings()
{
	return GetWindowName() == "Параметры";
}

bool WinApp::isStartingNewGame()
{
	return GetWindowName() == "Новая игра";
}



bool WinApp::isClosed()
{
	if (m_hProc == nullptr && m_pID == 0)
	{
		return true;
	}

	if (GetHWND() == nullptr)
	{
		return true;
	}

	return false;
}


// For each child window, this callback will be called.
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	// If you need hwnd, you can use it.
	// If you need the Control ID of child window then,
	UINT CtrlID = GetDlgCtrlID(hwnd);
	// Use the Control ID.

	TCHAR Name[MAX_PATH];
	GetWindowText(hwnd, Name, GetWindowTextLength(hwnd) + 1);

	std::string str(Name);

	if (str.find("Новая игра") != std::string::npos)
	{
		SendMessage(hwnd, BM_CLICK, 0, 0);
	}

	return TRUE;
}

void WinApp::StartNewGame()
{
	EnumChildWindows(GetHWND(), EnumChildProc, 0);
}



std::string WinApp::GetWindowName()
{
	if (isClosed())
	{
		return "";
	}


	HWND hWnd = GetMainWindow(m_pID);

	TCHAR Name[MAX_PATH];
	GetWindowText(hWnd, Name, GetWindowTextLength(hWnd) + 1);

	return std::string(Name);
}



cv::Mat WinApp::GetScreenShot()
{
	return Picture(GetMainWindowScreenshot()).GetCVImg();
}



void WinApp::UpdatePID()
{
	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pInfo;
	pInfo.dwSize = sizeof(PROCESSENTRY32);

	if(Process32First(Snapshot, &pInfo))
	{
		while (Process32Next(Snapshot, &pInfo))
		{
			std::string str(pInfo.szExeFile);

			if (str == m_pName)
			{
				m_pID = pInfo.th32ProcessID;
				CloseHandle(Snapshot);
				return;
			}
		}
	}
	m_pID = 0;
	CloseHandle(Snapshot);
}



void WinApp::Open(DWORD accessRights)
{
	m_hProc = OpenProcess(accessRights, false, m_pID);
}



void WinApp::Close()
{
	CloseHandle(m_hProc);
}



DWORD WinApp::getPID()
{
	return m_pID;
}



HWND WinApp::GetHWND()
{
	Open();
	HWND hWnd = GetMainWindow(m_pID);
	Close();

	return hWnd;
}



HBITMAP WinApp::GetMainWindowScreenshot()
{
	HWND hWnd = GetMainWindow(m_pID);

	SetForegroundWindow(hWnd);
	BringWindowToTop(hWnd);

	RECT rc;
	GetClientRect(hWnd, &rc);

	POINT p = { 0 };
	MapWindowPoints(hWnd, GetDesktopWindow(), &p, 1);

	HDC hdcScreen = GetDC(NULL);
	HDC hdc = CreateCompatibleDC(hdcScreen);

	HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, rc.right - rc.left, rc.bottom - rc.top);

	SelectObject(hdc, hbmp);

	//Print to memory hdc
	BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcScreen, p.x, p.y, SRCCOPY);
	//PrintWindow(hWnd, hdc, PW_CLIENTONLY);

	ReleaseDC(hWnd, hdc);
	DeleteDC(hdcScreen);

	DeleteObject(hbmp);

	return hbmp;
}






typedef struct tagENUMINFO
{
	// In Parameters
	DWORD PId;

	// Out Parameters
	HWND  hWnd;
	HWND  hEmptyWnd;
	HWND  hInvisibleWnd;
	HWND  hEmptyInvisibleWnd;
} ENUMINFO, *PENUMINFO;



BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	DWORD       pid = 0;
	PENUMINFO   pInfo = (PENUMINFO)lParam;
	TCHAR       szTitle[_MAX_PATH+1];

	// sanity checks
	if (pInfo == NULL)
		// stop the enumeration if invalid parameter is given
		return(FALSE);

	// get the processid for this window
	if (!::GetWindowThreadProcessId(hWnd, &pid))
		// this should never occur :-)
		return(TRUE);

	// compare the process ID with the one given as search parameter
	if (pInfo->PId == pid)
	{
		// look for the visibility first
		if (::IsWindowVisible(hWnd))
		{
			// look for the title next
			if (::GetWindowText(hWnd, szTitle, _MAX_PATH) != 0)
			{
				pInfo->hWnd = hWnd;

				// we have found the right window
				return(FALSE);
			}
			else
				pInfo->hEmptyWnd = hWnd;
		}
		else
		{
			// look for the title next
			if (::GetWindowText(hWnd, szTitle, _MAX_PATH) != 0)
			{
				pInfo->hInvisibleWnd = hWnd;
			}
			else
				pInfo->hEmptyInvisibleWnd = hWnd;
		}
	}

	// continue the enumeration
	return(TRUE);
}






static HWND GetMainWindow(DWORD PId)
{
	ENUMINFO EnumInfo;

	// set the search parameters
	EnumInfo.PId = PId;

	// set the return parameters to default values
	EnumInfo.hWnd               = NULL;
	EnumInfo.hEmptyWnd          = NULL;
	EnumInfo.hInvisibleWnd      = NULL;
	EnumInfo.hEmptyInvisibleWnd = NULL;

	// do the search among the top level windows
	::EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)&EnumInfo);

	// return the one found if any
	if (EnumInfo.hWnd != NULL)
		return(EnumInfo.hWnd);
	else if (EnumInfo.hEmptyWnd != NULL)
		return(EnumInfo.hEmptyWnd);
	else if (EnumInfo.hInvisibleWnd != NULL)
		return(EnumInfo.hInvisibleWnd);
	else
		return(EnumInfo.hEmptyInvisibleWnd);
}