#pragma once

#include "DbReader.h"
#include <tuple>
#include <set>

class CountEntityReader : public DbReader
{
public:
	CountEntityReader();
	~CountEntityReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	void finalize();  // �������͵ļ̳й�ϵ����ͳ�Ƹ���ĸ���
	void addOne(const std::wstring& className);
	bool hasSubClass(const std::wstring& className) const;
	unsigned int calcSuperclassCount(const std::wstring& className);

private:
	std::map<std::wstring, unsigned int> result;
	std::map<std::wstring, unsigned int> cache;
	unsigned int numCustomEntities;
};

extern CountEntityReader countEntity;