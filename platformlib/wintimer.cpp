#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "wintimer.h"



class CTimerRule
{
public:
	static void start();
	static void end();
	static float getSpan();
	
private:
	static LARGE_INTEGER m_nFreq;
	static LARGE_INTEGER m_start;
	static LARGE_INTEGER m_end;
};

LARGE_INTEGER CTimerRule::m_nFreq = LARGE_INTEGER();
LARGE_INTEGER CTimerRule::m_start = LARGE_INTEGER();
LARGE_INTEGER CTimerRule::m_end = LARGE_INTEGER();

void CTimerRule::start()
{
	if ( m_nFreq.QuadPart == 0)
	{
		QueryPerformanceCounter( &m_start );
	}
	else
	{
		QueryPerformanceFrequency( &m_nFreq);	
	}
}

void CTimerRule::end()
{
	QueryPerformanceCounter( &m_end );
}

float CTimerRule::getSpan()
{
	return (m_end.QuadPart - m_start.QuadPart) / (float)m_nFreq.QuadPart;;
}

//******************************************************************************
// Function: TimerInit
// Brief:   ����ÿ��Tick������  
// Returns: double
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
double TimerInit()
{
    double secondsPerTick;
	LARGE_INTEGER lv;
	
    QueryPerformanceFrequency( &lv );	     /* ��ȡÿ�����CPU Performance Tick */
	
    secondsPerTick = 1.0 / lv.QuadPart;      /* ת��Ϊÿ��Tick������             */
    
	return secondsPerTick;
}

//******************************************************************************
// Function: GetTimeElapsedTotal
// Brief:   ��ȡCPU���е����ڵ�Tick�����Ӷ���������ʱ�� 
// Returns: long
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
long GetTimeElapsedTotal()					/* ��ȡCPU���е����ڵ�Tick��        */
{
	LARGE_INTEGER lv;
	QueryPerformanceCounter( &lv );	        /* ��ȡCPU���е����ڵ�Tick��        */

	return lv.QuadPart;               
}

//******************************************************************************
// Function: PlatformSleep
// Brief:   ����ָ����ʱ�䣬��λ��ms  
// Returns: void
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
void PlatformSleep(DWORD time)				 /* ����ָ����ʱ��			        */   
{
	Sleep(time);
}


