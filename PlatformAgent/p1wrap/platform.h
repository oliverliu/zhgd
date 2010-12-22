#ifndef _PLATFORM_H
#define _PLATFORM_H

#ifdef VXWORKS_PLAT

#include <wrapper/wrapperHostLib.h>
#include <sockLib.h>
#include <strings.h>
#include <selectLib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

/* parameter of function platform_pthread_create */
#define  PLATFORM_PTHREAD_CREATE_PARA(start_addr)  void * (* start_addr)(void *)

/* type of thread return value */
#define  PLATFORM_THREAD_RETURN_VALUE   void *

/* prefix of thread function */
#define  PLATFORM_THREAD_PREFIX         void *

#elif  defined (LINUX_PLAT)

#include <error.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>

/* parameter of function platform_pthread_create */
#define  PLATFORM_PTHREAD_CREATE_PARA(start_addr)  void * (* start_addr)(void *)

/* type of thread return value */
#define  PLATFORM_THREAD_RETURN_VALUE   void *

/* prefix of thread function */
#define  PLATFORM_THREAD_PREFIX         void *

#elif  defined (WINDOWS_PLAT)

/* header file */
#include  <Winsock2.h>
#include  <Windows.h>
#include  <time.h>
#include  <process.h>
#include  <stdio.h>
#include  <sys/stat.h>
#include  <stdint.h>


/* variable type convention */

/* type of mutex */
#define  pthread_mutex_t      HANDLE

/* thread ID */
#define  pthread_t            HANDLE

/* type of event */
#define  pthread_cond_t       HANDLE

/* lenth of socket addr convention */
#define  socklen_t            int  

#define  ssize_t              int

#define  caddr_t              char *


/* parameter of function platform_pthread_create */
#define  PLATFORM_PTHREAD_CREATE_PARA(start_addr)    unsigned (__stdcall * start_addr)(void *)

/* type of thread return value */
#define  PLATFORM_THREAD_RETURN_VALUE   unsigned 

/* prefix of thread function */
#define  PLATFORM_THREAD_PREFIX        unsigned  __stdcall

/* function declaration */

/* load socket lib */
int  safeSocket_loadSockLib (void);

#define   snprintf    _snprintf

#else
#error "Platform not supported."

#endif 


#define  PLATFORM_ZERO     0
/* function declaration */

/* init condition variable */
int  platform_cond_init (pthread_cond_t *  pCond);

/* signal condition variable */
void  platform_cond_signal (pthread_cond_t *  pCond);

/* wait for condition variable until it's signaled */
void  platform_cond_wait (pthread_cond_t *  pCond);

/* wait for condition variable until it's signaled or timeout */
void  platform_cond_timewait (pthread_cond_t *  pCond, int timeout);

/* init mutex variable */
int  platform_mutex_init (pthread_mutex_t *  pMutex);

/* lock using mutex */
void  platform_mutex_lock (pthread_mutex_t *  pMutex);

/* unlock releasing mutex */
void  platform_mutex_unlock (pthread_mutex_t *  pMutex);

/* create thread */
int  platform_pthread_create (pthread_t * tidp, PLATFORM_PTHREAD_CREATE_PARA(start_addr));

/* close socket */
void  platform_socket_close (int socket);

/* thread join */
void  platform_pthread_join (pthread_t thread, void **value_ptr );

#endif //_PLATFORM_H

