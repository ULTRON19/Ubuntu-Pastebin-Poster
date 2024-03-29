#ifndef __WINAPP__
#define __WINAPP__

#include <functional>
#include "logger.h"

#define Screen_FullWidth	(GetSystemMetrics (SM_CXSCREEN))
#define Screen_FullHeight	(GetSystemMetrics (SM_CYSCREEN))

// This structure is used for 'CreateWindowEx' function, its memeber 
// map to the parameters of the 'CreateWindowEx' function
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
	// Global message queue
	static LRESULT CALLBACK GlobalWndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	// Initialize window
	LPCSTR Initialize (const WNDCLASSEXA& wcexa, const WNDSTYLEEXA& wsexa, std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> _WndProc, std::function <void (HWND)> _MainLoop = nullptr);

	HWND GetHWND () const;
	WPARAM EnterMsgLoop ();
	
	WINAPP ();
private:
	// Window Handle
	HWND hwnd;

	// Individual message queue
	std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> WndProc;

	// Main Loop
	std::function <void (HWND)> MainLoop;
};

#endif
