// ZCSimDoc.h : interface of the CZCSimDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZCSIMDOC_H__2CE64C7D_D5EF_45DD_9FCA_98D98B8CFAA4__INCLUDED_)
#define AFX_ZCSIMDOC_H__2CE64C7D_D5EF_45DD_9FCA_98D98B8CFAA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CZCSimDoc : public CDocument
{
protected: // create from serialization only
	CZCSimDoc();
	DECLARE_DYNCREATE(CZCSimDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZCSimDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZCSimDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CZCSimDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZCSIMDOC_H__2CE64C7D_D5EF_45DD_9FCA_98D98B8CFAA4__INCLUDED_)
