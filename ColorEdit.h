// ColorEdit.h : main header file for the COLOREDIT application
//

#if !defined(AFX_COLOREDIT_H__E10C8ED4_2CCF_4B7E_8DF3_59B2186B68ED__INCLUDED_)
#define AFX_COLOREDIT_H__E10C8ED4_2CCF_4B7E_8DF3_59B2186B68ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CColorEditApp:
// See ColorEdit.cpp for the implementation of this class
//

class CColorEditApp : public CWinApp
{
public:
	CColorEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CColorEditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOREDIT_H__E10C8ED4_2CCF_4B7E_8DF3_59B2186B68ED__INCLUDED_)
