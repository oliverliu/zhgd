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

class 	CZc;
class  packetprocess;

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
	void			OutputUint(PLAT_UINT8 * ,int);									/*���ݰ�����Ԫ���ݴ�ӡ����*/

private:
	packetprocess*  m_pei;																/*�����ƽ̨��������Ķ���*/

	PLAT_UINT8 *send;																/*ƽ̨���յ����ݰ����׵�ַ*/
	PLAT_UINT8 *recv;																/*ƽ̨���յ����ݰ����׵�ַ*/
	
	static PLAT_UINT32 srcID;																/*��������ԴID*/
	PLAT_UINT32 dstID;																/*�������ݰ�����Ԫ��Ŀ�ĵ�ID*/
    PLAT_UINT32 platID;																/*����������Դ��ص�ƽ̨ID*/
 
	char src[IDSIZE];                 
	char dst[IDSIZE];
	char plat[IDSIZE];
	PLAT_UINT8 uintBuf[SIZE];														/*�ݴ����ݰ��и���Ԫ������*/
    PLAT_UINT8 platBuf[SIZE];														/*�ݴ�ƽ̨���ݻ������е����ĵ�Ԫ����*/
    
	PLAT_UINT64   readcount;														/*��¼ƽ̨�Ķ����ڸ���*/
	PLAT_UINT64   writecount;														/*��¼ƽ̨��д���ڸ���*/

	CZc*      m_pzc;
};
#endif
