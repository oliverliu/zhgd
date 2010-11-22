/*------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
 *Data��2010.09.28
 *Author����־��
 *Version��V1.00
 *File��platform.h
 *Note��ƽ̨�ӿ�ģ�飬������ʼ�������ݰ��Ķ���д�ĺ���
-------------------------------------------------------------------------*/
#ifndef PLATFORM_H
#define PLATFORM_H
#include <list>
#include "platformhead.h"

#pragma pack(1)																		/*�趨Ϊ1�ֽڶ���*/

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
	PLAT_INT32		AppInit(PLAT_UINT8* ,PLAT_UINT8* ,PLAT_UINT32 ,char *);			/*ƽ̨�ṩ�ĳ�ʼ������*/
	PLAT_INT32		AppWrite();                                                     /*ƽ̨�ṩ��д���ݰ�����*/                                         
	PLAT_INT32		AppRead();                                                      /*ƽ̨�ṩ�Ķ����ݰ�����*/	
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
	void			OutputUint(PLAT_UINT8 * ,int);									/*���ݰ�����Ԫ���ݴ�ӡ����*/

private:
	packetprocess*  m_pei;																/*�����ƽ̨��������Ķ���*/
    ZRedis * m_pRedis;

	PLAT_UINT8 *send;																/*ƽ̨���յ����ݰ����׵�ַ*/
	PLAT_UINT8 *recv;																/*ƽ̨���յ����ݰ����׵�ַ*/
	
	static PLAT_UINT32 srcID;																/*��������ԴID*/
	PLAT_UINT32 dstID;																/*�������ݰ�����Ԫ��Ŀ�ĵ�ID*/
    PLAT_UINT32 platID;																/*����������Դ��ص�ƽ̨ID*/
 
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
