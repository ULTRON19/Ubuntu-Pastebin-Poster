#ifndef __POSTER__
#define __POSTER__

#ifndef __GNUC__
#pragma comment (lib, "winhttp.lib")
#endif

#define PASTE_URL			L"https://paste.ubuntu.com"
#define PASTE_SERVER		L"paste.ubuntu.com"

#include <iterator>

#include "wincvt.h"
#include "winhttprequest.h"

class POSTER
{
public:
	// Only one instance of POSTER can exist at one time
	static POSTER* GetInstance ();
	
	// Initialize
	bool Initialize ();
	void UnInitialize ();
	
	// Submit the data
	bool Post (std::string sPoster, std::string sSyntax, std::string sExpiration, std::string sFilePath);
	
	// Set call back
	void SetPostCompleteCallBack (std::function <void (std::wstring)> cbPostComplete);
	
	~POSTER ();
	
private:
	POSTER ();
	
	// Prepare form
	bool LoadFile (std::string sFilePath, std::string& sFile);
	
	// CallBack function
	void WinHttpStatusCallBack (DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	void WinHttpPostCompleteHandle (bool isSuccess);
	
	// Http client
	WINHTTPREQUEST* pWinHttpRequest;
	
	// Call back
	std::function <void (std::wstring)> postcompleteCallBack;
};

#endif
