#include "platform.h"

CAppInterface g_appInter;

void InitPlatformInterface ()
{
	g_appInter.AppInit();
}

void ReadFormPlatform ()
{
	g_appInter.AppRead();
}

void WriteToPlatform()
{
	g_appInter.AppWrite();
}