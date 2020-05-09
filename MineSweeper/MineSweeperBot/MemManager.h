#ifndef _WINAPLICATION_H
#define _WINAPLICATION_H



#include <Windows.h>
#include <TlHelp32.h>

#include <string>



class WinApp
{
public:
	WinApp(const std::string &procName);
	~WinApp();


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



