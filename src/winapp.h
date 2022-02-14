#ifndef __WINAPP__
#define __WINAPP__

#include <functional>
#include "erh.h"

#define Screen_FullWidth	GetSystemMetrics (SM_CXSCREEN)
#define Screen_FullHeight	GetSystemMetrics (SM_CYSCREEN)

// This structure is used for 'CreateWindowA' function, its memeber 
// correspond to the parameters of the 'CreateWindow' function
struct WNDSTYLEEXA
{
	DWORD		dwExStyle;
	LPCSTR		lpClassName;
	LPCSTR		lpWindowName;
	DWORD		dwStyle;
	int			x;
	int			y;
	int			nWidth;
	int			nHeight;
	HWND		hWndParent;
	HMENU		hMenu;
	HINSTANCE	hInstance;
	LPVOID		lpParam;
};

class WINAPP
{
public:
	// Autocomplete GetLastError and submit to ERH
	static void WinErrorReport (std::string mainFunc, const char* func, bool isAlert = false);
	
	// Global message queue
	static LRESULT CALLBACK G_WndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	// Initialize window
	LPCSTR InitializeWindow (const WNDCLASSEXA& wcexa, const WNDSTYLEEXA& wsexa, std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> _WndProc, std::function <void (HWND)> _MainLoop = nullptr);

	HWND GetHWND ();
	WPARAM EnterMsgLoop ();
	
	WINAPP ();
private:
	// Window Handle
	HWND m_hwnd;

	// Individual message queue
	std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> WndProc;

	// Main Loop
	std::function <void (HWND)> MainLoop;
};

#endif
