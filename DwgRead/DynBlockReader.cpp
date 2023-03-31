#include "DynBlockReader.h"
#include "dbdynblk.h"

// Field name definitions
const std::wstring dynBlockDefinitionField = L"DynBlockDefinition";
const std::wstring dynBlockReferenceField  = L"DynBlockReference";

DynBlockReader dynBlockReader;

DynBlockReader::DynBlockReader()
{
	result[dynBlockDefinitionField] = 0;
	result[dynBlockReferenceField] = 0;
}

DynBlockReader::~DynBlockReader()
{
}

std::tuple<bool, std::wstring> DynBlockReader::read(AcDbDatabase* pDb)
{
	auto res = std::make_tuple(true, _T(""));
    // 打开块表
    AcDbBlockTable* pBlockTable = nullptr;
    if (Acad::eOk != pDb->getBlockTable(pBlockTable, AcDb::kForRead) || !pBlockTable) {
        res = std::make_tuple(false, _T("failed to get block table"));
        return res;
    }
    AcDbBlockTableIterator* pBtIter = nullptr;
    pBlockTable->newIterator(pBtIter);
    for (; !pBtIter->done(); pBtIter->step()) {
        AcDbObjectId btrId;
        pBtIter->getRecordId(btrId);
        if (!AcDbDynBlockReference::isDynamicBlock(btrId)) {
            continue;
        }
        result[dynBlockDefinitionField] += 1;
        unsigned int numReference = 0;
        AcDbDynBlockTableRecord dynBtr(btrId);
        AcDbObjectIdArray anonymousIds;
        // getAnonymousBlockIds is not implemented yet.
        if (Acad::eOk != dynBtr.getAnonymousBlockIds(anonymousIds)) {
            continue;
        }
        for (const auto& objectId : anonymousIds) {
            // 打开块表记录
            AcDbBlockTableRecord* pBtr = nullptr;
            Acad::ErrorStatus es = zcdbOpenObject(pBtr, objectId);
            if (es != Acad::eOk) {
                continue;
            }
            AcDbObjectIdArray referenceIds;
            pBtr->getBlockReferenceIds(referenceIds);
            numReference += referenceIds.length();
            pBtr->close();
        }
        result[dynBlockReferenceField] += numReference;
    }
    delete pBtIter;
    pBlockTable->close();
	return res;
}

const std::map<std::wstring, unsigned int>& DynBlockReader::getResult()
{
	return result;
}
