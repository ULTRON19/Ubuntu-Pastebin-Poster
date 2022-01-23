#ifndef __MAINFRONT__
#define __MAINFRONT__

#include <vector>

#include "front.h"

typedef std::initializer_list <std::pair <std::string, std::string>> LSTPSS;
typedef std::unordered_map <std::string, std::string> COMBOBOX_ITEM;

class MAINFRONT
{
public:
	// Only one instance of MAINFRONT can exist at one time
	static MAINFRONT* GetInstance ();
	
	// Initialize
	bool Initialize ();
	
	// Set option for control
	void SetComboBoxItem (LSTPSS& vctSyntax, LSTPSS& vctExpiration);
	void SetFilePath (const std::wstring& wsFilePath);
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
	
	// Functions about controls
	void AddComboBoxItem (HWND& hControl, COMBOBOX_ITEM& cbiControl, LSTPSS& vctControl);
	bool CollectForm (std::string& sPoster, std::string& sSyntax, std::string& sExpiration, std::string& sFilePath);
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
	
	// Combo box item
	COMBOBOX_ITEM cbiSyntax, cbiExpiration;
	
	// Callback
	std::function <bool (std::string, std::string, std::string, std::string)> postCallBack;
	std::function <bool (bool)> rightmenuCallBack;
};

#endif
