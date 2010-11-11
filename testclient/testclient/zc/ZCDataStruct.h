#ifndef ZCDataStruct
#define ZCDataStruct

#include "../platform/platformhead.h"

//#include "platformlib\platformhead.h"
#pragma pack(1)					/*设定为1字节对齐 */

/*------------ZC发送的通信报文的报文类型定义------------*/
#define  MT_ZC_TO_CC     0x00014101
#define  MT_CC_TO_ZC     0x00011401 
#define  MT_ZC_TO_CBI	 0x00014201
#define  MT_CBI_TO_ZC    0x00012401
#define  MT_ZC_TO_ATS    0x00014301
#define  MT_ATS_TO_ZC    0x00013401
#define  MT_DSU_TO_ZC    0x00015401


//ZC发送给ATP的周期性数据结构体
typedef struct
{
	PLAT_UINT32	seqNo;		//报文序号
	PLAT_UINT32	trainId;	//目标列车ID
	PLAT_UINT8	EbCmd;	    //紧急停车命令，EB(0xAA)，非EB(0x55)
	PLAT_UINT8	stopAssReq;	//停车保证请求，有请求（0xAA），无请求（0x55）
	PLAT_UINT32	stopSigId;	//停车信号机编号
	PLAT_UINT8	MalDir;		//MAL方向（0xAA），西向（0x55）
	PLAT_UINT32	MalSecId;	//MAL点位置，区段ID
	PLAT_UINT32	MalOffset;	//MAL点位置，区段中的偏移，单位：毫米
	PLAT_UINT8	MalEndType;	//MAL终点类型

}ZC_TO_CCCIR; 

//ZC发送给ATP的广播数据结构体
typedef struct
{
	PLAT_UINT32		gdDbVer;	  		//轨道数据库版本
	PLAT_UINT32		tsrDbVer;			//临时限速数据库版本
	PLAT_UINT8		swNum;				//道岔数目
	PLAT_UINT16		swId[5];  			//道岔ID
	PLAT_UINT8		swState[5];	  		//道岔定位(0xAA)，反位（0x55）,失去表示0xA5
	PLAT_UINT8		stationNum;			//站台数目
	PLAT_UINT16		stationId[3];		//站台ID
	PLAT_UINT8		stationState[3];	//站台状态：1代表有操作，0代表无操作

}ZC_TO_CCBRODE;

#endif