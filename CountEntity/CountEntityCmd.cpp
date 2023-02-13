#include "pch.h"
#include "CountEntityCmd.h"
#include "CountEntityResult.h"
#include <afxwin.h>
#include "acedads.h"
#include "adscodes.h"
#include "acutads.h"
#include "dbmain.h"
#include <memory>

void countEntityCmd()
{
	AcString strPath;
	if (RTNORM != acedGetString(1, _T("\nPlease input DWG/DXF file path: "), strPath)) {
		acutPrintf(_T("\nNot Input path!"));
		return;
	}
	CFileStatus status;
	if (!CFile::GetStatus(strPath, status) || status.m_attribute == CFile::directory) {
		acutPrintf(_T("\nFile %s does not exist!"), strPath.kTCharPtr());
		return;
	}
	std::unique_ptr<AcDbDatabase> pDb(new AcDbDatabase(Adesk::kFalse));
	if (strPath.right(4).compareNoCase(_T(".dwg")) == 0) {
		if (Acad::eOk != pDb->readDwgFile(strPath.kTCharPtr())) {
			acutPrintf(_T("\nRead DWG file %s failed!"), strPath.kTCharPtr());
			return;
		}
	}
	else if (strPath.right(4).compareNoCase(_T(".dxf")) == 0) {
		if (Acad::eOk != pDb->dxfIn(strPath.kTCharPtr())) {
			acutPrintf(_T("\nRead DXF file %s failed!"), strPath.kTCharPtr());
			return;
		}
	}
	else {
		acutPrintf(_T("\nInvalid file type!"));
		return;
	}

	CountEntityResult result(strPath.utf8Ptr());

	// 打开块表
	AcDbBlockTable* pBlockTable = nullptr;
	if (Acad::eOk != pDb->getBlockTable(pBlockTable, AcDb::kForRead) || !pBlockTable) {
		acutPrintf(_T("\nFailed to get block table."));
		return;
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
			result.addOne(className.utf8Ptr());
			pEnt->close();
		}
		delete pBtrIter;
		pBtr->close();
	}
	delete pBtIter;
	pBlockTable->close();
	
	result.finalize();
	result.print();
	result.insertToDb("D:\\test.db");
}
