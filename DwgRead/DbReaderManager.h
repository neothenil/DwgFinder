#pragma once

#include "DbReader.h"
#include <vector>
#include <tuple>
#include <map>

class DbReaderManager
{
public:
	DbReaderManager();
	~DbReaderManager();

	void registerReader(DbReader *reader);
	std::tuple<bool, std::wstring> read(std::wstring dwgPath);
	void releaseDatabase();
	void printResult();
	std::tuple<bool, std::wstring> insertToDb(std::wstring dbPath);

private:
	AcDbDatabase* pDb;
	AcString filePath;
	std::vector<DbReader*> readerList;
	std::map<std::wstring, unsigned int> result;
};