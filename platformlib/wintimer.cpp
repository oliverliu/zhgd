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
// Brief:   计算每个Tick多少秒  
// Returns: double
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
double TimerInit()
{
    double secondsPerTick;
	LARGE_INTEGER lv;
	
    QueryPerformanceFrequency( &lv );	     /* 获取每秒多少CPU Performance Tick */
	
    secondsPerTick = 1.0 / lv.QuadPart;      /* 转换为每个Tick多少秒             */
    
	return secondsPerTick;
}

//******************************************************************************
// Function: GetTimeElapsedTotal
// Brief:   获取CPU运行到现在的Tick数，从而运算运行时间 
// Returns: long
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
long GetTimeElapsedTotal()					/* 获取CPU运行到现在的Tick数        */
{
	LARGE_INTEGER lv;
	QueryPerformanceCounter( &lv );	        /* 获取CPU运行到现在的Tick数        */

	return lv.QuadPart;               
}

//******************************************************************************
// Function: PlatformSleep
// Brief:   休眠指定的时间，单位：ms  
// Returns: void
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
void PlatformSleep(DWORD time)				 /* 休眠指定的时间			        */   
{
	Sleep(time);
}


