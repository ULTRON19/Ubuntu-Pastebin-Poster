#include "winApp.h"

WINAPP::WINAPP ():
	m_hwnd (nullptr),
	WndProc (nullptr),
	MainLoop (nullptr)
{
}

void WINAPP::WinErrorReport (std::string mainFunc, const char* func, bool isAlert)
{
	ERH ErrorReport;
	char msgbuf [256] = {0};
	int err = GetLastError ();

	msgbuf [0] = '\0';

	// Convert error codes to messages
	FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err,
					MAKELANGID (LANG_ENGLISH, SUBLANG_DEFAULT), msgbuf, sizeof (msgbuf), NULL);

	// Improve error reporting
	std::string errMsg (msgbuf);
	errMsg += "Win Error Code: " + std::to_string (err);

	(mainFunc += " - ") += func;
	ErrorReport (mainFunc, errMsg, isAlert);
}

LRESULT CALLBACK WINAPP::G_WndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WINAPP* pThis = nullptr;
	LRESULT lRet = 0;

	if (Message == WM_NCCREATE)
	{
		auto pcs = reinterpret_cast <LPCREATESTRUCT> (lParam);
		pThis = reinterpret_cast <WINAPP*> (pcs -> lpCreateParams);

		SetWindowLongPtr (hwnd, GWLP_USERDATA, reinterpret_cast <LONG_PTR> (pThis));
		lRet = DefWindowProc (hwnd, Message, wParam, lParam);
	}

	else
	{
		pThis = reinterpret_cast <WINAPP*> (GetWindowLongPtr (hwnd, GWLP_USERDATA));

		if (pThis)
			lRet = pThis -> WndProc (hwnd, Message, wParam, lParam);

		else
			lRet = DefWindowProc (hwnd, Message, wParam, lParam);
	}

	return lRet;
}

LPCSTR WINAPP::InitializeWindow (const WNDCLASSEXA& wcexa, const WNDSTYLEEXA& wsexa, std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> _WndProc, std::function <void (HWND)> _MainLoop)
{
	WndProc = _WndProc;
	MainLoop = _MainLoop;

	// Register the class
	if (!RegisterClassExA (&wcexa))
	{
		// 1410: Class already exists.
		if (GetLastError () != 1410)
		{
			WinErrorReport (__FUNCTION__, "RegisterClassExA", true);
			return nullptr;
		}		
	}

	// Create window
	m_hwnd = CreateWindowExA (wsexa.dwExStyle,
							  wsexa.lpClassName,
							  wsexa.lpWindowName,
							  wsexa.dwStyle,
							  wsexa.x,
							  wsexa.y,
							  wsexa.nWidth,
							  wsexa.nHeight,
							  wsexa.hWndParent,
							  wsexa.hMenu,
							  wsexa.hInstance,
							  wsexa.lpParam);

	if (m_hwnd == nullptr)
	{
		WinErrorReport (__FUNCTION__, "CreateWindowExA", true);
		return nullptr;
	}

	return wcexa.lpszClassName;
}

HWND WINAPP::GetHWND ()
{
	return this -> m_hwnd;
}

WPARAM WINAPP::EnterMsgLoop ()
{
	MSG msg;
	ZeroMemory (&msg, sizeof (MSG));

	while (msg.message != WM_QUIT)
		if (PeekMessage (&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
		else
		{
			if (MainLoop)
				MainLoop (m_hwnd);

			Sleep (30);
		}
		
	m_hwnd = nullptr;
	return msg.wParam;
}
