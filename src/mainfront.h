#ifndef __MAINFRONT__
#define __MAINFRONT__

#include <vector>

#include "inimanager.h"
#include "resmanager.h"

class MAINFRONT
{
public:
	// Only one instance of MAINFRONT can exist at one time
	static MAINFRONT* GetInstance ();
	
	// Init
	bool Initialize ();
	
	// Set option for control
	void SetFilePath (std::string sFilePath);
	void SetFilePath (std::wstring wsFilePath);
	void SetRightMenu (bool bRightMenu);
	
	// Post complete callback handler
	void PostCompleteHandle (std::wstring wsURL);
	
	// Set callback
	void SetPostCallBack (std::function <bool (std::string, std::string, std::string, std::string)> cbPost);
	void SetRightMenuCallBack (std::function <bool (bool)> cbRightMenu);
	
	// Main loop
	int EnterLoop ();
	
private:
	MAINFRONT ();

	// Window procedure
	LRESULT proxyMainProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	
	// Update ini
	void UpdateIni (std::string sSuffix);
	void UpdateIni (std::string sPoster, unsigned int uiSyntax, unsigned int uiExpiration);
	
	// Functions about controls
	void SetBoxItem ();
	bool CollectForm (std::string& sPoster, unsigned int& uiSyntax, unsigned int& uiExpiration, std::string& sFilePath);
	
	bool CopyURLToClipboard (std::wstring wsURL);
	
	bool GetChooseFilePath (std::string& sFilePath);
	bool GetDropFilePath (std::string& sFilePath, HDROP hDropInfo);
	
	void DisableAllControls ();
	void EnableAllControls ();
	void ZeroAllControls ();
	
	// Main window
	WINAPP mainApp;
	
	// Static control handle
	HWND hstcPoster, hstcSyntax, hstcExpiration, hstcFilePath, hstcURL;
	// Edit control handle
	HWND hedtPoster, hedtFilePath, hedtURL;
	// Combo box control handle
	HWND hcbxSyntax, hcbxExpiration;
	// Button control handle
	HWND hbtnPost, hbtnRightMenu;
	
	// Callback
	std::function <bool (std::string, std::string, std::string, std::string)> postCallBack;
	std::function <bool (bool)> rightmenuCallBack;
};

#endif
