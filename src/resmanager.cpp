#include "resmanager.h"

RESMANAGER::RESMANAGER ():
	hcArrow (nullptr), hcIbeam (nullptr), hcUpArrow (nullptr), hcWait (nullptr),
	hiUbuntu (nullptr),
	hInstance (nullptr)
{
}

RESMANAGER::~RESMANAGER ()
{
	UnInitialize ();
}

RESMANAGER* RESMANAGER::GetInstance ()
{
	static RESMANAGER instance;
	return &instance;
}

bool RESMANAGER::Initialize (HINSTANCE _hInstance)
{
	hInstance = _hInstance;
	
	bool res = true;
	
	hiUbuntu = LoadIconA (hInstance, MAKEINTRESOURCEA (ICON_UBUNTU));
	if (hiUbuntu == nullptr)
	{
		WINAPP::WinErrorReport (__FUNCTION__, "LoadIconA", false);
		res = false;
	}
	
	hcArrow = LoadCursorA (hInstance, MAKEINTRESOURCEA (CURSOR_ARROW));
	if (hcArrow == nullptr)
	{
		WINAPP::WinErrorReport (__FUNCTION__, "LoadCursorA", false);
		res = false;
	}
	
	hcIbeam = LoadCursorA (hInstance, MAKEINTRESOURCEA (CURSOR_IBEAM));
	if (hcIbeam == nullptr)
	{
		WINAPP::WinErrorReport (__FUNCTION__, "LoadCursorA", false);
		res = false;
	}
	
	hcUpArrow = LoadCursorA (hInstance, MAKEINTRESOURCEA (CURSOR_UPARROW));
	if (hcUpArrow == nullptr)
	{
		WINAPP::WinErrorReport (__FUNCTION__, "LoadCursorA", false);
		res = false;
	}
	
	hcWait = LoadCursorA (hInstance, MAKEINTRESOURCEA (CURSOR_WAIT));
	if (hcWait == nullptr)
	{
		WINAPP::WinErrorReport (__FUNCTION__, "LoadCursorA", false);
		res = false;
	}
	
	if (!rfUbuntu.Initialize (hInstance, L"Ubuntu", MAKEINTRESOURCEW (FONT_UBUNTU), 20, FW_DONTCARE, false, false, false))
		res = false;
		
	if (!rSetup.Initialize (hInstance, MAKEINTRESOURCEW (INI_SETUP), L"INI"))
		res = false;
		
	return res;
}

void RESMANAGER::UnInitialize ()
{
	SAFE_DELETE_OBJ (hiUbuntu);
	SAFE_DELETE_OBJ (hcArrow);
	SAFE_DELETE_OBJ (hcIbeam);
	SAFE_DELETE_OBJ (hcUpArrow);
	SAFE_DELETE_OBJ (hcWait);
	rfUbuntu.UnInitialize ();
	rSetup.UnInitialize ();
}

HICON RESMANAGER::GetIconHandle (DWORD dwIndex)
{
	switch (dwIndex)
	{
		case ICON_UBUNTU:		return hiUbuntu;
	}
	
	return nullptr;
}
	
HCURSOR RESMANAGER::GetCursorHandle (DWORD dwIndex)
{
	switch (dwIndex)
	{
		case CURSOR_ARROW:		return hcArrow;
		case CURSOR_IBEAM:		return hcIbeam;
		case CURSOR_UPARROW:	return hcUpArrow;
		case CURSOR_WAIT:		return hcWait;
	}
	
	return nullptr;
}

HFONT RESMANAGER::GetFontHandle (DWORD dwIndex)
{
	switch (dwIndex)
	{
		case FONT_UBUNTU:		return rfUbuntu.GetFontHandle ();
	}
	
	return nullptr;
}

WINRES* RESMANAGER::GetResourcePointer (DWORD dwIndex)
{
	switch (dwIndex)
	{
		case INI_SETUP:			return &rSetup;
	}
	
	return nullptr;
}

bool RESMANAGER::InitWindow (WINAPP* pWinApp, DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, 
	DWORD dwStyle, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, 
	std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> _WndProc, 
	std::function <void (HWND)> _MainLoop)
{
	// Initialize the window class
	WNDCLASSEXA wcex;
	memset (&wcex, 0, sizeof (wcex));
	
	wcex.cbSize			=	sizeof (WNDCLASSEX);
	wcex.style			=	CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	=	WINAPP::GlobalWndProc;
    wcex.cbClsExtra		=	0;
    wcex.cbWndExtra		=	sizeof (LONG_PTR);
    wcex.hInstance		=	hInstance;
    wcex.hIcon			=	hiUbuntu;
    wcex.hIconSm		=	hiUbuntu;
    wcex.hCursor		=	hcArrow;
    wcex.hbrBackground	=	CreateSolidBrush (CLIENT_BKCOLOR);
    wcex.lpszClassName	=	lpClassName;
    
    // Initialize the window style
    WNDSTYLEEXA wsex;
	memset (&wsex, 0, sizeof (wsex));
    
	wsex.lpClassName	=	lpClassName;
	wsex.lpWindowName	=	lpWindowName;

	wsex.dwExStyle		=	dwExStyle;
	wsex.dwStyle		=	dwStyle;

	RECT rc = {0, 0, nWidth, nHeight};
	AdjustWindowRectEx (&rc, wsex.dwStyle, false, wsex.dwExStyle);

	wsex.nWidth			=	rc.right - rc.left;
	wsex.nHeight		=	rc.bottom - rc.top;
	wsex.x				=	(Screen_FullWidth - wsex.nWidth) / 2;
	wsex.y				=	(Screen_FullHeight - wsex.nHeight) / 2;

	wsex.hWndParent		=	hWndParent;
	wsex.hMenu			=	hMenu;
	wsex.hInstance		=	hInstance;
	wsex.lpParam		=	pWinApp;
	
	using namespace std::placeholders;
	return pWinApp -> Initialize (wcex, wsex, _WndProc, _MainLoop);
}

bool RESMANAGER::InitControl (HWND& hControl, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, 
	int nWidth, int nHeight, HWND hWndParent, int hIndex, DWORD dwIndexFont, DWORD dwIndexCursor)
{
	HMENU hMenu;
	
	// HMENU is not really supposed to be a pointer that points anywhere. 
	// 32-bit compilation is different from 64-bit compilation
#if defined (_M_X64) || defined (__amd64__)
	hMenu = (HMENU) static_cast <long long> (hIndex);
#else
	hMenu = (HMENU) hIndex;
#endif

	hControl = CreateWindowA (lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, NULL);
	
	if (hControl == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "CreateWindowA", true), false;
	
	// Set font
	SendMessage (hControl, WM_SETFONT, (WPARAM) (dwIndexFont ? GetFontHandle (dwIndexFont) : 0), 0);
	
	if (dwIndexCursor)
	{
		HCURSOR hCursor = GetCursorHandle (dwIndexCursor);
	
		// Let the pointer change form when the focus is on the control
		// 32-bit compilation is different from 64-bit compilation
#if defined (_M_X64) || defined (__amd64__)
		SetClassLongPtr (hControl, GCLP_HCURSOR, (LONG_PTR) reinterpret_cast <long long> (hCursor));
#else
		SetClassLongPtr (hControl, GCL_HCURSOR, (LONG_PTR) hCursor);
#endif	
	}
	
	// If the control is a edit box
	// Adjust the area so the text is at a distance from the border
	if (strcmp (lpClassName, WC_EDITA) == 0)
	{
		RECT rect;
		GetWindowRect (hControl, &rect);
		OffsetRect (&rect, -rect.left + 4, -rect.top + 2);
		rect.right -= 8;
		SendMessage (hControl, EM_SETRECT, 0, (LPARAM) &rect);
	}
	
	return true;
}
