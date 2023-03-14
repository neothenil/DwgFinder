#include "pch.h"
#include "CountEntityResult.h"
#include "classInfo.h"
#include "sqlite3/sqlite3.h"
#include <algorithm>
#include <sstream>

CountEntityResult::CountEntityResult()
{
	for (const auto& className : gClassNames) {
		mCounter[className] = 0;
	}
}

CountEntityResult::CountEntityResult(const std::string& utf8Path):
	CountEntityResult()
{
	setFilePath(utf8Path);
}

void CountEntityResult::setFilePath(const std::string& utf8Path)
{
	mFilePath = utf8Path;
}

std::string CountEntityResult::filePath() const
{
	return mFilePath;
}

void CountEntityResult::finalize()
{
	for (const auto& className : gClassNames) {
		if (!hasSubClass(className))
			continue;
		if (mCache.find(className) != std::end(mCache)) {
			mCounter[className] = mCache[className];
			continue;
		}
		mCounter[className] = calcSuperclassCount(className);
	}
}

void CountEntityResult::addOne(const std::string& className)
{
	if (mCounter.find(className) == std::end(mCounter)) {
		mUncountedClasses.insert(className);
		return;
	}
	mCounter[className] += 1;
}

void CountEntityResult::print() const
{
	for (const auto& className : gClassNames) {
		if (mCounter.at(className)) {
			acutPrintf(_T("\n%s: %d"), AcString(className.c_str(), AcString::Utf8).kTCharPtr(), mCounter.at(className));
		}
	}
	if (!mUncountedClasses.empty()) {
		acutPrintf(_T("\nUncounted Entities:"));
		for (const auto& className : mUncountedClasses)
			acutPrintf(_T("\n%s"), AcString(className.c_str(), AcString::Utf8).kTCharPtr());
	}
}

void CountEntityResult::insertToDb(const std::string& dbPath) const
{
	sqlite3* db;
	char* zErrMsg = 0;
	int rc;

	/* Open database */
	rc = sqlite3_open(dbPath.c_str(), &db);

	if (rc) {
		acutPrintf(_T("\nCan't open database: %s"), AcString(sqlite3_errmsg(db), AcString::Utf8).kTCharPtr());
		return;
	}

	/* Create SQL statement */
	std::string sql;
	std::ostringstream strColumnsBuffer("path", std::stringstream::ate);
	std::ostringstream strValuesBuffer("\"" + mFilePath + "\"", std::stringstream::ate);

	for (const auto& className : gClassNames) {
		strColumnsBuffer << ", " << className;
		strValuesBuffer << ", " << mCounter.at(className);
	}

	sql = std::string("INSERT INTO ") + gConfig["db_table"].GetString()
		+ " (" + strColumnsBuffer.str() + ") "
		+ std::string("VALUES (") + strValuesBuffer.str() + ");";

	//acutPrintf(_T("\nSQL string: %s"), AcString(sql.c_str(), AcString::Utf8).kTCharPtr());

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		acutPrintf(_T("\nSQL error: %s"), AcString(zErrMsg, AcString::Utf8).kTCharPtr());
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
}

bool CountEntityResult::hasSubClass(const std::string& className) const
{
	return gSubclassMap.find(className) != std::cend(gSubclassMap);
}

UINT CountEntityResult::calcSuperclassCount(const std::string& className)
{
	if (!hasSubClass(className))
		return mCounter[className];

	// already calculated before
	if (mCache.find(className) != std::end(mCache))
		return mCache[className];

	UINT selfCount = mCounter[className];
	auto& subclasses = gSubclassMap.at(className);
	for (const auto& subclassName : subclasses) {
		selfCount += calcSuperclassCount(subclassName);
	}
	mCache[className] = selfCount;
	return selfCount;
}
