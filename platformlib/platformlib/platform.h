/*------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
 *Data：2010.09.28
 *Author：刘志盼
 *Version：V1.00
 *File：platform.h
 *Note：平台接口模块，包括初始化及数据包的读、写的函数
-------------------------------------------------------------------------*/
#ifndef PLATFORM_H
#define PLATFORM_H
#include <list>
#include "platformhead.h"

#pragma pack(1)																		/*设定为1字节对齐*/

class 	CZc;
class  packetprocess;

class CAppInterface 
{
public:
	CAppInterface();
	~CAppInterface();

	PLAT_INT32		AppInit();			
	PLAT_INT32		AppInit(PLAT_UINT8* ,PLAT_UINT8* ,PLAT_UINT32 ,char *);			/*平台提供的初始化函数*/
	PLAT_INT32		AppWrite();                                                     /*平台提供的写数据包函数*/                                         
	PLAT_INT32		AppRead();                                                      /*平台提供的读数据包函数*/	
	PLAT_INT32		AppClose();

private:
	void			procConnectState(PLAT_UBYTE* p);
	void			procConnectControl(PLAT_UBYTE* p);
	void			procBroadMsg(PLAT_UBYTE* p);
	void			procInputAppStatus(PLAT_UBYTE* p);

	void write2PlatBuffer(PLAT_UBYTE*  ppack);
	void updatePlatBuffer(PLAT_UBYTE*  ppack);
	void initPackage(PLAT_UBYTE*  ppack, PLAT_UINT len, int type);//len is T_UNIT_HEADE.size
	void unInitPackage(PLAT_UBYTE* ppack);

	void			newPackage(int type);
	void			write2PlatBuffer();
	PLAT_INT32		AppWrite(char *);                          
	PLAT_INT32		AppRead(char *);
	void			OutputUint(PLAT_UINT8 * ,int);									/*数据包各单元数据打印函数*/

private:
	packetprocess*  m_pei;																/*定义的平台包处理类的对象*/

	PLAT_UINT8 *send;																/*平台接收的数据包的首地址*/
	PLAT_UINT8 *recv;																/*平台接收的数据包的首地址*/
	
	static PLAT_UINT32 srcID;																/*保存数据源ID*/
	PLAT_UINT32 dstID;																/*保存数据包各单元的目的地ID*/
    PLAT_UINT32 platID;																/*保存与数据源相关的平台ID*/
 
	char src[IDSIZE];                 
	char dst[IDSIZE];
	char plat[IDSIZE];
	PLAT_UINT8 uintBuf[SIZE];														/*暂存数据包中各单元的数据*/
    PLAT_UINT8 platBuf[SIZE];														/*暂存平台数据缓冲区中弹出的单元数据*/
    
	PLAT_UINT64   readcount;														/*记录平台的读周期个数*/
	PLAT_UINT64   writecount;														/*记录平台的写周期个数*/

	CZc*      m_pzc;
};
#endif
