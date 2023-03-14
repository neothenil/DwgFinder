#pragma once

#include "DbReader.h"
#include <tuple>

class FindGroupReader : public DbReader
{
public:
	FindGroupReader();
	~FindGroupReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	std::map<std::wstring, unsigned int> result;
};

extern FindGroupReader findGroup;