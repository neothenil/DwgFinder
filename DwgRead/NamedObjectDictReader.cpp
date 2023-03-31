#include "NamedObjectDictReader.h"
#include "dbdict.h"

// Field name definitions
const std::wstring colorField            = L"Color";
const std::wstring dataLinkField         = L"DataLink";
const std::wstring detailViewStyleField  = L"DetailViewStyle";
const std::wstring groupField            = L"Groups";
const std::wstring layoutField           = L"Layout";
const std::wstring materialField         = L"Material";
const std::wstring mLeaderStyleField     = L"MLeaderStyle";
const std::wstring mLineStyleField       = L"MLineStyle";
const std::wstring plotSettingsField     = L"PlotSettings";
const std::wstring plotStyleNameField    = L"PlotStyleName";
const std::wstring scaleListField        = L"ScaleList";
const std::wstring sectionViewStyleField = L"SectionViewStyle";
const std::wstring tableStyleField       = L"TableStyle";
const std::wstring visualStyleField      = L"VisualStyle";

NamedObjectDictReader namedObjectDictReader;

NamedObjectDictReader::NamedObjectDictReader()
{
	result[colorField           ] = 0;
	result[dataLinkField        ] = 0;
	result[detailViewStyleField ] = 0;
	result[groupField           ] = 0;
	result[layoutField          ] = 0;
	result[materialField        ] = 0;
	result[mLeaderStyleField    ] = 0;
	result[mLineStyleField      ] = 0;
	result[plotSettingsField    ] = 0;
	result[plotStyleNameField   ] = 0;
	result[scaleListField       ] = 0;
	result[sectionViewStyleField] = 0;
	result[tableStyleField      ] = 0;
	result[visualStyleField     ] = 0;
}

NamedObjectDictReader::~NamedObjectDictReader()
{
}

std::tuple<bool, std::wstring> NamedObjectDictReader::read(AcDbDatabase* pDb)
{
	auto res = std::make_tuple(true, _T(""));
	std::map<std::wstring, AcDbObjectId> objectDictId;

	objectDictId[colorField] = pDb->colorDictionaryId();
	objectDictId[dataLinkField] = pDb->dataLinkDictionaryId();
	objectDictId[detailViewStyleField] = pDb->detailViewStyleDictionaryId();
	objectDictId[groupField] = pDb->groupDictionaryId();
	objectDictId[layoutField] = pDb->layoutDictionaryId();
	objectDictId[materialField] = pDb->materialDictionaryId();
	objectDictId[mLeaderStyleField] = pDb->mleaderStyleDictionaryId();
	objectDictId[mLineStyleField] = pDb->mLStyleDictionaryId();
	objectDictId[plotSettingsField] = pDb->plotSettingsDictionaryId();
	objectDictId[plotStyleNameField] = pDb->plotStyleNameDictionaryId();
	objectDictId[scaleListField] = pDb->scaleListDictionaryId();
	objectDictId[sectionViewStyleField] = pDb->sectionViewStyleDictionaryId();
	objectDictId[tableStyleField] = pDb->tableStyleDictionaryId();
	objectDictId[visualStyleField] = pDb->visualStyleDictionaryId();

	for (const auto& [field, dictId] : objectDictId) {
		if (dictId == AcDbObjectId::kNull) {
			continue;
		}
		AcDbObject* pObj = nullptr;
		Acad::ErrorStatus es = acdbOpenObject(pObj, dictId, AcDb::kForRead);
		if (es != Acad::eOk) {
			continue;
		}
		AcDbDictionary* pDict = AcDbDictionary::cast(pObj);
		if (pDict == nullptr) {
			pObj->close();
			continue;
		}
		result[field] = pDict->numEntries();
		pDict->close();
	}

	return res;
}

const std::map<std::wstring, unsigned int>& NamedObjectDictReader::getResult()
{
	return result;
}
