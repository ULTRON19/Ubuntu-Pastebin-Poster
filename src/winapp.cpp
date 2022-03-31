#include "winApp.h"

WINAPP::WINAPP ():
	hwnd (nullptr),
	WndProc (nullptr),
	MainLoop (nullptr)
{
}

LRESULT CALLBACK WINAPP::GlobalWndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
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

LPCSTR WINAPP::Initialize (const WNDCLASSEXA& wcexa, const WNDSTYLEEXA& wsexa, std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> _WndProc, std::function <void (HWND)> _MainLoop)
{
	WndProc = _WndProc;
	MainLoop = _MainLoop;

	// Register the class
	if (!RegisterClassExA (&wcexa))
	{
		// 1410: Class already exists.
		if (GetLastError () != 1410)
		{
			WINREPORT ("RegisterClassExA");
			return nullptr;
		}		
	}

	// Create window
	hwnd = CreateWindowExA (wsexa.dwExStyle,
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

	if (hwnd == nullptr)
	{
		WINREPORT ("CreateWindowExA");
		return nullptr;
	}

	return wcexa.lpszClassName;
}

HWND WINAPP::GetHWND () const
{
	return hwnd;
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
				MainLoop (hwnd);

			Sleep (30);
		}
		
	hwnd = nullptr;
	return msg.wParam;
}
