/*-------------------------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS *RESERVED
 *Date：2010.09.23
 *Author：zhanghaihai
 *Modifier: ChenHua
 *Version：v0.01
 *File：safeSocket.h
 -------------------------------------------------------------------------------------------*/
#ifndef _SAFESOCKET_H
#define _SAFESOCKET_H

//#include "safestruct.h"

typedef signed char int8;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

#define TCB_MAX_NUM		1280	   	   		/*每个进程最多能打开的socket数量*/

#define  SOL_SAFESOCKET		100				/* setoptinon指定的level层次 */

#define	 SS_LINK_STATUS		1000			/* getoption取链路状态参数 */
#define  SS_BLOCK_TYPE		1001			/* getoption/setoption读取/设置阻塞模式参数 */
#define	 SS_CALLNUM			1002			/* 读取/设置每个服务器可以同时接收的建立连接请求数 */
#define  SS_WAITSENDBUF		1003			/* 读取/设置允许等待发送缓冲区最大数目 */
#define  SS_WAITRECVBUF		1004			/* 读取/设置允许等待接收缓冲区最大数目 */
#define	 SS_INITTIME		1005			/* 读取/设置初始化连接超时时间 */
#define  SS_INITRETRY		1006			/* 读取/设置初始化重试次数 */
#define  SS_WAITACK			1007			/* 读取/设置数据包等待确认超时时间 */
#define  SS_WAITACKNUM		1008			/* 读取/设置数据包重发次数 */
#define  SS_HEARTBEAT		1009			/* 读取/设置心跳定时器时间 */
#define  SS_DATASTALE		1010			/* 读取/设置链路超时时间 */
#define  SS_NAKNUM			1011			/* 读取/设置连接接收到最大错误次数 */

/* 使用的协议 */
#define   SS_PROTOCOL_UDP         1			/* UDP协议 */
#define   SS_PROTOCOL_SERIAL      2			/* 串口协议 */

/* 数据类型 */
#define   SS_VITAL_DATA		0			/* 重要数据 */
#define   SS_NON_VITAL_DATA     1			/* 非重要数据 */

/* 连接类型 */
#define   SS_WITHCONNECTION       1			/* 面向连接的类型 */
#define   SS_CONNECTIONLESS       2			/* 面向无连接的类型 */

/* 链路状态 */
#define   SS_LINK_CONNECT         0x01		/* 链路处于连接中,可以发送数据 */
#define   SS_LINK_DISCONNECT      0x02		/* 链路未建立连接 */
#define   SS_LINK_ERROR           0x03		/* 链路出错 */
#define   SS_LINK_CLOSED		  0x04		/* 链路已经关闭*/

/* Socket使用模式*/
#define   SS_BLOCK                 0		/* 阻塞模式 */
#define   SS_UNBLOCK               1		/* 非阻塞模式 */

/* 函数执行出错 */
#define   SS_FAILED                (-1)
#define   SS_SUCCESS				0

/*----------------------可以外部配置的参数---------------------------*/
typedef struct _TSafeStruct_CfgData
{
	int			iLinkState;
	bool		bNonBlock;
	int			ucSocketCnt;					/*允许最大的socket个数*/
	uint32		ucSendBuffCnt;					/*发送缓冲区个数*/
	uint32		ucRecvBuffCnt;					/*接收缓冲区个数*/
	int			iInitTime;						/*初始化连接超时时间，单位是毫秒，缺省值*/
	uint8		ucInitRetryNum;					/*初始化重试次数，缺省是3次，超过次数及按heartbeat周期发送初始化包*/
	int			iWaitackTime;					/*数据包等待确认超时时间，单位是毫秒*/
	uint8		ucWaitackRetryNum;				/*数据包重发次数，缺省是3次，超时即认为连接断开，要求重新初始化*/
	int			iHeartBeatTime;					/*心跳定时器时间，单位是毫秒，超时就发送状态信息*/
	int			iDataStaleTime;					/*链路超时时间，单位是毫秒，超时即认为链路断开，要求重新初始化*/
	uint8		ucCallMaxCnt;					/*申请建立连接请求最大数*/
	uint8		ucNakMaxCnt;					/*连续发送NAK的次数 */
	uint32		ucMaxSendBuffCnt;				/*流量控制:最大使用的发送缓冲区数*/
	uint32		ucMaxRecvBuffCnt;				/*流量控制:最大使用的接收缓冲区数*/
}TSafeStruct_CfgData;

/*-------------------------FD_SET集合参数--------------------------*/
typedef struct _TSafeStruct_FdSet
{		
	int ucFdCnt;								/*set集合里socket_fd的个数*/
	int iarrFd[TCB_MAX_NUM];					/*set集合里面有哪些socket_fd*/
	uint8 ucarrFdIsSet[TCB_MAX_NUM];			/*socket_fd对应的状态*/
}TSafeStruct_FdSet;

typedef TSafeStruct_FdSet TSafeSocket_FdSet;

/* 将safeSock从set集合中删除 */
#define  SS_FD_CLR(safeSock, set)  do\
  { \
    uint32 _i;\
	\
    for (_i = 0; _i < ((TSafeSocket_FdSet *)(set))->ucFdCnt; _i ++)\
       { \
	      if (safeSock == ((TSafeSocket_FdSet *)(set))->iarrFd[_i])\
             {\
			    while (_i < ((TSafeSocket_FdSet *)(set))->ucFdCnt - 1)\
                  {\
				     ((TSafeSocket_FdSet *)(set))->iarrFd[_i] = \
					 ((TSafeSocket_FdSet *)(set))->iarrFd[_i + 1];\
					  _i ++;\
                  }\
		        ((TSafeSocket_FdSet *)(set))->ucFdCnt --;\
			     break;\
             }\
       }\
  }while (0)


/* 将safeSock加入到set集合中 */
#define  SS_FD_SET(safeSock, set)  do \
  {\
	 if (safeSock > TCB_MAX_NUM || safeSock <= 0)\
		break;\
     uint32  indx = ((TSafeSocket_FdSet *)(set))->ucFdCnt;\
	 \
     if (indx < TCB_MAX_NUM + 1)\
       {\
	      ((TSafeSocket_FdSet *)(set))->iarrFd[indx] = safeSock;\
		  ((TSafeSocket_FdSet *)(set))->ucFdCnt ++;\
       }\
  }while(0)


/* 清空set集合 */
#define  SS_FD_ZERO(set)   (((TSafeSocket_FdSet *)(set))->ucFdCnt = 0)

/* 判断safeSock是否在set集合中 */
#define  SS_FD_ISSET(safeSock, set)  safeSocket_isSet(safeSock, (TSafeSocket_FdSet *)(set))


/*错误代码*/
#define SS_BASEERR				50000				/* safesocket错误码从50000开始 */
#define SS_EBADFD				(SS_BASEERR + 1)    /* send or recv on a locall closed socket */
#define SS_ENOTSOCKET			(SS_BASEERR + 2)	/* Socket operation on non-socket */
#define SS_EACCES				(SS_BASEERR + 3)	/* Permission denied */
#define SS_EBADADDR				(SS_BASEERR + 4)	/* Bad address */
#define SS_EINVAL				(SS_BASEERR + 5)	/* Invalid argument */
#define SS_ETMSOCK				(SS_BASEERR + 6)	/* Too many open socket */
#define SS_EWOULDBLOCK			(SS_BASEERR + 7)	/* Resource temporarily unavailable */
#define SS_ESTATE				(SS_BASEERR + 8)	/* can not execute the operation in current state */
#define SS_EDESTADDRREQ			(SS_BASEERR + 9)	/* Destination address required */
#define SS_EMSGSIZE				(SS_BASEERR + 10)	/* Message too long */
#define SS_EPROTOTYPE			(SS_BASEERR + 11)	/* Protocol wrong type for socket */
#define SS_ENOPROTOOPT			(SS_BASEERR + 12)	/* Bad protocol option */
#define SS_EPROTONOSUPPORT		(SS_BASEERR + 13)	/* Protocol not supported */
#define SS_ESOCKNOSUPPORT		(SS_BASEERR + 14)	/* Socket type not supported */
#define SS_EOPNOTSUPP			(SS_BASEERR + 15)	/* Operation not supported*/
#define SS_ENETRESET			(SS_BASEERR + 16)	/* Network dropped connection on reset */
#define SS_ECONNABORTED			(SS_BASEERR + 17)	/* Software caused connection abort */
#define SS_ECONNRESET			(SS_BASEERR + 18)	/* Connection reset by peer */
#define SS_ENOBUFS				(SS_BASEERR + 19)	/* No buffer space available */
#define SS_EISCONN				(SS_BASEERR + 20)	/* Socket is not conneted */
#define SS_ENOTCONN				(SS_BASEERR + 21)	/* Socket is not connected */
#define SS_ETIMEDOUT			(SS_BASEERR + 22)	/* Timed out */
#define SS_ECONNRERUSED			(SS_BASEERR + 23)	/* Connection refused */
#define SS_EHOSTDOWN			(SS_BASEERR + 24)	/* Host is down */
#define SS_ELIMITCALL			(SS_BASEERR + 25)   /* Too many call */
#define SS_EUSERS				(SS_BASEERR + 26)	/* Too many users */
#define SS_EDISCON				(SS_BASEERR + 27)	/* Graceful shuudown in progress */
#define SS_ENOTINIT				(SS_BASEERR + 28)	/* Successful startProtocol not yet performed */
#define SS_NOCONNECTREQ			(SS_BASEERR + 29)	/* No connection request */
#define SS_ERUNOUTBUF			(SS_BASEERR + 30)	/* No buffer for this connection */


/*--------------------------------接口函数声明------------------------------*/

/* 开始协议栈 */
int safeSocket_startSocket(TSafeStruct_CfgData * cfg);

/* 创建安全套接字 */
int safeSocket_socket(uint8 protocol, uint8 type);

/* 建立连接 */
int safeSocket_connect(int safeSock, const struct sockaddr *addr, socklen_t addrlen);

/* 服务器可以接收连接请求 */
int safeSocket_listen(int safeSock, int backlog);

/* 将地址绑定到指定的套接字 */
int safeSocket_bind(int safeSock, const struct sockaddr* addr, socklen_t addrlen);

/* 服务器获得连接请求并建立连接 */
int safeSocket_accept(int safeSock, struct sockaddr *addr, socklen_t *addrlen);

/* 检查readfds,writefds,exceptfds集合是否有变化 */
int safeSocket_select(int nfds, TSafeStruct_FdSet* readfds, TSafeStruct_FdSet* writefds, TSafeStruct_FdSet* exceptfds,const struct timeval* timeout);

/* 发送数据 */
ssize_t safeSocket_send(int sockfd, const void* buf, size_t len, int flags);

/* 接收数据 */
ssize_t safeSocket_recv(int safeSock, void* buf, size_t len, int flags);

/* 接收数据,并可以得到数据发送者的源地址 */
ssize_t safeSocket_recvfrom(int safeSock, void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);

/* 发送数据,在无链接的安全套机字指定一个目标地址 */
ssize_t safeSocket_sendto(int safeSock, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);

/* 关闭安全套接字 */
int safeSocket_close(int safeSock);

/* 停止协议栈 */
void safeSocket_stopSocket(void);

/* 判断fd是是否在描述符集中 */
int safeSocket_isSet(int safeSock, TSafeStruct_FdSet * set);

/* 取出错误码 */
int safeSocket_getErrorNum(int safeSock);

/* 获得出错码的描述 */
char* safeSocket_getErrDesp(int errNum);

/* 设置配置文件的选项 */
int safeSocket_setSockopt(int safeSock, int level, int optname, const void* optval, socklen_t len);

/* 取得配置文件的选项 */
int safeSocket_getSockopt(int safeSock, int level, int optname, void* optval, socklen_t *optlen);

#endif
