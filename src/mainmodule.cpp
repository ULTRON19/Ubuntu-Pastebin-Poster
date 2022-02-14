#include "mainmodule.h"

MAINMODULE::MAINMODULE ()
{
	pResManager = RESMANAGER::GetInstance ();
	pMainFront = MAINFRONT::GetInstance ();
	
	pRegedit = new REGEDIT;
	
	pPoster = POSTER::GetInstance ();
}

MAINMODULE::~MAINMODULE ()
{
	CoInitialize ();
}

MAINMODULE* MAINMODULE::GetInstance ()
{
	static MAINMODULE instance;
	return &instance;
}

bool MAINMODULE::Initialize (HINSTANCE hInstance)
{
	// Locate the file
	char* cbuffer = new char [MAX_PATHLEN];
	GetModuleFileNameA (NULL, cbuffer, MAX_PATH);
	
	// Initialize regedit
	pRegedit -> Initialize (cbuffer);
	
	// Get the path and initial error reporter
	_tcsrchr (cbuffer, '\\') [0] = '\0';
	ERH::sDefaultFilePath = std::string (cbuffer);
	
	// Set info path
	sSettingsPath = std::string (cbuffer) + DEFAULT_SETTINGSNAME;
	
	delete [] cbuffer;
	cbuffer = nullptr;
	
	ERH erh;
	
	// Initialize front
	if (!pResManager -> Initialize (hInstance))
		return false;
	
	if (!pMainFront -> Initialize ())
		return false;
		
	// Initialize poster
	if (!pPoster -> Initialize ())
		return false;
	
	// Set info
	std::wstring wsFilePath;
	
	if (GetCmdFilePath (wsFilePath))
		pMainFront -> SetFilePath (wsFilePath);
	
	pMainFront -> SetComboBoxItem (lstDefaultSyntax, lstDefaultExpiration);
	pMainFront -> SetRightMenu (pRegedit -> GetStatus ());
	
	// Set callback
	using namespace std::placeholders;
	
	pMainFront -> SetPostCallBack (std::bind (&POSTER::Post, pPoster, _1, _2, _3, _4));
	pMainFront -> SetRightMenuCallBack (std::bind (&REGEDIT::SetStatus, pRegedit, _1));
	
	pPoster -> SetPostCompleteCallBack (std::bind (&MAINFRONT::PostCompleteHandle, pMainFront, _1));
	
	return true;
}

void MAINMODULE::CoInitialize ()
{
	if (pRegedit)
	{
		delete pRegedit;
		pRegedit = nullptr;
	}
}

int MAINMODULE::EnterLoop ()
{
	return pMainFront -> EnterLoop ();
}

bool MAINMODULE::GetCmdFilePath (std::wstring& wsFilePath)
{
	// Get args
	bool res = false;
	int nArgs;
	LPWSTR *szArglist = CommandLineToArgvW (GetCommandLineW (), &nArgs);
	
	if (szArglist == nullptr)
	{
		WINAPP::WinErrorReport (__FUNCTION__, "CommandLineToArgvW", false);
		return false;
	}
	
	// no additional input
	if (nArgs == 1)
	{
		LocalFree (szArglist);
		return false;
	}
	
	if (nArgs > 2)
		MessageBoxA (NULL, "Multiple files are not currently supported, the first valid file will be read.", 
			"Warning", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
	
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
		MessageBoxA (NULL, "Please select a valid file!", "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	
	LocalFree (szArglist);
	return res;
}
