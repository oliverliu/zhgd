/*------------------------------------------------------------------------
 *(C) COPYRIGHT 2010 BY ZHEDAWANGXINGUIDAO CORPORATION ALL RIGHTS RESERVED
 *Data��2010.09.28
 *Author����־��
 *Version��V1.00
 *File��wintimer.h
 *Note����װ����ʱ����ص�windows����
-------------------------------------------------------------------------*/
#ifndef WINTIMER_H
#define WINTIMER_H

typedef unsigned long  DWORD;

double TimerInit();							 /* ÿ��Tick������            */
long   GetTimeElapsedTotal();                /* ��ȡCPU���е����ڵ�Tick�� */
void   PlatformSleep(DWORD time);            /* ��������ָ��ʱ��          */

#endif