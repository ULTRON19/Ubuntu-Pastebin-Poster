#include "registrant.h"

void REGISTRANT::RegErrorReport (std::string mainFunc, const char* func, LSTATUS errCode)
{
	ERRHANDLER ErrorReport;
	char msgbuf [256] = {0};

	// Convert error codes to messages
	FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD) errCode,
					MAKELANGID (LANG_ENGLISH, SUBLANG_DEFAULT), msgbuf, sizeof (msgbuf), NULL);

	// Improve error reporting
	std::string errMsg (msgbuf);
	errMsg += "Reg Error Code: " + std::to_string (errCode);
	
	(mainFunc += " - ") += func;
	ErrorReport (mainFunc, errMsg, false);
	
	// Warn user
	std::string errAlert = "An error has occurred. You may need to run this program with administrator privileges. ";
	errAlert += "Reg Error Code: " + std::to_string (errCode);
	MessageBoxA (NULL, errAlert.c_str (), "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
}

void REGISTRANT::Initialize (LPCSTR lpwsApplication)
{
	sRegKey = std::string ("*\\shell\\") + DEFAULT_REGKEYNAME;
    sRegSubKey = std::string ("*\\shell\\") + DEFAULT_REGKEYNAME + "\\command";
    sRegIconKey = "Icon";
    sMenuText = DEFAULT_MENUTEXT;
    sApplication = lpwsApplication;
    bRightMenu = RegCheckExist ();
}

bool REGISTRANT::GetStatus ()
{
	return bRightMenu;
}

bool REGISTRANT::SetStatus (bool _bRightMenu)
{
	if (bRightMenu == _bRightMenu)
		return true;
	
	bool res = _bRightMenu ? RegAdd () : RegDel ();
	
	if (res)
		bRightMenu = _bRightMenu;
	
	return res;
}

bool REGISTRANT::RegCheckExist ()
{
	HKEY hKey;
	
	LSTATUS lResult = RegOpenKeyExA (HKEY_CLASSES_ROOT, sRegKey.c_str (), 0, KEY_READ, &hKey);
	
	if (lResult != ERROR_SUCCESS)
	{
		if (lResult != ERROR_FILE_NOT_FOUND)
			RegErrorReport (__FUNCTION__, "RegOpenKeyEx", lResult);
		
		return false;
	}
	
	RegCloseKey (hKey);
	return true;
}

bool REGISTRANT::RegAdd ()
{
	if (sApplication.empty ())
		return false;
	
	if (RegCheckExist ())
		return true;
	
	HKEY hKey;
    LSTATUS lResult = 0;
	DWORD dwPos = 0;
    
    // Create main key
    lResult = RegCreateKeyExA (HKEY_CLASSES_ROOT, sRegKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, 
		KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, NULL, &hKey, &dwPos);
		
	if (lResult != ERROR_SUCCESS)
        return RegErrorReport (__FUNCTION__, "RegCreateKeyExA", lResult), false;
    
    // Set value
    lResult = RegSetValueExA (hKey, NULL, 0, REG_SZ, (BYTE*) sMenuText.c_str (), (DWORD) sMenuText.length ());
    
    if (lResult != ERROR_SUCCESS)
    {
    	RegErrorReport (__FUNCTION__, "RegSetValueExA", lResult);
    	RegCloseKey (hKey);
		return false;
	}
    
    lResult = RegSetValueExA (hKey, sRegIconKey.c_str (), 0, REG_SZ, (BYTE*) sApplication.c_str (), (DWORD) sApplication.length ());
    
    if (lResult != ERROR_SUCCESS)
    {
    	RegErrorReport (__FUNCTION__, "RegSetValueExA", lResult);
    	RegCloseKey (hKey);
		return false;
	}
        
    RegCloseKey (hKey);
    
    // Create sub key
    lResult = RegCreateKeyExA (HKEY_CLASSES_ROOT, sRegSubKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, 
		KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, NULL, &hKey, &dwPos);
		
	if (lResult != ERROR_SUCCESS)
        return RegErrorReport (__FUNCTION__, "RegCreateKeyExA", lResult), false;
    
    std::string sAppCommand = sApplication + " \"%1\"";
    lResult = RegSetValueExA (hKey, NULL, 0, REG_SZ, (BYTE*) sAppCommand.c_str(), (DWORD) sAppCommand.length ());
    
    if (lResult != ERROR_SUCCESS)
    {
    	RegErrorReport (__FUNCTION__, "RegSetValueEx", lResult);
    	RegCloseKey (hKey);
		return false;
	}
        
    RegCloseKey (hKey);
    return true;
}

bool REGISTRANT::RegDel ()
{
	if (!RegCheckExist ())
		return true;
	
	HKEY hKey;
    LSTATUS lResult = 0;
    
    lResult = RegOpenKeyExA (HKEY_CLASSES_ROOT, sRegKey.c_str (), 0, KEY_READ, &hKey);
    
    if (lResult != ERROR_SUCCESS)
        return RegErrorReport (__FUNCTION__, "RegOpenKeyExA", lResult), false;
    
#if defined (_M_X64) || defined (__amd64__)
	lResult = RegDeleteKeyExA (hKey, "command", KEY_WOW64_64KEY, 0);
#else
	lResult = RegDeleteKeyA (hKey, "command");
#endif
    
    if (lResult != ERROR_SUCCESS)
    {
    	RegErrorReport (__FUNCTION__, "RegDeleteKeyA", lResult);
    	RegCloseKey (hKey);
        return false;
	}
    
    RegCloseKey (hKey);
    lResult = RegOpenKeyExA (HKEY_CLASSES_ROOT, "*\\shell", 0, KEY_READ, &hKey);
    
    if (lResult != ERROR_SUCCESS)
        return RegErrorReport (__FUNCTION__, "RegOpenKeyExA", lResult), false;
        
#if defined (_M_X64) || defined (__amd64__)
	lResult = RegDeleteKeyExA (hKey, DEFAULT_REGKEYNAME, KEY_WOW64_64KEY, 0);
#else
	lResult = RegDeleteKeyA (hKey, DEFAULT_REGKEYNAME);
#endif
    
    if (lResult != ERROR_SUCCESS)
    {
    	RegErrorReport (__FUNCTION__, "RegDeleteKeyA", lResult);
    	RegCloseKey (hKey);
        return false;
	}
        
    RegCloseKey (hKey);  
    return true;
}

