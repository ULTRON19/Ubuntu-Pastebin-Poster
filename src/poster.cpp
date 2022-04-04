#include "poster.h"

#include <fstream>

POSTER::POSTER ():
	pWinHttpRequest (nullptr)
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
            	if (pWinHttpRequest -> SetOption (WINHTTP_OPTION_SECURITY_FLAGS, 
					SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                    SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE))
            	{
            		if (!pWinHttpRequest -> AsyncOperate (&WINHTTPREQUEST::SendRequest))
						WinHttpPostCompleteHandle (false);
				}
				else
					WinHttpPostCompleteHandle (false);
				
				bSSLError = false;
			}
			else
				WinHttpPostCompleteHandle (false);
				
			break;
		}
		
		// Send requeset succeed, go to next step	
		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
			if (!pWinHttpRequest -> AsyncOperate (&WINHTTPREQUEST::SendData))
				WinHttpPostCompleteHandle (false);
			break;
		
		// Send data succeed, go to next step
		case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
			if (!pWinHttpRequest -> AsyncOperate (&WINHTTPREQUEST::ReceiveResponse))
				WinHttpPostCompleteHandle (false);
			break;
		
		// Receive response succeed, go to next step
		case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
			WinHttpPostCompleteHandle (true);
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

    if (!pWinHttpRequest -> OpenRequest (L"POST", L"/", NULL, WINHTTP_NO_REFERER, 
		WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE))
		return false;
		
	if (!pWinHttpRequest -> SetOption (WINHTTP_OPTION_DISABLE_FEATURE, WINHTTP_DISABLE_REDIRECTS))
	{
		pWinHttpRequest -> CloseRequest ();
		return false;
	}
		
	if (!pWinHttpRequest -> AddHeader ({
		{L"Content-type", L"multipart/form-data; boundary=WebKitFormBoundary7MA4YWxkTrZu0gW"}
		}, WINHTTP_ADDREQ_FLAG_ADD))
	{
		pWinHttpRequest -> CloseRequest ();
		return false;
	}
		
	// Set callback
	using namespace std::placeholders;
	pWinHttpRequest -> SetRequestCallBack (std::bind (&POSTER::WinHttpStatusCallBack, this, _1, _2, _3));
    
    pWinHttpRequest -> ClearContent ();
    pWinHttpRequest -> AddContent (FORMDATA, {
		{"poster", sPoster},
		{"syntax", sSyntax},
		{"expiration", sExpiration},
		{"content", sFile}
	}, "WebKitFormBoundary7MA4YWxkTrZu0gW");
    
    // First asynchronous operation
    return pWinHttpRequest -> AsyncOperate (&WINHTTPREQUEST::SendRequest);
}

void POSTER::SetPostCompleteCallBack (std::function <void (std::wstring)> cbPostComplete)
{
	postcompleteCallBack = cbPostComplete;
}

bool POSTER::Initialize ()
{
	pWinHttpRequest = new WINHTTPREQUEST;
	
	if (!pWinHttpRequest -> Initialize (L"Paste", PASTE_SERVER, INTERNET_DEFAULT_HTTPS_PORT, true))
		return false;
		
	return true;
}

void POSTER::UnInitialize ()
{
	if (pWinHttpRequest)
	{
		delete pWinHttpRequest;
		pWinHttpRequest = nullptr;
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
	
	for (char c = 0; !fin.eof (); sFile.append (1, c))
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
			wc = (unsigned char) sFile [(size_t) i + !isBE] << 8 | (unsigned char) sFile [(size_t) i + isBE];
			wsTemp.append (1, wc);
		}
	}
    
    else if (!WINCVT::StringToWString (wsTemp, sFile, CP_ACP))
    	return false;
    
	return WINCVT::WStringToString (sFile, wsTemp, CP_UTF8);
}

void POSTER::WinHttpPostCompleteHandle (bool isSuccess)
{
	if (isSuccess)
	{	
		std::wstring wsURL;
		
		if (pWinHttpRequest -> ReadHeader (WINHTTP_QUERY_LOCATION, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX, wsURL))
		{
			wsURL = PASTE_URL + wsURL;
			
			if (postcompleteCallBack)	
				postcompleteCallBack (wsURL);

			pWinHttpRequest -> CloseRequest ();
			return;
		}
	}
	
	if (postcompleteCallBack)	
		postcompleteCallBack (L"");

	pWinHttpRequest -> CloseRequest ();
}
