#include "env.h"
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <dbmain.h>

extern "C" CDECL bool InitZwDbEnv(const ZTCHAR * pszCompanyName, const ZTCHAR * pszProductName, const ZTCHAR * pszLanguageDir, const ZTCHAR * pszLangugae, const ZTCHAR * pszAuthCode);
extern "C" CDECL bool UnInitZwDbEnv();

rapidjson::Document gConfig;


void initLocale()
{
	// Windows needs a little non-standard magic.
	constexpr char cp_utf16le[] = ".1200"; // UTF-16 little-endian locale.
	setlocale(LC_ALL, cp_utf16le);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
}

bool setupEnv()
{
	TCHAR szModulePath[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(NULL, szModulePath, MAX_PATH);

	CString strTmp = szModulePath;
	int iPos = strTmp.ReverseFind(_T('\\'));
	CString strModulePath = strTmp.Left(iPos + 1);

	CString strAuthFileName = _T("ZwActivate.lic");
	CString strAuthFullPath = strModulePath + strAuthFileName;
	return InitZwDbEnv(_T("OEMSoft"), _T("OEMZWCAD"), _T("xlator"), _T("en-US"), (LPCTSTR)strAuthFullPath);
}

bool unsetupEnv()
{
	return UnInitZwDbEnv();
}

bool initConfig()
{
	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);
	AcString configPath = currentDir;
	configPath += "\\config.json";
	CFileStatus status;
	if (!CFile::GetStatus(configPath, status) || status.m_attribute == CFile::directory) {
		return false;
	}
	CFile configFile;
	if (!configFile.Open(configPath, CFile::shareDenyWrite)) {
		return false;
	}
	ULONGLONG fileSize = configFile.GetLength();
	auto content = new char[fileSize + 1];
	configFile.Read(content, static_cast<UINT>(fileSize) + 1);
	configFile.Close();
	content[fileSize] = '\0';
	gConfig.Parse(content);
	delete[] content;
	if (gConfig.GetType() != rapidjson::kObjectType) {
		return false;
	}
	// TODO: Validate config file.
	return true;
}
