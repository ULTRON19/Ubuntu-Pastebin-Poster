#include "inimanager.h"

INIMANAGER* INIMANAGER::GetInstance ()
{
	static INIMANAGER instance;
	return &instance;
}

bool INIMANAGER::Initialize (std::string sFolderPath)
{
	sFileName = sFolderPath + DEFAULT_ININAME;
	
	if (!~access (sFileName.c_str (), 4))
		if (!ExtractIniFromResource ())
			return false;
		
	bool res = LoadPosterNameFromIni ();
	
	for (const std::pair <std::string, std::string>& pir: psSyntaxPairList)
	{
		vSyntaxList.emplace_back (pir.first, pir.second);
		
		if (!vSyntaxList.back ().LoadFromIni (sFileName))
			res = false;
	}
	
	std::sort (vSyntaxList.begin (), vSyntaxList.end ());
	
	for (const std::pair <std::string, std::string>& pir: psExpirationPairList)
	{
		vExpirationList.emplace_back (pir.first, pir.second);
		
		if (!vExpirationList.back ().LoadFromIni (sFileName))
			res = false;
	}
	
	std::sort (vExpirationList.begin (), vExpirationList.end ());
	return res;
}

bool INIMANAGER::Save ()
{
	if (!~access (sFileName.c_str (), 2))
		return false;

	bool res = SavePosterNameToIni ();

	for (SYNTAXITEM& pir: vSyntaxList)
		if (!pir.SaveToIni (sFileName))
			res = false;
			
	for (EXPIRATIONITEM& pir: vExpirationList)
		if (!pir.SaveToIni (sFileName))
			res = false;
	
	return res;
}

std::string INIMANAGER::GetPosterName () const
{
	return sPosterName;
}

const std::vector <SYNTAXITEM>& INIMANAGER::GetSyntaxList () const
{
	return vSyntaxList;
}

const std::vector <EXPIRATIONITEM>& INIMANAGER::GetExpirationList () const
{
	return vExpirationList;
}

void INIMANAGER::UpdatePosterName (std::string _sPosterName)
{
	sPosterName = _sPosterName;
}

void INIMANAGER::UpdateSyntaxList (std::string sSuffix)
{
	if (SYNTAXITEM::sNowSuffix == sSuffix)
		return;
	
	SYNTAXITEM::sNowSuffix = sSuffix;
	std::sort (vSyntaxList.begin (), vSyntaxList.end ());
}

bool INIMANAGER::UpdateSyntaxList (unsigned int idx)
{
	if (idx < 0 || vSyntaxList.size () <= idx)
		return false;
	
	vSyntaxList [idx].AddPrefer ();
	std::sort (vSyntaxList.begin (), vSyntaxList.end ());
	
	return true;
}

bool INIMANAGER::UpdateExpirationList (unsigned int idx)
{
	if (idx < 0 || vExpirationList.size () <= idx)
		return false;
	
	vExpirationList [idx].AddPrefer ();
	std::sort (vExpirationList.begin (), vExpirationList.end ());
	
	return true;
}

bool INIMANAGER::ExtractIniFromResource ()
{
	RESMANAGER* pResManager = RESMANAGER::GetInstance ();
	WINRES* prConfiguration = pResManager -> GetResourcePointer (INI_CONFIGURATION);
	return prConfiguration -> ExtractToLocal (sFileName.c_str ());
}

bool INIMANAGER::LoadPosterNameFromIni ()
{
	char* pcBuffer = new char [MAX_KEYLEN];
	bool res = true;
	
	if (GetPrivateProfileStringA ("Poster", "name", "", pcBuffer, MAX_KEYLEN, sFileName.c_str ()))
		sPosterName = std::string (pcBuffer);
	else
	{
		DWORD dwSize = MAX_KEYLEN;
		
		if (GetUserNameA (pcBuffer, &dwSize))
			sPosterName = std::string (pcBuffer);
		else
			res = false;
	}
	
	if (sPosterName.length () > 30)
		sPosterName.resize (30);
	
	delete [] pcBuffer;
	return res;
}

bool INIMANAGER::SavePosterNameToIni ()
{
	if (!sPosterName.empty ())
		if (!WritePrivateProfileStringA ("Poster", "name", sPosterName.c_str (), sFileName.c_str ()))
			return WINAPP::WinErrorReport (__FUNCTION__, "WritePrivateProfileStringA", false), false;
			
	return true;
}
