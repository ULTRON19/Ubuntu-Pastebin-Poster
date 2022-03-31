#include "poster.h"

#include <fstream>

POSTER::POSTER ():
	pHttpClient (nullptr)
{
}

POSTER::~POSTER ()
{
	UnInitialize ();
}

POSTER* POSTER::GetInstance ()
{
	static POSTER instance;
	return &instance;
}

void POSTER::WinHttpStatusCallBack (DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	static bool bSSLError = false;
	
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
            	if (pHttpClient -> SetOption (WINHTTP_OPTION_SECURITY_FLAGS, 
					SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                    SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE))
            	{
            		if (!pHttpClient -> AsyncOperate (&HTTPCLIENT::SendRequest))
						WinHttpPostCallBack (false);
				}
				else
					WinHttpPostCallBack (false);
				
				bSSLError = false;
			}
			else
				WinHttpPostCallBack (false);
				
			break;
		}
		
		// Send requeset succeed, go to next step	
		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
			if (!pHttpClient -> AsyncOperate (&HTTPCLIENT::SendData))
				WinHttpPostCallBack (false);
			break;
		
		// Send data succeed, go to next step
		case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
			if (!pHttpClient -> AsyncOperate (&HTTPCLIENT::ReceiveResponse))
				WinHttpPostCallBack (false);
			break;
		
		// Receive response succeed, go to next step
		case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
			WinHttpPostCallBack (true);
			break;
		
		default:
			break;
    }
}

bool POSTER::Post (std::string sPoster, std::string sSyntax, std::string sExpiration, std::string sFilePath)
{   	
	std::string sFile;
    if (!LoadFile (sFilePath, sFile))
    	return false;
    
    if (!WINCVT::AnsiToUtf8 (sPoster, sPoster))
    	return false;

    if (!pHttpClient -> OpenRequest (L"POST", L"/", NULL, WINHTTP_NO_REFERER, 
		WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE))
		return false;
		
	if (!pHttpClient -> SetOption (WINHTTP_OPTION_DISABLE_FEATURE, WINHTTP_DISABLE_REDIRECTS))
	{
		pHttpClient -> CloseRequest ();
		return false;
	}
		
	if (!pHttpClient -> AddHeader ({
		{L"Content-type", L"multipart/form-data; boundary=WebKitFormBoundary7MA4YWxkTrZu0gW"}
		}, WINHTTP_ADDREQ_FLAG_ADD))
	{
		pHttpClient -> CloseRequest ();
		return false;
	}
		
	// Set callback
	using namespace std::placeholders;
	pHttpClient -> SetRequestCallBack (std::bind (&POSTER::WinHttpStatusCallBack, this, _1, _2, _3));
    
    pHttpClient -> ClearContent ();
    pHttpClient -> AddContent (FORMDATA, {
		{"poster", sPoster},
		{"syntax", sSyntax},
		{"expiration", sExpiration},
		{"content", sFile}
	}, "WebKitFormBoundary7MA4YWxkTrZu0gW");
    
    // First asynchronous operation
    return pHttpClient -> AsyncOperate (&HTTPCLIENT::SendRequest);
}

void POSTER::SetPostCompleteCallBack (std::function <void (std::wstring)> cbPostComplete)
{
	postcompleteCallBack = cbPostComplete;
}

bool POSTER::Initialize ()
{
	pHttpClient = new HTTPCLIENT;
	
	if (!pHttpClient -> OpenHttp (L"Paste", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC))
		return false;
	
	if (!pHttpClient -> ConnectHttp (PASTE_SERVER, INTERNET_DEFAULT_HTTPS_PORT))
		return false;
		
	return true;
}

void POSTER::UnInitialize ()
{
	if (pHttpClient)
	{
		delete pHttpClient;
		pHttpClient = nullptr;
	}
}

bool POSTER::LoadFile (std::string sFilePath, std::string& sFile)
{
	// Check the file path
	if (!~_access (sFilePath.c_str (), 4))
		return REPORT (LVERROR, "Invalid file path"), false;
	
	std::fstream fin (sFilePath);
	
	if (!fin.good ())
		return REPORT (LVERROR, "Cannot open file"), false;
	
	sFile.clear ();
	
	for (char c; !fin.eof (); sFile.append (1, c))
		fin.read (&c, 1);
	
	fin.close ();
	
	sFile.pop_back ();
	UINT uiFileCode = WINCVT::DetectFileCode (sFile);
	
	if (uiFileCode == CODE_UTF8_BOM)
	{
		sFile = sFile.substr (3);
		return true;
	}
	
	std::wstring wsTemp;
	
	if (uiFileCode == CODE_UTF8)
	{
		if (WINCVT::StringToWString (wsTemp, sFile, CP_UTF8) &&
			WINCVT::WStringToString (sFile, wsTemp, CP_UTF8))
			return true;
	
		else
			uiFileCode = CODE_ANSI;
	}
		
	if (uiFileCode != CODE_ANSI)
	{
		bool isBE = uiFileCode == CODE_UNICODE_BE;
		wchar_t wc = 0;
		
		for (unsigned i = 2; i < sFile.size (); i += 2)
		{
			wc = (unsigned char) sFile [i + !isBE] << 8 | (unsigned char) sFile [i + isBE];
			wsTemp.append (1, wc);
		}
	}
    
    else if (!WINCVT::StringToWString (wsTemp, sFile, CP_ACP))
    	return false;
    
	return WINCVT::WStringToString (sFile, wsTemp, CP_UTF8);
}

void POSTER::WinHttpPostCallBack (bool isSuccess)
{
	if (isSuccess)
	{	
		std::wstring wsURL;
		
		if (pHttpClient -> ReadHeader (WINHTTP_QUERY_LOCATION, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX, wsURL))
		{
			wsURL = PASTE_URL + wsURL;
			
			if (postcompleteCallBack)	
				postcompleteCallBack (wsURL);

			pHttpClient -> CloseRequest ();
			return;
		}
	}
	
	if (postcompleteCallBack)	
		postcompleteCallBack (L"");

	pHttpClient -> CloseRequest ();
}
