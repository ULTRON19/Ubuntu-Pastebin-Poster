#include "winres.h"

RES_OTHER::RES_OTHER ():
	hMem (nullptr),
	dwSize (0)
{
}

RES_OTHER::~RES_OTHER ()
{
	FreeRes ();
}

HGLOBAL RES_OTHER::GetHGLOBAL ()
{
	return hMem;
}

DWORD RES_OTHER::GetSize ()
{
	return dwSize;
}

bool RES_OTHER::LoadRes (HINSTANCE hInstance, LPCWSTR lpName, LPCWSTR lpType)
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

bool RES_OTHER::ExtractToLocal (LPCWSTR lpFileName)
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

void RES_OTHER::FreeRes ()
{
	if (hMem)
		FreeResource (hMem);
		
	hMem = nullptr;
}


