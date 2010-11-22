/************************************************************************/
/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
   日期： 2010.9.6
   作者： 杨明
   版本： v1
   名称： safeStruct.h
   说明： safein.c的头文件，宏定义、结构体定义                          */                                                                   
/************************************************************************/

#ifndef  _WINSOCK2_H
#define  _WINSOCK2_H

#include <WINSOCK2.H>

#endif


#ifndef  _SAFESTRUCT_H
#define  _SAFESTRUCT_H

#include <stdio.h>


/* 数据类型 */
#define  uint8    unsigned char                        
#define  uint16   unsigned short int
#define  uint32   unsigned int


/* 使用的底层协议 */
#define  SAFESTRUCT_UDP                   1            /* 底层协议: UDP */
#define  SAFESTRUCT_SERIAL                2            /* 底层协议: SERIAL */


/* 连接类型 */
#define  SAFESTRUCT_WITHCONNECTION        1            /* 面向连接 */
#define  SAFESTRUCT_CONNECTIONLESS        2            /* 无连接 */


/* 协议版本号 */
#define  SAFESTRUCT_VERSION               0x0001            
   

/* 缓冲区信息 */
#define  SAFESTRUCT_BUFFERSIZE            100          /* 缓冲区数据部分字节数 */
#define  SAFESTRUCT_RECVBUFFCNT           40           /* 空闲接收缓冲单元数 */
#define  SAFESTRUCT_SENDBUFFCNT           40           /* 空闲发送缓冲单元数 */
#define  SAFESTRUCT_MAXTCBNUM             32           /* 最大TCB数目 */


/* 定时器信息 */
/* 定时器定时间隔 */
#define  SAFESTRUCT_TIMERGAP              20*1000  
    
/* 定时器数目 */
#define  SAFESTRUCT_TIMERCNT              4   
    
/* 定时器类别 */
/* 建立连接定时器 */     
#define  SAFESTRUCT_INITTIMER             0   

/* 报文确认定时器 */         
#define  SAFESTRUCT_ACKTIMER              1   

/* 心跳定时器 */         
#define  SAFESTRUCT_HEARTBEATTIMER        2    

/* 连接空闲定时器 */        
#define  SAFESTRUCT_STALEDATATIMER        3            




/* 定时器操作类别标志 */
/* 重新启动定时器 */
#define  SAFESTRUCT_TIMER_OPEN               1

/* 复位定时器 */
#define  SAFESTRUCT_TIMER_RESET              2

/* 关闭定时器 */
#define  SAFESTRUCT_TIMER_CLOSE              3



/* 定时器初值 */
/* 建立连接定时器初值 */
#define  SAFESTRUCT_INITTIMERVALUE        180*1000    

/* 建立连接尝试次数 */  
#define  SAFESTRUCT_INITRETRYTIME         3     


/* 报文确认定时器初值 */       
#define  SAFESTRUCT_ACKTIMERVALUE         180*1000    

/* 报文重传次数 */  
#define  SAFESTRUCT_MSGRETRYTIME          3           



/* 心跳定时器初值 */
#define  SAFESTRUCT_HEARTTIMERVALUE       900*1000      

/* 连接空闲定时器初值 */
#define  SAFESTRUCT_STALETIMERVALUE       1800*1000     


/* 最大等待建立连接数目 */
#define  SAFESTRUCT_MAXCALLCNT            3      


/* 最大NAK报文传送次数 */      
#define  SAFESTRUCT_MAXNAKCNT             3            


#define  SAFESTRUCT_LISTHEAD              0             /* 链表尾编号 */
#define  SAFESTRUCT_LISTTAIL              0             /* 链表头编号 */


/* 函数调用返回值 */
#define  FAIL                            -1             /* 函数调用失败 */
#define  SUCCESS                          0             /* 函数调用成功 */

#define  SAFESTRUCT_FAIL                 -1             /* 函数调用失败 */
#define  SAFESTRUCT_SUCCESS               0             /* 函数调用成功 */
#define  SAFESTRUCT_ZERO                  0             /* 函数返回零值 */
#define  SAFESTRUCT_TRUE                  1


 


/* 报文类别 */
/* 初始化报文 */
#define  SAFESTRUCT_INIT_MSG               0x01    

/* 关闭连接报文 */      
#define  SAFESTRUCT_FIN_MSG                0x02   

/* 包含重要数据报文 */       
#define  SAFESTRUCT_VDATA_MSG              0x03     

/* 包含非重要数据报文 */
#define  SAFESTRUCT_NVDATA_MSG             0x04

/* 状态报文 */ 
#define  SAFESTRUCT_STATUS_MSG             0x05

/* 服务信号报文 */
#define  SAFESTRUCT_SERVICE_MSG            0x06    


/* 报文标志位 */
/* 确认标志位 */
#define  SAFESTRUCT_FLAG_ACK                 0x0001

/* NAK标志位 */
#define  SAFESTRUCT_FLAG_NAK                 0x0002

/* 要求ACK标志位 */
#define  SAFESTRUCT_FLAG_REQ_ACK             0x0004

/* 要求初始化标志位 */
#define  SAFESTRUCT_FLAG_REQ_INIT            0x0008

/* 时钟校准标志位 */
#define  SAFESTRUCT_FLAG_RST_TIMESTAMP       0x0010





/* 待发送数据为重要数据 */
#define    SAFESTRUCT_DATA_IMPORTANT           1

/* 待发送数据为非重要数据 */
#define    SAFESTRUCT_DATA_UNIMPORTANT         0


/* CRC校验多项式值 */
#define  SAFESTRUCT_CRC_POLYNOMIAL             0x04c11db7 


/* CRC校验值长度 */
#define  SAFESTRUCT_CRC_LENGTH                 4


/* 初始化报文预留数据字节数 */
#define  SAFESTRUCT_INIT_DATARESVD_BYTES       59


/* 修改：预留字节数不作为初始化报文一部分 2010.10.27 */
/* 初始化报文数据部分长度 */
//#define  SAFESTRUCT_INITMSG_DATASIZE         2 + SAFESTRUCT_CRC_LENGTH + 1 + SAFESTRUCT_INIT_DATARESVD_BYTES
#define  SAFESTRUCT_INITMSG_DATASIZE           2 + SAFESTRUCT_CRC_LENGTH                                              
                                               

/* 定位指针至初始化报文体版本信息位置 */
#define  SAFESTRUCT_POINTO_INIT_VERSION(pData)    ((uint16 *) pData)

/* 定位指针至初始化报文体CRC信息位置*/
#define  SAFESTRUCT_POINTO_INIT_CRCSEED(pData)    ((uint32 *) (pData + 2))

/* 定位指针至初始化报文体保留字节数位置 */
#define  SAFESTRUCT_POINTO_INIT_RESVD(pData)      ((uint8 *) (pData + 2 + SAFESTRUCT_CRC_LENGTH))


/* 初始化报文体保留字节位置距报文体起始位置偏移 */
#define  SAFESTRUCT_INIT_RSVD_DATA_OFFSET          7

/* 数据报文体中数据存放位置到报文体偏移 */
#define  SAFESTRUCT_DATA_MSG_CONTENT_OFFSET        1



/* 主机名最大长度 */
#define  SAFESTRUCT_HOSTNAME_MAX_LEN         128


/* 整型ip地址占字节数 */
#define  SAFESTRUCT_IPADDR_LEN                4


/* 报文序号比较 */
#define  SAFESTRUCT_SEQ_LESS(p, q)            ((int) ((p) - (q)) < 0)  
#define  SAFESTRUCT_SEQ_EQUAL(p, q)           ((p) == (q))
							



/* 时钟信息 */
typedef  struct   t_safeStruct_timestamp
{
	uint8    ucYear;
	uint8    ucMonth;
	uint8    ucDay;

	uint8    ucHour;
	uint8    ucMin;
	uint8    ucSec;

}TSafeStruct_TimeStamp;


/* 报文头部结构 */
typedef  struct   t_safeStruct_msgHead
{
	uint8    ucMsgType;                    /* 消息类别 */
	uint16   usMsgFlag;                    /* 报文标志位 */

	uint16   usSrcAddr;                    /* 源端地址 */
	uint16   usDstAddr;                    /* 目的地址 */

	uint8    ucTxSeq;                      /* 发送序列号 */
	uint8    ucRxSeq;                      /* 期待接收序列号 */

	TSafeStruct_TimeStamp    tTimeStamp;   /* 时钟信息 */

	char     cReserved;                    /* 保留位 */

}TSafeStruct_MsgHead;




/* 缓冲区结构 */
typedef  struct   t_safeStruct_buff
{
	struct t_safeStruct_buff *   ptPreBuff;
	struct t_safeStruct_buff *   ptNextBuff;
       
	     TSafeStruct_MsgHead     tMsgHead;                  /* 报文头结构 */

	             int    iSockFD;		                    /* 应用层safeSock索引号 */
				char    arrData[SAFESTRUCT_BUFFERSIZE];     /* 报文数据域 */
	          uint32    uiCRCResult;                        /* CRC校验值 */
              uint16    uhLen;                              /* arrData数组中有效内容长度 */

}TSafeStruct_Buff; 



/* 缓冲队列头结构 */
typedef  struct   t_safeStruct_buffQueHead
{
	TSafeStruct_Buff *  ptBuffHead;
	TSafeStruct_Buff *  ptBuffTail;

	          uint32    uiBuffNum;
	          HANDLE    hBuffMutex;

}TSafeStruct_BuffQueHead;


/* tcb使用状态 */
typedef  enum
{
	unalloc_tcb,
	unused_tcb,
	used_tcb,
	call_tcb

}ESafeStruct_TcbState;


/* 协议状态 */
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



/* 错误类型 */
typedef  enum
{
	noError,
	neligibleError,
	fatalError

}ESafeStruct_ErrState;



/* 定时器结构 */
typedef  struct  t_safeStruct_timer
{
	int   iTime;
	bool  iIsUsed;
	char  cRelatedNum;
	
}TSafeStruct_Timer;




typedef  struct  t_safeStruct_option
{
	int   iLinkState;       /* 链路状态， SS_LINK_CONNECT：   链路处于连接中，可以发送数据；
							              SS_LINK_DISCONNECT：链路未建立连接；
										  SS_LINK_ERROR：     链路出错 */

	bool  bNonBlock;        /* socket模式， TRUE：非阻塞
							                FALSE:  阻塞 */

}TSafeStruct_Option;






/* 传输控制块结构 */
typedef  struct   t_safeStruct_tcb
{
	SOCKET                  sockID;	 	
	ESafeStruct_SockState   eSockState;     /* socket状态 */
	int                     iErrNum;        /* socket错误码 */
	HANDLE                  hErrMutex;      /* 修改错误码使用的互斥锁 */

    ESafeStruct_ErrState    eErrState;      /* 连接错误状态 */

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

	HANDLE               hTimerMutex;                      /* 定时器操作互斥量 */
	TSafeStruct_Timer    tarrTimer[SAFESTRUCT_TIMERCNT];

	TSafeStruct_BuffQueHead   tWaitSendBuffHead;
	TSafeStruct_BuffQueHead   tWaitAckBuffHead;
	TSafeStruct_BuffQueHead   tRecvBuffHead;

	bool     bPermitSend;
	HANDLE   hPermitMutex;

	uint8    ucExcept;                                      /* 协议层发生错误标志 */
	uint8    ucReadIsOk;                                    /* 应用层可以读取数据 */
	uint8    ucWriteIsOk;                                   /* 应用层可以发送数据 */

	/* select使用的标志锁 增加 2010.11.12 */
	HANDLE   hSelectionFlagMutex;                              

	TSafeStruct_Option   tOption;                           /* socket参数选项 */

	uint8    ucVitalData;                                   /* 重要数据标志位 */
	uint8    ucMaxNakCnt;                                   /* 允许连续发送NAK次数 */

}TSafeStruct_Tcb;




/* 传输控制信息 */
typedef  struct   t_safeStruct_tcbInfo
{
	int  iPre;
	int  iNext;

	TSafeStruct_Tcb *      ptTcb;
	ESafeStruct_TcbState   eTcbState;

	uint8    uiProtocol;         /* 使用的底层协议 */
	uint16   uiProVersion;       /* 协议版本号 */
	uint8    uiConnType;         /* 连接类型 */

	uint8    uiMaxCalCnt;        /* 最大申请建立连接数 */
	HANDLE   hMaxCalMutex;
	uint8    uiCalCnt;           /* 正在建立的连接数 */

	bool     bIsVirtual;
	int      iSubSockLink;

}TSafeStruct_TcbInfo;  



/* 全部连接资源、控制信息 */
typedef  struct   t_safeStruct_service
{
	TSafeStruct_TcbInfo   tarrTcbPool[SAFESTRUCT_MAXTCBNUM + 1];

	int    iUnalloc;          /* 未使用的TCB链表头编号 */
	int    iUsed;             /* 使用的TCB链表头编号 */
	int    iUnused;           /* 释放的TCB链表头编号 */
	UINT   uiTcbUsedCnt;   

	TSafeStruct_BuffQueHead  tFreeSendBuffHead;    /* 空闲发送缓冲队列头 */
	TSafeStruct_BuffQueHead  tFreeRecvBuffHead;    /* 空闲接收缓冲队列头 */
	TSafeStruct_BuffQueHead  tNextSendBuffHead;    /* 准备发送队列头 */

	bool     bCloseThread;
	HANDLE   hLinkMutex;

}TSafeStruct_Service;





/* 配置信息结构 */
typedef  struct  t_safeStruct_cfgData
{
	uint8    ucSocketCnt;                 /* 允许最大socket个数 */
	uint8    ucSendBuffCnt;               /* 全部连接空闲缓冲个数 */
	uint8    ucRecvBuffCnt;               /* 全部连接空闲接收缓冲个数 */
	uint8    ucMaxNakCnt;                 /* 连续发送NAK次数 */
	uint8    ucMaxCallCnt;                /* 最大建立连接请求数 */
    
	int      iLog;                        /* 日志需要记录的数据类型 */

	TSafeStruct_Timer    tarrTimerCfg[SAFESTRUCT_TIMERCNT];
	TSafeStruct_Option   tOption;

	/* 流量控制 */
	uint8    ucMax_wait_send_que_lenth;   /* 等待发送队列最大长度 */
	uint8    ucMax_wait_recv_que_lenth;   /* 等待接收队列最大长度 */

}TSafeStruct_CfgData;




/* 消息类别与标志位 */
typedef  struct   t_safeStruct_msgOpt
{
	uint8    ucMsgType;
	uint16   uiMsgFlag;

}TSafeStruct_MsgOpt;





/* 时间与随机值 */
typedef  struct   t_safeStruct_time_rand
{
	struct tm *    ptSysTime;            /* 系统时间 */
	uint32         uiRand;               /* 根据系统时间产生的随机值 */

}TSafeStruct_Time_Rand;



/* 全局变量 */
extern  TSafeStruct_Service *  gptService;    /* 全部连接资源、管理信息 */
extern  TSafeStruct_CfgData *  gptCfgData;    /* 全部连接配置信息 */

extern  HANDLE   ghReqSend;                   /* 唤醒发送线程信号量 */
extern  HANDLE   ghChange;                    /* 唤醒safeSocket_select()函数信号量 */


extern  HANDLE   ghFile;                      /* 文件输出互斥量 */
extern  FILE *   gpLogFile;                   /* 日志文件指针 */




#endif