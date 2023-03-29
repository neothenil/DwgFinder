#include "env.h"
#include <iostream>
#include "dbmain.h"
#include "DbReaderManager.h"
#include "CountEntityReader.h"
#include "FindGroupReader.h"
#include "SymbolTableReader.h"

int wmain(int argc, wchar_t* argv[])
{
	initLocale();
	if (argc != 2) {
		std::wcerr << _T("Usage: ") << argv[0] << _T(" <dwg-file>") << std::endl;
		return 1;
	}
	bool ok = initConfig();
	if (!ok) {
		std::wcerr << _T("Fatal error: ") << _T("failed to initialize config.") << std::endl;
		return 1;
	}
	ok = setupEnv();
	if (!ok) {
		std::wcerr << _T("Fatal error: ") << _T("failed to setup DBSDK environment.") << std::endl;
		return 1;
	}

	std::wstring dwgPath = argv[1];
	DbReaderManager dbrMgr;
	dbrMgr.registerReader(&countEntity);
	dbrMgr.registerReader(&symbolTableReader);
	dbrMgr.registerReader(&findGroup);
	auto [ok1, err1] = dbrMgr.read(argv[1]);
	if (!ok1) {
		std::wcerr << _T("Read file ") << dwgPath << _T(" failed: ") << err1 << std::endl;
		dbrMgr.releaseDatabase();
		unsetupEnv();
		return 1;
	}

#ifdef _DEBUG
	dbrMgr.printResult();
#endif

	std::wstring dbPath = AcString(gConfig["db_path"].GetString()).kTCharPtr();
	auto [ok2, err2] = dbrMgr.insertToDb(dbPath);
	if (!ok2) {
		std::wcerr << _T("Insert to database failed: ") << err2 << std::endl;
		dbrMgr.releaseDatabase();
		unsetupEnv();
		return 1;
	}
	dbrMgr.releaseDatabase();
	unsetupEnv();
	return 0;
}