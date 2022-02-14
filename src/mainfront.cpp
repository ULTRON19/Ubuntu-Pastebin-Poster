#include "mainfront.h"

MAINFRONT::MAINFRONT ():
	hstcPoster (nullptr), hstcSyntax (nullptr), hstcExpiration (nullptr), hstcFilePath (nullptr), hstcURL (nullptr),
	hedtPoster (nullptr), hedtFilePath (nullptr), hedtURL (nullptr),
	hcbxSyntax (nullptr), hcbxExpiration (nullptr),
	hbtnPost (nullptr), hbtnRightMenu (nullptr),
	postCallBack (nullptr), rightmenuCallBack (nullptr)
{	
}

bool MAINFRONT::Initialize ()
{
	using namespace std::placeholders;

	RESMANAGER* front = RESMANAGER::GetInstance ();
	
	// Initialize window
	if (!front -> InitWindow (&mainApp, WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES,
		"MainApp", "Ubuntu Pastebin Poster",
		WS_VISIBLE | (WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME),
		CLIENT_FULLWIDTH, CLIENT_FULLHEIGHT, nullptr, nullptr, 
		std::bind (&MAINFRONT::proxyMainProc, this, _1, _2, _3, _4)))
		return false;
		
	HWND hwnd = mainApp.GetHWND ();
	
	// Initialize static control
	if (!front -> InitControl (hstcPoster, WC_STATICA, "Poster:", DEFAULT_SSTATIC, 
		20, 10, 80, 25, hwnd, INDEX_STCPOSTER, FONT_UBUNTU, 0))
		return false;
		
	if (!front -> InitControl (hstcSyntax, WC_STATICA, "Syntax:", DEFAULT_SSTATIC,
		190, 10, 80, 25, hwnd, INDEX_STCSYNTAX, FONT_UBUNTU, 0))
		return false;
		
	if (!front -> InitControl (hstcExpiration, WC_STATICA, "Expiration:", DEFAULT_SSTATIC,
		360, 10, 80, 25, hwnd, INDEX_STCEXPIRATION, FONT_UBUNTU, 0))
		return false;
		
	if (!front -> InitControl (hstcFilePath, WC_STATICA, "File path:", DEFAULT_SSTATIC,
		20, 83, 80, 25, hwnd, INDEX_STCFILEPATH, FONT_UBUNTU, 0))
		return false;
		
	if (!front -> InitControl (hstcURL, WC_STATICA, "URL:", DEFAULT_SSTATIC, 
		35, 128, 80, 25, hwnd, INDEX_STCURL, FONT_UBUNTU, 0))
		return false;
	
	// Initialize edit control
	if (!front -> InitControl (hedtPoster, WC_EDITA, "", DEFAULT_SEDIT, 
		20, 35, 155, 30, hwnd, INDEX_EDTPOSTER, FONT_UBUNTU, CURSOR_IBEAM))
		return false;
		
	if (!front -> InitControl (hedtFilePath, WC_EDITA, "", DEFAULT_SEDIT | ES_READONLY, 
		105, 80, 285, 30, hwnd, INDEX_EDTFILEPATH, FONT_UBUNTU, CURSOR_IBEAM))
		return false;
		
	if (!front -> InitControl (hedtURL, WC_EDITA, "", DEFAULT_SEDIT | ES_READONLY, 
		105, 125, 285, 30, hwnd, INDEX_EDTURL, FONT_UBUNTU, CURSOR_IBEAM))
		return false;
		
	// Initialize combo Box control
	if (!front -> InitControl (hcbxSyntax, WC_COMBOBOXA, "", DEFAULT_SCOMBOBOX | CBS_NOINTEGRALHEIGHT, 
		190, 35, 155, 300, hwnd, INDEX_CBXSYNTAX, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
		
	if (!front -> InitControl (hcbxExpiration, WC_COMBOBOXA, "", DEFAULT_SCOMBOBOX, 
		360, 35, 155, 50, hwnd, INDEX_CBXEXPIRATION, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
	
	// Initialize button control
	if (!front -> InitControl (hbtnPost, WC_BUTTONA, "Post", DEFAULT_SBUTTON,
		410, 80, 100, 75, hwnd, INDEX_BTNPOST, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
		
	if (!front -> InitControl (hbtnRightMenu, WC_BUTTONA, "Add to the right-mouse menu (Administrator privileges required)", 
		WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 30, 165, 500, 25, hwnd, INDEX_BTNRIGHTMENU, FONT_UBUNTU, CURSOR_UPARROW))
		return false;
	
	return true;
}

void MAINFRONT::SetComboBoxItem (LSTPSS& vctSyntax, LSTPSS& vctExpiration)
{
	AddComboBoxItem (hcbxSyntax, cbiSyntax, vctSyntax);
	AddComboBoxItem (hcbxExpiration, cbiExpiration, vctExpiration);
}

void MAINFRONT::SetFilePath (const std::wstring& sFilePath)
{
	SetWindowTextW (hedtFilePath, sFilePath.c_str ());
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
	
	// Enable all controls when post complete
	EnableAllControls ();
	
	// Change the button state  
	SetWindowTextA (hbtnPost, "Post"); 
	
	if (!wsURL.empty ())
		SetWindowTextW (hedtURL, wsURL.c_str ());
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

void MAINFRONT::AddComboBoxItem (HWND& hControl, COMBOBOX_ITEM& cbiControl, LSTPSS& lstControl)
{
	for (auto& pir: lstControl)
	{
		cbiControl [pir.first] = pir.second;
		SendMessage (hControl, CB_ADDSTRING, -1, (LPARAM) pir.first.c_str ());
	}
	
	SendMessage (hControl, CB_SETCURSEL, 0, 0);
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
							char* cbuffer = new char [MAX_PATHLEN];
							GetWindowTextA (hedtPoster, cbuffer, MAX_CHARACTER + 5);

							if (lstrlenA (cbuffer) > MAX_CHARACTER)
							{
								cbuffer [MAX_CHARACTER] = 0;
								SetWindowTextA (hedtPoster, cbuffer);
								SendMessageA (hedtPoster, EM_SETSEL, 0, -1);
							}

							delete [] cbuffer;
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
								SetWindowTextA (hedtFilePath, sFilePath.c_str ());
							
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
					
					RESMANAGER* front = RESMANAGER::GetInstance ();
					std::string sPoster, sSyntax, sExpiration, sFilePath;
					std::wstring wsUrl;
					
					// Collect form
					if (!CollectForm (sPoster, sSyntax, sExpiration, sFilePath))
						break;
					
					// Disable all controls when posting
					DisableAllControls ();
					
					// Change the button state  
					SetWindowTextA (hbtnPost, "Posting"); 
					
					// Change the cursor state
#if defined (_M_X64) || defined (__amd64__)
					SetClassLongPtr (mainApp.GetHWND (), GCLP_HCURSOR, 
						(LONG_PTR) reinterpret_cast <long long> (front -> GetCursorHandle (CURSOR_WAIT)));
#else
					SetClassLongPtr (mainApp.GetHWND (), GCL_HCURSOR, 
						(LONG_PTR) front -> GetCursorHandle (CURSOR_WAIT));
#endif	

					if (!postCallBack (sPoster, sSyntax, sExpiration, sFilePath))
					{
						MessageBoxA (mainApp.GetHWND (), "Post failed!", "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
						PostCompleteHandle (L"");
					}
					
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
				SetWindowText (hedtFilePath, sFilePath.c_str ());
				
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

bool MAINFRONT::CollectForm (std::string& sPoster, std::string& sSyntax, std::string& sExpiration, std::string& sFilePath)
{
	char* cbuffer = new char [MAX_PATHLEN];
	
	GetWindowTextA (hedtPoster, cbuffer, MAX_CHARACTER); 
	sPoster = std::string ((const char*) cbuffer);
	
	// Check the 'Poster' field is empty or not
	if (sPoster.length () == 0)
	{
		MessageBoxA (mainApp.GetHWND (), "Please fill out 'Poster' field!", 
			"Warning", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
		delete [] cbuffer;
		return false;
	}
	
	// Replace with the fields required for the post request				
	GetWindowTextA (hcbxSyntax, cbuffer, MAX_CHARACTER); 
	sSyntax = cbiSyntax [std::string ((const char* ) cbuffer)];
	
	// Replace with the fields required for the post request
	GetWindowTextA (hcbxExpiration, cbuffer, MAX_CHARACTER); 
	sExpiration = cbiExpiration [std::string ((const char*) cbuffer)];
					
	GetWindowTextA (hedtFilePath, cbuffer, MAX_PATHLEN); 
	sFilePath = std::string ((const char*) cbuffer);
	
	// Check the 'File path' field is empty	or not
	if (sFilePath.length () == 0)
	{
		MessageBoxA (mainApp.GetHWND (), "Please select a valid file!", 
			"Warning", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
		delete [] cbuffer;
		return false;
	}
	
	// Check the file is empty or not				
	ULARGE_INTEGER uliFileSize;
	memset (&uliFileSize, 0, sizeof (uliFileSize));
	uliFileSize.LowPart = GetCompressedFileSizeA (sFilePath.c_str (), &uliFileSize.HighPart);
					
	if (uliFileSize.QuadPart == 0)
	{
		MessageBoxA (mainApp.GetHWND (), "This is an empty file!", 
			"Warning", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
		delete [] cbuffer;
		return false;
	}
	
	delete [] cbuffer;
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
			std::string errMsg = "A error occured.\nDialog error code: " + std::to_string (errCode);
			ERRHANDLER () ("GetChooseFilePath - GetOpenFileNameA", errMsg, true);
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
		MessageBoxA (mainApp.GetHWND (), "Multiple files are not currently supported, the first valid file will be read.", 
			"Warning", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
		
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
		MessageBoxA (mainApp.GetHWND (), "Please select a valid file!", 
			"Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
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
}

