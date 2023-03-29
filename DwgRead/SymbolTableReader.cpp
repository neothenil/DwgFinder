#include "SymbolTableReader.h"
#include "dbsymtb.h"

// Field name definitions
const std::wstring blockTableRecordField     = L"BlockTableRecord";
const std::wstring layerTableRecordField     = L"LayerTableRecord";
const std::wstring textStyleTableRecordField = L"TextStyleTableRecord";
const std::wstring linetypeTableRecordField  = L"LinetypeTableRecord";
const std::wstring viewTableRecordField      = L"ViewTableRecord";
const std::wstring ucsTableRecordField       = L"UCSTableRecord";
const std::wstring viewportTableRecordField  = L"ViewportTableRecord";
const std::wstring regAppTableRecordField    = L"RegAppTableRecord";
const std::wstring dimStyleTableRecordField  = L"DimStyleTableRecord";

SymbolTableReader symbolTableReader;

SymbolTableReader::SymbolTableReader()
{
	result[blockTableRecordField] = 0;
	result[layerTableRecordField] = 0;
	result[textStyleTableRecordField] = 0;
	result[linetypeTableRecordField] = 0;
	result[viewTableRecordField] = 0;
	result[ucsTableRecordField] = 0;
	result[viewportTableRecordField] = 0;
	result[regAppTableRecordField] = 0;
	result[dimStyleTableRecordField] = 0;
}

SymbolTableReader::~SymbolTableReader()
{
}

std::tuple<bool, std::wstring> SymbolTableReader::read(AcDbDatabase* pDb)
{
	std::tuple<bool, std::wstring> res;

	// Block table
	AcDbBlockTable* pBlockTable = nullptr;
	Acad::ErrorStatus es = pDb->getBlockTable(pBlockTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get block table"));
		return res;
	}
	if (!scanOneTable(pBlockTable, blockTableRecordField)) {
		pBlockTable->close();
		res = std::make_tuple(false, _T("failed to scan block table"));
		return res;
	}
	pBlockTable->close();
	pBlockTable = nullptr;

	// Layer table
	AcDbLayerTable* pLayerTable = nullptr;
	es = pDb->getLayerTable(pLayerTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get layer table"));
		return res;
	}
	if (!scanOneTable(pLayerTable, layerTableRecordField)) {
		pLayerTable->close();
		res = std::make_tuple(false, _T("failed to scan layer table"));
		return res;
	}
	pLayerTable->close();
	pLayerTable = nullptr;

	// Text style table
	AcDbTextStyleTable* pTextStyleTable = nullptr;
	es = pDb->getTextStyleTable(pTextStyleTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get text style table"));
		return res;
	}
	if (!scanOneTable(pTextStyleTable, textStyleTableRecordField)) {
		pTextStyleTable->close();
		res = std::make_tuple(false, _T("failed to scan text style table"));
		return res;
	}
	pTextStyleTable->close();
	pTextStyleTable = nullptr;

	// Line type table
	AcDbLinetypeTable* pLineTypeTable = nullptr;
	es = pDb->getLinetypeTable(pLineTypeTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get line type table"));
		return res;
	}
	if (!scanOneTable(pLineTypeTable, linetypeTableRecordField)) {
		pLineTypeTable->close();
		res = std::make_tuple(false, _T("failed to scan line type table"));
		return res;
	}
	pLineTypeTable->close();
	pLineTypeTable = nullptr;

	// View table
	AcDbViewTable* pViewTable = nullptr;
	es = pDb->getViewTable(pViewTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get view table"));
		return res;
	}
	if (!scanOneTable(pViewTable, viewTableRecordField)) {
		pViewTable->close();
		res = std::make_tuple(false, _T("failed to scan view table"));
		return res;
	}
	pViewTable->close();
	pViewTable = nullptr;

	// UCS table
	AcDbUCSTable* pUCSTable = nullptr;
	es = pDb->getUCSTable(pUCSTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get ucs table"));
		return res;
	}
	if (!scanOneTable(pUCSTable, ucsTableRecordField)) {
		pUCSTable->close();
		res = std::make_tuple(false, _T("failed to scan ucs table"));
		return res;
	}
	pUCSTable->close();
	pUCSTable = nullptr;

	// Viewport table
	AcDbViewportTable* pViewportTable = nullptr;
	es = pDb->getViewportTable(pViewportTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get viewport table"));
		return res;
	}
	if (!scanOneTable(pViewportTable, viewportTableRecordField)) {
		pViewportTable->close();
		res = std::make_tuple(false, _T("failed to scan viewport table"));
		return res;
	}
	pViewportTable->close();
	pViewportTable = nullptr;

	// RegApp table
	AcDbRegAppTable* pRegAppTable = nullptr;
	es = pDb->getRegAppTable(pRegAppTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get regapp table"));
		return res;
	}
	if (!scanOneTable(pRegAppTable, regAppTableRecordField)) {
		pRegAppTable->close();
		res = std::make_tuple(false, _T("failed to scan regapp table"));
		return res;
	}
	pRegAppTable->close();
	pRegAppTable = nullptr;

	// Dimension style table
	AcDbDimStyleTable* pDimStyleTable = nullptr;
	es = pDb->getDimStyleTable(pDimStyleTable, AcDb::kForRead);
	if (es != Acad::eOk) {
		res = std::make_tuple(false, _T("failed to get dim style table"));
		return res;
	}
	if (!scanOneTable(pDimStyleTable, dimStyleTableRecordField)) {
		pDimStyleTable->close();
		res = std::make_tuple(false, _T("failed to scan dim style table"));
		return res;
	}
	pDimStyleTable->close();
	pDimStyleTable = nullptr;

	return std::make_tuple(true, _T(""));
}

const std::map<std::wstring, unsigned int>& SymbolTableReader::getResult()
{
	return result;
}

bool SymbolTableReader::scanOneTable(const AcDbSymbolTable* pSymTable, std::wstring fieldName)
{
	AcDbSymbolTableIterator* pSymTableIter = nullptr;
	Acad::ErrorStatus es = pSymTable->newIterator(pSymTableIter);
	if (es != Acad::eOk) {
		return false;
	}
	if (pSymTable->isKindOf(AcDbLayerTable::desc())) {
		static_cast<ZcDbLayerTableIterator*>(pSymTableIter)->setSkipHidden(false);
	}
	unsigned int count = 0;
	for (; !pSymTableIter->done(); pSymTableIter->step()) {
		count += 1;
	}
	delete pSymTableIter;
	result[fieldName] = count;
	return true;
}
