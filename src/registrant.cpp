#include "registrant.h"

void REGISTRANT::Initialize (std::string _sApplication)
{
	sRegKey = std::string ("*\\shell\\") + DEFAULT_REGKEYNAME;
    sRegSubKey = std::string ("*\\shell\\") + DEFAULT_REGKEYNAME + "\\command";
    sRegIconKey = "Icon";
    sMenuText = DEFAULT_MENUTEXT;
    sApplication = _sApplication;
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
			REGREPORT ("RegOpenKeyEx", lResult);
		
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
        return REGREPORT ("RegCreateKeyExA", lResult), false;
    
    // Set value
    lResult = RegSetValueExA (hKey, NULL, 0, REG_SZ, (BYTE*) sMenuText.c_str (), (DWORD) sMenuText.length ());
    
    if (lResult != ERROR_SUCCESS)
    {
    	REGREPORT ("RegSetValueExA", lResult);
    	RegCloseKey (hKey);
		return false;
	}
    
    lResult = RegSetValueExA (hKey, sRegIconKey.c_str (), 0, REG_SZ, (BYTE*) sApplication.c_str (), (DWORD) sApplication.length ());
    
    if (lResult != ERROR_SUCCESS)
    {
    	REGREPORT ("RegSetValueExA", lResult);
    	RegCloseKey (hKey);
		return false;
	}
        
    RegCloseKey (hKey);
    
    // Create sub key
    lResult = RegCreateKeyExA (HKEY_CLASSES_ROOT, sRegSubKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, 
		KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, NULL, &hKey, &dwPos);
		
	if (lResult != ERROR_SUCCESS)
        return REGREPORT ("RegCreateKeyExA", lResult), false;
    
    std::string sAppCommand = sApplication + " \"%1\"";
    lResult = RegSetValueExA (hKey, NULL, 0, REG_SZ, (BYTE*) sAppCommand.c_str(), (DWORD) sAppCommand.length ());
    
    if (lResult != ERROR_SUCCESS)
    {
    	REGREPORT ("RegSetValueEx", lResult);
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
        return REGREPORT ("RegOpenKeyExA", lResult), false;
    
#if defined (_M_X64) || defined (__amd64__)
	lResult = RegDeleteKeyExA (hKey, "command", KEY_WOW64_64KEY, 0);
#else
	lResult = RegDeleteKeyA (hKey, "command");
#endif
    
    if (lResult != ERROR_SUCCESS)
    {
    	REGREPORT ("RegDeleteKeyA", lResult);
    	RegCloseKey (hKey);
        return false;
	}
    
    RegCloseKey (hKey);
    lResult = RegOpenKeyExA (HKEY_CLASSES_ROOT, "*\\shell", 0, KEY_READ, &hKey);
    
    if (lResult != ERROR_SUCCESS)
        return REGREPORT ("RegOpenKeyExA", lResult), false;
        
#if defined (_M_X64) || defined (__amd64__)
	lResult = RegDeleteKeyExA (hKey, DEFAULT_REGKEYNAME, KEY_WOW64_64KEY, 0);
#else
	lResult = RegDeleteKeyA (hKey, DEFAULT_REGKEYNAME);
#endif
    
    if (lResult != ERROR_SUCCESS)
    {
    	REGREPORT ("RegDeleteKeyA", lResult);
    	RegCloseKey (hKey);
        return false;
	}
        
    RegCloseKey (hKey);  
    return true;
}

