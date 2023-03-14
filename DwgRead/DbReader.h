#pragma once

#include <afxwin.h>
#include "dbmain.h"
#include <tuple>
#include <map>

class DbReader
{
public:
	virtual ~DbReader() = default;
	virtual std::tuple<bool, std::wstring> read(AcDbDatabase* pDb) = 0;
	virtual const std::map<std::wstring, unsigned int>& getResult() = 0;
};