#include "mainmodule.h"

MAINMODULE::MAINMODULE ()
{
	pResourceManager = RESMANAGER::GetInstance ();
	pIniManager = INIMANAGER::GetInstance ();
	pMainFront = MAINFRONT::GetInstance ();
	pRegistrant = new REGISTRANT;
	pPoster = POSTER::GetInstance ();
}

MAINMODULE::~MAINMODULE ()
{
	UnInitialize ();
}

MAINMODULE* MAINMODULE::GetInstance ()
{
	static MAINMODULE instance;
	return &instance;
}

bool MAINMODULE::Initialize (HINSTANCE hInstance)
{
	// Locate the file
	std::string sFolderPath, sFileName;
	
	if (!GetExecuteFilePath (sFolderPath, sFileName))
		return false;
		
	// Initialize regedit
	pRegistrant -> Initialize (sFolderPath + "\\" + sFileName);
	
	// Initialize logger
	LOGGER::SetFolderPath (sFolderPath);
	
	// Initialize resource
	if (!pResourceManager -> Initialize (hInstance))
		return false;
		
	// Initialize ini
	if (!pIniManager -> Initialize (sFolderPath))
		return false;
		
	// Initialize poster
	if (!pPoster -> Initialize ())
		return false;
	
	// Initialize main front
	if (!pMainFront -> Initialize ())
		return false;
		
	// Set main window handle for logger
	LOGGER::SetWindowHandle (pMainFront -> GetMainWindowHandle ());
	
	std::wstring wsFilePath;
	
	if (GetCmdFilePath (wsFilePath))
		pMainFront -> SetFilePath (wsFilePath);
	
	pMainFront -> SetRightMenu (pRegistrant -> GetStatus ());
	
	// Set callback
	using namespace std::placeholders;
	
	pMainFront -> SetPostCallBack (std::bind (&POSTER::Post, pPoster, _1, _2, _3, _4));
	pMainFront -> SetRightMenuCallBack (std::bind (&REGISTRANT::SetStatus, pRegistrant, _1));
	
	pPoster -> SetPostCompleteCallBack (std::bind (&MAINFRONT::PostCompleteHandle, pMainFront, _1));
	
	return true;
}

void MAINMODULE::UnInitialize ()
{
	pIniManager -> Save ();
	
	if (pRegistrant)
	{
		delete pRegistrant;
		pRegistrant = nullptr;
	}
}

int MAINMODULE::EnterLoop ()
{
	return pMainFront -> EnterLoop ();
}

bool MAINMODULE::GetExecuteFilePath (std::string& sFolderPath, std::string& sFileName)
{
	char* pcBuffer = new char [MAX_PATHLEN];
	
	if (GetModuleFileNameA (NULL, pcBuffer, MAX_PATH) == 0)
	{
		ALERT (LVERROR, "Locate the file failed!");
		
		delete [] pcBuffer;
		return false;
	}
	
	char* pSeparator = _tcsrchr (pcBuffer, '\\');
	pSeparator [0] = '\0';
	
	sFolderPath = std::string (pcBuffer);
	sFileName = std::string (++ pSeparator);
	
	delete [] pcBuffer;
	return true;
}

bool MAINMODULE::GetCmdFilePath (std::wstring& wsFilePath)
{
	// Get args
	bool res = false;
	int nArgs;
	LPWSTR *szArglist = CommandLineToArgvW (GetCommandLineW (), &nArgs);
	
	if (szArglist == nullptr)
		return WINRECORD ("CommandLineToArgvW"), false;
	
	// no additional input
	if (nArgs == 1)
	{
		LocalFree (szArglist);
		return false;
	}
	
	if (nArgs > 2)
		ALERT (LVWARN, "Multiple files are not currently supported, the first valid file will be read.");
	
	for (int i = 1; i < nArgs; i ++)
	{
		DWORD fileCode = GetFileAttributesW (szArglist [i]);
		
		// check if it is a folder
		if (fileCode != INVALID_FILE_ATTRIBUTES && !(fileCode & FILE_ATTRIBUTE_DIRECTORY))
		{
			res = true;
			wsFilePath = std::wstring (szArglist [i]);
			break;
		}
	}
	
	if (!res)
		ALERT (LVERROR, "Please select a valid file!");
	
	LocalFree (szArglist);
	return res;
}
