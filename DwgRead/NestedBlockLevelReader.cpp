#include "NestedBlockLevelReader.h"
#include "dbents.h"

// Field name definitions
const std::wstring maxNestedBlockLevelField = L"MaxNestedBlockLevel";

NestedBlockLevelReader nestedBlockLevelReader;

NestedBlockLevelReader::NestedBlockLevelReader()
{
	result[maxNestedBlockLevelField] = 0;
}

NestedBlockLevelReader::~NestedBlockLevelReader()
{
}

std::tuple<bool, std::wstring> NestedBlockLevelReader::read(AcDbDatabase* pDb)
{
    std::tuple<bool, std::wstring> res = std::make_tuple(true, _T(""));
    unsigned int maxNestedLevel = 0;
    // ´ò¿ª¿é±í
    AcDbBlockTable* pBlockTable = nullptr;
    if (Acad::eOk != pDb->getBlockTable(pBlockTable, AcDb::kForRead) || !pBlockTable) {
        res = std::make_tuple(false, _T("failed to get block table"));
        return res;
    }
    AcDbBlockTableRecord* pModelSpace = nullptr;
    if (Acad::eOk != pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace)) {
        pBlockTable->close();
        res = std::make_tuple(false, _T("failed to get model space"));
        return res;
    }
    pBlockTable->close();
    AcDbBlockTableRecordIterator* pBtrIter = nullptr;
    pModelSpace->newIterator(pBtrIter);
    for (; !pBtrIter->done(); pBtrIter->step()) {
        AcDbObjectId objectId;
        if (Acad::eOk != pBtrIter->getEntityId(objectId) || objectId == AcDbObjectId::kNull) {
            continue;
        }
        AcRxClass* objectClass = objectId.objectClass();
        if (objectClass != AcDbBlockReference::desc()) {
            continue;
        }
        unsigned int curMaxNestedLevel = getMaxNestedLevel(objectId);
        if (curMaxNestedLevel > maxNestedLevel)
            maxNestedLevel = curMaxNestedLevel;
    }
    result[maxNestedBlockLevelField] = maxNestedLevel;
    delete pBtrIter;
    pModelSpace->close();
	return res;
}

const std::map<std::wstring, unsigned int>& NestedBlockLevelReader::getResult()
{
	return result;
}

unsigned int NestedBlockLevelReader::getMaxNestedLevel(AcDbObjectId objId)
{
    unsigned int maxNestedLevel = 0;
    getMaxNestedLevel(objId, 1, &maxNestedLevel);
    return maxNestedLevel;
}

void NestedBlockLevelReader::getMaxNestedLevel(AcDbObjectId objId, unsigned int curLevel, unsigned int* pMaxLevel)
{
    if (curLevel > *pMaxLevel) {
        *pMaxLevel = curLevel;
    }
    AcDbBlockReference* pBlockRef = nullptr;
    if (Acad::eOk != acdbOpenObject(pBlockRef, objId)) {
        return;
    }
    AcDbObjectId btrId = pBlockRef->blockTableRecord();
    pBlockRef->close();
    if (btrId == AcDbObjectId::kNull) {
        return;
    }
    AcDbBlockTableRecord* pBtr = nullptr;
    if (Acad::eOk != zcdbOpenObject(pBtr, btrId)) {
        return;
    }
    AcDbBlockTableRecordIterator* pBtrIter = nullptr;
    pBtr->newIterator(pBtrIter);
    for (; !pBtrIter->done(); pBtrIter->step()) {
        AcDbObjectId objectId;
        if (Acad::eOk != pBtrIter->getEntityId(objectId) || objectId == AcDbObjectId::kNull) {
            continue;
        }
        AcRxClass* objectClass = objectId.objectClass();
        if (objectClass != AcDbBlockReference::desc()) {
            continue;
        }
        getMaxNestedLevel(objectId, curLevel + 1, pMaxLevel);
    }
    delete pBtrIter;
    pBtr->close();
}
