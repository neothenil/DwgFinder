#pragma once

#include "DbReader.h"

class NamedObjectDictReader: public DbReader
{
public:
	NamedObjectDictReader();
	~NamedObjectDictReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	std::map<std::wstring, unsigned int> result;
};

extern NamedObjectDictReader namedObjectDictReader;