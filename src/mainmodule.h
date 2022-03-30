#ifndef __MAINMODULE__
#define __MAINMODULE__

#include <tchar.h>

#include "poster.h"
#include "registrant.h"
#include "mainfront.h"

class MAINMODULE
{
public:
	// Only one instance of MAINMODULE can exist at one time
	static MAINMODULE* GetInstance ();
	
	// Initialize
	bool Initialize (HINSTANCE hInstance);
	void UnInitialize ();
	
	// Main loop
	int EnterLoop ();
	
	~MAINMODULE ();
	
private:
	MAINMODULE ();
	
	bool GetExecuteFilePath (std::string& sFolderPath, std::string& sFileName);
	bool GetCmdFilePath (std::wstring& wsFilePath);

	RESMANAGER* pResourceManager;
	INIMANAGER* pIniManager;
	MAINFRONT* pMainFront;
	REGISTRANT* pRegistrant;
	POSTER* pPoster;
};

#endif
