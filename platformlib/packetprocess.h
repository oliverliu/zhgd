/*------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
 *Data：2010.09.29
 *Author：刘志盼
 *Version：V1.1
 *File：packetprocess.h
 *Note：平台数据包处理模块，包括数据包解包、分发、组包函数
-------------------------------------------------------------------------*/
#ifndef PACKETPROCESS_H
#define PACKETPROCESS_H

#include "InterfaceDataStruct.h"   
#include "platformhead.h"
#pragma pack(1)															/*设定为1字节对齐*/

class packetprocess      	
{
	
public:
	packetprocess();
	~packetprocess();
	
    void Decoder(PLAT_UINT8 *);												/*数据包解包函数*/
	void Encoder(PLAT_UINT8 *, PLAT_UINT8 *, int );							/*数据包组包函数*/
    PLAT_UINT32 GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 );		/*得到数据包中各单元的目的地ID*/
	void HandlePlatData(PLAT_UINT32, PLAT_UINT8 *, PLAT_UINT8 *);			/*平台端读写数据处理函数*/
	friend class CAppInterface;	
private:
	bool hasMsgHeader() ;
	PLAT_UINT32 ATO_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 ); /*得到ATO发送数据包中各单元的目的地ID*/

	PLAT_UINT32 ATP_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 ); /*得到ATP发送数据包中各单元的目的地ID*/

	PLAT_UINT32 RS_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 );  /*得到RS发送数据包中各单元的目的地ID*/

	PLAT_UINT32 ZC_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 );  /*得到ZC发送数据包中各单元的目的地ID*/

private:

	T_DATA_INDEX	index;													/*数据包索引头*/                     
    T_UNIT_HEAD		unithead;												/*数据包各单元的单元头*/  
	PLAT_UINT32		id[256];												/*数据包各单元的单元ID构成的数组*/
	PLAT_UINT32		unitsize[256];											/*数据包各单元的单元大小构成的数组*/
	PLAT_UINT8*		paddr; //首地址*/
	PLAT_UINT8*		pPlataddr;
	T_MESSAGE_HEAD	messageHeadOut;											/*数据包中通信报文输出数据报文头*/
};

#endif