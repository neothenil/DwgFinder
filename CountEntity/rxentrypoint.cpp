#include "pch.h"

#include "adsmigr.h"
#include "adsdef.h"
#include "adscodes.h"
#include "acestext.h"
#include "acedads.h"
#include "helloworld.h"
#include "CountEntityCmd.h"

rapidjson::Document gConfig;

void initConfig()
{
	const AcString defaultConfig = "{\"db_path\": \"D:/test.db\", \"db_table\" : \"EntityCount\"}";
	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);
	AcString configPath = currentDir;
	configPath += "\\config.json";
	CFileStatus status;
	if (!CFile::GetStatus(configPath, status) || status.m_attribute == CFile::directory) {
		acutPrintf(_T("\nFile %s does not exist. Use default config."), configPath.kTCharPtr());
		gConfig.Parse(defaultConfig.utf8Ptr());
		return;
	}
	CFile configFile(configPath, CFile::modeRead);
	ULONGLONG fileSize = configFile.GetLength();
	auto content = new char[fileSize + 1];
	configFile.Read(content, fileSize + 1);
	configFile.Close();
	gConfig.Parse(content);
	delete[] content;
}


void initapp()
{
	initConfig();
	acedRegCmds->addCommand(cmd_group_name, _T("helloworld"), _T("helloworld"), ACRX_CMD_MODAL, helloworld);
	acedRegCmds->addCommand(cmd_group_name, _T("countEntity"), _T("countEntity"), ACRX_CMD_MODAL, countEntityCmd);
}

void unloadapp()
{
	acedRegCmds->removeGroup(cmd_group_name);
}


#define ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

struct func_entry { TCHAR *func_name; int(*func) (struct resbuf *); };

struct func_entry func_table[] = {
	{_T("showhello"), showhello},
	{_T("c:showhello"), showhellocmd},
};

int funcload()
{
	for (int i = 0; i < ELEMENTS(func_table); i++)
	{
		if (!acedDefun(func_table[i].func_name, i))
			return RTERROR;
	}

	return RTNORM;
}

int dofunc()
{
	struct resbuf* rb = NULL;
	int val = 0;

	if ((val = acedGetFunCode()) < 0 || val >= ELEMENTS(func_table))
	{
		acdbFail(_T("Received nonexistent function code."));
		return RTERROR;
	}
	rb = acedGetArgs();
	val = (*func_table[val].func)(rb);
	acutRelRb(rb);
	return val;
}


#ifdef ARX
#define zcrxEntryPoint acrxEntryPoint
#endif

extern "C" AcRx::AppRetCode zcrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
	switch (msg)
	{
		case AcRx::kInitAppMsg:
		{
			acrxDynamicLinker->unlockApplication(appId);
			acrxDynamicLinker->registerAppMDIAware(appId);
			initapp();
		}
		break;
		case AcRx::kUnloadAppMsg:
		{
			unloadapp();
		}
		break;
		
		case AcRx::kLoadDwgMsg:
		{
			funcload();
		}
		break;
		case AcRx::kUnloadDwgMsg:
		{
			// Add your code here.
		}
		break;
		case AcRx::kInvkSubrMsg:
		{
			dofunc();
		}
		break;
		
		default:
			break;
	}
	return AcRx::kRetOK;
}


#ifdef ARX
#ifdef _WIN64
#pragma comment(linker, "/export:acrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:acrxGetApiVersion,PRIVATE")
#else // WIN32
#pragma comment(linker, "/export:_acrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:_acrxGetApiVersion,PRIVATE")
#endif
#else
#ifdef _WIN64
#pragma comment(linker, "/export:zcrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:zcrxGetApiVersion,PRIVATE")
#else // WIN32
#pragma comment(linker, "/export:_zcrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:_zcrxGetApiVersion,PRIVATE")
#endif
#endif // ARX

