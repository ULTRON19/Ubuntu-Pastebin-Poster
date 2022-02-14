#ifndef __ERRHANDLER__
#define __ERRHANDLER__

// if compiled on visual studio, error about 'ctime' function 
// will occur and it need to be ignored
#ifdef _MSC_VER 
#pragma warning (disable: 4996)
#endif

#define DEFAULT_LOGNAME "\\ErrorReport.log"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <io.h>

class ERRHANDLER
{
public:
	static std::string sDefaultFilePath;
	explicit ERRHANDLER (const char* csFilePath = nullptr);
		
	// External interface
	void operator () (const std::string& func, const std::string& errMsg, bool isAlert);

private:
	// When an error occurs, a pop-up window will pop up to remind the user
	void ErrorAlert (const std::string& errMsg);
	
	// When an error occurs, an error report will be written to the log file
	void ErrorRecord (const std::string& func, const std::string& errMsg);
		
	// Get the current time
	std::string GetTime ();
	
	// log path
	std::string sLogFileName;
};

#endif
