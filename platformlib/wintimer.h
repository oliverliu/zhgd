/*------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
 *Data：2010.09.28
 *Author：刘志盼
 *Version：V1.00
 *File：wintimer.h
 *Note：封装的与时间相关的windows函数
-------------------------------------------------------------------------*/
#ifndef WINTIMER_H
#define WINTIMER_H

typedef unsigned long  DWORD;

double TimerInit();							 /* 每个Tick多少秒            */
long   GetTimeElapsedTotal();                /* 获取CPU运行到现在的Tick数 */
void   PlatformSleep(DWORD time);            /* 程序休眠指定时间          */

#endif