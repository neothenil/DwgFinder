#include <afxwin.h>
#include <iostream>
#include <sstream>
#include "env.h"
#include "DbReaderManager.h"
#include "sqlite3/sqlite3.h"

DbReaderManager::DbReaderManager()
	: pDb(nullptr)
{
}

DbReaderManager::~DbReaderManager()
{
	if (pDb != nullptr) {
		releaseDatabase();
	}
}

void DbReaderManager::registerReader(DbReader* reader)
{
	readerList.push_back(reader);
}

std::tuple<bool, std::wstring> DbReaderManager::read(std::wstring dwgPath)
{
	std::tuple<bool, std::wstring> res;
	CFileStatus status;
	filePath = dwgPath.c_str();
	if (!CFile::GetStatus(filePath, status) || status.m_attribute == CFile::directory) {
		res = std::make_tuple(false, _T("file does not exist"));
		return res;
	}
	pDb = new AcDbDatabase(Adesk::kFalse);
	if (filePath.right(4).compareNoCase(_T(".dwg")) == 0) {
		if (Acad::eOk != pDb->readDwgFile(filePath.kTCharPtr())) {
			res = std::make_tuple(false, _T("AcDbDatabase::readDwgFile failed"));
			return res;
		}
	}
	else if (filePath.right(4).compareNoCase(_T(".dxf")) == 0) {
		if (Acad::eOk != pDb->dxfIn(filePath.kTCharPtr())) {
			res = std::make_tuple(false, _T("AcDbDatabase::dxfIn failed"));
			return res;
		}
	}
	else {
		res = std::make_tuple(false, _T("invalid file type"));
		return res;
	}

	for (auto& reader : readerList) {
		res = reader->read(pDb);
		if (!std::get<0>(res)) {
			return res;
		}
		const auto& readerResult = reader->getResult();
		for (const auto& [column, value] : readerResult) {
			if (result.find(column) == std::end(result)) {
				result[column] = value;
			}
			else {
				result[column] += value;
			}
		}
	}
	return std::make_tuple(true, _T(""));
}

void DbReaderManager::releaseDatabase()
{
	if (pDb != nullptr) {
		delete pDb;
		pDb = nullptr;
	}
}

void DbReaderManager::printResult()
{
	for (const auto& [column, value] : result) {
		if (value == 0)
			continue;
		std::wcout << column << L": " << value << std::endl;
	}
}

std::tuple<bool, std::wstring> DbReaderManager::insertToDb(std::wstring dbPath)
{
	sqlite3* db;
	char* zErrMsg = 0;
	AcString strDbPath(dbPath.c_str());
	std::tuple<bool, std::wstring> res;
	int rc;

	/* Open database */
	rc = sqlite3_open(strDbPath.utf8Ptr(), &db);

	if (rc) {
		std::wstring errorMsg = _T("can't open database: ");
		errorMsg += AcString(sqlite3_errmsg(db), AcString::Utf8).kTCharPtr();
		errorMsg += _T("\n");
		res = std::make_tuple(false, errorMsg);
		return res;
	}

	/* Create SQL statement */
	std::string sql;  // UTF8 encoded SQL statement
	std::ostringstream strColumnsBuffer("path", std::stringstream::ate);
	std::ostringstream strValuesBuffer("\"" + std::string(filePath.utf8Ptr()) + "\"", std::stringstream::ate);

	for (const auto& column : gConfig["db_columns"].GetArray()) {
		std::string utf8Str = column.GetString();
		std::wstring utf16Str = AcString(column.GetString(), AcString::Utf8).kTCharPtr();
		strColumnsBuffer << ", " << utf8Str;
		strValuesBuffer << ", " << result.at(utf16Str);
	}

	sql = std::string("INSERT INTO ") + gConfig["db_table"].GetString()
		+ " (" + strColumnsBuffer.str() + ") "
		+ std::string("VALUES (") + strValuesBuffer.str() + ");";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		std::wstring errorMsg = _T("SQL error: ");
		errorMsg += AcString(zErrMsg, AcString::Utf8).kTCharPtr();
		errorMsg += _T("\n");
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		res = std::make_tuple(false, errorMsg);
		return res;
	}
	sqlite3_close(db);
	return std::make_tuple(true, std::wstring());
}
