#pragma once

#include <map>
#include <set>
#include <string>

class CountEntityResult
{
public:
	CountEntityResult();
	CountEntityResult(const std::string& utf8Path);
	~CountEntityResult() = default;

	void setFilePath(const std::string& utf8Path);
	std::string filePath() const;

	void finalize();  // 根据类型的继承关系重新统计父类的个数
	void addOne(const std::string& className);
	void print() const;
	void insertToDb(const std::string& dbPath) const;

private:
	bool hasSubClass(const std::string& className) const;
	UINT calcSuperclassCount(const std::string& className);

	std::map<std::string, UINT> mCounter;
	std::map<std::string, UINT> mCache;  // used in finalization stage
	std::set<std::string> mUncountedClasses;
	std::string mFilePath;  // UTF8-encoded file path
};
