// SwitchVar.h: interface for the SwitchVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SWITCHVAR_H__AAE40618_E963_40E7_8B93_9C2F62FD11FB__INCLUDED_)
#define AFX_SWITCHVAR_H__AAE40618_E963_40E7_8B93_9C2F62FD11FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZCDataStruct.h"

class SwitchVar  
{
public:
	SwitchVar();
	virtual ~SwitchVar();

	PLAT_UINT16	 m_switchID;
	PLAT_UINT8	 m_switchstatus;
//	CString m_switchstatus;
};

#endif // !defined(AFX_SWITCHVAR_H__AAE40618_E963_40E7_8B93_9C2F62FD11FB__INCLUDED_)
