/*------------------------------------------------------------------------
*(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
*Data：2010.11.03
*Author：liuning	
*Version：v0.1
*File：InterfaceDataStruct.h
*Note：与平台相关的结构体
--------------------------------------------------------------------------*/
#ifndef INTERFACEDATASTRUCT_H
#define INTERFACEDATASTRUCT_H

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
	PLAT_UINT16 msgLen;			/*通信报文数据大小*/
	
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



// kinds of type data structure
typedef T_UNIT stInputAppStatus;
typedef T_UNIT stInputComMsg;
typedef T_UNIT stComLinkStatus;
typedef T_UNIT stDbStatus;
typedef T_UNIT stOutputComRegion;
typedef T_UNIT stLinkCtrl;
typedef T_UNIT stDbCtrl;
typedef T_UNIT stEventRecord;
typedef T_UNIT stOutputAppStatus;


/*------------------------------------------
* 定义ATO输入到平台的事件记录的数据的数据结构（未定）
* -----------------------------------------*/
typedef struct         
{
	PLAT_UINT32  eventCode;
	
}T_EVENT_DATA;



/*------------------------------------------
* 定义一个表示通信连接状态数据输入的结构体（待定）
* -----------------------------------------*/
typedef struct						
{
	PLAT_UINT8 CtrlRequest;		/*控制请求：0x20-连接未建立状态；0x21-正在连接状态；0x22-连接删除状态；
								0x23-连接正常；0x24-连接超时；0x25-连接错误*/
}T_COM_LINK_IN;

/*------------------------------------------
* 定义一个表示DB数据状态输入的结构体
* -----------------------------------------*/
typedef struct						
{
	PLAT_UINT32	addr;			/*DB数据首地址*/
	PLAT_UINT32	size;			/*DB数据大小*/
	PLAT_UINT32	version;		/*DB数据版本号*/
	PLAT_UINT8	state;			/*DB状态：0x10-正在更新；0x11-更新错误；0x12-无法更新
								0x50-DB数据正常；0xF0-DB数据无效*/
}T_TRACK_DB_IN;

/*------------------------------------------
* 定义一个表示DB数据控制数据项的结构体
* -----------------------------------------*/
typedef struct					
{
	PLAT_UINT8	control;		/*DB数据控制请求：0x11-请求从本地更新DB；0x12-请求从网络更新*/
	PLAT_UINT8	reserve;		/*保留，暂无数据*/
	PLAT_UINT32	dstID;			/*目标地址*/
	PLAT_UINT8	fileName[18];	/*文件名*/
	
}T_DB_CTRL;

/*------------------------------------------
* 应用程序中间状态同步输出数据（暂未定）
* -----------------------------------------*/
typedef struct
{
	
}T_SYNDATA_OUT;

#endif
