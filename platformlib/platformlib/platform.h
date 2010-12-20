/*------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
 *Create Data：2010.09.28
 *Create Author：liuzhipan
 *Last Modify data: 2010.12.01
 *Version: V2.00
 *File: platform.h
 *Note：
-------------------------------------------------------------------------*/
#ifndef PLATFORM_H
#define PLATFORM_H
#include <list>
#include <stdio.h>
#include <string>
#include "platformhead.h"

#pragma pack(1)																		/*设定为1字节对齐*/

//#define _MESSAGE_BIG_ENDIAN    //if no define it, regard the message is little endian sequence.

class  CZc;
class  ZRedis;
class  ZSocketClient;


class CAppInterface 
{
public:
	CAppInterface();
	~CAppInterface();

	//init simulator platform
	virtual PLAT_INT32		AppInit();			
	virtual PLAT_INT32		AppInit(PLAT_UINT8* s ,PLAT_UINT8* r ,PLAT_UINT32 sid,char *dbhost = NULL);
	virtual PLAT_INT32		AppWrite();                                                                            
	virtual PLAT_INT32		AppRead(); 
	virtual PLAT_INT32		AppClose();
	void			              InitSrcId();
	PLAT_INT32		       GetSrcId();

	void                             setLog(const char* file);

protected:
	void			procConnectState(PLAT_UBYTE* p);
	void			procMsgOut(PLAT_UBYTE* p);
	void			procConnectControl(PLAT_UBYTE* p);
	void			procBroadMsg(PLAT_UBYTE* p);
	void			procInputAppStatus(PLAT_UBYTE* p);

	PLAT_UINT32     getPlatformID( PLAT_UINT32 srcid);

	//void                write2PlatBuffer(PLAT_UBYTE*  ppack);
       //void                write2DbBuffer(PLAT_UBYTE*  ppack);
	//void                updatePlatBuffer(PLAT_UBYTE*  ppack);
       //void                updateDbBuffer(PLAT_UBYTE*  ppack);

	//void                initPackage(PLAT_UBYTE*  ppack, PLAT_UINT len, int type, int value);//len is T_UNIT_HEADE.size
	//void                initPackage(PLAT_UBYTE*  _ppack, PLAT_UINT did, int type, int connectvalue);
      // void                unInitPackage(PLAT_UBYTE* ppack);

	//void			newPackage(int type);
	void			write2PlatBuffer();
	PLAT_INT32	    AppWrite(char *);                          
	PLAT_INT32	    AppRead(char *);
	void			OutputUint(PLAT_UINT8 * ,int);	//output help function								

   
    void            plog(const char* format, ...);
    void            outputPackage(const PLAT_UBYTE * buf);
    void            outputLittlepack(const PLAT_UBYTE * buf);


protected:															
	ZRedis*                 m_pRedis;
	ZSocketClient*          m_pSockClient;
	PLAT_UINT8 *            send;	/*平台接收的数据包的首地址*/															
	PLAT_UINT8 *            recv;	/*平台接收的数据包的首地址*/

	PLAT_UINT32             m_srcID;	/*保存数据源ID*/
	PLAT_UINT32 dstID;		/*保存数据包各单元的目的地ID*/
	PLAT_UINT32 platID;		/*保存与数据源相关的平台ID*/

	char src[IDSIZE];                 
	char dst[IDSIZE];
	char plat[IDSIZE];
	//little package
	PLAT_UINT8 uintBuf[SIZE];//save data from other terminal send me, the data from other termianl is big endian
	PLAT_UINT8 platBuf[SIZE];//save data in platform, the data is liitle endian for internal use
    //save data in db, the data is little endian, that used for app_get/app_set
    PLAT_UINT8 m_dbBuf[SIZE];

	CZc*      m_pzc;

	FILE	*m_fpFromTerminalLog;
    std::string m_strfileLog;

    bool    m_bUseP1;
//	fprintf(fp, "READ: \n");
};
#endif
