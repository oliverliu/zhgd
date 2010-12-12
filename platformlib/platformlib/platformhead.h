#ifndef PLATFORMHEAD_H
#define PLATFORMHEAD_H

/*-----------------------------------------------------------------------------
* 平台定义的数据类型
* ----------------------------------------------------------------------------*/ 
//typedef     __int64                 PLAT_INT64;			/* 64 bits, signed  */ 
//typedef     unsigned __int64        PLAT_UINT64;		/* 64 bits, unsigned*/   
typedef     long long 		    PLAT_INT64;			/* 64 bits, signed  */ 
typedef     unsigned long long      PLAT_UINT64;		/* 64 bits, unsigned*/   
typedef     signed short            PLAT_INT16;			/* 16 bits, signed  */
typedef     unsigned short		PLAT_UINT16;		/* 16 bits, unsigned*/
typedef     int                     PLAT_INT;			/* platform int     */
typedef		unsigned int			PLAT_UINT;			/* unsigned int     */
typedef     char                    PLAT_BYTE;			/* 8 bits, signed   */
typedef     unsigned char           PLAT_UBYTE;			/* 8 bits, unsigned */
typedef     signed char             PLAT_INT8;			/* 8 bits, signed   */
typedef		unsigned char			PLAT_UINT8;			/* 8 bits, unsigned */
typedef     signed long		        PLAT_INT32;			/* long ,  signed   */
typedef     unsigned long	        PLAT_UINT32;		/* long ,  unsigned */

/*-----------------------------------------------------------------------------
* 平台定义的各类长度值
* ----------------------------------------------------------------------------*/ 
#define SIZE				2048						/*数据包中一个单元的长度*/                         
#define NETSIZE				2048*4						/*数据包的长度          */                       
#define CHARSIZE			SIZE*2+1
#define IDSIZE				9							/*数据包ID的长度        */
 
#define APP_DB_ADDR         0xc1000000					/*平台线路数据首地址	*/  
#define GETSID		        0x3F						/*用来与操作取源ID的常量*/ 



#endif

