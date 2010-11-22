/************************************************************************/
/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
   ���ڣ� 2010.9.6
   ���ߣ� ����
   �汾�� v1
   ���ƣ� safeStruct.h
   ˵���� safein.c��ͷ�ļ����궨�塢�ṹ�嶨��                          */                                                                   
/************************************************************************/

#ifndef  _WINSOCK2_H
#define  _WINSOCK2_H

#include <WINSOCK2.H>

#endif


#ifndef  _SAFESTRUCT_H
#define  _SAFESTRUCT_H

#include <stdio.h>


/* �������� */
#define  uint8    unsigned char                        
#define  uint16   unsigned short int
#define  uint32   unsigned int


/* ʹ�õĵײ�Э�� */
#define  SAFESTRUCT_UDP                   1            /* �ײ�Э��: UDP */
#define  SAFESTRUCT_SERIAL                2            /* �ײ�Э��: SERIAL */


/* �������� */
#define  SAFESTRUCT_WITHCONNECTION        1            /* �������� */
#define  SAFESTRUCT_CONNECTIONLESS        2            /* ������ */


/* Э��汾�� */
#define  SAFESTRUCT_VERSION               0x0001            
   

/* ��������Ϣ */
#define  SAFESTRUCT_BUFFERSIZE            100          /* ���������ݲ����ֽ��� */
#define  SAFESTRUCT_RECVBUFFCNT           40           /* ���н��ջ��嵥Ԫ�� */
#define  SAFESTRUCT_SENDBUFFCNT           40           /* ���з��ͻ��嵥Ԫ�� */
#define  SAFESTRUCT_MAXTCBNUM             32           /* ���TCB��Ŀ */


/* ��ʱ����Ϣ */
/* ��ʱ����ʱ��� */
#define  SAFESTRUCT_TIMERGAP              20*1000  
    
/* ��ʱ����Ŀ */
#define  SAFESTRUCT_TIMERCNT              4   
    
/* ��ʱ����� */
/* �������Ӷ�ʱ�� */     
#define  SAFESTRUCT_INITTIMER             0   

/* ����ȷ�϶�ʱ�� */         
#define  SAFESTRUCT_ACKTIMER              1   

/* ������ʱ�� */         
#define  SAFESTRUCT_HEARTBEATTIMER        2    

/* ���ӿ��ж�ʱ�� */        
#define  SAFESTRUCT_STALEDATATIMER        3            




/* ��ʱ����������־ */
/* ����������ʱ�� */
#define  SAFESTRUCT_TIMER_OPEN               1

/* ��λ��ʱ�� */
#define  SAFESTRUCT_TIMER_RESET              2

/* �رն�ʱ�� */
#define  SAFESTRUCT_TIMER_CLOSE              3



/* ��ʱ����ֵ */
/* �������Ӷ�ʱ����ֵ */
#define  SAFESTRUCT_INITTIMERVALUE        180*1000    

/* �������ӳ��Դ��� */  
#define  SAFESTRUCT_INITRETRYTIME         3     


/* ����ȷ�϶�ʱ����ֵ */       
#define  SAFESTRUCT_ACKTIMERVALUE         180*1000    

/* �����ش����� */  
#define  SAFESTRUCT_MSGRETRYTIME          3           



/* ������ʱ����ֵ */
#define  SAFESTRUCT_HEARTTIMERVALUE       900*1000      

/* ���ӿ��ж�ʱ����ֵ */
#define  SAFESTRUCT_STALETIMERVALUE       1800*1000     


/* ���ȴ�����������Ŀ */
#define  SAFESTRUCT_MAXCALLCNT            3      


/* ���NAK���Ĵ��ʹ��� */      
#define  SAFESTRUCT_MAXNAKCNT             3            


#define  SAFESTRUCT_LISTHEAD              0             /* ����β��� */
#define  SAFESTRUCT_LISTTAIL              0             /* ����ͷ��� */


/* �������÷���ֵ */
#define  FAIL                            -1             /* ��������ʧ�� */
#define  SUCCESS                          0             /* �������óɹ� */

#define  SAFESTRUCT_FAIL                 -1             /* ��������ʧ�� */
#define  SAFESTRUCT_SUCCESS               0             /* �������óɹ� */
#define  SAFESTRUCT_ZERO                  0             /* ����������ֵ */
#define  SAFESTRUCT_TRUE                  1


 


/* ������� */
/* ��ʼ������ */
#define  SAFESTRUCT_INIT_MSG               0x01    

/* �ر����ӱ��� */      
#define  SAFESTRUCT_FIN_MSG                0x02   

/* ������Ҫ���ݱ��� */       
#define  SAFESTRUCT_VDATA_MSG              0x03     

/* ��������Ҫ���ݱ��� */
#define  SAFESTRUCT_NVDATA_MSG             0x04

/* ״̬���� */ 
#define  SAFESTRUCT_STATUS_MSG             0x05

/* �����źű��� */
#define  SAFESTRUCT_SERVICE_MSG            0x06    


/* ���ı�־λ */
/* ȷ�ϱ�־λ */
#define  SAFESTRUCT_FLAG_ACK                 0x0001

/* NAK��־λ */
#define  SAFESTRUCT_FLAG_NAK                 0x0002

/* Ҫ��ACK��־λ */
#define  SAFESTRUCT_FLAG_REQ_ACK             0x0004

/* Ҫ���ʼ����־λ */
#define  SAFESTRUCT_FLAG_REQ_INIT            0x0008

/* ʱ��У׼��־λ */
#define  SAFESTRUCT_FLAG_RST_TIMESTAMP       0x0010





/* ����������Ϊ��Ҫ���� */
#define    SAFESTRUCT_DATA_IMPORTANT           1

/* ����������Ϊ����Ҫ���� */
#define    SAFESTRUCT_DATA_UNIMPORTANT         0


/* CRCУ�����ʽֵ */
#define  SAFESTRUCT_CRC_POLYNOMIAL             0x04c11db7 


/* CRCУ��ֵ���� */
#define  SAFESTRUCT_CRC_LENGTH                 4


/* ��ʼ������Ԥ�������ֽ��� */
#define  SAFESTRUCT_INIT_DATARESVD_BYTES       59


/* �޸ģ�Ԥ���ֽ�������Ϊ��ʼ������һ���� 2010.10.27 */
/* ��ʼ���������ݲ��ֳ��� */
//#define  SAFESTRUCT_INITMSG_DATASIZE         2 + SAFESTRUCT_CRC_LENGTH + 1 + SAFESTRUCT_INIT_DATARESVD_BYTES
#define  SAFESTRUCT_INITMSG_DATASIZE           2 + SAFESTRUCT_CRC_LENGTH                                              
                                               

/* ��λָ������ʼ��������汾��Ϣλ�� */
#define  SAFESTRUCT_POINTO_INIT_VERSION(pData)    ((uint16 *) pData)

/* ��λָ������ʼ��������CRC��Ϣλ��*/
#define  SAFESTRUCT_POINTO_INIT_CRCSEED(pData)    ((uint32 *) (pData + 2))

/* ��λָ������ʼ�������屣���ֽ���λ�� */
#define  SAFESTRUCT_POINTO_INIT_RESVD(pData)      ((uint8 *) (pData + 2 + SAFESTRUCT_CRC_LENGTH))


/* ��ʼ�������屣���ֽ�λ�þ౨������ʼλ��ƫ�� */
#define  SAFESTRUCT_INIT_RSVD_DATA_OFFSET          7

/* ���ݱ����������ݴ��λ�õ�������ƫ�� */
#define  SAFESTRUCT_DATA_MSG_CONTENT_OFFSET        1



/* ��������󳤶� */
#define  SAFESTRUCT_HOSTNAME_MAX_LEN         128


/* ����ip��ַռ�ֽ��� */
#define  SAFESTRUCT_IPADDR_LEN                4


/* ������űȽ� */
#define  SAFESTRUCT_SEQ_LESS(p, q)            ((int) ((p) - (q)) < 0)  
#define  SAFESTRUCT_SEQ_EQUAL(p, q)           ((p) == (q))
							



/* ʱ����Ϣ */
typedef  struct   t_safeStruct_timestamp
{
	uint8    ucYear;
	uint8    ucMonth;
	uint8    ucDay;

	uint8    ucHour;
	uint8    ucMin;
	uint8    ucSec;

}TSafeStruct_TimeStamp;


/* ����ͷ���ṹ */
typedef  struct   t_safeStruct_msgHead
{
	uint8    ucMsgType;                    /* ��Ϣ��� */
	uint16   usMsgFlag;                    /* ���ı�־λ */

	uint16   usSrcAddr;                    /* Դ�˵�ַ */
	uint16   usDstAddr;                    /* Ŀ�ĵ�ַ */

	uint8    ucTxSeq;                      /* �������к� */
	uint8    ucRxSeq;                      /* �ڴ��������к� */

	TSafeStruct_TimeStamp    tTimeStamp;   /* ʱ����Ϣ */

	char     cReserved;                    /* ����λ */

}TSafeStruct_MsgHead;




/* �������ṹ */
typedef  struct   t_safeStruct_buff
{
	struct t_safeStruct_buff *   ptPreBuff;
	struct t_safeStruct_buff *   ptNextBuff;
       
	     TSafeStruct_MsgHead     tMsgHead;                  /* ����ͷ�ṹ */

	             int    iSockFD;		                    /* Ӧ�ò�safeSock������ */
				char    arrData[SAFESTRUCT_BUFFERSIZE];     /* ���������� */
	          uint32    uiCRCResult;                        /* CRCУ��ֵ */
              uint16    uhLen;                              /* arrData��������Ч���ݳ��� */

}TSafeStruct_Buff; 



/* �������ͷ�ṹ */
typedef  struct   t_safeStruct_buffQueHead
{
	TSafeStruct_Buff *  ptBuffHead;
	TSafeStruct_Buff *  ptBuffTail;

	          uint32    uiBuffNum;
	          HANDLE    hBuffMutex;

}TSafeStruct_BuffQueHead;


/* tcbʹ��״̬ */
typedef  enum
{
	unalloc_tcb,
	unused_tcb,
	used_tcb,
	call_tcb

}ESafeStruct_TcbState;


/* Э��״̬ */
typedef  enum
{
	sockState_closed,       
	sockState_listen,
	sockState_init_recv,
	sockState_init_sent,
	sockState_estab,
	sockState_reinit,	
	sockState_close_wait,
	sockState_fin_wait,
	sockState_closing

}ESafeStruct_SockState;



/* �������� */
typedef  enum
{
	noError,
	neligibleError,
	fatalError

}ESafeStruct_ErrState;



/* ��ʱ���ṹ */
typedef  struct  t_safeStruct_timer
{
	int   iTime;
	bool  iIsUsed;
	char  cRelatedNum;
	
}TSafeStruct_Timer;




typedef  struct  t_safeStruct_option
{
	int   iLinkState;       /* ��·״̬�� SS_LINK_CONNECT��   ��·���������У����Է������ݣ�
							              SS_LINK_DISCONNECT����·δ�������ӣ�
										  SS_LINK_ERROR��     ��·���� */

	bool  bNonBlock;        /* socketģʽ�� TRUE��������
							                FALSE:  ���� */

}TSafeStruct_Option;






/* ������ƿ�ṹ */
typedef  struct   t_safeStruct_tcb
{
	SOCKET                  sockID;	 	
	ESafeStruct_SockState   eSockState;     /* socket״̬ */
	int                     iErrNum;        /* socket������ */
	HANDLE                  hErrMutex;      /* �޸Ĵ�����ʹ�õĻ����� */

    ESafeStruct_ErrState    eErrState;      /* ���Ӵ���״̬ */

	SOCKADDR_IN             tSrcSockAddr;
	SOCKADDR_IN             tDestSockAddr;
	uint8                   uiSrcAddrLen;
	uint8                   uiDestAddrLen;

	char      carrSrcHostName[SAFESTRUCT_HOSTNAME_MAX_LEN];
	char      carrDstHostName[SAFESTRUCT_HOSTNAME_MAX_LEN];

    uint32    uiCrcRxSeed;
	uint32    uiCrcTxSeed;

	uint8     cTxSeq;
	uint8     cRxSeq;

	HANDLE               hTimerMutex;                      /* ��ʱ������������ */
	TSafeStruct_Timer    tarrTimer[SAFESTRUCT_TIMERCNT];

	TSafeStruct_BuffQueHead   tWaitSendBuffHead;
	TSafeStruct_BuffQueHead   tWaitAckBuffHead;
	TSafeStruct_BuffQueHead   tRecvBuffHead;

	bool     bPermitSend;
	HANDLE   hPermitMutex;

	uint8    ucExcept;                                      /* Э��㷢�������־ */
	uint8    ucReadIsOk;                                    /* Ӧ�ò���Զ�ȡ���� */
	uint8    ucWriteIsOk;                                   /* Ӧ�ò���Է������� */

	/* selectʹ�õı�־�� ���� 2010.11.12 */
	HANDLE   hSelectionFlagMutex;                              

	TSafeStruct_Option   tOption;                           /* socket����ѡ�� */

	uint8    ucVitalData;                                   /* ��Ҫ���ݱ�־λ */
	uint8    ucMaxNakCnt;                                   /* ������������NAK���� */

}TSafeStruct_Tcb;




/* ���������Ϣ */
typedef  struct   t_safeStruct_tcbInfo
{
	int  iPre;
	int  iNext;

	TSafeStruct_Tcb *      ptTcb;
	ESafeStruct_TcbState   eTcbState;

	uint8    uiProtocol;         /* ʹ�õĵײ�Э�� */
	uint16   uiProVersion;       /* Э��汾�� */
	uint8    uiConnType;         /* �������� */

	uint8    uiMaxCalCnt;        /* ������뽨�������� */
	HANDLE   hMaxCalMutex;
	uint8    uiCalCnt;           /* ���ڽ����������� */

	bool     bIsVirtual;
	int      iSubSockLink;

}TSafeStruct_TcbInfo;  



/* ȫ��������Դ��������Ϣ */
typedef  struct   t_safeStruct_service
{
	TSafeStruct_TcbInfo   tarrTcbPool[SAFESTRUCT_MAXTCBNUM + 1];

	int    iUnalloc;          /* δʹ�õ�TCB����ͷ��� */
	int    iUsed;             /* ʹ�õ�TCB����ͷ��� */
	int    iUnused;           /* �ͷŵ�TCB����ͷ��� */
	UINT   uiTcbUsedCnt;   

	TSafeStruct_BuffQueHead  tFreeSendBuffHead;    /* ���з��ͻ������ͷ */
	TSafeStruct_BuffQueHead  tFreeRecvBuffHead;    /* ���н��ջ������ͷ */
	TSafeStruct_BuffQueHead  tNextSendBuffHead;    /* ׼�����Ͷ���ͷ */

	bool     bCloseThread;
	HANDLE   hLinkMutex;

}TSafeStruct_Service;





/* ������Ϣ�ṹ */
typedef  struct  t_safeStruct_cfgData
{
	uint8    ucSocketCnt;                 /* �������socket���� */
	uint8    ucSendBuffCnt;               /* ȫ�����ӿ��л������ */
	uint8    ucRecvBuffCnt;               /* ȫ�����ӿ��н��ջ������ */
	uint8    ucMaxNakCnt;                 /* ��������NAK���� */
	uint8    ucMaxCallCnt;                /* ��������������� */
    
	int      iLog;                        /* ��־��Ҫ��¼���������� */

	TSafeStruct_Timer    tarrTimerCfg[SAFESTRUCT_TIMERCNT];
	TSafeStruct_Option   tOption;

	/* �������� */
	uint8    ucMax_wait_send_que_lenth;   /* �ȴ����Ͷ�����󳤶� */
	uint8    ucMax_wait_recv_que_lenth;   /* �ȴ����ն�����󳤶� */

}TSafeStruct_CfgData;




/* ��Ϣ������־λ */
typedef  struct   t_safeStruct_msgOpt
{
	uint8    ucMsgType;
	uint16   uiMsgFlag;

}TSafeStruct_MsgOpt;





/* ʱ�������ֵ */
typedef  struct   t_safeStruct_time_rand
{
	struct tm *    ptSysTime;            /* ϵͳʱ�� */
	uint32         uiRand;               /* ����ϵͳʱ����������ֵ */

}TSafeStruct_Time_Rand;



/* ȫ�ֱ��� */
extern  TSafeStruct_Service *  gptService;    /* ȫ��������Դ��������Ϣ */
extern  TSafeStruct_CfgData *  gptCfgData;    /* ȫ������������Ϣ */

extern  HANDLE   ghReqSend;                   /* ���ѷ����߳��ź��� */
extern  HANDLE   ghChange;                    /* ����safeSocket_select()�����ź��� */


extern  HANDLE   ghFile;                      /* �ļ���������� */
extern  FILE *   gpLogFile;                   /* ��־�ļ�ָ�� */




#endif