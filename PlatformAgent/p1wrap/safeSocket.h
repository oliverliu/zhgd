#ifndef _SAFESOCKET_H
#define _SAFESOCKET_H

#include "safestruct.h"
#include "platform.h"

#define LINK_STATUS		1			
#define BLOCK_TYPE		2			


/*阻塞的模式，默认为非阻塞模式*/
#define	 BLOCK			0			/*阻塞模式*/
#define  NONBLOCK		1			/*非阻塞模式*/

#define  SAFESOCKET			100		/*setoptinon指定的level层次*/
#define	 SS_LINK_STATUS		1000	/*getoption取链路状态参数*/
#define  SS_BLOCK_TYPE		1001	/*getoption/setoption读取/设置阻塞模式参数*/
#define	 SS_CALLNUM			1002	/*读取/设置每个服务器可以同时接收的建立连接请求数*/
#define  SS_WAITSENDBUF		1003	/*读取/设置允许等待发送缓冲区最大数目*/
#define  SS_WAITRECVBUF		1004	/*读取/设置允许等待接收缓冲区最大数目*/
#define	 SS_INITTIME		1005	/*读取/设置初始化连接超时时间*/
#define  SS_INITRETRY		1006	/*读取/设置初始化重试次数*/
#define  SS_WAITACK			1007	/*读取/设置数据包等待确认超时时间*/
#define  SS_WAITACKNUM		1008	/*读取/设置数据包重发次数*/
#define  SS_HEARTBEAT		1009	/*读取/设置心跳定时器时间*/
#define  SS_DATASTALE		1010	/*读取/设置链路超时时间*/
#define  SS_NAKNUM			1011	/*读取/设置连接接收到最大错误次数*/

/* 使用的协议 */

#define   SS_PROTOCOL_UDP         1
#define   SS_PROTOCOL_SERIAL      2

/* 连接类型 */
#define   SS_WITHCONNECTION       1			/*面向连接*/
#define   SS_CONNECTIONLESS       2			/*无连接 */

/*链路状态*/
#define		LINK_CONNECT	0x01			/*链路处于连接中*/
#define		LINK_DISCON		0x02			/*链路还未建立连接*/
#define		LINK_ERROR		0x03			/*链路出错*/
#define		LINK_CLOSED		0x04			/*链路已关闭*/
/* 链路状态 */

/* 链路处于连接中，可以发送数据 */
#define   SS_LINK_CONNECT         1

/* 链路未建立连接 */
#define   SS_LINK_DISCONNECT      2

/* 链路出错 */
#define   SS_LINK_ERROR           3

/* socket模式，默认为非阻塞模式 */

/* 阻塞模式 */
#define   SS_BLOCK                 0

/* 非阻塞模式 */
#define   SS_UNBLOCK               1

/* sock级别 */
#define   SS_LEVEL_SAFESOCK         1

/* 函数执行出错 */
#define   SS_FAILED                -1


typedef struct _TSafeStruct_FdSet TSafeSocket_FdSet;


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
#define SS_ERUNOUTBUF		(SS_BASEERR + 30)	/*No buffer for this connection */


/*--------------------------接口函数声明------------------------*/

/*申请服务的内存空间，启动定时器线程，初始化CRCTable*/
int safeSocket_startSocket(TSafeStruct_CfgData * cfg);

/*释放该服务的内存空间，包括服务结构体，TCB缓冲池*/
void safeSocket_stopSocket(void);

/*发送数据*/
ssize_t safeSocket_send(int sockfd, const void* buf, size_t len, int flags);

/*接收数据*/
ssize_t safeSocket_recv(int safeSock, void* buf, size_t len, int flags);

/*select函数*/
int safeSocket_select(int nfds, TSafeStruct_FdSet* readfds, TSafeStruct_FdSet* writefds, TSafeStruct_FdSet* exceptfds,const struct timeval* timeout);

/*接收数据*/
ssize_t safeSocket_recvfrom(int safeSock, void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);

/*发送数据*/
ssize_t safeSocket_sendto(int safeSock, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);

/*创建安全socket*/
int safeSocket_socket(uint8 protocol, uint8 type);

int safeSocket_connect(int safeSock, const struct sockaddr *addr, socklen_t addrlen);

int safeSocket_listen(int safeSock, int backlog);

int safeSocket_bind(int safeSock, const struct sockaddr* addr, socklen_t addrlen);

int safeSocket_accept(int safeSock, struct sockaddr *addr, socklen_t *addrlen);

/*关闭安全socket*/
int safeSocket_close(int safeSock);

/* 判断 fd是否有数据。*/
int safeSocket_isSet(int safeSock, TSafeStruct_FdSet * set);

/*取出出错码*/
int safeSocket_getErrorNum(int safeSock);

/*获得出错码的描述*/
char* safeSocket_getErrDesp(int errNum);

/* 设置选项 */
int safeSocket_setSockopt(int safeSock, int level, int optname, const void* optval, socklen_t len);

/* 得到选项 */
int safeSocket_getSockopt(int safeSock, int level, int optname, void* optval, socklen_t *optlen);

#endif
