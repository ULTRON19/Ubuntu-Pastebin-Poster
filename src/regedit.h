#ifndef __REGEDIT__
#define __REGEDIT__

#define DEFAULT_REGKEYNAME	"Ubuntu Pastebin Poster"
#define DEFAULT_MENUTEXT	"Post by Ubuntu Pastebin Poster"

#include "erh.h"

/*
-- HKEY_CLASSES_ROOT
	-- *
		-- shell
			-- strRegKeyName	-- strMenuText
								-- Icon
				-- command		-- strApplication

*/

class REGEDIT
{
public:
	// Error report
	static void RegErrorReport (std::string mainFunc, const char* func, LSTATUS errCode);
	
	void Initialize (LPCSTR lpwsApplication);
	
	// Get / Set status
	bool GetStatus ();
	bool SetStatus (bool _bRightMenu);

private:
	bool RegCheckExist ();
	bool RegAdd ();
	bool RegDel ();
	
	std::string sRegKey, sRegSubKey, sRegIconKey;
    std::string sMenuText;
	std::string sApplication;
	
	bool bRightMenu;
};

#endif
