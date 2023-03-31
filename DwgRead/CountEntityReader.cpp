#include "CountEntityReader.h"
#include <vector>
#include "dbents.h"
#include "dbmain.h"

const std::vector<std::wstring> gClassNames = {
    L"AcDbEntity",
    L"AcDbBlockReference",
    L"AcDbMInsertBlock",
    L"AcDbTable",
    L"AcDbViewRepBlockReference",
    L"AcDbCamera",
    L"AcDbFace",
    L"AcDbFcf",
    L"AcDbFrame",
    L"AcDbOleFrame",
    L"AcDbOle2Frame",
    L"AcDbHatch",
    L"AcDbImage",
    L"AcDbRasterImage",
    L"AcDbWipeout",
    L"AcDbLight",
    L"AcDbMLeader",
    L"AcDbMline",
    L"AcDbMPolygon",
    L"AcDbMText",
    L"AcDbPoint",
    L"AcDbPolyFaceMesh",
    L"AcDbPolygonMesh",
    L"AcDbProxyEntity",
    L"AcDbSection",
    L"AcDbShape",
    L"AcDbSolid",
    L"AcDbText",
    L"AcDbAttribute",
    L"AcDbAttributeDefinition",
    L"AcDbTrace",
    L"AcDbViewport",
    L"AcDbCurve",
    L"AcDb2dPolyline",
    L"AcDb3dPolyline",
    L"AcDbArc",
    L"AcDbCircle",
    L"AcDbEllipse",
    L"AcDbLeader",
    L"AcDbLine",
    L"AcDbPolyline",
    L"AcDbRay",
    L"AcDbSpline",
    L"AcDbHelix",
    L"AcDbXline",
    L"AcDbDimension",
    L"AcDb2LineAngularDimension",
    L"AcDb3PointAngularDimension",
    L"AcDbAlignedDimension",
    L"AcDbArcDimension",
    L"AcDbDiametricDimension",
    L"AcDbOrdinateDimension",
    L"AcDbRadialDimension",
    L"AcDbRadialDimensionLarge",
    L"AcDbRotatedDimension",
    L"AcDb3dSolid",
    L"AcDbBody",
    L"AcDbRegion",
    L"AcDbSubDMesh",
    // L"AcDbBlockBegin",
    // L"AcDbBlockEnd",
    // L"AcDbSequenceEnd",
    L"AcDbSurface",
    L"AcDbExtrudedSurface",
    L"AcDbLoftedSurface",
    L"AcDbNurbSurface",
    L"AcDbPlaneSurface",
    L"AcDbRevolvedSurface",
    L"AcDbSweptSurface",
    // L"AcDbVertex",
    // L"AcDb2dVertex",
    // L"AcDb3dPolylineVertex",
    // L"AcDbFaceRecord",
    // L"AcDbPolyFaceMeshVertex",
    // L"AcDbPolygonMeshVertex",
    L"AcDbViewBorder",
    L"AcDbViewSymbol",
    L"AcDbDetailSymbol",
    L"AcDbSectionSymbol",
    L"AcDbUnderlayReference",
    L"AcDbDgnReference",
    L"AcDbDwfReference",
    L"AcDbPdfReference",
    L"AcDbArcAlignedText"
};

const std::map<std::wstring, std::vector<std::wstring>> gSubclassMap = {
    {L"AcDbEntity", {L"AcDbBlockReference", L"AcDbCamera", L"AcDbFace", L"AcDbFcf", L"AcDbFrame",
                     L"AcDbHatch", L"AcDbImage", L"AcDbLight", L"AcDbMLeader", L"AcDbMline", L"AcDbMPolygon",
                     L"AcDbMText", L"AcDbPoint", L"AcDbPolyFaceMesh", L"AcDbPolygonMesh", L"AcDbProxyEntity",
                     L"AcDbSection", L"AcDbShape", L"AcDbSolid", L"AcDbText", L"AcDbTrace", L"AcDbViewport",
                     L"AcDbCurve", L"AcDbDimension", L"AcDb3dSolid", L"AcDbBody", L"AcDbRegion",
                     L"AcDbSubDMesh", /* L"AcDbBlockBegin", L"AcDbBlockEnd", L"AcDbSequenceEnd", */
                     L"AcDbSurface", /* L"AcDbVertex", */ L"AcDbViewBorder", L"AcDbViewSymbol",
                     L"AcDbUnderlayReference", L"AcDbArcAlignedText"}},
    {L"AcDbBlockReference", {L"AcDbMInsertBlock", L"AcDbTable", L"AcDbViewRepBlockReference"}},
    {L"AcDbFrame", {L"AcDbOleFrame"}},
    {L"AcDbOleFrame", {L"AcDbOle2Frame"}},
    {L"AcDbImage", {L"AcDbRasterImage"}},
    {L"AcDbRasterImage", {L"AcDbWipeout"}},
    {L"AcDbText", {L"AcDbAttribute", L"acDbAttributeDefinition"}},
    {L"AcDbCurve", {L"AcDb2dPolyline", L"AcDb3dPolyline", L"AcDbArc", L"AcDbCircle", L"AcDbEllipse",
                    L"AcDbLeader", L"AcDbLine", L"AcDbPolyline", L"AcDbRay", L"AcDbSpline", L"AcDbXline"}},
    {L"AcDbSpline", {L"AcDbHelix"}},
    {L"AcDbDimension", {L"AcDb2LineAngularDimension", L"AcDb3PointAngularDimension", L"AcDbAlignedDimension",
                        L"AcDbArcDimension", L"AcDbDiametricDimension", L"AcDbOrdinateDimension",
                        L"AcDbRadialDimension", L"AcDbRadialDimensionLarge", L"AcDbRotatedDimension"}},
    {L"AcDbSurface", {L"AcDbExtrudedSurface", L"AcDbLoftedSurface", L"AcDbNurbSurface", L"AcDbPlaneSurface",
                      L"AcDbRevolvedSurface", L"AcDbSweptSurface"}},
                      /* {L"AcDbVertex", {L"AcDb2dVertex", L"AcDb3dPolylineVertex", L"AcDbFaceRecord", L"AcDbPolyFaceMeshVertex",
                                          L"AcDbPolygonMeshVertex"}}, */
    {L"AcDbViewSymbol", {L"AcDbDetailSymbol", L"AcDbSectionSymbol"}},
    {L"AcDbUnderlayReference", {L"AcDbDgnReference", L"AcDbDwfReference", L"AcDbPdfReference"}}
};

CountEntityReader countEntity;

CountEntityReader::CountEntityReader() : numCustomEntities(0)
{
    for (const auto& className : gClassNames) {
        result[className] = 0;
    }
}

CountEntityReader::~CountEntityReader()
{
}

std::tuple<bool, std::wstring> CountEntityReader::read(AcDbDatabase* pDb)
{
    std::tuple<bool, std::wstring> res;
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
        AcDbBlockTableRecordIterator* pBtrIter = nullptr;
        pBtr->newIterator(pBtrIter);
        for (; !pBtrIter->done(); pBtrIter->step()) {
            // 打开实体
            AcDbEntity* pEnt = nullptr;
            pBtrIter->getEntity(pEnt);
            if (!pEnt)
                continue;
            AcString className(pEnt->isA()->name());
            addOne(className.kTCharPtr());
            pEnt->close();
        }
        delete pBtrIter;
        pBtr->close();
    }
    delete pBtIter;
    pBlockTable->close();

    finalize();
	return std::make_tuple(true, _T(""));
}

const std::map<std::wstring, unsigned int>& CountEntityReader::getResult()
{
    result[L"CustomEntity"] = numCustomEntities;
    return result;
}

void CountEntityReader::finalize()
{
    for (const auto& className : gClassNames) {
        if (!hasSubClass(className))
            continue;
        if (cache.find(className) != std::end(cache)) {
            result[className] = cache[className];
            continue;
        }
        result[className] = calcSuperclassCount(className);
    }
}

void CountEntityReader::addOne(const std::wstring& className)
{
    // if this is a builtin class
    if (result.find(className) != std::end(result)) {
        result[className] += 1;
        return;
    }
    // if this is a custom class
    numCustomEntities += 1;
}

bool CountEntityReader::hasSubClass(const std::wstring& className) const
{
    return gSubclassMap.find(className) != std::cend(gSubclassMap);
}

unsigned int CountEntityReader::calcSuperclassCount(const std::wstring& className)
{
    if (!hasSubClass(className))
        return result[className];

    // already calculated before
    if (cache.find(className) != std::end(cache))
        return cache[className];

    UINT selfCount = result[className];
    auto& subclasses = gSubclassMap.at(className);
    for (const auto& subclassName : subclasses) {
        selfCount += calcSuperclassCount(subclassName);
    }
    cache[className] = selfCount;
    return selfCount;
}
