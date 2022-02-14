#include "httpclient.h"

std::queue <std::pair <HTTPCLIENT*, bool (HTTPCLIENT::*) (void)>> HTTPCLIENT::pAsyncOperationList;

HTTPCLIENT::HTTPCLIENT ():
	hSession (nullptr), hConnect (nullptr), hRequest (nullptr),
	requestCallBack (nullptr),
	isAsync (false),
	pszOutBuffer (nullptr), dwSize (0)
{
}

HTTPCLIENT::~HTTPCLIENT ()
{
	if (pszOutBuffer)
	{
		delete pszOutBuffer;
		pszOutBuffer = nullptr;
	}
	
	RemoveRequestCallBack ();
	
	CloseRequest ();
	CloseConnect ();
	CloseHttp ();
}

bool HTTPCLIENT::OpenHttp (LPCWSTR pszAgentW, DWORD dwAccessType, LPCWSTR pszProxyW, LPCWSTR pszProxyBypassW, DWORD dwFlags)
{
	if (hSession)
		return false;
	
	// Use WinHttpOpen to obtain a session handle.
	// Set asynchronous mode by checking dwFlags
	hSession = WinHttpOpen (pszAgentW, dwAccessType, pszProxyW, pszProxyBypassW, dwFlags);
	
	if (!hSession)
		return HttpErrorReport (__FUNCTION__, "WinHttpOpen", true), false;
		
	isAsync = dwFlags & WINHTTP_FLAG_ASYNC;
	return true;
}

bool HTTPCLIENT::ConnectHttp (LPCWSTR pswzServerName, INTERNET_PORT nServerPort)
{
	if (!hSession || hConnect)
		return false;
	
	// Specify an HTTP server
	hConnect = WinHttpConnect (hSession, pswzServerName, nServerPort, 0);
	
	if (!hConnect)
		return HttpErrorReport (__FUNCTION__, "WinHttpConnect", true), false;
	
	return true;
}

bool HTTPCLIENT::OpenRequest (LPCWSTR pwszVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags)
{
	if (!hSession || !hConnect || hRequest)
		return false;
	
	hRequest = WinHttpOpenRequest (hConnect, pwszVerb, pwszObjectName, pwszVersion, pwszReferrer, ppwszAcceptTypes, dwFlags);
	
	if (!hRequest)
		return HttpErrorReport (__FUNCTION__, "WinHttpOpenRequest", true), false;
	
	return true;
}

bool HTTPCLIENT::AddHeader (WSPIRLIST listHeader, DWORD dwModifiers)
{	
	if (!hRequest)
		return false;

	// Generate request header
	std::wstring header = L"";
	
	for (auto& pir: listHeader)
		header += pir.first + L": " + pir.second + L"\r\n";
	
	if (!WinHttpAddRequestHeaders (hRequest, header.c_str (), DWORD (header.length ()), WINHTTP_ADDREQ_FLAG_ADD))
		return HttpErrorReport (__FUNCTION__, "WinHttpAddRequestHeaders", true), false;
	
	return true;
}

bool HTTPCLIENT::AddContent (DWORD dwContentType, SPIRLIST listContent, std::string sBoundary)
{
	if (!hRequest)
		return false;
	
	sRequest.clear ();
	
	if (dwContentType == XWWWFORMURLENCODED)
		for (auto I = listContent.begin (); I != listContent.end (); I ++)
		{
			sRequest += I -> first + "=" + I -> second;
			
			if (I != listContent.end () - 1)
				sRequest += "&";
		}
	
	else
	{
		for (auto& pir: listContent)
		{
			sRequest += "--" + sBoundary + "\r\n";
			sRequest += "Content-Disposition: form-data; ";
			sRequest += "name=\"" + pir.first + "\"\r\n\r\n";
			sRequest += pir.second + "\r\n";
		}
		
		sRequest += "\r\n--" + sBoundary + "\r\n";
	}
	
	return true;
}

void HTTPCLIENT::ClearContent ()
{
	sRequest.clear ();
}

bool HTTPCLIENT::SendRequest ()
{
	if (!hRequest)
		return false;
		
	// In the case of asynchronous mode, the error is handed over to the callback function.
	if (isAsync)
	{
		if (!requestCallBack)
			return false;
		
		if (!WinHttpSendRequest (hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA,
			0, (DWORD) sRequest.length (), reinterpret_cast <DWORD_PTR> (this)))
			return HttpErrorReport (__FUNCTION__, "WinHttpSendRequest", true), false;
			
		return true;
	}
	
	// Certain circumstances dictate that we may need to loop on WinHttpSendRequest
	bool retry = false, res = false;
	
	do
	{
		retry = false;
		
		// no retry on success, possible retry on failure
		if (!(res = WinHttpSendRequest (hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 
			0, (DWORD) sRequest.length (), 0)))
		{
			int errMsg = GetLastError ();
			
			// (1) If you want to allow SSL certificate errors and continue
			// with the connection, you must allow and initial failure and then
			// reset the security flags. 
			
			if (errMsg == ERROR_WINHTTP_SECURE_FAILURE)
			{
				if (SetOption (WINHTTP_OPTION_SECURITY_FLAGS, SECURITY_FLAG_IGNORE_UNKNOWN_CA |
						SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
						SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
						SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE))
					retry = true;
			}
			
			// (2) Negotiate authorization handshakes may return this error
			// and require multiple attempts
			else if (errMsg == ERROR_WINHTTP_RESEND_REQUEST)
				retry = true;
			
			else
				res = true;
		}
	}
	while (retry);
	
    return res;
}

bool HTTPCLIENT::SendData ()
{
	if (!hRequest || sRequest.empty ())
		return false;
		
	DWORD dwBytesWritten = 0;
	if (!WinHttpWriteData (hRequest, sRequest.c_str (), (DWORD) sRequest.length (), isAsync ? nullptr : &dwBytesWritten))
		return HttpErrorReport (__FUNCTION__, "WinHttpWriteData", true), false;

	return true;
}

bool HTTPCLIENT::ReceiveResponse ()
{
	if (!hRequest)
		return false;
	
	if (!WinHttpReceiveResponse (hRequest, NULL))
		return HttpErrorReport (__FUNCTION__, "ReceiveResponse", true), false;
	
	return true;
}

bool HTTPCLIENT::QueryDataAvaliable ()
{
	if (!hRequest)
		return false;
		
	dwSize = 0;
	if (!WinHttpQueryDataAvailable (hRequest, isAsync ? nullptr : &dwSize))
		return HttpErrorReport (__FUNCTION__, "WinHttpQueryDataAvailable", true), false;
	
	return true;
}

bool HTTPCLIENT::ReadData ()
{
	if (!hRequest)
		return false;
	
	// In the case of asynchronous mode, the error is handed over to the callback function.
	if (isAsync)
	{
		if (!dwSize || !requestCallBack)
			return false;
		
		// Resize space for the buffer.
		pszOutBuffer = new char [(size_t) dwSize + 1];
		pszOutBuffer [dwSize] = '\0';
		
		if (!WinHttpReadData (hRequest, pszOutBuffer, dwSize, nullptr))
			return HttpErrorReport (__FUNCTION__, "WinHttpReadData", true), false;
			
		return true;
	}
	
	DWORD dwDownloaded = 0;
	
	do
	{
		// Verify available data.
		if (!QueryDataAvaliable ())
			return false;
		
		// Resize space for the buffer.
		pszOutBuffer = new char [(size_t) dwSize + 1];
		pszOutBuffer [dwSize] = '\0';

		if (!WinHttpReadData (hRequest, pszOutBuffer, dwSize, &dwDownloaded))
			return HttpErrorReport (__FUNCTION__, "WinHttpReadData", true), false;

		else
			sResponse += std::string (pszOutBuffer);
		
		ClearBuffer ();
	}
	while (dwSize > 0);

	return true;
}

bool HTTPCLIENT::ReadHeader (DWORD dwInfoLevel, LPCWSTR pwszName, LPDWORD lpdwIndex, std::wstring& wsOutBuffer)
{
	if (!hRequest)
		return false;
		
	DWORD dwSize = 0;
	wsOutBuffer.clear ();

	// First, use WinHttpQueryHeaders to obtain the size of the buffer.
	WinHttpQueryHeaders (hRequest, dwInfoLevel,
		dwInfoLevel == WINHTTP_QUERY_CUSTOM ? pwszName : WINHTTP_HEADER_NAME_BY_INDEX,
		NULL, &dwSize, lpdwIndex);

	// Allocate memory for the buffer.
	if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
	{
		wsOutBuffer.resize (dwSize / sizeof (WCHAR));

		// Now, use WinHttpQueryHeaders to retrieve the header.
		return WinHttpQueryHeaders (hRequest, dwInfoLevel,
			dwInfoLevel == WINHTTP_QUERY_CUSTOM ? pwszName : WINHTTP_HEADER_NAME_BY_INDEX,
			(LPVOID) wsOutBuffer.c_str (), &dwSize, lpdwIndex);
	}

	return false;
}

void HTTPCLIENT::MoveResponseData (std::string& sOutBuffer)
{
	sOutBuffer = std::move (sResponse);
	sResponse.clear ();
}

bool HTTPCLIENT::AsyncOperate (bool (HTTPCLIENT::*operation) (void))
{
	if (operation == &HTTPCLIENT::CloseHttp ||
		operation == &HTTPCLIENT::CloseConnect ||
		operation == &HTTPCLIENT::CloseRequest)
			return false;
	
	pAsyncOperationList.push ({this, operation});
	
	if (pAsyncOperationList.size () == 1)
	{
		if (WinHttpSetStatusCallback (hRequest, WinHttpGlobalCallBack, 
			WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
			// It's an unstable deal
			return HttpErrorReport (__FUNCTION__, "WinHttpSetStatusCallback", true), false;
				
		return (this ->* operation) ();
	}
	
	return true;
}

bool HTTPCLIENT::SetOption (DWORD dwOption, DWORD dwFlags)
{
	if (!hRequest)
		return false;
	
	if (!WinHttpSetOption (hRequest, dwOption, &dwFlags, sizeof (dwFlags)))
		return HttpErrorReport (__FUNCTION__, "WinHttpSetOption", true), false;
		
	return true;
}

void HTTPCLIENT::SetRequestCallBack (std::function <void (DWORD, LPVOID, DWORD)> cbRequest)
{
	requestCallBack = cbRequest;
}

void HTTPCLIENT::RemoveRequestCallBack ()
{
	if (!requestCallBack)
		return;
		
	requestCallBack = nullptr;
	
	if (WinHttpSetStatusCallback (hRequest, nullptr, 
		WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
		HttpErrorReport (__FUNCTION__, "WinHttpSetStatusCallback", true);
}

bool HTTPCLIENT::CloseHttp ()
{
	if (!hSession)
		return true;
	
	if (!WinHttpCloseHandle (hSession))
		return HttpErrorReport (__FUNCTION__, "WinHttpCloseHandle", false), false;
	
	hSession = nullptr;
	return true;
}

bool HTTPCLIENT::CloseConnect ()
{
	if (!hConnect)
		return true;
	
	if (!WinHttpCloseHandle (hConnect))
		return HttpErrorReport (__FUNCTION__, "WinHttpCloseHandle", false), false;
	
	hConnect = nullptr;
	return true;
}

bool HTTPCLIENT::CloseRequest ()
{
	if (!hRequest)
		return true;
	
	if (!WinHttpCloseHandle (hRequest))
		return HttpErrorReport (__FUNCTION__, "WinHttpCloseHandle", false), false;
	
	hRequest = nullptr;
	return true;
}

void HTTPCLIENT::ClearBuffer ()
{
	if (!pszOutBuffer)
		return;
	
	delete [] pszOutBuffer;
	pszOutBuffer = nullptr;
}

void HTTPCLIENT::HttpErrorReport (std::string mainFunc, const char* func, bool isAlert)
{
	ERH ErrorReport;
	int err = GetLastError ();

	// Improve error reporting
	std::string errMsg = "Http Error Code: " + std::to_string (err);

	(mainFunc += " - ") += func;
	ErrorReport (mainFunc, errMsg, isAlert);
}

void CALLBACK HTTPCLIENT::WinHttpGlobalCallBack (HINTERNET hInternet, DWORD_PTR dwContext, 
	DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	if (dwContext)
	{
		HTTPCLIENT* pHttpClient = reinterpret_cast <HTTPCLIENT*> (dwContext);
		
		switch (dwInternetStatus)
		{
			// It's needed to clear buffer when an error occurred
			case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
				pHttpClient -> ClearBuffer ();
				break;
				
			// Outputs the contents of the buffer to the response data and clear buffer
			case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
				pHttpClient -> sResponse += std::string (pHttpClient -> pszOutBuffer);
				pHttpClient -> ClearBuffer ();
				break;
			
			// Update dwSize when query data available succeed
			case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
				pHttpClient -> dwSize = dwStatusInformationLength;
				break;
				
			default:
				break;
		}
		
		if (pHttpClient -> requestCallBack)
			pHttpClient -> requestCallBack (dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
	
		// Get the next operation if the current operation is asynchronous
		if (pHttpClient -> isAsync)
			switch (dwInternetStatus)
			{
				case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
				case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
				case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
				case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
				case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
				case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
				{
					std::pair <HTTPCLIENT*, bool (HTTPCLIENT::*) (void)> lstOperation = pAsyncOperationList.front ();
					pAsyncOperationList.pop ();
					
					if (!pAsyncOperationList.empty ())
					{
						std::pair <HTTPCLIENT*, bool (HTTPCLIENT::*) (void)> crtOperation = pAsyncOperationList.front ();
						
						if (lstOperation.first != crtOperation.first)
							if (WinHttpSetStatusCallback (crtOperation.first -> hRequest, WinHttpGlobalCallBack, 
								WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
									HttpErrorReport (__FUNCTION__, "WinHttpSetStatusCallback", true);
									
						(crtOperation.first ->* crtOperation.second) ();
					}
					
					break;
				}
			}
	}

	// Messages should be processed when dwContext is NULL, but I'm too lazy :)
}
