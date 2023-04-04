#pragma once

#include "DbReader.h"

class NestedBlockLevelReader : public DbReader
{
public:
	NestedBlockLevelReader();
	~NestedBlockLevelReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	unsigned int getMaxNestedLevel(AcDbObjectId objId);
	void getMaxNestedLevel(AcDbObjectId btrId, unsigned int curLevel, unsigned int* pMaxLevel);

	std::map<std::wstring, unsigned int> result;
};

extern NestedBlockLevelReader nestedBlockLevelReader;