/************************************************************************/
/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
   日期： 2010.9.3
   作者： 杨明
   版本： v1
   名称： safeSocket.h
   说明： safeSocket.c的头文件，宏定义、函数说明                        */                                                                   
/************************************************************************/

#ifndef  _SAFESOCKET_H
#define  _SAFESOCKET_H


#include  <stddef.h>
#include  "safeStruct.h"


/* 数据类型 */

#define  uint8    unsigned char                        
#define  uint16   unsigned short int
#define  uint32   unsigned int




/* 使用的协议 */

#define   SS_PROTOCOL_UDP         1
 
#define   SS_PROTOCOL_SERIAL      2


/* 连接类型 */

/* 面向连接 */
#define   SS_WITHCONNECTION       1

/* 无连接 */
#define   SS_CONNECTIONLESS       2




/* 数据类型 */

/* 重要数据 */
#define  SS_DATA_IMPORTANT        1

/* 非重要数据 */
#define  SS_DATA_UNIMPORTANT      0




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




/* safeSock集合信息 */
typedef  struct   t_safeSocket_fdSet
{
	uint8    ucFdCnt;                                   /* iarrFd数组中safeSock个数 */
	int      iarrFd[SAFESTRUCT_MAXTCBNUM + 1];
	uint8    ucarrFdIsSet[SAFESTRUCT_MAXTCBNUM + 1];  

}TSafeSocket_FdSet;


/* 将safeSock从set集合中删除 */
#define  SS_FD_CLR(safeSock, set)  do\
  { \
    uint8 _i;\
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
     uint8  indx = ((TSafeSocket_FdSet *)(set))->ucFdCnt;\
	 \
     if (indx < SAFESTRUCT_MAXTCBNUM + 1)\
       {\
	      ((TSafeSocket_FdSet *)(set))->iarrFd[indx] = safeSock;\
		  ((TSafeSocket_FdSet *)(set))->ucFdCnt ++;\
       }\
  }while(0)



/* 清空set集合 */
#define  SS_FD_ZERO(set)   (((TSafeSocket_FdSet *)(set))->ucFdCnt = 0)


/* 判断safeSock是否在set集合中 */
#define  SS_FD_ISSET(safeSock, set)  safeSocket_isSet(safeSock, (TSafeSocket_FdSet *)(set))
	




/* 错误码信息 */

/* 错误码值偏移基准 */
#define   SS_BASEERR               50000


 
/* 参数出错 */
#define   SS_EINVAL                SS_BASEERR + 5

/* 当前操作与tcb状态不匹配 */
#define   SS_ESTATE                SS_BASEERR + 8
 
/* 协议不被支持 */
#define   SS_EPROTONOSUPPORT       SS_BASEERR + 13      

/* sock类别不被支持 */
#define   SS_ESOCKTNOSUPPORT       SS_BASEERR + 14

/* 没有空闲缓冲单元可以使用 */
#define   SS_ENOBUFS               SS_BASEERR + 19

/* 服务器端没有未处理的已建立好连接的客户端请求 */
#define   SS_ENOCONNECTREQ         SS_BASEERR + 29







/* safeSocket初始化 */
bool  safeSocket_startSocket ();


/* 关闭safeSocket */
void  safeSocket_stopSocket (void);


/* 获得safeSock供应用层使用 */
int  safeSocket_socket (uint8  protocol, uint8  type);


/* 绑定ip地址、端口 */
int  safeSocket_bind (int safeSock, const struct sockaddr * pSockAddr, size_t addrLen);


/* 服务器端调用，使safeSock处于侦听状态 */
int  safeSocket_listen (int safeSock, int backLog);


/* 服务器端调用，得到已建立好连接的safeSock */
int  safeSocket_accept (int safeSock, struct sockaddr *  pSockAddr, size_t * pAddrLen);


/* 向pSockAddr指向的地址发起连接 */
int  safeSocket_connect (int safeSock, const struct sockaddr * pSockAddr, size_t addrLen);


/* 发送pBuff中len字节数数据 */
size_t  safeSocket_send (int safeSock, const void *  pBuff, size_t len, int flag);


/* 若safeSock中存在等待应用层接收的数据，则将其拷贝到pBuff中 */
size_t  safeSocket_recv (int safeSock, void *  pBuff, size_t len, int * pFlag);


/* 在timeout时间后，判断三个集合中是否存在可执
   行读、写操作或需要处理特殊事件的safeSock   */
int  safeSocket_select (int nfds, TSafeSocket_FdSet * readfds, TSafeSocket_FdSet * writefds,
						          TSafeSocket_FdSet * exceptfds,  const  timeval * timeout);


/* 无连接方式，发送pBuff中len字节数数据 */
size_t  safeSocket_sendto (int safeSock, const void *  pBuff,     size_t len, int flags,
						      const struct sockaddr *  dest_addr, size_t addrLen);


/* 无连接方式，从源端接收数据，拷贝到pBuff中 */
size_t  safeSocket_recvfrom (int safeSock, void *  pBuff,    size_t   len,   int flags,
                                struct sockaddr *  src_addr, size_t * addrLen);


/* 获取safeSock或udp sock相应选项内容 */								
int  safeSocket_getSockOpt (int  safeSock, int    level, 
							int  optname,  void * optval, size_t * optlen);


/* 根据level类别，用optval中内容设置safeSock或udp sock相应选项 */
int  safeSocket_setSockOpt (int  safeSock, int   level,  int     optname, 
							        const void * optval, size_t  optlen);


/* 关闭连接 */
int  safeSocket_close (int safeSock);


/* 判断集合pSet中safeSock是否置位 */
int  safeSocket_isSet (int safeSock, TSafeSocket_FdSet * pSet);


/* 取得与safeSock对应的错误码 */
int  safeSocket_getErrorNum (int safeSock);



#endif


