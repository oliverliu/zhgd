/*------------------------------------------------------------------------
*(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
*Data��2010.11.03
*Author��liuning	
*Version��v0.1
*File��InterfaceDataStruct.h
*Note����ƽ̨��صĽṹ��
--------------------------------------------------------------------------*/
#ifndef INTERFACEDATASTRUCT_H
#define INTERFACEDATASTRUCT_H

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
	PLAT_UINT16 msgLen;			/*ͨ�ű������ݴ�С*/
	
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
* ����ATO���뵽ƽ̨���¼���¼�����ݵ����ݽṹ��δ����
* -----------------------------------------*/
typedef struct         
{
	PLAT_UINT32  eventCode;
	
}T_EVENT_DATA;



/*------------------------------------------
* ����һ����ʾͨ������״̬��������Ľṹ�壨������
* -----------------------------------------*/
typedef struct						
{
	PLAT_UINT8 CtrlRequest;		/*��������0x20-����δ����״̬��0x21-��������״̬��0x22-����ɾ��״̬��
								0x23-����������0x24-���ӳ�ʱ��0x25-���Ӵ���*/
}T_COM_LINK_IN;

/*------------------------------------------
* ����һ����ʾDB����״̬����Ľṹ��
* -----------------------------------------*/
typedef struct						
{
	PLAT_UINT32	addr;			/*DB�����׵�ַ*/
	PLAT_UINT32	size;			/*DB���ݴ�С*/
	PLAT_UINT32	version;		/*DB���ݰ汾��*/
	PLAT_UINT8	state;			/*DB״̬��0x10-���ڸ��£�0x11-���´���0x12-�޷�����
								0x50-DB����������0xF0-DB������Ч*/
}T_TRACK_DB_IN;

/*------------------------------------------
* ����һ����ʾDB���ݿ���������Ľṹ��
* -----------------------------------------*/
typedef struct					
{
	PLAT_UINT8	control;		/*DB���ݿ�������0x11-����ӱ��ظ���DB��0x12-������������*/
	PLAT_UINT8	reserve;		/*��������������*/
	PLAT_UINT32	dstID;			/*Ŀ���ַ*/
	PLAT_UINT8	fileName[18];	/*�ļ���*/
	
}T_DB_CTRL;

/*------------------------------------------
* Ӧ�ó����м�״̬ͬ��������ݣ���δ����
* -----------------------------------------*/
typedef struct
{
	
}T_SYNDATA_OUT;

#endif
