/*---------------------------------------------------------------------------
*(C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS *RESERVED
*Data：2010.8.30
*Author：zhanghaihai
*Version：v0.01
*File：safestruct.h
*Note：safein.cpp的头文件，内部接口函数和结构体的定义。
*--------------------------------------------------------------------------*/
#ifndef _SAFESTRUCT_H
#define _SAFESTRUCT_H

#include <stdio.h>						/*IO-头文件*/
#include <stdlib.h>					    /*标准库-文件*/
#include <string.h>

#include "platform.h"


#if  defined (LINUX_PLAT) || defined (VXWORKS_PLAT)
#include <sys/socket.h>					/*socket-头文件*/
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>					/*线程-头文件*/
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>

#endif


#define SUCCESS			0				/*返回 0表示成功*/
#define FAILURE		   -1				/*返回-1表示失败*/
#define TRUE			1				/*返回 1表示正确*/
#define FALSE			0				/*返回 0表示错误*/

typedef unsigned char  uint8;			            /*8位无符号字符型*/
typedef unsigned short uint16, uint16_t;			/*16位无符号短整型*/
typedef unsigned int   uint32, uint32_t;			/*32位无符号整型*/

/*-----------------------------宏定义--------------------------------------*/
#define HOSTNAME_MAX_LEN		   128			/*主机名称的最大长度*/
#define ERRDESP_MAX_LEN			   128			/*错误描述的最大长度*/
#define TCB_MAX_NUM				   128    	   	/*每个进程最多能打开的socket数量*/
#define SENDBUFF_MAX_NUM	(TCB_MAX_NUM*100)	/*协议发送缓冲区总数量*/
#define RECVBUFF_MAX_NUM	(TCB_MAX_NUM*100)   /*协议接收缓冲区总数量*/
#define SENDBUFF_NUM	(3*20)				/*每个socket允许的等待发送冲区数量, 流量控制*/
#define RECVBUFF_NUM	(3*20)				/*每个socket允许的等待接收缓冲区数量, 流量控制*/
#define MSG_HEAD_SIZE				16          /*消息头部大小*/
#define MSG_DATA_SIZE				59			/*数据的最大值*/
#define MSG_DATAFIELD_MAX_SIZE		59          /*消息体最大值*/
#define CRC_LEN						 4			/*crc码长度*/
												/*状态消息、service消息长度*/
#define  STATUS_SERVICE_MSG_SIZE  MSG_HEAD_SIZE+CRC_LEN 
												/*数据消息最大长度*/    
#define  DATA_MSG_MAX_SIZE  MSG_HEAD_SIZE+ MSG_DATAFIELD_MAX_SIZE+ CRC_LEN  

#define UDP					0x00000001			/*UDP协议的表示*/
#define SERIAL				0x00000002			/*Serial串口的表示*/

#define SAFESOCK_WITHCONNECTION		 1          /*网络有连接*/
#define SAFESOCK_CONNECTLESS		 2			/*网络无连接*/


/*MESSAGE TYPE：信息类型*/
#define INIT_MSG		0x01			/*Data Initialization Message*/
#define FIN_MSG			0x02            /*FIN message*/
#define VDATA_MSG		0x03            /*Vital Data Message*/
#define NVDATA_MSG		0x04			/*Non-Vital Data Message*/
#define STATUS_MSG		0x05			/*Peer Statue Message*/
#define SERVICE_MSG		0x06            /*Service signal Message*/
#define ERROR_MSG		0x07			/*Error Message*/

/*MESSAGE FLAGS: 消息标志*/
#define ACK				0x0001			/*成功接收到信息后，返回的确认*/
#define NAK				0x0002			/*传输出错的发送的信息*/
#define ACK_REQ			0x0004			/*要求接收方发送确认信息*/
#define INIT_REQ		0x0008			/*要求接受方发送初始化信息*/
#define TIMESTAMP_RST	0X0010			/*用时间戳来重置接收方的系统时钟*/
#define FORBID_FLAG		0X0020			/*不允许对方发送数据,流量控制*/
#define PERMIT_FLAG		0X0040			/*允许对方发送数据,流量控制*/

/*TIMER TYPES:定时器类型*/
#define TIMER_NUM		  4				/*定时器个数*/
#define INIT_TIMER		  0				/*初始化连接超时定时器*/
#define WAITACK_TIMER	  1				/*等待ACK定时器*/
#define HEARTBEAT_TIMER	  2				/*保活定时器*/
#define DATASTALE_TIMER   3				/*链路过期定时器*/

/*定时器操作的类型*/
#define TIMER_OPEN		  1				/*定时器开启操作*/
#define TIMER_RESET		  2				/*定时器复位操作*/
#define TIMER_CLOSE		  3				/*定时器关闭操作*/

/*定时器缺省时间，具体时间需要调整*/
#define TIMEGAP				10			/*每隔TIMEGAP MS检查一次所有套接字的定时器*/
#define RETRY_INIT_NUM		5			/*尝试建立连接的次数，默认是5*/
#define RETRY_DATA_NUM		3			/*未收到ACK数据是的重发次数，默认是3*/
#define UPDATE_STATUS_NUM	2			/*更新周期，默认是2*/
#define RETRY_NAK_NUM       3			/*收到数据出错，重发NAK次数，默认为3*/

#define TIME_INIT		3000			/* 连接超时时间，默认为100ms */
#define TIME_WAITACK		6000			/* 应答超时时间，默认为100ms */
#define TIME_HEARTBEAT		9000			/* 心跳间隔时间，默认为3000ms */
#define TIME_DATASTALE		10000			/* 数据过期时间，默认为8000ms */

/*错误代码*/
#define SS_BASEERR			50000				/*safesocket错误码从50000开始*/
#define SS_EBADFD			(SS_BASEERR + 1)    /*send or recv on a locall closed socket*/
#define SS_ENOTSOCKET		(SS_BASEERR + 2)	/*Socket operation on non-socket*/
#define SS_EACCES			(SS_BASEERR + 3)	/*Permission denied*/
#define SS_EBADADDR			(SS_BASEERR + 4)	/*Bad address*/
#define SS_EINVAL			(SS_BASEERR + 5)	/*Invalid argument*/
#define SS_ETMSOCK			(SS_BASEERR + 6)	/*Too many open socket*/
#define SS_EWOULDBLOCK		(SS_BASEERR + 7)	/*Resource temporarily unavailable*/
#define SS_ESTATE			(SS_BASEERR + 8)	/*can not execute the operation in current state*/
#define SS_EDESTADDRREQ		(SS_BASEERR + 9)	/*Destination address required*/
#define SS_EMSGSIZE			(SS_BASEERR + 10)	/*Message too long*/
#define SS_EPROTOTYPE		(SS_BASEERR + 11)	/*Protocol wrong type for socket*/
#define SS_ENOPROTOOPT		(SS_BASEERR + 12)	/*Bad protocol option*/
#define SS_EPROTONOSUPPORT	(SS_BASEERR + 13)	/*Protocol not supported*/
#define SS_ESOCKNOSUPPORT	(SS_BASEERR + 14)	/*Socket type not supported*/
#define SS_EOPNOTSUPP		(SS_BASEERR + 15)	/*Operation not supported*/
#define SS_ENETRESET		(SS_BASEERR + 16)	/*Network dropped connection on reset*/
#define SS_ECONNABORTED		(SS_BASEERR + 17)	/*Software caused connection abort*/
#define SS_ECONNRESET		(SS_BASEERR + 18)	/*Connection reset by peer*/
#define SS_ENOBUFS			(SS_BASEERR + 19)	/*No buffer space available*/
#define SS_EISCONN			(SS_BASEERR + 20)	/*Socket is not conneted*/
#define SS_ENOTCONN			(SS_BASEERR + 21)	/*Socket is not connected*/
#define SS_ETIMEDOUT		(SS_BASEERR + 22)	/*Timed out*/
#define SS_ECONNRERUSED		(SS_BASEERR + 23)	/*Connection refused*/
#define SS_EHOSTDOWN		(SS_BASEERR + 24)	/*Host is down*/
#define SS_ELIMITCALL		(SS_BASEERR + 25)   /*Too many call*/
#define SS_EUSERS			(SS_BASEERR + 26)	/*Too many users*/
#define SS_EDISCON			(SS_BASEERR + 27)	/*Graceful shuudown in progress*/
#define SS_ENOTINIT			(SS_BASEERR + 28)	/*Successful startProtocol not yet performed*/
#define SS_NOCONNECTREQ		(SS_BASEERR + 29)	/*No connection request*/

/*输出的错误信息类型*/
#define LOG_MSG_A			1					/*A类输出信息*/
#define LOG_MSG_B			2					/*B类输出信息*/

/*有错误，写log*/
#define FORMAT_E "%s  %d  %s  "
/*没有错误，写log*/
#define FORMAT_R "%s  %s  "

/*数据的类型*/
#if 0
#define VITAL_DATA			1			/*重要数据*/
#define NON_VITAL_DATA		0			/*非重要数据*/
#endif

/*是否关闭定时器，收发线程*/
#define NON_CLOSE_THREAD    0			/*不关闭定时器，收发线程*/
#define CLOSE_THREAD		1			/*关闭定时器，收发线程*/

/*是否允许发送数据*/
#define PERMIT_SEND			1			/*允许发送数据*/
#define NON_PERMIT_SEND		0			/*不允许发送数据*/

/*s是否为重发的数据*/
#define RETRY_MSGFLAGS		0			/*重发的数据*/
#define NON_RETRY_MSGFLAGS  1			/*非重发的数据*/

/*pdata检验去空操作*/
//#define OFFSET	 1						/*偏移字节数*/

/*输出的程序信息*/
#define A_SYSMSG	0					/*A消息*/
#define B_SYSMSG	1					/*B消息*/

/*-------------------------------------------------------------------------*/
typedef enum				            /*tcb状态*/
{					
	TCB_UNALLOC,	/*未分配TCB*/
	TCB_USED,	/*在使用中的TCB*/
	TCB_UNUSED,	/*已分配socket但未使用的TCB*/
	TCB_CALL	/*已侦听到请求，正处于初始化链接状态*/

}ESafeStruct_TcbState;

typedef enum							/*错误的状态*/
{
	no_err = 0,							/*没有错误*/
	negligible = 1,						/*可以忽烈的错误*/
	fatal = 2							/*严重的错误*/
}ESafeStruct_ErrState;

typedef enum							/*socket的状态*/
{
	STATE_CLOSED,						/*没有任何的连接*/   
	STATE_LISTEN,						/*等待任何一个从远地(IP,port)传来的连接请求*/
	STATE_INIT_RCVD,  				    /*在收到一个连接请求发出INIT ACK后，正在等待确认*/
	STATE_INIT_SENT,					/*发送一个连接请求后，正在等待对应的连接请求的到来*/
	STATE_ESTABLISHED,					/*已经建立好一个连接*/
	STATE_REINIT,						/*发送REINIT报文后进入重新初始化状态*/
	STATE_CLOSE_WAIT,					/*收到FIN，发送FIN+ACK，等待最后的ACK*/
    STATE_FIN_WAIT,						/*等待自己先发送的断开请求的确认*/
	STATE_CLOSING						/*发送FIN后等待对方的确认*/
}ESafeStruct_SocketState;
/*-----------------------------------------------------------------------*/
/*------------------------------时间戳-----------------------------------*/
typedef struct _TSafeStruct_TimeStampe
{
	uint8 ucYear;							/* 年 */
	uint8 ucMonth;							/* 月 */
	uint8 ucDay;							/* 日 */
	uint8 ucHour;							/* 时 */
	uint8 ucMin;							/* 分 */
	uint8 ucSec;							/* 秒 */
}TSafeStruct_TimeStampe;
/*----------------------------------------------------------------------*/
/*------------------------------信息头结构------------------------------*/
typedef struct _TSafeStruct_MsgHead
{
	uint8 ucMsgType;								/* 报文类型 */
	uint16 uhMsgFlags;								/* 报文标志位 */
	uint16 uhSrc;									/* 源地址 */
	uint16 uhDst;									/* 目的地址 */
	uint8 ucTxSeq;									/* 接收序列号 */
	uint8 ucRxSeq;									/* 发送序列号 */
	TSafeStruct_TimeStampe tTimeStampe;				/* 时间戳 */
	char cReserved;									/* 保留值 */
}TSafeStruct_MsgHead;
/*------------------------------------------------------------------------*/
/*-------------------buf缓冲区的结构-------------------------------------*/
typedef struct _TSafeStruct_Buff
{
	struct _TSafeStruct_Buff *ptNext;	/*下一个缓冲区指针*/
	struct _TSafeStruct_Buff *ptPre;	/*上一个缓冲区指针*/
 
	uint16 uhLen;						/*整个消息的实际长度*/
	int iSockfd;						/*发送数据是该buf属于哪个safesocket*/
	TSafeStruct_MsgHead tMsgHead;	    /*帧头*/
	uint8 arrData[MSG_DATA_SIZE + 1];	/*帧体*/
	uint32 uiCRCResult;					/*校验码*/
}TSafeStruct_Buff;
/*-----------------------------------------------------------------------*/

/*-----------------------buf缓冲区的头结构-------------------------------*/
typedef struct _TSafeStruct_BuffQueHead
{
	TSafeStruct_Buff *ptBuffHead;		/*缓冲区队列头指针*/
	TSafeStruct_Buff *ptBuffTail;		/*缓冲区队列尾指针*/
	uint16			  ucBuffcnt;		/*缓冲区的个数*/
	pthread_mutex_t	  pMutex;			/*缓冲互斥锁*/
}TSafeStruct_BuffQueHead;
/*-----------------------------------------------------------------------*/

/*--------------------发送和接收缓存的头结构-----------------------------*/
typedef struct _TSafeStruct_SendBuffHead
{	
	TSafeStruct_BuffQueHead tWaitSendHead;	/*等待发送缓冲区队列头指针，发送时从队列取下，
											  设置消息头，然后挂到pLastSend*/
	TSafeStruct_BuffQueHead tWaitAck;		/*等待ack消息指针，如果ack等待超时，
											  把这个消息取下挂到pLastSend*/ 
}TSafeStruct_SendBuffHead;

typedef struct _TSafeStrcut_RecvBuffHead
{
	TSafeStruct_BuffQueHead tWaitRecvHead;	/*从UDP接收到的数据处理后放在这个队列队尾
											  等待用户接收，用户从队列头读取数据*/
}TSafeStruct_RecvBuffHead;
/*----------------------------------------------------------------------*/
/*----------------------------定时器结构--------------------------------*/
typedef struct _TSafeStruct_Timer
{
	int iTime;								/* 定时器时间 */
	bool bIsUsed;							/* 是否启用定时器 */
	int ucRetryTimes;						/* 重复次数 */
}TSafeStruct_Timer;
/*---------------------------------------------------------------------*/
/*----------------------------缺省链路状态-------------------------------*/
typedef struct _TSafeStruct_Option
{
	int iLinkState;								/* 链路状态（已连接、未连接、不正常）*/
	bool bNonBlock;								/* 阻塞模式 */
}TSafeStruct_Option;
/*------------------------tcb传输控制块结构----------------------------*/
typedef struct _TSafeStruct_tcb
{
	int iSocketID;							/*Udp层提供的socket号*/

	ESafeStruct_SocketState eState;			/*目前的连接状态*/
	ESafeStruct_ErrState eErrState;			/*错误状态*/
	uint8 ucType;							/*连接类型(面向连接还是无连接)*/
	struct sockaddr_in tSrcAddr;				/*本地地址*/
	socklen_t srcAddrLen;					/*地址长度*/
	char carrSrcHostname[HOSTNAME_MAX_LEN];	/*本主机名称*/
	struct sockaddr_in tDstAddr;				/*对方地址*/
	socklen_t dstAddrLen;					/*地址长度*/
	char carrDstHostname[HOSTNAME_MAX_LEN]; /*目的地址名称*/
	uint32 uiCRCTxSeed;						/*该连接的发送方向校验种子值,CRC Initialization Value*/
	uint32 uiCRCRxSeed;						/*该连接的接收方向校验种子值,CRC Initialization Value*/
	uint8 ucTxSeq;							/*本次发送的序号*/
	uint8 ucRxSeq;							/*下一个待接收的接收序号*/
	TSafeStruct_Timer tarrTimer[4];			/*定时器结构*/
   pthread_mutex_t timeMutex;				/*定时器锁，如果需要修改定时器中的定时器数据，需要加锁*/
	TSafeStruct_SendBuffHead tSendBuff;		/*发送缓冲区*/
	TSafeStruct_RecvBuffHead tRecvBuff;		/*接收缓冲区*/
	bool bPermitSend;						/*是否允许数据发送标志*/
	pthread_mutex_t PermitSendMutex;		/*bPermitSend操作互斥锁*/
	uint8 ucVDateField;						/*Vital data标志*/
	uint8 ucReadIsOk;						/*应用层有数据可读,TSafeStruct_FdSet使用*/
	uint8 ucWriteIsOk;						/*应用层可以发送数据,TSafeStruct_FdSet使用*/
	uint8 ucExcept;							/*是否发生错误,TSafeStruct_FdSet使用*/
	pthread_mutex_t fdsetMutex;					/* fdset标志位锁 */
	TSafeStruct_Option tOption;				/*socket的参数选项*/
	uint8 ucNakCnt;							/*连续发送NAK的次数，到达该值需要重传*/
	int  iErrNum;							/*在该socket上发生错误时的错误码<共有3种状态>*/
	pthread_mutex_t errMutex;				/*修改iErrNum的互斥锁*/
	uint8	recvCtrl;					/* 接收控制，TRUE表示不允许对方发送数据过来，FALSE允许*/
	pthread_mutex_t recvCtrlMutex;
	uint8	sendCtrl;					/* 发送控制，TRUE表示对方不允许发送数据过去，FALSE允许*/
	pthread_mutex_t sendCtrlMutex;
}TSafeStruct_Tcb;
/*----------------------------------------------------------------------*/

/*------------------------tcb传输控制块头结构---------------------------*/
typedef struct _TSafeStruct_TcbHead
{
	int iNext;								/*指向下一个TCBLink,最后一个指针指向0*/
	int iPre;								/*指向前一个TCBLink，构成双向链表*/
    TSafeStruct_Tcb *ptTcb;					/*指向TCB，每个socket分配一个TCB，如果使用其他协议连接，
											那么这个指针指向其他类型的control block */
	ESafeStruct_TcbState eState;			/*状态*/
	uint8 ucCallMaxCnt;						/*申请建立连接请求最大数*/
	uint8 ucCallCnt;						/*向该safesocket正在建立连接的数量*/
	uint8 ucVersion;
	uint8 ucIsVirtual;						/*服务器接收到连接请求后建立起来的tcb是虚拟的(这个标志为1)，与服务器使用相
												同的udpsocket，接收进程收到数据后根据源地址和端口将数据分派给合适的tcb*/
	uint8 ucProtocol;						/*使用的底层协议*/
	int hSubSockLink;						/*服务器safesocket生成的子 safesocket组成的链表，
											  子safesocket该值则为母safesocket号*/
}TSafeStruct_TcbHead;
/*-----------------------------------------------------------------------*/

/*-------------------------service结构-----------------------------------*/
typedef struct _TSafeStruct_Service
{
	TSafeStruct_TcbHead tarrTcbPool[TCB_MAX_NUM+1];	/*下标就是给应用进程的socket号,0号作为各链表的尾，不指向具体的TCB*/
	int hTcbUsedCnt;								/*使用的TCB数*/
    int hUnAlloc;									/*未使用的TCBLink头指针，使用数组下标表示*/
	int hUsed;
	/*正在使用中的TCBLink链头指针,使用数组下标表示*/
	int hUnUsed;
	/*已经使用过但被应用释放的TCBLink链头指针，使用数组下标表示,但不立即释放
													  系统socket，作为scoket池*/
	bool bCloseThread;								/*是否关闭时钟、收发线程*/
	int recvSyn[2];									/*管道*/	
	pthread_mutex_t pLinkMutex;						/*对链表操作时需要互斥*/
	TSafeStruct_BuffQueHead tFreeSendBufHead;		/*发送缓冲区空闲队列头*/
	TSafeStruct_BuffQueHead tFreeRecvBufHead;		/*接收缓冲区空闲队列头*/
	TSafeStruct_BuffQueHead	tNextSend;				/*下一次需要发送的缓冲区队列，发送完毕后
													  释放回空闲队列或者挂到空闲pwaitAck*/
}TSafeStruct_Service;


/*----------------------------------------------------------------------*/ 

/*---------------------------------消息参数-------------------------------*/

typedef struct _TSafeStruct_FdSet
{		
	int ucFdCnt;								/*set集合里socket_fd的个数*/
	int iarrFd[TCB_MAX_NUM];					/*set集合里面有哪些socket_fd*/
	uint8 ucarrFdIsSet[TCB_MAX_NUM];			/*socket_fd对应的状态*/
}TSafeStruct_FdSet;



/*----------------------------缺省链路状态-------------------------------*/

/*----------------------所有需要外部配置的参数---------------------------*/
typedef struct _TSafeStruct_CfgData
{
	uint32 ucSocketCnt;							/*允许最大的socket个数*/
	uint32 ucSendBuffCnt;						/*发送缓冲区个数*/
	uint32 ucRecvBuffCnt;						/*接收缓冲区个数*/
	int	 iInitTime;								/*初始化连接超时时间，单位是毫秒，缺省值*/
	uint8 ucInitRetryNum;						/*初始化重试次数，缺省是3次，超过次数及按heartbeat周期发送初始化包，次数不限，直至连接上或者应用层关闭连接*/
	int iWaitackTime;							/*数据包等待确认超时时间，单位是毫秒*/
	uint8 ucWaitackRetryNum;					/*数据包重发次数，缺省是3次，超时即认为连接断开，要求重新初始化*/
	int iHeartBeatTime;							/*心跳定时器时间，单位是毫秒，超时就发送状态信息*/
	int iDataStaleTime;							/*链路超时时间，单位是毫秒，超时即认为链路断开，要求重新初始化*/
	TSafeStruct_Timer tarrTimerCfg[TIMER_NUM];	/*初始化定时器，只有定时器时间和重试次数有效*/
	int iLog;									/*日志需要记录的类型*/
	uint16 ucCallMaxCnt;							/*申请建立连接请求最大数*/
	TSafeStruct_Option tOption;					/*链路选项缺省值*/
	uint8 ucNakMaxCnt;							/*连续发送NAK的次数 */
	uint32	ucMaxSendBuffCnt;					/*流量控制:最大使用的发送缓冲区数*/
	uint32	ucMaxRecvBuffCnt;					/*流量控制:最大使用的接收缓冲区数*/
}TSafeStruct_CfgData;


/*--------------------------------信息头---------------------------------*/
typedef struct _TSafeStruct_MsgOpt
{
	uint8 ucMsgType;							/*消息类型*/
	uint16 uiMsgFlags;							/*消息标志*/
}TSafeStruct_MsgOpt;


/*------------------------------错误信息结构-----------------------------*/
typedef struct _TSafeStruct_ErrInfo
{
	int iErrNum;							/*错误代码*/
	char carrErrDesp[ERRDESP_MAX_LEN];		/*错误描述*/
}TSafeStruct_ErrInfo;

#define VERSION_NUMBER (0x0001)


#endif




