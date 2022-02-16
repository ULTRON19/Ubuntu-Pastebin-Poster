#ifndef __FRONT__
#define __FRONT__

// default setting
#define MAX_CHARACTER			30
#define MAX_PATHLEN				32767

#define CLIENT_FULLWIDTH		535
#define CLIENT_FULLHEIGHT		200
#define CLIENT_BKCOLOR			RGB (247, 247, 247)

#define SETTINGS_FULLWIDTH		390
#define SETTINGS_FULLHEIGHT		130

// resource
#define ICON_UBUNTU				0x11

#define CURSOR_ARROW			0x21
#define CURSOR_IBEAM			0x22
#define CURSOR_UPARROW			0x23
#define CURSOR_WAIT				0x24

#define FONT_UBUNTU				0x31

#define INI_CONFIGURATION		0x41

// control index
#define INDEX_STCPOSTER			1001
#define INDEX_STCSYNTAX			1002
#define INDEX_STCEXPIRATION		1003
#define INDEX_STCFILEPATH		1004
#define INDEX_STCURL			1005

#define INDEX_EDTPOSTER			1006
#define INDEX_EDTFILEPATH		1007
#define INDEX_EDTURL			1008

#define INDEX_CBXSYNTAX			1009
#define INDEX_CBXEXPIRATION		1010

#define INDEX_BTNPOST			1011
#define INDEX_BTNRIGHTMENU		1012

// default control style
#define DEFAULT_SSTATIC			(WS_CHILD | WS_VISIBLE | SS_LEFT)
#define DEFAULT_SEDIT			(WS_CHILD | WS_VISIBLE | WS_BORDER | BS_TEXT | ES_MULTILINE | ES_AUTOHSCROLL)
#define DEFAULT_SCOMBOBOX		(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | CBS_DISABLENOSCROLL | CBS_DROPDOWNLIST)
#define DEFAULT_SBUTTON			(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON)

#include <windows.h>
#include <commctrl.h>
#include <unordered_map>
#include "winres.h"

class RESMANAGER
{
public:
	// Only one instance of FRONT can exist at one time
	static RESMANAGER* GetInstance ();
	
	// Initialize
	bool Initialize (HINSTANCE _hInstance);
	void CoInitialize ();
	
	// Get resource
	HICON GetIconHandle (DWORD dwIndex);
	HCURSOR GetCursorHandle (DWORD dwIndex);
	HFONT GetFontHandle (DWORD dwIndex);
	WINRES* GetResourcePointer (DWORD dwIndex);
	
	// Initialize window
	bool InitWindow (WINAPP* pWinApp, DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, 
		DWORD dwStyle, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, 
		std::function <LRESULT (HWND, UINT, WPARAM, LPARAM)> _WndProc, 
		std::function <void (HWND)> _MainLoop = nullptr);
	
	// Initialize control
	bool InitControl (HWND& hControl, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
		int nWidth, int nHeight, HWND hWndParent, int hIndex, DWORD dwIndexFont, DWORD dwIndexCursor);

	~RESMANAGER ();
	
private:
	RESMANAGER ();
	
	// Font
	WINFONT rfUbuntu;
	// Cursor
	HCURSOR hcArrow, hcIbeam, hcUpArrow, hcWait;
	// ICON
	HICON hiUbuntu;
	// Ini
	WINRES rConfiguration;
	
	// Instance handle
	HINSTANCE hInstance;
};

#endif
