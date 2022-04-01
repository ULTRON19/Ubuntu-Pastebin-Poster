#ifndef __LOGGER__
#define __LOGGER__

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <windows.h>
#include <commctrl.h>
#include <io.h>

#define LVINFO			0
#define LVDEBUG			1
#define LVWARN			2
#define LVERROR			3

#define LOGNAME_INFO	"InfoReport.log"
#define LOGNAME_DEBUG	"DebugReport.log"
#define LOGNAME_WARN	"WarnReport.log"
#define LOGNAME_ERROR	"ErrorReport.log"

#define STRING_INFO		"Info"
#define STRING_DEBUG	"Debug"
#define STRING_WARN		"Warning"
#define STRING_ERROR	"Error"

#define WSTRING_INFO	L"Info"
#define WSTRING_DEBUG	L"Debug"
#define WSTRING_WARN	L"Warning"
#define WSTRING_ERROR	L"Error"

#define REPORT(LEVEL,MSG) LOGGER (LEVEL).Report (__FUNCTION__, MSG)
#define RECORD(LEVEL,MSG) LOGGER (LEVEL).Record (__FUNCTION__, MSG)
#define ALERT(LEVEL,MSG) LOGGER (LEVEL).Alert (MSG)
#define TIP(LEVEL,WND,MSG) LOGGER (LEVEL).BalloonTip (WND, MSG)

class LOGGER
{
public:
	static void SetFolderPath (const std::string& _sFolderPath);
	static void SetWindowHandle (HWND _hwndGlobal);

	LOGGER (unsigned int _loggerLevel): loggerLevel (_loggerLevel) {}
	
	void Report (const char* pcFunc, const char* pcErrMsg);
	void Record (const char* pcFunc, const char* pcErrMsg);
	void Alert (const char* pcErrMsg);
	void BalloonTip (HWND hControl, const wchar_t* pszText);
	
private:
	static std::string sFolderPath;
	static HWND hwndGlobal;

	void GetTime (std::ostream& os);
	unsigned int loggerLevel;	
};

#define WINREPORT(FUNC) WINERRLOGGER ().Report (__FUNCTION__, FUNC)
#define WINRECORD(FUNC) WINERRLOGGER ().Record (__FUNCTION__, FUNC)
#define WINALERT() WINERRLOGGER ().Alert () 

class WINERRLOGGER: public LOGGER
{
public:
	WINERRLOGGER (): LOGGER (LVERROR) {}
	
	void Report (const char* pcFunc, const char* pcWinFunc);
	void Record (const char* pcFunc, const char* pcWinFunc);
	void Alert ();

private:
	static std::string GetWinErrorMessage ();
};

#define HTTPREPORT(FUNC) HTTPERRLOGGER ().Report (__FUNCTION__, FUNC)
#define HTTPRECORD(FUNC) HTTPERRLOGGER ().Record (__FUNCTION__, FUNC)
#define HTTPALERT() HTTPERRLOGGER ().Alert () 

class HTTPERRLOGGER: public LOGGER
{
public:
	HTTPERRLOGGER (): LOGGER (LVERROR) {}
	
	void Report (const char* pcFunc, const char* pcHttpFunc);
	void Record (const char* pcFunc, const char* pcHttpFunc);
	void Alert ();
};

#define REGREPORT(FUNC,CODE) REGERRLOGGER ().Report (__FUNCTION__, FUNC, CODE)
#define REGRECORD(FUNC,CODE) REGERRLOGGER ().Record (__FUNCTION__, FUNC, CODE)
#define REGALERT(CODE) REGERRLOGGER ().Alert (CODE)

class REGERRLOGGER: public LOGGER
{
public:
	REGERRLOGGER (): LOGGER (LVERROR) {}
	
	void Report (const char* pcFunc, const char* pcRegFunc, LSTATUS lsErrCode);
	void Record (const char* pcFunc, const char* pcRegFunc, LSTATUS lsErrCode);
	void Alert (LSTATUS lsErrCode);

private:
	static std::string GetRegErrorMessage (LSTATUS lsErrCode);
};

#endif
