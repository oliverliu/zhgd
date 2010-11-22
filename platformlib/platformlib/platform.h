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

//#define _MESSAGE_BIG_ENDIAN    //if no define it, regard the message is little endian sequence.

class  CZc;
class  packetprocess;
class  ZRedis;

extern const char * g_fromTerminalLog;

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
	void			InitSrcId();
	PLAT_INT32		GetSrcId();

private:
	void			procConnectState(PLAT_UBYTE* p);
	void			procMsgOut(PLAT_UBYTE* p);
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
    ZRedis * m_pRedis;

	PLAT_UINT8 *send;																/*平台接收的数据包的首地址*/
	PLAT_UINT8 *recv;																/*平台接收的数据包的首地址*/
	
	static PLAT_UINT32 srcID;																/*保存数据源ID*/
	PLAT_UINT32 dstID;																/*保存数据包各单元的目的地ID*/
    PLAT_UINT32 platID;																/*保存与数据源相关的平台ID*/
 
	char src[IDSIZE];                 
	char dst[IDSIZE];
	char plat[IDSIZE];
    //little package
	PLAT_UINT8 uintBuf[SIZE];//save data from other terminal send me, the data from other termianl is big endian
    PLAT_UINT8 platBuf[SIZE];//save data in platform, the data is liitle endian for internal use
    
	CZc*      m_pzc;

	FILE	*m_fpFromTerminalLog;
//	fprintf(fp, "READ: \n");
};
#endif
