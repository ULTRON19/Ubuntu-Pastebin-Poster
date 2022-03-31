#ifndef __WINCVT_H__
#define __WINCVT_H__

#define CODE_ANSI			0
#define CODE_UNICODE_LE		1
#define CODE_UNICODE_BE		2
#define CODE_UTF8			3
#define CODE_UTF8_BOM		4

#include <windows.h>
#include "logger.h"

namespace WINCVT
{
	UINT DetectFileCode (const std::string& sFile);
	
	bool AnsiToUtf8 (std::string& sTarget, const std::string& sSource);
	
	bool StringToWString (std::wstring& wsTarget, const std::string& sSource, UINT CodePage);
	bool WStringToString (std::string& sTarget, const std::wstring& wsSource, UINT CodePage);
};

#endif
