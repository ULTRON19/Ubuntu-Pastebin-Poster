#ifndef __MAINMODULE__
#define __MAINMODULE__

#define DEFAULT_SETTINGSNAME "\\settings.ini"

#include <tchar.h>

#include "poster.h"
#include "regedit.h"
#include "mainfront.h"
#include "defaultsettings.h"

class MAINMODULE
{
public:
	// Only one instance of MAINMODULE can exist at one time
	static MAINMODULE* GetInstance ();
	
	// Initialize
	bool Initialize (HINSTANCE hInstance);
	void CoInitialize ();
	
	// Main loop
	int EnterLoop ();
	
	~MAINMODULE ();
	
private:
	MAINMODULE ();
	
	// A function that handles command-line arguments
	bool GetCmdFilePath (std::wstring& wsFilePath);

	RESMANAGER* pResManager;
	MAINFRONT* pMainFront;
	REGEDIT* pRegedit;
	POSTER* pPoster;
	
	// info path
	std::string sSettingsPath;
};

#endif
