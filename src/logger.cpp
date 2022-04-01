#include "logger.h"

std::string LOGGER::sFolderPath = ".";
HWND LOGGER::hwndGlobal = nullptr;

void LOGGER::SetFolderPath (const std::string& _sFolderPath)
{
	sFolderPath = _sFolderPath;
}

void LOGGER::SetWindowHandle (HWND _hwndGlobal)
{
	hwndGlobal = _hwndGlobal;
}

void LOGGER::Report (const char* pcFunc, const char* pcErrMsg)
{
	Record (pcFunc, pcErrMsg);
	Alert (pcErrMsg);
}

void LOGGER::Record (const char* pcFunc, const char* pcErrMsg)
{
	using std::ios_base;
	
	std::string sLogFileName (sFolderPath + '\\');
	
	switch (loggerLevel)
	{
		case LVINFO:	sLogFileName += LOGNAME_INFO;	break;
		case LVDEBUG:	sLogFileName += LOGNAME_DEBUG;	break;
		case LVWARN:	sLogFileName += LOGNAME_WARN;	break;
		case LVERROR:	sLogFileName += LOGNAME_ERROR;	break;
		default: return;
	}
	
	// If there is no log file in the directory where the executable file is located, create one
	if (!~_access (sLogFileName.c_str (), 0))
	{
		std::ofstream fs;
		fs.open (sLogFileName.c_str (), std::ios::out);
		
		if (fs.good ())
		{
			fs << "Create Time: ";
			GetTime (fs);
			
			fs << "Version: 1.0.2\n" << std::endl;	
		}
		
		fs.close ();
	}
	
	// Write an error report at the end of the file by appending	
	std::ofstream fs;
	fs.open (sLogFileName.c_str (), ios_base::in | ios_base::out | ios_base::app);
			
	fs << "Time: ";
	GetTime (fs);
	
	fs << "Func: " << pcFunc << '\n';
	fs << "ErrMsg: " << pcErrMsg;
	fs << '\n' << std::endl;
	
	fs.close ();
}

void LOGGER::Alert (const char* pcErrMsg)
{
	const char* pcCaption = nullptr;
	UINT uType = MB_OK | MB_TASKMODAL;
	
	switch (loggerLevel)
	{
		case LVINFO:
			pcCaption = STRING_INFO;
			uType |= MB_ICONINFORMATION;
			break;
			
		case LVDEBUG:
			pcCaption = STRING_DEBUG;
			uType |= MB_ICONINFORMATION;
			break;
		
		case LVWARN:
			pcCaption = STRING_WARN;
			uType |= MB_ICONWARNING;
			break;
		
		case LVERROR:
			pcCaption = STRING_ERROR;
			uType |= MB_ICONERROR;
			break;
			
		default: break;
	}
	
	// Create a pop-up window
	MessageBoxA (hwndGlobal, pcErrMsg, pcCaption, uType);
}

void LOGGER::BalloonTip (HWND hControl, const wchar_t* pszMsg)
{
	EDITBALLOONTIP editTip = {0};
	
	editTip.cbStruct = sizeof (EDITBALLOONTIP);
	editTip.pszText = pszMsg;
	
	switch (loggerLevel)
	{
		case LVINFO:
			editTip.ttiIcon = TTI_INFO;
			editTip.pszTitle = WSTRING_INFO;
			break;
			
		case LVDEBUG:
			editTip.ttiIcon = TTI_NONE;
			editTip.pszTitle = WSTRING_DEBUG;
			break;
		
		case LVWARN:
			editTip.ttiIcon = TTI_WARNING;
			editTip.pszTitle = WSTRING_WARN;
			break;
		
		case LVERROR:
			editTip.ttiIcon = TTI_ERROR;
			editTip.pszTitle = WSTRING_ERROR;
			break;
			
		default: break;
	}
	
	Edit_ShowBalloonTip (hControl, &editTip);
}

void LOGGER::GetTime (std::ostream& os)
{
	SYSTEMTIME st;
	GetLocalTime (&st);
	
	os << st.wYear << '.';
	os << std::setw (2) << std::setfill ('0') << st.wMonth << '.';
	os << std::setw (2) << std::setfill ('0') << st.wDay << ' ';
	os << std::setw (2) << std::setfill ('0') << st.wHour << ':';
	os << std::setw (2) << std::setfill ('0') << st.wMinute << ':';
	os << std::setw (2) << std::setfill ('0') << st.wSecond << std::endl;
}

// WINERRLOGGER

void WINERRLOGGER::Report (const char* pcFunc, const char* pcWinFunc)
{
	std::string sFuncInfo = std::string (pcFunc) + " - " + pcWinFunc;
	std::string sErrMsg (GetWinErrorMessage ());
	
	LOGGER::Record (sFuncInfo.c_str (), sErrMsg.c_str ());
	
	sErrMsg = "An error has occurred.\nError message: " + sErrMsg;
	LOGGER::Alert (sErrMsg.c_str ());
}

void WINERRLOGGER::Record (const char* pcFunc, const char* pcWinFunc)
{
	std::string sFuncInfo = std::string (pcFunc) + " - " + pcWinFunc;
	LOGGER::Record (sFuncInfo.c_str (), GetWinErrorMessage ().c_str ());
}

void WINERRLOGGER::Alert ()
{
	std::string sErrMsg = "An error has occurred.\nError message: ";
	sErrMsg += GetWinErrorMessage ();
	
	LOGGER::Alert (sErrMsg.c_str ());
}

std::string WINERRLOGGER::GetWinErrorMessage ()
{
	char cWinMsgBuf [256] = {0};
	int iErrCode = GetLastError ();

	// Convert error codes to messages
	FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, iErrCode,
					MAKELANGID (LANG_ENGLISH, SUBLANG_DEFAULT), cWinMsgBuf, sizeof (cWinMsgBuf), NULL);

	// Improve error reporting
	std::string sErrMsg (cWinMsgBuf);
	sErrMsg += "Win Error Code: " + std::to_string (iErrCode);
	
	return sErrMsg;
}

// HTTPERRLOGGER
void HTTPERRLOGGER::Report (const char* pcFunc, const char* pcHttpFunc)
{
	std::string sFuncInfo = std::string (pcFunc) + " - " + pcHttpFunc;
	std::string sErrMsg = "Http Error Code: " + std::to_string (GetLastError ());
	
	LOGGER::Record (sFuncInfo.c_str (), sErrMsg.c_str ());
	
	sErrMsg = "An error has occurred.\n" + sErrMsg;
	LOGGER::Alert (sErrMsg.c_str ());
}

void HTTPERRLOGGER::Record (const char* pcFunc, const char* pcHttpFunc)
{
	std::string sFuncInfo = std::string (pcFunc) + " - " + pcHttpFunc;
	std::string sErrMsg = "Http Error Code: " + std::to_string (GetLastError ());
	
	LOGGER::Record (sFuncInfo.c_str (), sErrMsg.c_str ());
}

void HTTPERRLOGGER::Alert ()
{
	std::string sErrMsg = "An error has occurred.\nHttp Error Code: ";
	sErrMsg += std::to_string (GetLastError ());
	
	LOGGER::Alert (sErrMsg.c_str ());
}

// REGERRLOGGER

void REGERRLOGGER::Report (const char* pcFunc, const char* pcRegFunc, LSTATUS lsErrCode)
{
	std::string sFuncInfo = std::string (pcFunc) + " - " + pcRegFunc;
	std::string sErrMsg (GetRegErrorMessage (lsErrCode));
	
	LOGGER::Record (sFuncInfo.c_str (), sErrMsg.c_str ());
	
	sErrMsg = "An error has occurred. You may need to run this program with administrator privileges.\nError message: " + sErrMsg;
	LOGGER::Alert (sErrMsg.c_str ());
}

void REGERRLOGGER::Record (const char* pcFunc, const char* pcRegFunc, LSTATUS lsErrCode)
{
	std::string sFuncInfo = std::string (pcFunc) + " - " + pcRegFunc;
	std::string sErrMsg (GetRegErrorMessage (lsErrCode));
	
	LOGGER::Record (sFuncInfo.c_str (), sErrMsg.c_str ());
}

void REGERRLOGGER::Alert (LSTATUS lsErrCode)
{
	std::string sErrMsg = "An error has occurred. You may need to run this program with administrator privileges.\nError message: ";
	sErrMsg += GetRegErrorMessage (lsErrCode);
	
	LOGGER::Alert (sErrMsg.c_str ());
}

std::string REGERRLOGGER::GetRegErrorMessage (LSTATUS lsErrCode)
{
	char cWinMsgBuf [256] = {0};

	// Convert error codes to messages
	FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD) lsErrCode,
					MAKELANGID (LANG_ENGLISH, SUBLANG_DEFAULT), cWinMsgBuf, sizeof (cWinMsgBuf), NULL);

	// Improve error reporting
	std::string sErrMsg (cWinMsgBuf);
	sErrMsg += "Reg Error Code: " + std::to_string (lsErrCode);
	
	return sErrMsg;
}
