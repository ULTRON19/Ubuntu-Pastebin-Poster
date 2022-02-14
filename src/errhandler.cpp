#include "errhandler.h"

std::string ERRHANDLER::sDefaultFilePath;

ERRHANDLER::ERRHANDLER (const char* csFilePath)
{
	if (csFilePath == nullptr)
		sLogFileName = (sDefaultFilePath.empty () ?  "." : sDefaultFilePath) + DEFAULT_LOGNAME;
	
	// If there is no log file in the directory where the executable file is located, create one
	if (!~_access (sLogFileName.c_str (), 0))
	{
		std::ofstream fs;
		fs.open (sLogFileName.c_str (), std::ios::out);

		fs << "Create Time: " << GetTime () << std::endl;
		fs.close ();
	}
}

void ERRHANDLER::operator () (const std::string& func, const std::string& errMsg, bool isAlert)
{
	ErrorRecord (func, errMsg);
	
	if (isAlert)
		ErrorAlert (errMsg);
}

void ERRHANDLER::ErrorAlert (const std::string& errMsg)
{
	// Create a pop-up window
	MessageBoxA (NULL, errMsg.c_str (), "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
}

void ERRHANDLER::ErrorRecord (const std::string& func, const std::string& errMsg)
{
	using std::ios_base;
	
	// Write an error report at the end of the file by appending	
	std::ofstream fs;
	fs.open (sLogFileName.c_str (), ios_base::in | ios_base::out | ios_base::app);
			
	fs << "Time: " << GetTime ();
	fs << "Func: " << func << '\n';
	fs << "ErrMsg: " << errMsg;
	fs << '\n' << std::endl;
	fs.close ();
}

std::string ERRHANDLER::GetTime ()
{
	time_t nowTime = time (NULL);
	return ctime (&nowTime);	
}
