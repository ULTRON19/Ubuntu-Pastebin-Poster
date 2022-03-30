#ifndef __WINRES__
#define __WINRES__

#define SAFE_DELETE_OBJ(obj) \
{\
	if(obj)\
	{\
		DeleteObject(obj);\
		obj = NULL;\
	}\
}

#include "winapp.h"

class WINRES
{
public:
	// Init / CoInit
	bool Initialize (HINSTANCE hInstance, LPCWSTR lpName, LPCWSTR lpType);
	void UnInitialize ();
	
	// Write resource to local file
	bool ExtractToLocal (LPCSTR lpFileName);
	
	WINRES ();
	~WINRES ();
	
protected:
	HGLOBAL hMem;
	DWORD dwSize;	
};

class WINFONT: private WINRES
{
public:
	// Extract font from resource, install it and create the handle
	bool Initialize (HINSTANCE hInstance, LPCWSTR _fontName, LPCWSTR _resId = NULL, int _size = FW_DONTCARE, int _bold = FW_DONTCARE, bool _italic = false, bool _underline = false, bool _strlikeOut = false);
	void UnInitialize ();
	
	// Get the handle
	HFONT GetFontHandle ();
	
	WINFONT ();
	~WINFONT ();
	
private:
	// Install and uninstall font
	bool InstallFont (HINSTANCE hInstance, LPCWSTR _resId);
	void UninstallFont ();
	
	// Create the handle
	bool CreateFontHandle (LPCWSTR _fontName, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut);
	void DestroyFontHandle ();
	
	HANDLE fontInstaller;
	HFONT fontHandle;
};

#endif
