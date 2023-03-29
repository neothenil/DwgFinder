#pragma once

#include "DbReader.h"

class SymbolTableReader: public DbReader
{
public:
	SymbolTableReader();
	~SymbolTableReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	bool scanOneTable(const AcDbSymbolTable* pSymTable, std::wstring fieldName);

private:
	std::map<std::wstring, unsigned int> result;
};

extern SymbolTableReader symbolTableReader;