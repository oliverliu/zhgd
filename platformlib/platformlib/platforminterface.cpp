#include "platform.h"

CAppInterface g_appInter;

signed int InitPlatformInterface ()
{
	return g_appInter.AppInit();
}

signed int ReadFromPlatform ()
{
	return g_appInter.AppRead();
}

signed int WriteToPlatform()
{
	return g_appInter.AppWrite();
}