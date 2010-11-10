// ZCSimView.h : interface of the CZCSimView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZCSIMVIEW_H__97CA1357_12C8_4338_9294_BB0E70E2F1F5__INCLUDED_)
#define AFX_ZCSIMVIEW_H__97CA1357_12C8_4338_9294_BB0E70E2F1F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SwitchVar.h"
#include "StationVar.h"
#include "afxtempl.h"
#include "ZCDataStruct.h"

class CZCSimView : public CFormView
{
protected: // create from serialization only
	CZCSimView();
	DECLARE_DYNCREATE(CZCSimView)

public:
	//{{AFX_DATA(CZCSimView)
	enum{ IDD = IDD_ZCSIM_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CZCSimDoc* GetDocument();
	PLAT_UINT32   trackdataversion;
	PLAT_UINT32   speedlimitversion;
	PLAT_UINT8    SwitchCount;   
    PLAT_UINT8    StationCount;
	CList<SwitchVar*, SwitchVar*> m_switchlist;
	CList<StationVar*, StationVar*> m_stationlist;
	int m_sendflag;
	PLAT_UINT8   m_EbCmd;       //����ͣ�����EB(0xAA)����EB(0x55)
	PLAT_UINT8   m_stopAssReq;	//ͣ����֤����������(0xAA����������0x55��
	PLAT_UINT32	 m_stopSigId;	    //ͣ���źŻ����
	PLAT_UINT8   m_MalDir;		//MAL����0xAA��������0x55��
	PLAT_UINT32  m_MalSecId;		//MAL��λ�ã�����ID
	PLAT_UINT32	 m_MalOffset;		//MAL��λ�ã������е�ƫ�ƣ���λ������
	PLAT_UINT8   m_MalEndType;	   //MAL�յ�����,0���Σ�1�յ�END,2����

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZCSimView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZCSimView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CZCSimView)
	afx_msg void OnReadConfig();
	afx_msg void OnBeaginSend();
	afx_msg void OnStopSend();
	afx_msg void OnBtnEmg();
	afx_msg void OnBtnStopreq();
	afx_msg void OnRadioEastdir();
	afx_msg void OnRadioWestdir();
	afx_msg void OnSetok();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ZCSimView.cpp
inline CZCSimDoc* CZCSimView::GetDocument()
   { return (CZCSimDoc*)m_pDocument; }
#endif

UINT  DataCommThread(LPVOID);
void  outputUint(FILE*, PLAT_UINT8* ,int);
void  DataSend(PLAT_UINT8*);	

#endif // !defined(AFX_ZCSIMVIEW_H__97CA1357_12C8_4338_9294_BB0E70E2F1F5__INCLUDED_)
