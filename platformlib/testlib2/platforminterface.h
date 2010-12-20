#ifndef _PLATFORM_INTERFACE
#define _PLATFORM_INTERFACE

#define SIZE				1024						/*数据包中一个单元的长度*/                         
#define NETSIZE				SIZE*4						/*数据包的长度          */    

#define  APP_READ_ADDR    PLAT2APP_DATA
#define  APP_WRITE_ADDR   APP2PLAT_DATA

extern  unsigned char APP_READ_ADDR[NETSIZE];
extern  unsigned char APP_WRITE_ADDR[NETSIZE];

extern signed int InitPlatformInterface ();
extern signed int ReadFormPlatform ();
extern signed int WriteToPlatform();

#endif //_PLATFORM_INTERFACE