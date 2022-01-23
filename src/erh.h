#ifndef __ERH__
#define __ERH__

// if compiled on visual studio, error about 'ctime' function 
// will occur and it need to be ignored
#ifdef _MSC_VER 
#pragma warning (disable: 4996)
#endif

#define DEFAULT_LOGNAME "\\Error_Report.log"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <io.h>

typedef class ERROR_HANDLE
{
public:
	static std::string sDefaultFilePath;
	explicit ERROR_HANDLE (const char* csFilePath = nullptr);
		
	// External interface
	void operator () (const std::string& func, const std::string& errMsg, bool isAlert);

private:
	// When an error occurs, a pop-up window will pop up to remind the user
	void Error_Alert (const std::string& errMsg);
	
	// When an error occurs, an error report will be written to the log file
	void Error_Record (const std::string& func, const std::string& errMsg);
		
	// Get the current time
	std::string Get_Time ();
	
	// log path
	std::string sLogFileName;
}ERH;

#endif
