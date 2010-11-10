/*------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
 *Data��2010.09.29
 *Author����־��
 *Version��V1.1
 *File��packetprocess.h
 *Note��ƽ̨���ݰ�����ģ�飬�������ݰ�������ַ����������
-------------------------------------------------------------------------*/
#ifndef PACKETPROCESS_H
#define PACKETPROCESS_H

#include "InterfaceDataStruct.h"   
#include "platformhead.h"
#pragma pack(1)															/*�趨Ϊ1�ֽڶ���*/

class packetprocess      	
{
	
public:
	packetprocess();
	~packetprocess();
	
    void Decoder(PLAT_UINT8 *);												/*���ݰ��������*/
	void Encoder(PLAT_UINT8 *, PLAT_UINT8 *, int );							/*���ݰ��������*/
    PLAT_UINT32 GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 );		/*�õ����ݰ��и���Ԫ��Ŀ�ĵ�ID*/
	void HandlePlatData(PLAT_UINT32, PLAT_UINT8 *, PLAT_UINT8 *);			/*ƽ̨�˶�д���ݴ�����*/
	friend class CAppInterface;	
private:
	bool hasMsgHeader() ;
	PLAT_UINT32 ATO_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 ); /*�õ�ATO�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/

	PLAT_UINT32 ATP_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 ); /*�õ�ATP�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/

	PLAT_UINT32 RS_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 );  /*�õ�RS�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/

	PLAT_UINT32 ZC_GetDestID(PLAT_UINT32 ,PLAT_UINT8 *,int ,PLAT_UINT32 );  /*�õ�ZC�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/

private:

	T_DATA_INDEX	index;													/*���ݰ�����ͷ*/                     
    T_UNIT_HEAD		unithead;												/*���ݰ�����Ԫ�ĵ�Ԫͷ*/  
	PLAT_UINT32		id[256];												/*���ݰ�����Ԫ�ĵ�ԪID���ɵ�����*/
	PLAT_UINT32		unitsize[256];											/*���ݰ�����Ԫ�ĵ�Ԫ��С���ɵ�����*/
	PLAT_UINT8*		paddr; //�׵�ַ*/
	PLAT_UINT8*		pPlataddr;
	T_MESSAGE_HEAD	messageHeadOut;											/*���ݰ���ͨ�ű���������ݱ���ͷ*/
};

#endif