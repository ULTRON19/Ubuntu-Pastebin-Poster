#ifndef __COMBOBOXITEM__
#define __COMBOBOXITEM__

#define PREFER_KEYNAME	"prefer"
#define SUFFIX_KEYNAME	"suffix"

#define MAX_KEYLEN	256

#include <vector>
#include "logger.h"

class BASEITEM
{
public:
	BASEITEM (std::string _sName, std::string _sKey);
	bool operator < (const BASEITEM& rhs);
	
	bool LoadFromIni (std::string sFilePath); 
	bool SaveToIni (std::string sFilePath);
	
	std::string GetName () const;
	std::string GetKey () const;
	
	void AddPrefer ();

protected:
	std::string sName;
	std::string sKey;
	unsigned int uiPrefer;
};

class SYNTAXITEM: public BASEITEM
{
public:
	SYNTAXITEM (std::string _sName, std::string _sKey);
	bool operator < (const SYNTAXITEM& rhs);
	
	bool LoadFromIni (std::string sFilePath);
	bool SaveToIni (std::string sFilePath);
	
	static std::string sNowSuffix;

private:
	bool isSuffixIn () const;
	
	std::vector <std::string> vsSuffixList;
};

typedef BASEITEM EXPIRATIONITEM;

#endif
