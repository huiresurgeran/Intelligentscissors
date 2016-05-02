
// Intelligent_Scissor.h : main header file for the Intelligent_Scissor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CIntelligent_ScissorApp:
// See Intelligent_Scissor.cpp for the implementation of this class
//

class CIntelligent_ScissorApp : public CWinAppEx
{
public:
	CIntelligent_ScissorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIntelligent_ScissorApp theApp;
