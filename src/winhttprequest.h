#ifndef __WINHTTPREQUEST__
#define __WINHTTPREQUEST__

#ifndef __GNUC__
#pragma comment (lib, "winhttp.lib")
#endif

#include <windows.h>
#include <winhttp.h>
#include <functional>
#include <map>
#include <queue>

#include "logger.h"

#define XWWWFORMURLENCODED 0
#define FORMDATA 1

typedef std::initializer_list <std::pair <std::string, std::string>> SPIRLIST;
typedef std::initializer_list <std::pair <std::wstring, std::wstring>> WSPIRLIST;

class WINHTTPREQUEST
{
public:
	// Initialize
	bool Initialize (LPCWSTR pszAgentW, LPCWSTR pswzServerName, INTERNET_PORT nServerPort, bool _isAsync);
	bool UnInitialize ();
	
	// Open / Close request handle
	bool OpenRequest (LPCWSTR pwszVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags);
	bool CloseRequest ();
	
	// Set proxy server
	bool UseIEProxy ();
	bool UseCustomProxy (WINHTTP_PROXY_INFO wpiProxyInfo);
	bool UseCustomProxy (DWORD dwAccessType, LPWSTR lpszProxy, LPWSTR lpszProxyBypass);
	bool SetProxyUserInfo (LPCSTR pszUserName, LPCSTR pszUserPassword);
	
	// Send request and data
	bool AddHeader (WSPIRLIST listHeader, DWORD dwModifiers);
	bool AddContent (DWORD dwContentType, SPIRLIST listContent, std::string sBoundary);
	void ClearContent ();
	bool SendRequest ();
	bool SendData ();
	
	// Receive response and data
	bool ReceiveResponse ();
	bool QueryDataAvaliable ();	
	bool ReadData ();
	bool ReadHeader (DWORD dwInfoLevel, LPCWSTR pwszName, LPDWORD lpdwIndex, std::wstring& wsOutBuffer);
	void MoveResponseData (std::string& sOutBuffer);
	
	// Asynchronous operate
	bool AsyncOperate (bool (WINHTTPREQUEST::*operation) (void));
	
	// Set option
	bool SetOption (DWORD dwOption, DWORD dwFlags);
	
	// Set request callback
	void SetRequestCallBack (std::function <void (DWORD, LPVOID, DWORD)> cbRequest);
	void RemoveRequestCallBack ();
	
	WINHTTPREQUEST ();
	~WINHTTPREQUEST ();
		
private:
	// CallBack function
	static void CALLBACK WinHttpGlobalCallBack (HINTERNET hInternet, DWORD_PTR dwContext, 
		DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	
	// A queue is used to process each operation because only one asynchronous 
	// operation can exist at a time. It follows the FIFO principle.
	static std::queue <std::pair <WINHTTPREQUEST*, bool (WINHTTPREQUEST::*) (void)>> qAsyncOperation;
	
	// Clear buffer for receiving data
	void ClearBuffer ();
	
	// WinHttp handles
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	
	// Asynchronous mode
	bool isAsync;
	std::function <void (DWORD, LPVOID, DWORD)> requestCallBack;
	
	// Receive buffer
	char* pszOutBuffer;
	DWORD dwSize;
	
	// Request info
	std::wstring wsRequestServer;
	std::wstring wsRequestUrl;
	std::string sRequestBody;
	
	// Response info
	std::string sResponseBody;
};

#endif
