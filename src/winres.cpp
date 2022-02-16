#include "winres.h"

// WINRES

WINRES::WINRES ():
	hMem (nullptr),
	dwSize (0)
{
}

WINRES::~WINRES ()
{
	CoInitialize ();
}

bool WINRES::Initialize (HINSTANCE hInstance, LPCWSTR lpName, LPCWSTR lpType)
{
	HRSRC hRes = FindResourceW (hInstance, lpName, lpType);
	if (hRes == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "FindResourceW", false), false; 
	
	hMem = LoadResource (nullptr, hRes);
	
	if (hMem == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "Initializeource", false), false;
		
	dwSize = SizeofResource (nullptr, hRes);
	return true;
}

bool WINRES::ExtractToLocal (LPCSTR lpFileName)
{
	HANDLE hFile = CreateFileA (lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
		return WINAPP::WinErrorReport (__FUNCTION__, "CreateFileW", true), false;
	
	DWORD dwWrite = 0;
	bool res = WriteFile (hFile, hMem, dwSize, &dwWrite, nullptr);
	
	if (!res)
		WINAPP::WinErrorReport (__FUNCTION__, "WriteFile", true);
	
	CloseHandle (hFile);
	return res;
}

void WINRES::CoInitialize ()
{
	if (hMem)
		FreeResource (hMem);
		
	hMem = nullptr;
}

// WINFONT

WINFONT::WINFONT ():
	fontInstaller (nullptr),
	fontHandle (nullptr)
{
}

WINFONT::~WINFONT ()
{
	CoUninitialize ();
}

HFONT WINFONT::GetFontHandle ()
{
	return fontHandle;
}

bool WINFONT::Initialize (HINSTANCE hInstance, LPCWSTR _fontName, LPCWSTR _resId, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut)
{
	if (hInstance == nullptr)
		return false;
	
	if (_resId && !InstallFont (hInstance, _resId))
		return false;
	
	return CreateFontHandle (_fontName, _size, _bold, _italic, _underline, _strlikeOut);
}

void WINFONT::CoUninitialize ()
{
	UninstallFont ();
	DestroyFontHandle ();
}

bool WINFONT::InstallFont (HINSTANCE hInstance, LPCWSTR _resId)
{	
	if (!WINRES::Initialize (hInstance, _resId, MAKEINTRESOURCEW (8))) // MAKEINTRESOURCEW (8) RT_FONT
		return false;
	
	HANDLE pvData = LockResource (hMem);
	if (pvData == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "LockResource", false), false;
	
	DWORD nFontsInstalled = 0;
	fontInstaller = AddFontMemResourceEx (pvData, dwSize, NULL, &nFontsInstalled);
	if (fontInstaller == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "AddFontMemResourceEx", false), false;
	
	return true;
}

bool WINFONT::CreateFontHandle (LPCWSTR _fontName, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut)
{
	fontHandle = CreateFontW (_size, 0, 0, 0, _bold, _italic, _underline, _strlikeOut, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, _fontName);
	return fontHandle ? true : false;
}

void WINFONT::UninstallFont ()
{
	if (fontInstaller) 
	{
		RemoveFontMemResourceEx (fontInstaller);
		fontInstaller = nullptr;
	}
}

void WINFONT::DestroyFontHandle ()
{
	SAFE_DELETE_OBJ (fontHandle);
}
