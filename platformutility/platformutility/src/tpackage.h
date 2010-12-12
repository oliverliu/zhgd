/*------------------------------------------------------------------------
*(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
*Data��2010.11.03
*Author��liuning	
*Version��v0.1
*File��InterfaceDataStruct.h
*Note����ƽ̨��صĽṹ��
--------------------------------------------------------------------------*/
#ifndef _T_PACKAGE_INTERFACEDATASTRUCT_H
#define _T_PACKAGE_INTERFACEDATASTRUCT_H

#include "platformhead.h"
#pragma pack(1)					/*�趨Ϊ1�ֽڶ��� */

/*------------------------------------------
* ����ÿ�����ݵ�Ԫ��������ID��Size�ṹ��
* -----------------------------------------*/
typedef struct
{
	PLAT_UINT32  unitId;		/*��ԪID*/       
	PLAT_UINT32  unitSize;		/*��Ԫ���ݴ�С*/
	
}T_UNIT_HEAD;


typedef struct
{
	PLAT_UINT32  unitId;		/*��ԪID*/       
	PLAT_UINT32  unitSize;		/*��Ԫ���ݴ�С*/
	// the address has limited, it must be follow unitSize in memory
	// the value is _ppack + sizeof(PLAT_BYTE) * sizeof(T_UNIT_HEAD)
	PLAT_BYTE*	 unitData;
}T_UNIT;


/*------------------------------------------
* ͨ�ű�������ͷ
* -----------------------------------------*/
typedef struct
{
	PLAT_UINT32 sequenceNum;	/*ͨ�ű������к�*/
	PLAT_UINT64 timeStamp;		/*ͨ�ű���ʱ���*/
	PLAT_UINT32 SID;			/*ͨ�ű���Դ��ַ*/
	PLAT_UINT32 DID;			/*ͨ�ű���Ŀ�ĵ�ַ*/
	PLAT_UINT32 msgType;		/*ͨ�ű�����Ϣ����*/
	PLAT_UINT32 msgLen;			/*ͨ�ű������ݴ�С*/
	
}T_MESSAGE_HEAD;

/*------------------------------------------
* ���ݼ�����
* -----------------------------------------*/
typedef struct
{
	PLAT_UINT32 regionUnitNum;		  /*��������Ԫ����������256*/
	PLAT_UINT32 platformStatus;		  /*ATO������״̬*/
	PLAT_UINT32 platformHealth;		  /*��������״̬*/
	PLAT_UINT32 unitAddrOffset[256];  /*���ݵ�Ԫ���ĵ�ַƫ��*/
	
}T_DATA_INDEX;

#endif
