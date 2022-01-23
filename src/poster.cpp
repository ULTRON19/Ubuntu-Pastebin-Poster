#include "poster.h"

POSTER::POSTER ():
	hSession (nullptr), hConnect (nullptr), hRequest (nullptr)
{

}

POSTER* POSTER::GetInstance ()
{
	static POSTER instance;
	return &instance;
}

void CALLBACK POSTER::WinHttpStatusCallBack (HINTERNET hInternet, DWORD_PTR dwContext, 
		DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	static bool bSSLError = false;
	POSTER* pPoster = POSTER::GetInstance ();
	
	switch (dwInternetStatus)
    {
        case WINHTTP_CALLBACK_STATUS_SECURE_FAILURE:
            bSSLError = true;
			break;
		
		// Failed
		case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
		{
			if (bSSLError)
			{
				// If you want to allow SSL certificate errors and continue with
            	// the connection, you must allow and initial failure and then
            	// reset the security flags. 
            
            	DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                    SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                    SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
                    SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
            
            	// Set option and retry
            	if (WinHttpSetOption (pPoster -> hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof (dwFlags)))
            	{
            		if (!WinHttpSendRequest (pPoster -> hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
						WINHTTP_NO_REQUEST_DATA, 0, (DWORD) pPoster -> sContent.length (), 0))
						{
							WINAPP::WinErrorReport (__FUNCTION__, "WinHttpSendRequest", false);
							pPoster -> PostCallBack (false);
						}
				}
				else
				{
					WINAPP::WinErrorReport (__FUNCTION__, "WinHttpSetOption", false);
					pPoster -> PostCallBack (false);
				}
				
				bSSLError = false;
			}
			else
				pPoster -> PostCallBack (false);
				
			break;
		}
		
		// Send requeset succeed, go to next step	
		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
			if (!WinHttpWriteData (pPoster -> hRequest, pPoster -> sContent.c_str (),
					(DWORD) pPoster -> sContent.length (), NULL))
			{
				WINAPP::WinErrorReport (__FUNCTION__, "WinHttpWriteData", false);
				pPoster -> PostCallBack (false);
			}
			break;
		
		// Write data succeed, go to next step
		case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
			if (!WinHttpReceiveResponse (pPoster -> hRequest, NULL))
			{
				WINAPP::WinErrorReport (__FUNCTION__, "WinHttpReceiveResponse", false);
				pPoster -> PostCallBack (false);
			}
			break;
		
		// Receive response succeed, go to next step
		case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
			pPoster -> PostCallBack (true);
			break;
		
		default:
			break;
    }
}

bool POSTER::Post (std::string sPoster, std::string sSyntax, std::string sExpiration, std::string sFilePath)
{	
    if (!GenerateForm (sPoster, sSyntax, sExpiration, sFilePath))
    	return false;
    	
    if (!InitHttp ())
    	return false;
    
    // First asynchronous operation
    if (!WinHttpSendRequest (hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, (DWORD) sContent.length (), 0))
		return WINAPP::WinErrorReport (__FUNCTION__, "WinHttpSendRequest", true), false;
		
	return true;
}

void POSTER::SetPostCompleteCallBack (std::function <void (std::wstring)> cbPostComplete)
{
	postcompleteCallBack = cbPostComplete;
}

bool POSTER::InitHttp ()
{
	DWORD dwFlags = WINHTTP_DISABLE_REDIRECTS;
	std::wstring wsHeader = std::wstring (L"Content-type: multipart/form-data; ") + 
		L"boundary=WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";

	// Use WinHttpOpen to obtain a session handle.
	if (!(hSession = WinHttpOpen (L"Paste", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC)))
		return WINAPP::WinErrorReport (__FUNCTION__, "WinHttpOpen", true), false;
		
	// Set callback
	if (WinHttpSetStatusCallback (hSession, WinHttpStatusCallBack, 
		WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
		return WINAPP::WinErrorReport (__FUNCTION__, "WinHttpSetStatusCallback", true), false;
    
    // Specify an HTTP server
	if (!(hConnect = WinHttpConnect (hSession, PASTE_SERVER, INTERNET_DEFAULT_HTTPS_PORT, 0)))
		return WINAPP::WinErrorReport (__FUNCTION__, "WinHttpConnect", true), false;
    
    // Open a request
    if (!(hRequest = WinHttpOpenRequest (hConnect, L"POST", L"/", NULL, WINHTTP_NO_REFERER, 
		WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE)))
    	return WINAPP::WinErrorReport (__FUNCTION__, "WinHttpOpenRequest", true), false;
    
    // Set option for a request
    if (!WinHttpSetOption (hRequest, WINHTTP_OPTION_DISABLE_FEATURE, &dwFlags, sizeof (dwFlags)))
		return WINAPP::WinErrorReport (__FUNCTION__, "WinHttpSetOption", true), false;
		
	// Add additonal header
	if (!WinHttpAddRequestHeaders (hRequest, wsHeader.c_str (), (DWORD) wsHeader.length (), WINHTTP_ADDREQ_FLAG_ADD))
        return WINAPP::WinErrorReport (__FUNCTION__, "WinHttpAddRequestHeaders", true), false;
	
	return true;
}

bool POSTER::CoInitHttp ()
{
	bool res = true;
	
	if (!WinHttpCloseHandle (hRequest))
	{
		res = false;
		WINAPP::WinErrorReport (__FUNCTION__, "WinHttpCloseHandle", false);
	}
	
	if (!WinHttpCloseHandle (hConnect))
	{
		res = false;
		WINAPP::WinErrorReport (__FUNCTION__, "WinHttpCloseHandle", false);
	}
	
	if (!WinHttpCloseHandle (hSession))
	{
		res = false;
		WINAPP::WinErrorReport (__FUNCTION__, "WinHttpCloseHandle", false);
	}
	
	hRequest = nullptr;
	hConnect = nullptr;
	hSession = nullptr;
	
	return res;
}

bool POSTER::GenerateForm (std::string sPoster, std::string sSyntax, std::string sExpiration, std::string sFilePath)
{
	std::string sFile;
	if (!LoadFile (sFilePath, sFile))
		return false;
	
	sContent.clear ();
	
	sContent += "--WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
	sContent += "Content-Disposition: form-data; ";
	sContent += "name=\"poster\"\r\n\r\n";
	sContent += sPoster + "\r\n";
	
	sContent += "--WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
	sContent += "Content-Disposition: form-data; ";
	sContent += "name=\"syntax\"\r\n\r\n";
	sContent += sSyntax + "\r\n";
	
	sContent += "--WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
	sContent += "Content-Disposition: form-data; ";
	sContent += "name=\"expiration\"\r\n\r\n";
	sContent += sExpiration + "\r\n";
	
	sContent += "--WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
	sContent += "Content-Disposition: form-data; ";
	sContent += "name=\"content\"\r\n\r\n";
	sContent += sFile + "\r\n";
	
	sContent += "\r\n--WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
	
	return true;
}

bool POSTER::LoadFile (std::string sFilePath, std::string& sFile)
{
	// Check the file path
	if (!~_access (sFilePath.c_str (), 4))
		return ERH () ("LoadFile", "Invalid file path", true), false;
	
	std::fstream file (sFilePath, std::ios::in | std::ios::binary);
	
	if (!file.good ())
		return ERH () ("LoadFile", "Cannot open file", true), false;
	
	// Ignoring spaces is not allowed
	file.unsetf (std::ios::skipws);
	
	std::istream_iterator <char> iter (file);
	std::istream_iterator <char> eof;
	
	std::copy (iter, eof, std::back_inserter (sFile));
	
	return true;
}

void POSTER::PostCallBack (bool isSuccess)
{
	if (isSuccess)
	{	
		std::wstring wsURL = L"";
		DWORD dwSize = 0;

    	// First, use WinHttpQueryHeaders to obtain the size of the buffer.
    	WinHttpQueryHeaders (hRequest, WINHTTP_QUERY_LOCATION, WINHTTP_HEADER_NAME_BY_INDEX,
			NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);

    	// Allocate memory for the buffer.
    	if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
    	{
    	    wsURL.resize (dwSize / sizeof (WCHAR));

    	    // Now, use WinHttpQueryHeaders to retrieve the header.
    	    if (WinHttpQueryHeaders (hRequest, WINHTTP_QUERY_LOCATION, WINHTTP_HEADER_NAME_BY_INDEX,
				(LPVOID) wsURL.c_str (), &dwSize, WINHTTP_NO_HEADER_INDEX))
			{
				wsURL = PASTE_URL + wsURL;
			
				if (postcompleteCallBack)	
					postcompleteCallBack (wsURL);	
			
				CoInitHttp ();
				return;
			}
    	}
	}
	
	CoInitHttp ();
	
	MessageBoxA (NULL, "An error has occurred when sending request", 
		"Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		
	if (postcompleteCallBack)	
		postcompleteCallBack (L"");	
}
