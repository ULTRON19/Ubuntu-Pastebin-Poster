#ifndef __REGISTRANT__
#define __REGISTRANT__

#define DEFAULT_REGKEYNAME	"Ubuntu Pastebin Poster"
#define DEFAULT_MENUTEXT	"Post by Ubuntu Pastebin Poster"

#include "logger.h"

/*
-- HKEY_CLASSES_ROOT
	-- *
		-- shell
			-- strRegKeyName	-- strMenuText
								-- Icon
				-- command		-- strApplication

*/

class REGISTRANT
{
public:
	void Initialize (std::string _sApplication);
	
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
