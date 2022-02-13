#ifndef __HTTPCLIENT__
#define __HTTPCLIENT__

#ifndef __GNUC__
#pragma comment (lib, "winhttp.lib")
#endif

#include <windows.h>
#include <winhttp.h>
#include <functional>
#include <map>
#include <queue>
#include "erh.h"

#define XWWWFORMURLENCODED 0
#define FORMDATA 1

typedef std::initializer_list <std::pair <std::string, std::string>> SPIRLIST;
typedef std::initializer_list <std::pair <std::wstring, std::wstring>> WSPIRLIST;

class HTTPCLIENT
{
public:
	// Initial handle
	bool OpenHttp (LPCWSTR pszAgentW, DWORD dwAccessType, LPCWSTR pszProxyW, LPCWSTR pszProxyBypassW, DWORD dwFlags);
	bool ConnectHttp (LPCWSTR pswzServerName, INTERNET_PORT nServerPort);
	bool OpenRequest (LPCWSTR pwszVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags);
	
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
	bool AsyncOperate (bool (HTTPCLIENT::*operation) (void));
	
	// Set option
	bool SetOption (DWORD dwOption, DWORD dwFlags);
	void SetRequestCallBack (std::function <void (DWORD, LPVOID, DWORD)> cbRequest);
	
	// Close handles
	bool CloseHttp ();
	bool CloseConnect ();
	bool CloseRequest ();
	
	HTTPCLIENT ();
	~HTTPCLIENT ();
		
private:
	// Autocomplete GetLastError and submit to ERH
	static void HttpErrorReport (std::string mainFunc, const char* func, bool isAlert);
	
	// CallBack function
	static void CALLBACK WinHttpGlobalCallBack (HINTERNET hInternet, DWORD_PTR dwContext, 
		DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	
	// A queue is used to process each operation because only one asynchronous 
	// operation can exist at a time. It follows the FIFO principle.
	static std::queue <std::pair <HTTPCLIENT*, bool (HTTPCLIENT::*) (void)>> pAsyncOperationList;
	
	// Clear buffer for receiving data
	void ClearBuffer ();
	
	// WinHttp handles
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
	
	// Callback function
	std::function <void (DWORD, LPVOID, DWORD)> requestCallBack;
	
	// Option
	bool isAsync;
	
	// Receive buffer
	char* pszOutBuffer;
	DWORD dwSize;
	
	// Request body and response body
	std::string sRequest;
	std::string sResponse;
};

#endif
