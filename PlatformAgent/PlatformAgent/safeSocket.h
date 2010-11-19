/************************************************************************/
/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
   ���ڣ� 2010.9.3
   ���ߣ� ����
   �汾�� v1
   ���ƣ� safeSocket.h
   ˵���� safeSocket.c��ͷ�ļ����궨�塢����˵��                        */                                                                   
/************************************************************************/

#ifndef  _SAFESOCKET_H
#define  _SAFESOCKET_H


#include  <stddef.h>
#include  "safeStruct.h"


/* �������� */

#define  uint8    unsigned char                        
#define  uint16   unsigned short int
#define  uint32   unsigned int




/* ʹ�õ�Э�� */

#define   SS_PROTOCOL_UDP         1
 
#define   SS_PROTOCOL_SERIAL      2


/* �������� */

/* �������� */
#define   SS_WITHCONNECTION       1

/* ������ */
#define   SS_CONNECTIONLESS       2




/* �������� */

/* ��Ҫ���� */
#define  SS_DATA_IMPORTANT        1

/* ����Ҫ���� */
#define  SS_DATA_UNIMPORTANT      0




/* ��·״̬ */

/* ��·���������У����Է������� */
#define   SS_LINK_CONNECT         1

/* ��·δ�������� */
#define   SS_LINK_DISCONNECT      2

/* ��·���� */
#define   SS_LINK_ERROR           3




/* socketģʽ��Ĭ��Ϊ������ģʽ */

/* ����ģʽ */
#define   SS_BLOCK                 0

/* ������ģʽ */
#define   SS_UNBLOCK               1



/* sock���� */
#define   SS_LEVEL_SAFESOCK         1

/* ����ִ�г��� */
#define   SS_FAILED                -1




/* safeSock������Ϣ */
typedef  struct   t_safeSocket_fdSet
{
	uint8    ucFdCnt;                                   /* iarrFd������safeSock���� */
	int      iarrFd[SAFESTRUCT_MAXTCBNUM + 1];
	uint8    ucarrFdIsSet[SAFESTRUCT_MAXTCBNUM + 1];  

}TSafeSocket_FdSet;


/* ��safeSock��set������ɾ�� */
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




/* ��safeSock���뵽set������ */
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



/* ���set���� */
#define  SS_FD_ZERO(set)   (((TSafeSocket_FdSet *)(set))->ucFdCnt = 0)


/* �ж�safeSock�Ƿ���set������ */
#define  SS_FD_ISSET(safeSock, set)  safeSocket_isSet(safeSock, (TSafeSocket_FdSet *)(set))
	




/* ��������Ϣ */

/* ������ֵƫ�ƻ�׼ */
#define   SS_BASEERR               50000


 
/* �������� */
#define   SS_EINVAL                SS_BASEERR + 5

/* ��ǰ������tcb״̬��ƥ�� */
#define   SS_ESTATE                SS_BASEERR + 8
 
/* Э�鲻��֧�� */
#define   SS_EPROTONOSUPPORT       SS_BASEERR + 13      

/* sock��𲻱�֧�� */
#define   SS_ESOCKTNOSUPPORT       SS_BASEERR + 14

/* û�п��л��嵥Ԫ����ʹ�� */
#define   SS_ENOBUFS               SS_BASEERR + 19

/* ��������û��δ������ѽ��������ӵĿͻ������� */
#define   SS_ENOCONNECTREQ         SS_BASEERR + 29







/* safeSocket��ʼ�� */
bool  safeSocket_startSocket ();


/* �ر�safeSocket */
void  safeSocket_stopSocket (void);


/* ���safeSock��Ӧ�ò�ʹ�� */
int  safeSocket_socket (uint8  protocol, uint8  type);


/* ��ip��ַ���˿� */
int  safeSocket_bind (int safeSock, const struct sockaddr * pSockAddr, size_t addrLen);


/* �������˵��ã�ʹsafeSock��������״̬ */
int  safeSocket_listen (int safeSock, int backLog);


/* �������˵��ã��õ��ѽ��������ӵ�safeSock */
int  safeSocket_accept (int safeSock, struct sockaddr *  pSockAddr, size_t * pAddrLen);


/* ��pSockAddrָ��ĵ�ַ�������� */
int  safeSocket_connect (int safeSock, const struct sockaddr * pSockAddr, size_t addrLen);


/* ����pBuff��len�ֽ������� */
size_t  safeSocket_send (int safeSock, const void *  pBuff, size_t len, int flag);


/* ��safeSock�д��ڵȴ�Ӧ�ò���յ����ݣ����俽����pBuff�� */
size_t  safeSocket_recv (int safeSock, void *  pBuff, size_t len, int * pFlag);


/* ��timeoutʱ����ж������������Ƿ���ڿ�ִ
   �ж���д��������Ҫ���������¼���safeSock   */
int  safeSocket_select (int nfds, TSafeSocket_FdSet * readfds, TSafeSocket_FdSet * writefds,
						          TSafeSocket_FdSet * exceptfds,  const  timeval * timeout);


/* �����ӷ�ʽ������pBuff��len�ֽ������� */
size_t  safeSocket_sendto (int safeSock, const void *  pBuff,     size_t len, int flags,
						      const struct sockaddr *  dest_addr, size_t addrLen);


/* �����ӷ�ʽ����Դ�˽������ݣ�������pBuff�� */
size_t  safeSocket_recvfrom (int safeSock, void *  pBuff,    size_t   len,   int flags,
                                struct sockaddr *  src_addr, size_t * addrLen);


/* ��ȡsafeSock��udp sock��Ӧѡ������ */								
int  safeSocket_getSockOpt (int  safeSock, int    level, 
							int  optname,  void * optval, size_t * optlen);


/* ����level�����optval����������safeSock��udp sock��Ӧѡ�� */
int  safeSocket_setSockOpt (int  safeSock, int   level,  int     optname, 
							        const void * optval, size_t  optlen);


/* �ر����� */
int  safeSocket_close (int safeSock);


/* �жϼ���pSet��safeSock�Ƿ���λ */
int  safeSocket_isSet (int safeSock, TSafeSocket_FdSet * pSet);


/* ȡ����safeSock��Ӧ�Ĵ����� */
int  safeSocket_getErrorNum (int safeSock);



#endif


