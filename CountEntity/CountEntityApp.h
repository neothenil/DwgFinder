// CountEntity.h : main header file for the CountEntity DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCountEntityApp
// See CountEntity.cpp for the implementation of this class
//

class CCountEntityApp : public CWinApp
{
public:
	CCountEntityApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
