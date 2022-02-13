#include "winres.h"

// RESOTHER

RESOTHER::RESOTHER ():
	hMem (nullptr),
	dwSize (0)
{
}

RESOTHER::~RESOTHER ()
{
	FreeRes ();
}

HGLOBAL RESOTHER::GetHGLOBAL ()
{
	return hMem;
}

DWORD RESOTHER::GetSize ()
{
	return dwSize;
}

bool RESOTHER::LoadRes (HINSTANCE hInstance, LPCWSTR lpName, LPCWSTR lpType)
{
	HRSRC hRes = FindResourceW (hInstance, lpName, lpType);
	if (hRes == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "FindResourceW", false), false; 
	
	hMem = LoadResource (nullptr, hRes);
	
	if (hMem == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "LoadResource", false), false;
		
	dwSize = SizeofResource (nullptr, hRes);
	return true;
}

bool RESOTHER::ExtractToLocal (LPCWSTR lpFileName)
{
	HANDLE hFile = CreateFileW (lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
		return WINAPP::WinErrorReport (__FUNCTION__, "CreateFileW", true), false;
	
	DWORD dwWrite = 0;
	bool res = WriteFile (hFile, hMem, dwSize, &dwWrite, nullptr);
	
	if (!res)
		WINAPP::WinErrorReport (__FUNCTION__, "WriteFile", true);
	
	CloseHandle (hFile);
	return res;
}

void RESOTHER::FreeRes ()
{
	if (hMem)
		FreeResource (hMem);
		
	hMem = nullptr;
}

// RESFONT

RESFONT::RESFONT ():
	fontInstaller (nullptr),
	fontHandle (nullptr)
{
}

RESFONT::~RESFONT ()
{
	CoUninitializeFont ();
}

HFONT RESFONT::GetHFONT ()
{
	return fontHandle;
}

bool RESFONT::InitializeFont (HINSTANCE hInstance, LPCWSTR _fontName, LPCWSTR _resId, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut)
{
	if (hInstance == nullptr)
		return false;
	
	if (_resId && !InstallFont (hInstance, _resId))
		return false;
	
	return CreateHFONT (_fontName, _size, _bold, _italic, _underline, _strlikeOut);
}

void RESFONT::CoUninitializeFont ()
{
	UninstallFont ();
	DestroyHFONT ();
}

bool RESFONT::InstallFont (HINSTANCE hInstance, LPCWSTR _resId)
{	
	RESOTHER fontRes;
	if (!fontRes.LoadRes (hInstance, _resId, MAKEINTRESOURCEW (8))) // MAKEINTRESOURCEW (8) RT_FONT
		return false;
	
	HANDLE pvData = LockResource (fontRes.GetHGLOBAL ());
	if (pvData == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "LockResource", false), false;
	
	DWORD nFontsInstalled = 0;
	fontInstaller = AddFontMemResourceEx (pvData, fontRes.GetSize (), NULL, &nFontsInstalled);
	if (fontInstaller == nullptr)
		return WINAPP::WinErrorReport (__FUNCTION__, "AddFontMemResourceEx", false), false;
	
	return true;
}

bool RESFONT::CreateHFONT (LPCWSTR _fontName, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut)
{
	fontHandle = CreateFontW (_size, 0, 0, 0, _bold, _italic, _underline, _strlikeOut, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, _fontName);
	return fontHandle ? true : false;
}

void RESFONT::UninstallFont ()
{
	if (fontInstaller) 
	{
		RemoveFontMemResourceEx (fontInstaller);
		fontInstaller = nullptr;
	}
}

void RESFONT::DestroyHFONT ()
{
	SAFE_DELETE_OBJ (fontHandle);
}
