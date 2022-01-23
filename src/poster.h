#ifndef __POSTER__
#define __POSTER__

#ifndef __GNUC__
#pragma comment (lib, "winhttp.lib")
#endif

#define PASTE_URL			L"https://paste.ubuntu.com"

#define LOGIN_SERVER		L"login.ubuntu.com"
#define PASTE_SERVER		L"paste.ubuntu.com"

#define LOGIN_REFERER		L"https://login.ubuntu.com/"

#include <windows.h>
#include <winhttp.h>
#include <regex>
#include <fstream>
#include "winapp.h"

class POSTER
{
public:
	// Only one instance of POSTER can exist at one time
	static POSTER* GetInstance ();
	
	// Winhttp status callback
	static void CALLBACK WinHttpStatusCallBack (HINTERNET hInternet, DWORD_PTR dwContext, 
		DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	
	// Submit the data
	bool Post (std::string sPoster, std::string sSyntax, std::string sExpiration, std::string sFilePath);
	
	// Set call back
	void SetPostCompleteCallBack (std::function <void (std::wstring)> cbPostComplete);
	
private:
	POSTER ();
	
	bool InitHttp ();
	bool CoInitHttp ();
	
	// Prepare form
	bool GenerateForm (std::string sPoster, std::string sSyntax, std::string sExpiration, std::string sFilePath);
	bool LoadFile (std::string sFilePath, std::string& sFile);
	
	// Send call back
	void PostCallBack (bool isSuccess);
	
	// http handle
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	
	// Call back
	std::function <void (std::wstring)> postcompleteCallBack;
	
	// Because it is an asynchronous operation, the data buffer needs to be saved.
	std::string sContent;
};

#endif
