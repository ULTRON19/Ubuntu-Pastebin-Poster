#include "winhttprequest.h"

std::queue <std::pair <WINHTTPREQUEST*, bool (WINHTTPREQUEST::*) (void)>> WINHTTPREQUEST::qAsyncOperation;

WINHTTPREQUEST::WINHTTPREQUEST ():
	hSession (nullptr), hConnect (nullptr), hRequest (nullptr),
	isAsync (false), requestCallBack (nullptr),
	pszOutBuffer (nullptr), dwSize (0)
{
}

WINHTTPREQUEST::~WINHTTPREQUEST ()
{
	if (pszOutBuffer)
	{
		delete pszOutBuffer;
		pszOutBuffer = nullptr;
	}
	
	CloseRequest ();
	UnInitialize ();
}

bool WINHTTPREQUEST::Initialize (LPCWSTR pszAgentW, LPCWSTR pswzServerName, INTERNET_PORT nServerPort, bool _isAsync)
{
	if (hSession || hConnect)
		return false;
	
	// Set proxy and secure to none
	if (!(hSession = WinHttpOpen (pszAgentW, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, _isAsync ? WINHTTP_FLAG_ASYNC : NULL)))
		return HTTPREPORT ("WinHttpOpen"), false;
		
	isAsync = _isAsync;
	
	if (!(hConnect = WinHttpConnect (hSession, pswzServerName, nServerPort, 0)))
		return HTTPREPORT ("WinHttpConnect"), false;
		
	wsRequestServer = std::wstring (pswzServerName);
	
	return true;
}

bool WINHTTPREQUEST::OpenRequest (LPCWSTR pwszVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags)
{
	if (!hSession || !hConnect || hRequest)
		return false;
	
	hRequest = WinHttpOpenRequest (hConnect, pwszVerb, pwszObjectName, pwszVersion, pwszReferrer, ppwszAcceptTypes, dwFlags);
	
	if (!hRequest)
		return HTTPREPORT ("WinHttpOpenRequest"), false;
		
	wsRequestUrl = (dwFlags & WINHTTP_FLAG_SECURE) ? L"https://" : L"http://";
	wsRequestUrl += wsRequestServer + pwszObjectName;
	
	return true;
}

bool WINHTTPREQUEST::UseIEProxy ()
{
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ieProxyConfig = {0};
	
	if (!WinHttpGetIEProxyConfigForCurrentUser (&ieProxyConfig))
		return HTTPREPORT ("WinHttpGetIEProxyConfigForCurrentUser"), false;
		
	if (ieProxyConfig.lpszProxy)
	{
		bool res = UseCustomProxy (WINHTTP_ACCESS_TYPE_NAMED_PROXY, 
			ieProxyConfig.lpszProxy, ieProxyConfig.lpszProxyBypass);
			
		GlobalFree (ieProxyConfig.lpszProxy);
		
		if (ieProxyConfig.lpszProxyBypass)
			GlobalFree (ieProxyConfig.lpszProxyBypass);
		
		if (res)
			return true;
	}
	
	if (ieProxyConfig.lpszAutoConfigUrl)
	{
		WINHTTP_AUTOPROXY_OPTIONS autoProxyOptions = {0};
		WINHTTP_PROXY_INFO wpiProxyInfo = {0};
		bool res = false;
	
		autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
		autoProxyOptions.lpszAutoConfigUrl = ieProxyConfig.lpszAutoConfigUrl;
		autoProxyOptions.fAutoLogonIfChallenged = true;
		
		if (!(res = WinHttpGetProxyForUrl (hSession, wsRequestUrl.c_str (), &autoProxyOptions, &wpiProxyInfo)))
			HTTPREPORT ("WinHttpGetProxyForUrl");
			
		else
			res = UseCustomProxy (wpiProxyInfo);
		
		if (wpiProxyInfo.lpszProxy)
			GlobalFree (wpiProxyInfo.lpszProxy);
			
		if (wpiProxyInfo.lpszProxyBypass)
			GlobalFree (wpiProxyInfo.lpszProxyBypass);
		
		GlobalFree (ieProxyConfig.lpszAutoConfigUrl);
		
		if (res)
			return true;
	}
	
	if (ieProxyConfig.fAutoDetect)
	{
		WINHTTP_AUTOPROXY_OPTIONS autoProxyOptions = {0};
		WINHTTP_PROXY_INFO wpiProxyInfo = {0};
		bool res = false;
		
		autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
		
		// Use DHCP and DNS-based auto-detection.
		autoProxyOptions.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP |
			WINHTTP_AUTO_DETECT_TYPE_DNS_A;
		
		// If obtaining the PAC script requires NTLM/Negotiate
		// authentication, then automatically supply the client
		// domain credentials.
		autoProxyOptions.fAutoLogonIfChallenged = true;
		
		if (!(res = WinHttpGetProxyForUrl (hSession, wsRequestUrl.c_str (), &autoProxyOptions, &wpiProxyInfo)))
			HTTPREPORT ("WinHttpGetProxyForUrl");
			
		else
			res = UseCustomProxy (wpiProxyInfo);
		
		if (wpiProxyInfo.lpszProxy)
			GlobalFree (wpiProxyInfo.lpszProxy);
			
		if (wpiProxyInfo.lpszProxyBypass)
			GlobalFree (wpiProxyInfo.lpszProxyBypass);
		
		if (res)
			return true;
	}
	
	return false;
}

bool WINHTTPREQUEST::UseCustomProxy (WINHTTP_PROXY_INFO wpiProxyInfo)
{
	if (!WinHttpSetOption (hSession, WINHTTP_OPTION_PROXY, &wpiProxyInfo, sizeof wpiProxyInfo))
		return HTTPREPORT ("WinHttpSetOption"), false;
		
	return true; 
}

bool WINHTTPREQUEST::UseCustomProxy (DWORD dwAccessType, LPWSTR lpszProxy, LPWSTR lpszProxyBypass)
{
	return UseCustomProxy ({dwAccessType, lpszProxy, lpszProxyBypass});
}

bool WINHTTPREQUEST::SetProxyUserInfo (LPCSTR pszUserName, LPCSTR pszUserPassword)
{
	if (!WinHttpSetOption (hRequest, WINHTTP_OPTION_PROXY_USERNAME, 
		const_cast <LPSTR> (pszUserName), lstrlenA (pszUserName)))
		return HTTPREPORT ("WinHttpSetOption"), false;
		
	if (!WinHttpSetOption (hRequest, WINHTTP_OPTION_PROXY_PASSWORD, 
		const_cast <LPSTR> (pszUserPassword), lstrlenA (pszUserPassword)))
		return HTTPREPORT ("WinHttpSetOption"), false;
		
	return true;
}

bool WINHTTPREQUEST::AddHeader (WSPIRLIST listHeader, DWORD dwModifiers)
{	
	if (!hRequest)
		return false;

	// Generate request header
	std::wstring header = L"";
	
	for (auto& pir: listHeader)
		header += pir.first + L": " + pir.second + L"\r\n";
	
	if (!WinHttpAddRequestHeaders (hRequest, header.c_str (), DWORD (header.length ()), WINHTTP_ADDREQ_FLAG_ADD))
		return HTTPREPORT ("WinHttpAddRequestHeaders"), false;
	
	return true;
}

bool WINHTTPREQUEST::AddContent (DWORD dwContentType, SPIRLIST listContent, std::string sBoundary)
{
	if (!hRequest)
		return false;
	
	sRequestBody.clear ();
	
	if (dwContentType == XWWWFORMURLENCODED)
		for (auto I = listContent.begin (); I != listContent.end (); I ++)
		{
			sRequestBody += I -> first + "=" + I -> second;
			
			if (I != listContent.end () - 1)
				sRequestBody += "&";
		}
	
	else
	{
		for (auto& pir: listContent)
		{
			sRequestBody += "--" + sBoundary + "\r\n";
			sRequestBody += "Content-Disposition: form-data; ";
			sRequestBody += "name=\"" + pir.first + "\"\r\n\r\n";
			sRequestBody += pir.second + "\r\n";
		}
		
		sRequestBody += "\r\n--" + sBoundary + "\r\n";
	}
	
	return true;
}

void WINHTTPREQUEST::ClearContent ()
{
	sRequestBody.clear ();
}

bool WINHTTPREQUEST::SendRequest ()
{
	if (!hRequest)
		return false;
		
	// In the case of asynchronous mode, the error is handed over to the callback function.
	if (isAsync)
	{
		if (!requestCallBack)
			return false;
		
		if (!WinHttpSendRequest (hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA,
			0, (DWORD) sRequestBody.length (), reinterpret_cast <DWORD_PTR> (this)))
			return HTTPREPORT ("WinHttpSendRequest"), false;
			
		return true;
	}
	
	// Certain circumstances dictate that we may need to loop on WinHttpSendRequest
	bool retry = false, res = false;
	
	do
	{
		retry = false;
		
		// no retry on success, possible retry on failure
		if (!(res = WinHttpSendRequest (hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 
			0, (DWORD) sRequestBody.length (), 0)))
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

bool WINHTTPREQUEST::SendData ()
{
	if (!hRequest)
		return false;
	
	if (sRequestBody.empty ())
		return true;
		
	DWORD dwBytesWritten = 0;
	if (!WinHttpWriteData (hRequest, sRequestBody.c_str (), (DWORD) sRequestBody.length (), isAsync ? nullptr : &dwBytesWritten))
		return HTTPREPORT ("WinHttpWriteData"), false;

	return true;
}

bool WINHTTPREQUEST::ReceiveResponse ()
{
	if (!hRequest)
		return false;
	
	if (!WinHttpReceiveResponse (hRequest, NULL))
		return HTTPREPORT ("ReceiveResponse"), false;
	
	return true;
}

bool WINHTTPREQUEST::QueryDataAvaliable ()
{
	if (!hRequest)
		return false;
		
	dwSize = 0;
	if (!WinHttpQueryDataAvailable (hRequest, isAsync ? nullptr : &dwSize))
		return HTTPREPORT ("WinHttpQueryDataAvailable"), false;
	
	return true;
}

bool WINHTTPREQUEST::ReadData ()
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
			return HTTPREPORT ("WinHttpReadData"), false;
			
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
			return HTTPREPORT ("WinHttpReadData"), false;

		else
			sResponseBody += std::string (pszOutBuffer);
		
		ClearBuffer ();
	}
	while (dwSize > 0);

	return true;
}

bool WINHTTPREQUEST::ReadHeader (DWORD dwInfoLevel, LPCWSTR pwszName, LPDWORD lpdwIndex, std::wstring& wsOutBuffer)
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

void WINHTTPREQUEST::MoveResponseData (std::string& sOutBuffer)
{
	sOutBuffer = std::move (sResponseBody);
	sResponseBody.clear ();
}

bool WINHTTPREQUEST::AsyncOperate (bool (WINHTTPREQUEST::*operation) (void))
{
	if (operation == &WINHTTPREQUEST::UnInitialize ||
		operation == &WINHTTPREQUEST::CloseRequest)
		return false;
	
	qAsyncOperation.push ({this, operation});
	
	if (qAsyncOperation.size () == 1)
	{
		if (WinHttpSetStatusCallback (hRequest, WinHttpGlobalCallBack, 
			WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
			// It's an unstable deal
			return HTTPREPORT ("WinHttpSetStatusCallback"), false;
				
		return (this ->* operation) ();
	}
	
	return true;
}

bool WINHTTPREQUEST::SetOption (DWORD dwOption, DWORD dwFlags)
{
	if (!hRequest)
		return false;
	
	if (!WinHttpSetOption (hRequest, dwOption, &dwFlags, sizeof (dwFlags)))
		return HTTPREPORT ("WinHttpSetOption"), false;
		
	return true;
}

void WINHTTPREQUEST::SetRequestCallBack (std::function <void (DWORD, LPVOID, DWORD)> cbRequest)
{
	requestCallBack = cbRequest;
}

void WINHTTPREQUEST::RemoveRequestCallBack ()
{
	if (!requestCallBack)
		return;
		
	requestCallBack = nullptr;
	
	if (WinHttpSetStatusCallback (hRequest, nullptr, 
		WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
		HTTPREPORT ("WinHttpSetStatusCallback");
}

bool WINHTTPREQUEST::UnInitialize ()
{
	if (!hSession && !hConnect)
		return true;
	
	if (hSession && !WinHttpCloseHandle (hSession))
		return HTTPRECORD ("WinHttpCloseHandle"), false;
	
	hSession = nullptr;
	
	if (hConnect && !WinHttpCloseHandle (hConnect))
		return HTTPRECORD ("WinHttpCloseHandle"), false;
	
	hConnect = nullptr;
	
	return true;
}

bool WINHTTPREQUEST::CloseRequest ()
{
	if (!hRequest)
		return true;
	
	RemoveRequestCallBack ();

	if (!WinHttpCloseHandle (hRequest))
		return HTTPRECORD ("WinHttpCloseHandle"), false;
	
	hRequest = nullptr;
	return true;
}

void WINHTTPREQUEST::ClearBuffer ()
{
	if (!pszOutBuffer)
		return;
	
	delete [] pszOutBuffer;
	pszOutBuffer = nullptr;
}

void CALLBACK WINHTTPREQUEST::WinHttpGlobalCallBack (HINTERNET hInternet, DWORD_PTR dwContext, 
	DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	if (dwContext)
	{
		WINHTTPREQUEST* pWinHttpRequest = reinterpret_cast <WINHTTPREQUEST*> (dwContext);
		
		switch (dwInternetStatus)
		{
			// It's needed to clear buffer when an error occurred
			case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
				pWinHttpRequest -> ClearBuffer ();
				break;
				
			// Outputs the contents of the buffer to the response data and clear buffer
			case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
				pWinHttpRequest -> sResponseBody += std::string (pWinHttpRequest -> pszOutBuffer);
				pWinHttpRequest -> ClearBuffer ();
				break;
			
			// Update dwSize when query data available succeed
			case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
				pWinHttpRequest -> dwSize = dwStatusInformationLength;
				break;
				
			default:
				break;
		}
		
		if (pWinHttpRequest -> requestCallBack)
			pWinHttpRequest -> requestCallBack (dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
	
		// Get the next operation if the current operation is asynchronous
		if (pWinHttpRequest -> isAsync)
			switch (dwInternetStatus)
			{
				case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
				case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
				case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
				case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
				case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
				case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
				{
					bool bIsNotSkip = true;
					
					WINHTTPREQUEST* pLastClient = qAsyncOperation.front ().first;
					qAsyncOperation.pop ();

					do
					{
						bIsNotSkip = true;
						
						if (!qAsyncOperation.empty ())
						{
							std::pair <WINHTTPREQUEST*, bool (WINHTTPREQUEST::*) (void)> pirOperation = qAsyncOperation.front ();
							WINHTTPREQUEST* pNextClient = pirOperation.first;
							bool (WINHTTPREQUEST::* pNextOperation) (void) = pirOperation.second;
						
							if (pLastClient != pNextClient)
								if (WinHttpSetStatusCallback (pNextClient -> hRequest, WinHttpGlobalCallBack,
									WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
								{
									HTTPREPORT ("WinHttpSetStatusCallback");
									bIsNotSkip = false;
								}
									
							if (bIsNotSkip)
							{
								pLastClient = pNextClient;
								bIsNotSkip = (pNextClient ->* pNextOperation) ();
							}

							if (!bIsNotSkip)
							{
								if (pNextClient -> requestCallBack)
									pNextClient -> requestCallBack (WINHTTP_CALLBACK_STATUS_REQUEST_ERROR, nullptr, 0);
									
								qAsyncOperation.pop ();
							}
						}
					}
					while (!bIsNotSkip);
					
					break;
				}
			}
	}

	// Messages should be processed when dwContext is NULL, but I'm too lazy :)
}
