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

#ifndef __RESOTHER__
#define __RESOTHER__

class RES_OTHER
{
public:
	// Load resource
	bool LoadRes (HINSTANCE hInstance, LPCWSTR lpName, LPCWSTR lpType);
	
	// Get the handle and size
	HGLOBAL GetHGLOBAL ();
	DWORD GetSize ();
	
	// Write resource to local file
	bool ExtractToLocal (LPCWSTR lpFileName);
	
	// Free resource
	void FreeRes ();
	
	RES_OTHER ();
	~RES_OTHER ();
	
private:
	HGLOBAL hMem;
	DWORD dwSize;	
};

#endif

#ifndef __RESFONT__
#define __RESFONT__

class RES_FONT
{
public:
	// Extract font from resource, install it and create the handle
	bool InitializeFont (HINSTANCE hInstance, LPCWSTR _fontName, LPCWSTR _resId = NULL, int _size = FW_DONTCARE, int _bold = FW_DONTCARE, bool _italic = false, bool _underline = false, bool _strlikeOut = false);
	void CoUninitializeFont ();
	
	// Get the handle
	HFONT GetHFONT ();
	
	RES_FONT ();
	~RES_FONT ();
	
private:
	// Install and uninstall font
	bool InstallFont (HINSTANCE hInstance, LPCWSTR _resId);
	void UninstallFont ();
	
	// Create the handle
	bool CreateHFONT (LPCWSTR _fontName, int _size, int _bold, bool _italic, bool _underline, bool _strlikeOut);
	void DestroyHFONT ();
	
	HANDLE fontInstaller;
	HFONT fontHandle;
};

#endif

#endif
