#include "comboboxitem.h"

BASEITEM::BASEITEM (std::string _sName, std::string _sKey):
	sName (_sName), sKey (_sKey), uiPrefer (0)
{
}

bool BASEITEM::operator < (const BASEITEM& rhs)
{
	if (uiPrefer != rhs.uiPrefer)
		return uiPrefer > rhs.uiPrefer;
		
	return sName < rhs.sName;
}

bool BASEITEM::LoadFromIni (std::string sFilePath)
{
	if (sFilePath.empty () || !~_access (sFilePath.c_str (), 4))
		return false;

	char* pcbuffer = new char [MAX_KEYLEN];
	bool res;

	if (!(res = GetPrivateProfileStringA (sName.c_str (), PREFER_KEYNAME, "", pcbuffer, MAX_KEYLEN, sFilePath.c_str ())))
		WINAPP::WinErrorReport (__FUNCTION__, "GetPrivateProfileStringA", false);
	
	if (res)
		uiPrefer = (unsigned int) atoi (pcbuffer);
	
	delete [] pcbuffer;
	return res;
}

bool BASEITEM::SaveToIni (std::string sFilePath)
{
	if (sFilePath.empty () || !~access (sFilePath.c_str (), 2))
		return false;
	
	if (!WritePrivateProfileStringA (sName.c_str (), PREFER_KEYNAME, std::to_string (uiPrefer).c_str (), sFilePath.c_str ()))
		return WINAPP::WinErrorReport (__FUNCTION__, "WritePrivateProfileStringA", false), false;
	
	return true;
}

std::string BASEITEM::GetName () const
{
	return sName;
}

std::string BASEITEM::GetKey () const
{
	return sKey;
}
	
void BASEITEM::AddPrefer ()
{
	uiPrefer ++;
}

std::string SYNTAXITEM::sNowSuffix;

SYNTAXITEM::SYNTAXITEM (std::string _sName, std::string _sKey):
	BASEITEM (_sName, _sKey)
{
}

bool SYNTAXITEM::operator < (const SYNTAXITEM& rhs)
{
	if (!sNowSuffix.empty ())
	{
		bool bLeftHit = isSuffixIn ();
		bool bRightHit = rhs.isSuffixIn ();

		if (bLeftHit != bRightHit)
			return bLeftHit > bRightHit;	
	}
	
	return BASEITEM::operator < (rhs);
}

bool SYNTAXITEM::LoadFromIni (std::string sFilePath)
{
	if (!BASEITEM::LoadFromIni (sFilePath))	
		return false;
	
	char* pcbuffer = new char [MAX_KEYLEN];

	if (GetPrivateProfileStringA (sName.c_str (), SUFFIX_KEYNAME, "", pcbuffer, MAX_KEYLEN, sFilePath.c_str ()))
	{
		int i = 0;
		std::string sSuffix;

		while (pcbuffer [++ i] != '\0')
			if (pcbuffer [i] == '.')
			{
				vsSuffixList.push_back (sSuffix);
				sSuffix.clear ();
			}
			else
				sSuffix.append (1, pcbuffer [i]);
		
		vsSuffixList.push_back (sSuffix);
	}
	
	delete [] pcbuffer;
	return true;
}

bool SYNTAXITEM::SaveToIni (std::string sFilePath)
{
	if (!BASEITEM::SaveToIni (sFilePath))
		return false;
	
	std::string sSuffix;

	for (std::string& str: vsSuffixList)
		sSuffix += "." + str;
	
	if (!sSuffix.empty ())
		if (!WritePrivateProfileStringA (sName.c_str (), SUFFIX_KEYNAME, sSuffix.c_str (), sFilePath.c_str ()))
			return WINAPP::WinErrorReport (__FUNCTION__, "WritePrivateProfileStringA", false), false;
	
	return true;
}

bool SYNTAXITEM::isSuffixIn () const
{
	for (const std::string& str: vsSuffixList)
		if (str == sNowSuffix)
			return true;
	
	return false;
}
