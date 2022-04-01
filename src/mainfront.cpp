#include "mainfront.h"

MAINFRONT::MAINFRONT ():
	hstcPoster (nullptr), hstcSyntax (nullptr), hstcExpiration (nullptr), hstcFilePath (nullptr), hstcURL (nullptr),
	hedtPoster (nullptr), hedtFilePath (nullptr), hedtURL (nullptr),
	hcbxSyntax (nullptr), hcbxExpiration (nullptr),
	hbtnPost (nullptr), hbtnRightMenu (nullptr),
	httsPoster (nullptr), httsSyntax (nullptr), httsExpiration (nullptr), httsFilePath (nullptr), httsURL (nullptr),
	postCallBack (nullptr), rightmenuCallBack (nullptr)
{	
}

bool MAINFRONT::Initialize ()
{
	using namespace std::placeholders;

	RESMANAGER* pResManager = RESMANAGER::GetInstance ();
	
	// Initialize window
	if (!pResManager -> InitWindow (&mainApp, WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES,
		"MainApp", "Ubuntu Pastebin Poster",
		WS_VISIBLE | (WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME),
		CLIENT_FULLWIDTH, CLIENT_FULLHEIGHT, nullptr, nullptr, 
		std::bind (&MAINFRONT::proxyMainProc, this, _1, _2, _3, _4)))
		return false;
		
	HWND hwnd = mainApp.GetHWND ();
	
	// Initialize static control
	if (!pResManager -> InitControl (hstcPoster, WC_STATICA, "Poster:", DEFAULT_SSTATIC, 
		20, 10, 80, 25, hwnd, INDEX_STCPOSTER, FONT_UBUNTU, 0))
		return false;
		
	if (!pResManager -> InitControl (hstcSyntax, WC_STATICA, "Syntax:", DEFAULT_SSTATIC,
		190, 10, 80, 25, hwnd, INDEX_STCSYNTAX, FONT_UBUNTU, 0))
		return false;
		
	if (!pResManager -> InitControl (hstcExpiration, WC_STATICA, "Expiration:", DEFAULT_SSTATIC,
		360, 10, 80, 25, hwnd, INDEX_STCEXPIRATION, FONT_UBUNTU, 0))
		return false;
		
	if (!pResManager -> InitControl (hstcFilePath, WC_STATICA, "File path:", DEFAULT_SSTATIC,
		20, 83, 80, 25, hwnd, INDEX_STCFILEPATH, FONT_UBUNTU, 0))
		return false;
		
	if (!pResManager -> InitControl (hstcURL, WC_STATICA, "URL:", DEFAULT_SSTATIC, 
		35, 128, 80, 25, hwnd, INDEX_STCURL, FONT_UBUNTU, 0))
		return false;
	
	// Initialize edit control
	if (!pResManager -> InitControl (hedtPoster, WC_EDITA, "", DEFAULT_SEDIT, 
		20, 35, 155, 30, hwnd, INDEX_EDTPOSTER, FONT_UBUNTU, CURSOR_IBEAM))
		return false;
		
	if (!pResManager -> InitControl (hedtFilePath, WC_EDITA, "", DEFAULT_SEDIT | ES_READONLY, 
		105, 80, 285, 30, hwnd, INDEX_EDTFILEPATH, FONT_UBUNTU, CURSOR_IBEAM))
		return false;
		
	if (!pResManager -> InitControl (hedtURL, WC_EDITA, "", DEFAULT_SEDIT | ES_READONLY, 
		105, 125, 285, 30, hwnd, INDEX_EDTURL, FONT_UBUNTU, CURSOR_IBEAM))
		return false;
		
	// Initialize combo Box control
	if (!pResManager -> InitControl (hcbxSyntax, WC_COMBOBOXA, "", DEFAULT_SCOMBOBOX | CBS_NOINTEGRALHEIGHT, 
		190, 35, 155, 300, hwnd, INDEX_CBXSYNTAX, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
		
	if (!pResManager -> InitControl (hcbxExpiration, WC_COMBOBOXA, "", DEFAULT_SCOMBOBOX, 
		360, 35, 155, 50, hwnd, INDEX_CBXEXPIRATION, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
	
	// Initialize button control
	if (!pResManager -> InitControl (hbtnPost, WC_BUTTONA, "Post", DEFAULT_SBUTTON,
		410, 80, 100, 75, hwnd, INDEX_BTNPOST, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
		
	if (!pResManager -> InitControl (hbtnRightMenu, WC_BUTTONA, "Add to the right-mouse menu (Administrator privileges required)", 
		WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 30, 165, 500, 25, hwnd, INDEX_BTNRIGHTMENU, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
		
	// Initialize tool tips
	if (!pResManager -> InitToolTips (httsPoster, hedtPoster, hwnd, DEFAULT_STOOLTIPS, 0, CURSOR_ARROW, 
		TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT, "Your name (30 characters max)"))
		return false;
		
	if (!pResManager -> InitToolTips (httsSyntax, hcbxSyntax, hwnd, DEFAULT_STOOLTIPS, 0, CURSOR_ARROW, 
		TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT, "The programming language of the file"))
		return false;
	
	if (!pResManager -> InitToolTips (httsExpiration, hcbxExpiration, hwnd, DEFAULT_STOOLTIPS, 0, CURSOR_ARROW, 
		TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT, "Duration of the URL"))
		return false;
	
	if (!pResManager -> InitToolTips (httsFilePath, hedtFilePath, hwnd, DEFAULT_STOOLTIPS, 0, CURSOR_ARROW, 
		TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT, "The path to the file you want to post"))
		return false;
	
	if (!pResManager -> InitToolTips (httsURL, hedtURL, hwnd, DEFAULT_STOOLTIPS, 0, CURSOR_ARROW, 
		TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT, "You can view your code through this URL"))
		return false;
		
	SetBoxItem ();
	return true;
}

HWND MAINFRONT::GetMainWindowHandle () const
{
	return mainApp.GetHWND ();
}

void MAINFRONT::SetBoxItem ()
{
	INIMANAGER* pIniManager = INIMANAGER::GetInstance ();
	
	const std::vector <SYNTAXITEM>& vSyntaxList = pIniManager -> GetSyntaxList ();
	const std::vector <EXPIRATIONITEM>& vExpirationList = pIniManager -> GetExpirationList ();
	
	SetWindowTextA (hedtPoster, pIniManager -> GetPosterName ().c_str ());
	
	SendMessage (hcbxSyntax, CB_RESETCONTENT, 0, 0);
	
	for (const SYNTAXITEM& item: vSyntaxList)
		SendMessage (hcbxSyntax, CB_ADDSTRING, 0, (LPARAM) item.GetName ().c_str ());
	
	SendMessage (hcbxSyntax, CB_SETCURSEL, 0, 0);
		
	SendMessage (hcbxExpiration, CB_RESETCONTENT, 0, 0);
	
	for (const EXPIRATIONITEM& item: vExpirationList)
		SendMessage (hcbxExpiration, CB_ADDSTRING, 0, (LPARAM) item.GetName ().c_str ());
		
	SendMessage (hcbxExpiration, CB_SETCURSEL, 0, 0);
}

void MAINFRONT::UpdateIni (std::string sSuffix)
{
	INIMANAGER* pIniManager = INIMANAGER::GetInstance ();
	pIniManager -> UpdateSyntaxList (sSuffix);
}

void MAINFRONT::UpdateIni (std::string sPoster, unsigned int uiSyntax, unsigned int uiExpiration)
{
	INIMANAGER* pIniManager = INIMANAGER::GetInstance ();
	
	pIniManager -> UpdatePosterName (sPoster);
	pIniManager -> UpdateSyntaxList (uiSyntax);
	pIniManager -> UpdateExpirationList (uiExpiration);
}

void MAINFRONT::SetFilePath (std::string sFilePath)
{
	SetWindowTextA (hedtFilePath, sFilePath.c_str ());
	
	std::string::size_type pos = sFilePath.find_last_of ('.');
	
	if (pos == (unsigned int) -1)
		return UpdateIni ("");
	
	std::string sSuffix (sFilePath.substr (pos + 1));
	std::transform (sSuffix.begin (), sSuffix.end (), sSuffix.begin (), ::tolower);
	UpdateIni (sSuffix);
	
	SetBoxItem ();
}

void MAINFRONT::SetFilePath (std::wstring wsFilePath)
{
	std::string sFilePath;
	
	if (WINCVT::WStringToString (sFilePath, wsFilePath, CP_ACP))
		SetFilePath (sFilePath);
	else
		SetWindowTextW (hedtFilePath, wsFilePath.c_str ());
}

void MAINFRONT::SetRightMenu (bool bRightMenu)
{
	SendMessage (hbtnRightMenu, BM_SETCHECK, bRightMenu, 0);
}

void MAINFRONT::SetPostCallBack (std::function <bool (std::string, std::string, std::string, std::string)> cbPost)
{
	postCallBack = cbPost;
}

void MAINFRONT::PostCompleteHandle (std::wstring wsURL)
{
	// Change the cursor state
#if defined (_M_X64) || defined (__amd64__)
	SetClassLongPtr (mainApp.GetHWND (), GCLP_HCURSOR, 
		(LONG_PTR) reinterpret_cast <long long> (RESMANAGER::GetInstance () -> GetCursorHandle (CURSOR_ARROW)));
#else
	SetClassLongPtr (mainApp.GetHWND (), GCL_HCURSOR, 
		(LONG_PTR) (RESMANAGER::GetInstance () -> GetCursorHandle (CURSOR_ARROW)));
#endif	

	if (!wsURL.empty ())
		CopyURLToClipboard (wsURL);
	
	// Enable all controls when post complete
	EnableAllControls ();
	
	// Change the button state  
	SetWindowTextA (hbtnPost, "Post"); 
	
	if (!wsURL.empty ())
		SetWindowTextW (hedtURL, wsURL.c_str ());
	else
		ALERT (LVERROR, "Post failed!");
}

void MAINFRONT::SetRightMenuCallBack (std::function <bool (bool)> cbRightMenu)
{
	rightmenuCallBack = cbRightMenu;
}

MAINFRONT* MAINFRONT::GetInstance ()
{
	static MAINFRONT instance;
	return &instance;
}

int MAINFRONT::EnterLoop ()
{
	int res = (int) mainApp.EnterMsgLoop ();
	ZeroAllControls ();
	return res;
}

LRESULT MAINFRONT::proxyMainProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_COMMAND:
			switch (LOWORD (wParam))                            
    		{
    			// Limit the character length of 'Poster' field to no more than 30
				case INDEX_EDTPOSTER:
					switch (HIWORD (wParam))
					{
						case EN_CHANGE:
						{
							wchar_t* pwcBuffer = new wchar_t [MAX_CHARACTER + 5];
							INIMANAGER* pIniManager = INIMANAGER::GetInstance ();
							
							GetWindowTextW (hedtPoster, pwcBuffer, MAX_CHARACTER + 5);

							if (lstrlenW (pwcBuffer) > MAX_CHARACTER)
							{
								pwcBuffer [MAX_CHARACTER] = 0;
								SetWindowTextW (hedtPoster, pwcBuffer);
								SendMessageW (hedtPoster, EM_SETSEL, MAX_CHARACTER, 0);
								TIP (LVWARN, hedtPoster, L"You can only enter 30 characters at most!");
							}
							
							std::string sPoster;
	
							if (WINCVT::WStringToString (sPoster, pwcBuffer, CP_ACP))
								pIniManager -> UpdatePosterName (sPoster);

							delete [] pwcBuffer;
							break;
						}

						default:
							return DefWindowProc (hwnd, Message, wParam, lParam);
					}
					break;   
				
				// If the 'File path' box is clicked, an Open dialog box will
				// be created that lets the user specify a file to be opened.
				case INDEX_EDTFILEPATH:
					switch (HIWORD (wParam))
					{
						case EN_SETFOCUS:
						{
							std::string sFilePath;
							
							// If it is a valid file, update 'File path' box
							if (GetChooseFilePath (sFilePath))
								SetFilePath (sFilePath);
							
							break;
						}
							
						default:
							return DefWindowProc (hwnd, Message, wParam, lParam);
					}     
					break;
				
				// If the 'URL' box is clicked, all text in it will be selected,
				// it will be convenient for user to copy.
				case INDEX_EDTURL:
					switch (HIWORD (wParam))
					{
						case EN_SETFOCUS:
						{
							SendMessageA (hedtURL, EM_SETSEL, 0, -1);
							break;
						}
						
						default:
							return DefWindowProc (hwnd, Message, wParam, lParam);
					}
					break;
					
				case INDEX_BTNRIGHTMENU:
				{
					if (rightmenuCallBack == nullptr)
						break;
					
					bool bRightMenu = SendMessage (hbtnRightMenu, BM_GETCHECK, 0, 0);
					
					if (rightmenuCallBack (!bRightMenu))
						SetRightMenu (!bRightMenu);
					
					break;
				}
				
				// If the 'Post' button is clicked, collect the form and post
				case INDEX_BTNPOST:
				{
            		if (postCallBack == nullptr)
            			break;
					
					RESMANAGER* pResManager = RESMANAGER::GetInstance ();
					INIMANAGER* pIniManager = INIMANAGER::GetInstance ();
					unsigned int uiSyntax, uiExpiration;
					std::string sPoster, sFilePath;
					std::wstring wsUrl;
					
					// Collect form
					if (!CollectForm (sPoster, uiSyntax, uiExpiration, sFilePath))
						break;
					
					// Disable all controls when posting
					DisableAllControls ();
					
					// Change the button state  
					SetWindowTextA (hbtnPost, "Posting"); 
					
					// Change the cursor state
#if defined (_M_X64) || defined (__amd64__)
					SetClassLongPtr (mainApp.GetHWND (), GCLP_HCURSOR, 
						(LONG_PTR) reinterpret_cast <long long> (pResManager -> GetCursorHandle (CURSOR_WAIT)));
#else
					SetClassLongPtr (mainApp.GetHWND (), GCL_HCURSOR, 
						(LONG_PTR) pResManager -> GetCursorHandle (CURSOR_WAIT));
#endif	

					if (!postCallBack (sPoster, 
						(pIniManager -> GetSyntaxList () [uiSyntax]).GetKey (), 
						(pIniManager -> GetExpirationList () [uiExpiration]).GetKey (), 
						sFilePath))
						PostCompleteHandle (L"");
					
					UpdateIni (sPoster, uiSyntax, uiExpiration);
            		break;  		
				}
				
				default:
					return DefWindowProc (hwnd, Message, wParam, lParam);
			}
			break;       
		
		// When a valid file is dragged in, get its path and update 'File path' edit box
		case WM_DROPFILES:
		{
			std::string sFilePath;
			
			if (GetDropFilePath (sFilePath, (HDROP) wParam))
				SetFilePath (sFilePath);
				
			break;
		}
			
		// Intercepting these messages makes the control's background transparent.
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC:
		{
			if ((HWND) lParam == hcbxSyntax || (HWND) lParam == hcbxExpiration)
				return DefWindowProc (hwnd, Message, wParam, lParam);
				
			// For checkbox, it's kind of special
			// need to return its class brush
			if ((HWND) lParam == hbtnRightMenu)
			{
				char* pcClassName = new char [100];
    			GetClassNameA (hwnd, pcClassName, 100);
    			
    			WNDCLASSA lpcls {};
    			GetClassInfoA (GetModuleHandle (0), pcClassName, &lpcls);
				
				delete [] pcClassName;
    			return (LRESULT) lpcls.hbrBackground;
			}
			
			SetBkColor ((HDC) wParam, CLIENT_BKCOLOR);
			// must return a null brush
            return (INT_PTR) GetStockObject (NULL_BRUSH);
		}
        
        // Destroy Window
		case WM_DESTROY:
			PostQuitMessage (0);
			break;
			
		default:
			return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	
	return 0;
}

bool MAINFRONT::CollectForm (std::string& sPoster, unsigned int& uiSyntax, unsigned int& uiExpiration, std::string& sFilePath)
{
	char* cbuffer = new char [MAX_PATHLEN];
	
	GetWindowTextA (hedtPoster, cbuffer, MAX_PATHLEN);
	sPoster = std::string ((const char*) cbuffer);
	
	// Check the 'Poster' field is empty or not
	if (sPoster.length () == 0)
	{
		TIP (LVWARN, hedtPoster, L"Please fill out this field!");
		delete [] cbuffer;
		return false;
	}
	
	LRESULT lres;
	
	// Get the index of syntax item				
	lres = SendMessage (hcbxSyntax, CB_GETCURSEL, 0, 0); 
	uiSyntax = (unsigned int) lres;
	
	// Get the index of expiration item				
	lres = SendMessage (hcbxExpiration, CB_GETCURSEL, 0, 0); 
	uiExpiration = (unsigned int) lres;
					
	GetWindowTextA (hedtFilePath, cbuffer, MAX_PATHLEN); 
	sFilePath = std::string ((const char*) cbuffer);
	delete [] cbuffer;
	
	// Check the 'File path' field is empty	or not
	if (sFilePath.length () == 0)
		return ALERT (LVWARN, "Please select a valid file!"), false;
	
	// Check the file is empty or not				
	ULARGE_INTEGER uliFileSize;
	memset (&uliFileSize, 0, sizeof (uliFileSize));
	uliFileSize.LowPart = GetCompressedFileSizeA (sFilePath.c_str (), &uliFileSize.HighPart);
					
	if (uliFileSize.QuadPart == 0)
		return ALERT (LVWARN, "This is an empty file!"), false;

	return true;
}

bool MAINFRONT::CopyURLToClipboard (std::wstring wsURL)
{
	if (wsURL.empty () || !OpenClipboard (nullptr))
		return false;

	EmptyClipboard ();

	size_t sLen = wsURL.size ();
	HGLOBAL hGolCopy = GlobalAlloc (GMEM_MOVEABLE, (sLen + 1) * sizeof (wchar_t));
	
	if (!hGolCopy)
	{
		CloseClipboard ();
		return false;
	}
	
	LPWSTR lpStrcpy = (LPWSTR) GlobalLock (hGolCopy);

	if (lpStrcpy == nullptr)
	{
		CloseClipboard ();
		return false;
	}

	memcpy (lpStrcpy, wsURL.c_str (), sLen * sizeof (wchar_t));
	lpStrcpy [sLen] = 0;
	
	GlobalUnlock (hGolCopy);
	SetClipboardData (CF_UNICODETEXT, hGolCopy);
	
	CloseClipboard ();
	return true;
}

bool MAINFRONT::GetChooseFilePath (std::string& sFilePath)
{
	char* cbuffer = new char [MAX_PATHLEN];
	
	OPENFILENAMEA ofn;
	memset (&ofn, 0, sizeof (ofn));
	
	ofn.lStructSize = sizeof (OPENFILENAMEA);
	ofn.lpstrFilter = "All files(*.*)\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = cbuffer;
	ofn.lpstrFile [0] = '\0';
	ofn.nMaxFile = MAX_PATHLEN;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; 
	
	// Create a dialog and let the user choose a file
	if (!GetOpenFileNameA (&ofn))
	{
		DWORD errCode = CommDlgExtendedError ();
		
		// When the user closed or canceled the dialog box, the return value is zero. 
		if (errCode)
		{
			std::string errMsg = "An error has occured.\nDialog error code: " + std::to_string (errCode);
			LOGGER (LVERROR).Report ("GetChooseFilePath - GetOpenFileNameA", errMsg.c_str ());
		}
		
		delete [] cbuffer;
		return false;
	}
	
	sFilePath = std::string ((const char*) cbuffer);
	delete [] cbuffer;
	return true;
}

bool MAINFRONT::GetDropFilePath (std::string& sFilePath, HDROP hDropInfo)
{
	char* cbuffer = new char [MAX_PATHLEN];
	
	UINT nFileCount = DragQueryFile (hDropInfo, (UINT) -1, NULL, 0);
	DWORD dwAttribute = 0;
	bool isDrop = false;
	
	if (nFileCount > 1)
		ALERT (LVWARN, "Multiple files are not currently supported, the first valid file will be read.");
		
	for (UINT i = 0; i < nFileCount; i ++)
	{
		DragQueryFileA (hDropInfo, i, cbuffer, MAX_PATHLEN);
		dwAttribute = GetFileAttributesA (cbuffer);
		
		// folder is not allowed		
		if (!(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
		{
			sFilePath = std::string ((const char*) cbuffer);
			isDrop = true;
			break;
		}
	}

	DragFinish (hDropInfo);
	delete [] cbuffer;
			
	if (!isDrop)
	{
		ALERT (LVWARN, "Please select a valid file!");
		return false;
	}
	
	return true;
}

void MAINFRONT::DisableAllControls ()
{
	// Disalbe all controls
	EnableWindow (hedtPoster, false);   
	EnableWindow (hcbxSyntax, false);   
	EnableWindow (hcbxExpiration, false);   
	EnableWindow (hedtFilePath, false);   
	EnableWindow (hedtURL, false);
	EnableWindow (hbtnPost, false);
	EnableWindow (hbtnRightMenu, false);
	EnableWindow (httsPoster, false);
	EnableWindow (httsSyntax, false);
	EnableWindow (httsExpiration, false);
	EnableWindow (httsFilePath, false);
	EnableWindow (httsURL, false);
}

void MAINFRONT::EnableAllControls ()
{
	// Enalbe all controls
	EnableWindow (hedtPoster, true);   
	EnableWindow (hcbxSyntax, true);   
	EnableWindow (hcbxExpiration, true);   
	EnableWindow (hedtFilePath, true);   
	EnableWindow (hedtURL, true);   
	EnableWindow (hbtnPost, true);
	EnableWindow (hbtnRightMenu, true);
	EnableWindow (httsPoster, true);
	EnableWindow (httsSyntax, true);
	EnableWindow (httsExpiration, true);
	EnableWindow (httsFilePath, true);
	EnableWindow (httsURL, true);
}

void MAINFRONT::ZeroAllControls ()
{
	hstcPoster = nullptr;
	hstcSyntax = nullptr;
	hstcExpiration = nullptr;
	hstcFilePath = nullptr;
	hstcURL = nullptr;
	
	hedtPoster = nullptr;
	hedtFilePath = nullptr;
	hedtURL = nullptr;
	
	hcbxSyntax = nullptr;
	hcbxExpiration = nullptr;
	
	hbtnPost = nullptr;
	hbtnRightMenu = nullptr;
	
	httsPoster = nullptr;
	httsSyntax = nullptr;
	httsExpiration = nullptr;
	httsFilePath = nullptr;
	httsURL = nullptr;
}

