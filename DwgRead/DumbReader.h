#pragma once

#include "DbReader.h"

class DumbReader : public DbReader
{
public:
	DumbReader();
	~DumbReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	std::map<std::wstring, unsigned int> result;
};

extern DumbReader dumbReader;