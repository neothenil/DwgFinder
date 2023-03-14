#include "FindGroupReader.h"

FindGroupReader findGroup;

// Empty implementation
// --------------------
FindGroupReader::FindGroupReader()
{
}

FindGroupReader::~FindGroupReader()
{
}

std::tuple<bool, std::wstring> FindGroupReader::read(AcDbDatabase* pDb)
{
	return std::make_tuple(true, _T(""));
}

const std::map<std::wstring, unsigned int>& FindGroupReader::getResult()
{
	return result;
}
