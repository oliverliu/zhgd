#ifndef PLATFORMHEAD_H
#define PLATFORMHEAD_H

/*-----------------------------------------------------------------------------
* 平台定义的数据类型
* ----------------------------------------------------------------------------*/ 
typedef     __int64                 PLAT_INT64;			/* 64 bits, signed  */ 
typedef     unsigned __int64        PLAT_UINT64;		/* 64 bits, unsigned*/   
typedef     signed short         	PLAT_INT16;			/* 16 bits, signed  */
typedef     unsigned short	        PLAT_UINT16;		/* 16 bits, unsigned*/
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
/*-----------------------------------------------------------------------------
* 平台用到的有关ATO输出的宏定义
* ----------------------------------------------------------------------------*/
#ifdef  ATODEFINE  
#define ATO_ID_IOOUT		0x09000000					/*表示IO输出数据的ID	*/
#define ATO_ID_MESSAGEOUT	0x0A000000					/*表示通信报文输出数据的ID*/
#define ATO_ID_ATPOUT		0x08000000					/*表示ATO到ATP数据的ID*/
#define ATO_ID_COMLINKOUT	0x0B000000					/*表示通信连接控制数据的ID*/
#define ATO_ID_TRACKDBOUT	0x0E000000					/*表示DB数据控制数据的ID*/
#define ATO_ID_COOUT		0x0D000000					/*表示电流输出数据的ID*/
#define ATO_ID_TMSOUT		0x0F000000					/*表示TMS输出数据的ID*/
#define ATO_ID_EVENTOUT		0x0C000000					/*表示事件记录输出数据的ID*/
#define ATO_ID_SYSDATAOUT	0x10000000					/*表示中间状态同步输出数据的ID*/
#define ATO_ID_ATSOUT		10							/*表示通信报文ATO到ATS输出数据的ID*/
#define ATO_ID_TODOUT		11							/*表示通信报文ATO到TOD输出数据的ID*/

#define ID_PLATLINK_TO_ATO  0x06000000					/*表示平台通信连接输出数据ID，目的地ATO*/ 
#define ID_PLATDB_TO_ATO    0x02000000					/*表示平台DB输出数据ID，目的地ATO*/
#endif

/*-----------------------------------------------------------------------------
* 平台用到的有关RS仿真输出的宏定义
* ----------------------------------------------------------------------------*/
#ifdef    RSDEFINE
#define RS_ID_VELOCITYOUT	0x03000000					/*表示列车的测速输出数据ID，目的地ATO*/
#define RS_ID_TO_PLOT       0x09000000					/*表示列车的位置输出数据ID，目的地轨旁仿真*/
#define RS_ID_TO_ATP        0x03010000 					/*表示列车的测速输出数据ID，目的地ATP*/
#define RS_ID_BOARD1_TO_ATP 0x04010000					/*表示列车的IO输出数据ID，目的地ATP*/
#define RS_ID_BOARD2_TO_ATP 0x04020000					/*表示列车的IO输出数据ID，目的地ATP*/
#define RS_ID_BOARD3_TO_ATP 0x04030000					/*表示列车的IO输出数据ID，目的地ATP*/
#endif

/*-----------------------------------------------------------------------------
* 平台用到的有关平台自身输出的宏定义
* ----------------------------------------------------------------------------*/
//#ifdef  PLATDEFINE
//#define ID_PLATLINK_TO_ATO  0x06000000					/*表示平台通信连接输出数据ID，目的地ATO*/ 
//#define ID_PLATDB_TO_ATO    0x02000000					/*表示平台DB输出数据ID，目的地ATO*/ 
//#define LINK1_STATUS_TO_ATP 0x06010000					/*连接1状态数据toATP*/
//#define LINK2_STATUS_TO_ATP 0x06020000					/*连接2状态数据toATP*/
//#endif
 
/*-----------------------------------------------------------------------------
* 平台用到的有关ATP输出的宏定义
* ----------------------------------------------------------------------------*/
#ifdef ATPDEFINE
#define ATP_ID_TO_ATO 		0x07000000					/*ATP至ATO的输出数据单元ID*/

#define ATP_ID_TO_BOARD1	0x09010000					/*ATP发送给列车IO输出板1的数据单元ID*/
#define ATP_ID_TO_BOARD2	0x09020000					/*ATP发送给列车IO输出板2的数据单元ID*/
#define ATP_ID_TO_BOARD3	0x09030000					/*ATP发送给列车IO输出板3的数据单元ID*/

#define ATP_ID_MESSAGEOUT1	0x0A010000					/*ATP的通信报文输出数据单元ID*/
#define ATP_ID_MESSAGEOUT2	0x0A020000					/*ATP的通信报文输出数据单元ID*/

#define ATP_ID_LINKCTRL1	0x0B010000					/*ATP的连接控制请求数据单元ID*/
#define ATP_ID_LINKCTRL2	0x0B020000					/*ATP的连接控制请求数据单元ID*/

#define ATP_ID_DBLINKCTRL	0x0E010000					/*ATP的请求DB信息数据单元ID  */

#define LINK1_STATUS_TO_ATP 0x06010000					/*连接1状态数据toATP*/
#define LINK2_STATUS_TO_ATP 0x06020000					/*连接2状态数据toATP*/
#endif

#define ZCDEFINE
/*-----------------------------------------------------------------------------
* 平台用到的有关ZC输出的宏定义
* ----------------------------------------------------------------------------*/
#ifdef ZCDEFINE

#define ZC_ID_MESSAGEOUT	0x0A000000					/*ZC的通信报文输出数据单元ID*/
#endif

#endif

extern  PLAT_UBYTE APP_READ_ADDR[SIZE];
extern  PLAT_UBYTE APP_WRITE_ADDR[SIZE];