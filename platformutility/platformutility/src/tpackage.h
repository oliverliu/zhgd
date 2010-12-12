/*------------------------------------------------------------------------
*(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
*Data：2010.11.03
*Author：liuning	
*Version：v0.1
*File：InterfaceDataStruct.h
*Note：与平台相关的结构体
--------------------------------------------------------------------------*/
#ifndef _T_PACKAGE_INTERFACEDATASTRUCT_H
#define _T_PACKAGE_INTERFACEDATASTRUCT_H

#include "platformhead.h"
#pragma pack(1)					/*设定为1字节对齐 */

/*------------------------------------------
* 定义每个数据单元所包含的ID和Size结构体
* -----------------------------------------*/
typedef struct
{
	PLAT_UINT32  unitId;		/*单元ID*/       
	PLAT_UINT32  unitSize;		/*单元数据大小*/
	
}T_UNIT_HEAD;


typedef struct
{
	PLAT_UINT32  unitId;		/*单元ID*/       
	PLAT_UINT32  unitSize;		/*单元数据大小*/
	// the address has limited, it must be follow unitSize in memory
	// the value is _ppack + sizeof(PLAT_BYTE) * sizeof(T_UNIT_HEAD)
	PLAT_BYTE*	 unitData;
}T_UNIT;


/*------------------------------------------
* 通信报文数据头
* -----------------------------------------*/
typedef struct
{
	PLAT_UINT32 sequenceNum;	/*通信报文序列号*/
	PLAT_UINT64 timeStamp;		/*通信报文时间戳*/
	PLAT_UINT32 SID;			/*通信报文源地址*/
	PLAT_UINT32 DID;			/*通信报文目的地址*/
	PLAT_UINT32 msgType;		/*通信报文信息类型*/
	PLAT_UINT32 msgLen;			/*通信报文数据大小*/
	
}T_MESSAGE_HEAD;

/*------------------------------------------
* 数据检索表
* -----------------------------------------*/
typedef struct
{
	PLAT_UINT32 regionUnitNum;		  /*数据区单元数，不超过256*/
	PLAT_UINT32 platformStatus;		  /*ATO的主备状态*/
	PLAT_UINT32 platformHealth;		  /*本机健康状态*/
	PLAT_UINT32 unitAddrOffset[256];  /*数据单元区的地址偏移*/
	
}T_DATA_INDEX;

#endif
