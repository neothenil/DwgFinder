#pragma once

#include "DbReader.h"

class AssocObjectReader : public DbReader
{
public:
	AssocObjectReader();
	~AssocObjectReader() override;

	std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) override;
	const std::map<std::wstring, unsigned int>& getResult() override;

private:
	void findAssocObject(AcDbObjectId assocNetworkId);

	std::map<std::wstring, unsigned int> result;
};

extern AssocObjectReader assocObjectReader;