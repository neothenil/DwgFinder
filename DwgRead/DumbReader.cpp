#include "DumbReader.h"

DumbReader dumbReader;

// Empty implementation
// --------------------
DumbReader::DumbReader()
{
}

DumbReader::~DumbReader()
{
}

std::tuple<bool, std::wstring> DumbReader::read(AcDbDatabase* pDb)
{
	return std::make_tuple(true, _T(""));
}

const std::map<std::wstring, unsigned int>& DumbReader::getResult()
{
	return result;
}
