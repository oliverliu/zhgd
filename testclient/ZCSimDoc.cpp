// ZCSimDoc.cpp : implementation of the CZCSimDoc class
//

#include "stdafx.h"
#include "ZCSim.h"

#include "ZCSimDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZCSimDoc

IMPLEMENT_DYNCREATE(CZCSimDoc, CDocument)

BEGIN_MESSAGE_MAP(CZCSimDoc, CDocument)
	//{{AFX_MSG_MAP(CZCSimDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZCSimDoc construction/destruction

CZCSimDoc::CZCSimDoc()
{
	// TODO: add one-time construction code here

}

CZCSimDoc::~CZCSimDoc()
{
}

BOOL CZCSimDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CZCSimDoc serialization

void CZCSimDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CZCSimDoc diagnostics

#ifdef _DEBUG
void CZCSimDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CZCSimDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CZCSimDoc commands
