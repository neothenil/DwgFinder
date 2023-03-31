#pragma once

#include "DbReader.h"

class DynBlockReader : public DbReader
{
public:
	DynBlockReader();
	~DynBlockReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	std::map<std::wstring, unsigned int> result;
};

extern DynBlockReader dynBlockReader;