#ifndef ZCDataStruct
#define ZCDataStruct

#include "../platform/platformhead.h"

//#include "platformlib\platformhead.h"
#pragma pack(1)					/*�趨Ϊ1�ֽڶ��� */

/*------------ZC���͵�ͨ�ű��ĵı������Ͷ���------------*/
#define  MT_ZC_TO_CC     0x00014101
#define  MT_CC_TO_ZC     0x00011401 
#define  MT_ZC_TO_CBI	 0x00014201
#define  MT_CBI_TO_ZC    0x00012401
#define  MT_ZC_TO_ATS    0x00014301
#define  MT_ATS_TO_ZC    0x00013401
#define  MT_DSU_TO_ZC    0x00015401


//ZC���͸�ATP�����������ݽṹ��
typedef struct
{
	PLAT_UINT32	seqNo;		//�������
	PLAT_UINT32	trainId;	//Ŀ���г�ID
	PLAT_UINT8	EbCmd;	    //����ͣ�����EB(0xAA)����EB(0x55)
	PLAT_UINT8	stopAssReq;	//ͣ����֤����������0xAA����������0x55��
	PLAT_UINT32	stopSigId;	//ͣ���źŻ����
	PLAT_UINT8	MalDir;		//MAL����0xAA��������0x55��
	PLAT_UINT32	MalSecId;	//MAL��λ�ã�����ID
	PLAT_UINT32	MalOffset;	//MAL��λ�ã������е�ƫ�ƣ���λ������
	PLAT_UINT8	MalEndType;	//MAL�յ�����

}ZC_TO_CCCIR; 

//ZC���͸�ATP�Ĺ㲥���ݽṹ��
typedef struct
{
	PLAT_UINT32		gdDbVer;	  		//������ݿ�汾
	PLAT_UINT32		tsrDbVer;			//��ʱ�������ݿ�汾
	PLAT_UINT8		swNum;				//������Ŀ
	PLAT_UINT16		swId[5];  			//����ID
	PLAT_UINT8		swState[5];	  		//����λ(0xAA)����λ��0x55��,ʧȥ��ʾ0xA5
	PLAT_UINT8		stationNum;			//վ̨��Ŀ
	PLAT_UINT16		stationId[3];		//վ̨ID
	PLAT_UINT8		stationState[3];	//վ̨״̬��1�����в�����0�����޲���

}ZC_TO_CCBRODE;

#endif