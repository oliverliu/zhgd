// ZCSim.h : main header file for the ZCSIM application
//

#if !defined(AFX_ZCSIM_H__F0532503_9D93_42BE_BF46_B52EA5576709__INCLUDED_)
#define AFX_ZCSIM_H__F0532503_9D93_42BE_BF46_B52EA5576709__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CZCSimApp:
// See ZCSim.cpp for the implementation of this class
//

class CZCSimApp : public CWinApp
{
public:
	CZCSimApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZCSimApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CZCSimApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZCSIM_H__F0532503_9D93_42BE_BF46_B52EA5576709__INCLUDED_)
