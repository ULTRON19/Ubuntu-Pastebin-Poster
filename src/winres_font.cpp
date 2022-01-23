#include "winres.h"

RES_FONT::RES_FONT ():
	fontInstaller (nullptr),
	fontHandle (nullptr)
{
}

RES_FONT::~RES_FONT ()
{
	CoUninitializeFont ();
}

HFONT RES_FONT::GetHFONT ()
{
	return fontHandle;
}

bool RES_FONT::InitializeFont (HINSTANCE hInstance, LPCWSTR _fontName, LPCWSTR _resId, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut)
{
	if (hInstance == nullptr)
		return false;
	
	if (_resId && !InstallFont (hInstance, _resId))
		return false;
	
	return CreateHFONT (_fontName, _size, _bold, _italic, _underline, _strlikeOut);
}

void RES_FONT::CoUninitializeFont ()
{
	UninstallFont ();
	DestroyHFONT ();
}

bool RES_FONT::InstallFont (HINSTANCE hInstance, LPCWSTR _resId)
{	
	RES_OTHER fontRes;
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

bool RES_FONT::CreateHFONT (LPCWSTR _fontName, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut)
{
	fontHandle = CreateFontW (_size, 0, 0, 0, _bold, _italic, _underline, _strlikeOut, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, _fontName);
	return fontHandle ? true : false;
}

void RES_FONT::UninstallFont ()
{
	if (fontInstaller) 
	{
		RemoveFontMemResourceEx (fontInstaller);
		fontInstaller = nullptr;
	}
}

void RES_FONT::DestroyHFONT ()
{
	SAFE_DELETE_OBJ (fontHandle);
}
