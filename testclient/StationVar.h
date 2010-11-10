// StationVar.h: interface for the StationVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATIONVAR_H__E8D9EA14_987E_4934_9E6F_0214453FC0B5__INCLUDED_)
#define AFX_STATIONVAR_H__E8D9EA14_987E_4934_9E6F_0214453FC0B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZCDataStruct.h"

class StationVar  
{
public:
	StationVar();
	virtual ~StationVar();
	
	PLAT_UINT16 m_stationID;
	PLAT_UINT8 m_stationStatus;
};

#endif // !defined(AFX_STATIONVAR_H__E8D9EA14_987E_4934_9E6F_0214453FC0B5__INCLUDED_)
