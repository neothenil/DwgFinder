#include "AssocObjectReader.h"
#include "AcDbAssocNetwork.h"
#include "AcDbAssoc2dConstraintGroup.h"
#include "dbents.h"
#include "dbmain.h"

// Field name definitions
const std::wstring assocArrayField      = L"AssocArray";
const std::wstring constraintGroupField = L"ConstraintGroup";
const std::wstring drawingViewField     = L"DrawingView";

AssocObjectReader assocObjectReader;

AssocObjectReader::AssocObjectReader()
{
	result[assocArrayField] = 0;
	result[constraintGroupField] = 0;
	result[drawingViewField] = 0;
}

AssocObjectReader::~AssocObjectReader()
{
}

std::tuple<bool, std::wstring> AssocObjectReader::read(AcDbDatabase* pDb)
{
    std::tuple<bool, std::wstring> res;
    // optimization
    if (AcDbAssocNetwork::getInstanceFromDatabase(pDb, false) == AcDbObjectId::kNull)
        return std::make_tuple(true, _T(""));
    // 打开块表
    AcDbBlockTable* pBlockTable = nullptr;
    if (Acad::eOk != pDb->getBlockTable(pBlockTable, AcDb::kForRead) || !pBlockTable) {
        res = std::make_tuple(false, _T("failed to get block table"));
        return res;
    }
    AcDbBlockTableIterator* pBtIter = nullptr;
    pBlockTable->newIterator(pBtIter);
    for (; !pBtIter->done(); pBtIter->step()) {
        // 打开块表记录
        AcDbBlockTableRecord* pBtr = nullptr;
        pBtIter->getRecord(pBtr);
        if (!pBtr)
            continue;
        if (!pBtr->isLayout()) {
            pBtr->close();
            continue;
        }
        AcDbObjectId assocNetworkId = AcDbAssocNetwork::getInstanceFromObject(pBtr->objectId(), false);
        if (assocNetworkId == AcDbObjectId::kNull) {
            pBtr->close();
            continue;
        }
        findAssocObject(assocNetworkId);
        pBtr->close();
    }
    delete pBtIter;
    pBlockTable->close();

    return std::make_tuple(true, _T(""));
}

const std::map<std::wstring, unsigned int>& AssocObjectReader::getResult()
{
	return result;
}

void AssocObjectReader::findAssocObject(AcDbObjectId assocNetworkId)
{
    AcDbAssocNetwork* pAssocNetwork = nullptr;
    if (Acad::eOk != acdbOpenObject(pAssocNetwork, assocNetworkId))
        return;
    AcDbObjectIdArray actionsId = pAssocNetwork->getActions();
    pAssocNetwork->close();
    for (const auto& actionId : actionsId) {
        AcDbAssocAction* pAction = nullptr;
        Acad::ErrorStatus es = acdbOpenObject(pAction, actionId);
        if (Acad::eOk != es)
            continue;
        AcRxClass* pActionClass = pAction->isA();
        if (pActionClass == AcDbAssoc2dConstraintGroup::desc()) {
            result[constraintGroupField] += 1;
            pAction->close();
            continue;
        }
        if (pAction->actionBody().isValid()) {
            AcRxClass* pActionBodyClass = pAction->actionBody().objectClass();
            std::wstring className = pActionBodyClass->name();
            if (className == L"AcDbAssocArrayActionBody") {
                result[assocArrayField] += 1;
            }
            if (className == L"AcDbAssocViewRepActionBody") {
                result[drawingViewField] += 1;
            }
        }
        pAction->close();
    }
}
