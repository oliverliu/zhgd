#ifndef _PLATFORM_INTERFACE
#define _PLATFORM_INTERFACE

#define NETSIZE				2048*4	

#define  APP_READ_ADDR    PLAT2APP_DATA
#define  APP_WRITE_ADDR   APP2PLAT_DATA

extern  unsigned char APP_READ_ADDR[NETSIZE];
extern  unsigned char APP_WRITE_ADDR[NETSIZE];

extern void InitPlatformInterface ();
extern void ReadFormPlatform ();
extern void WriteToPlatform();

#endif //_PLATFORM_INTERFACE