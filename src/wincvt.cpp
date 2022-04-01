#include "wincvt.h"

UINT WINCVT::DetectFileCode (const std::string& sFile)
{
	if (sFile.size () < 2)
		return CODE_ANSI;
		
	wchar_t wc = (unsigned char) sFile [0] << 8 | (unsigned char) sFile [1];
	
	switch (wc)
	{
		case 0xfeff: return CODE_UNICODE_BE;
		case 0xfffe: return CODE_UNICODE_LE;
		case 0xefbb: return CODE_UTF8_BOM;
		default:
			if (MultiByteToWideChar (CP_UTF8, MB_ERR_INVALID_CHARS, sFile.c_str (), -1, NULL, 0))
				return CODE_UTF8;
			return CODE_ANSI;
	}
}

bool WINCVT::AnsiToUtf8 (std::string& sTarget, const std::string& sSource)
{
	std::wstring wsTemp;
	
	if (!StringToWString (wsTemp, sSource, CP_ACP))
		return false;
	
	return WStringToString (sTarget, wsTemp, CP_UTF8);
}

bool WINCVT::StringToWString (std::wstring& wsTarget, const std::string& sSource, UINT CodePage)
{
	int len = 0;
	wchar_t* wstr = nullptr;
    	
    if (!(len = MultiByteToWideChar (CodePage, 0, sSource.c_str (), -1, NULL, 0)))
		return WINREPORT ("MultiByteToWideChar"), false;
			
	wstr = new wchar_t [(size_t) len + 1];
		
	if (!MultiByteToWideChar (CodePage, 0, sSource.c_str (), -1, wstr, len))
	{
		delete [] wstr;
		WINREPORT ("MultiByteToWideChar");
		return false;
	}
		
	wstr [len] = '\0';
	wsTarget = std::wstring (wstr);
		
	delete [] wstr;
	return true;
}

bool WINCVT::WStringToString (std::string& sTarget, const std::wstring& wsSource, UINT CodePage)
{
	int len = 0;
	char* str = nullptr;
	
	if (!(len = WideCharToMultiByte (CodePage, 0, wsSource.c_str (), -1, NULL, 0, NULL, NULL)))
		return WINREPORT ("WideCharToMultiByte"), false;
		
	str = new char [(size_t) len + 1];
			
	if (!WideCharToMultiByte (CodePage, 0, wsSource.c_str (), -1, str, len, NULL, NULL))
	{
		delete [] str;
		WINREPORT ("WideCharToMultiByte");
		return false;
	}
	
	str [len] = '\0';
    sTarget = std::string (str);
    
	delete [] str;
	return true;
}
