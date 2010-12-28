#ifndef GLOBLE_H
#define GLOBLE_H

/*----------------------------------
* 定义的数据源类型
* ---------------------------------*/
typedef enum   
{
		ATS,
		ZC,
		ATO,
		ATP,
		RS,
		CBI,
		PLAT,
		TOD
}T_TYPE;

/*----------------------------------
* 定义的数据源ID或目的地ID
* ---------------------------------*/

#define ATS1   0x00000001        //ATS1的ID    

#define ATS2   0x00000002        //ATS2的ID  

#define ZC1    211        //ZC1的ID

#define ZC2    0x20000002        //ZC2的ID

#define CC1    0x00000FF1        //暂定ZC发送给CC1的数据包的目的地ID

#define CC2    0x00000FF2        //暂定ZC发送给CC2的数据包的目的地ID

#define ATO1   0x40000001        //ATO1的ID

#define ATO2   0x40000002        //ATO2的ID

#define ATP1   0x60000001        //ATP1的ID

#define ATP2   0x60000002        //ATP2的ID

#define RS1    0x80000001        //RS1的ID

#define RS2    0x80000002        //RS2的ID

#define CBI1   0xA0000001        //CBI1的ID

#define CBI2   0xA0000002        //CBI2的ID

#define PLAT1  0xC0000001        //平台1的ID

#define PLAT2  0xC0000002        //平台2的ID

#define TOD1   0xE0000001        //TOD1的ID

#define TOD2   0xE0000002        //TOD2的ID

#define PLAT_ID_MESSAGEOUT  0x0A000000          /*表示通信报文输出数据的ID*/
#define PLOT   0xF0000001        //现场仿真的ID  

/*-----------------------------------------------------------------------------
* 平台用到的有关ATO输出的宏定义
* ----------------------------------------------------------------------------*/
#define PLAT_ATO_ID_IOOUT               0x09000000                                      /*表示IO输出数据的ID    */
#define PLAT_ATO_ID_MESSAGEOUT  0x0A000000                                      /*表示通信报文输出数据的ID*/
#define PLAT_ATO_ID_ATPOUT              0x08000000                                      /*表示ATO到ATP数据的ID*/
#define PLAT_ATO_ID_COMLINKOUT  0x0B000000                                      /*表示通信连接控制数据的ID*/
#define PLAT_ATO_ID_TRACKDBOUT  0x0E000000                                      /*表示DB数据控制数据的ID*/
#define PLAT_ATO_ID_COOUT               0x0D000000                                      /*表示电流输出数据的ID*/
#define PLAT_ATO_ID_TMSOUT              0x0F000000                                      /*表示TMS输出数据的ID*/
#define PLAT_ATO_ID_EVENTOUT    0x0C000000                                      /*表示事件记录输出数据的ID*/
#define PLAT_ATO_ID_SYSDATAOUT  0x10000000                                      /*表示中间状态同步输出数据的ID*/
#define PLAT_ATO_ID_ATSOUT              10                                                      /*表示通信报文ATO到ATS输出数据的ID*/
#define PLAT_ATO_ID_TODOUT              11                                                      /*表示通信报文ATO到TOD输出数据的ID*/

#define PLAT_ID_PLATLINK_TO_ATO  0x06000000                                     /*表示平台通信连接输出数据ID，目的地ATO*/ 
#define PLAT_ID_PLATDB_TO_ATO    0x02000000                                     /*表示平台DB输出数据ID，目的地ATO*/

/*-----------------------------------------------------------------------------
* 平台用到的有关RS仿真输出的宏定义
* ----------------------------------------------------------------------------*/

#define PLAT_RS_ID_VELOCITYOUT  0x03000000                                      /*表示列车的测速输出数据ID，目的地ATO*/
#define PLAT_RS_ID_TO_PLOT       0x09000000                                     /*表示列车的位置输出数据ID，目的地轨旁仿真*/
#define PLAT_RS_ID_TO_ATP        0x03010000                                     /*表示列车的测速输出数据ID，目的地ATP*/
#define PLAT_RS_ID_BOARD1_TO_ATP 0x04010000                                     /*表示列车的IO输出数据ID，目的地ATP*/
#define PLAT_RS_ID_BOARD2_TO_ATP 0x04020000                                     /*表示列车的IO输出数据ID，目的地ATP*/
#define PLAT_RS_ID_BOARD3_TO_ATP 0x04030000                                     /*表示列车的IO输出数据ID，目的地ATP*/

/*-----------------------------------------------------------------------------
* 平台用到的有关平台自身输出的宏定义
* ----------------------------------------------------------------------------*/
//#ifdef  PLATDEFINE
//#define ID_PLATLINK_TO_ATO  0x06000000                                        /*表示平台通信连接输出数据ID，目的地ATO*/ 
//#define ID_PLATDB_TO_ATO    0x02000000                                        /*表示平台DB输出数据ID，目的地ATO*/ 
//#define LINK1_STATUS_TO_ATP 0x06010000                                        /*连接1状态数据toATP*/
//#define LINK2_STATUS_TO_ATP 0x06020000                                        /*连接2状态数据toATP*/
//#endif
 
/*-----------------------------------------------------------------------------
* 平台用到的有关ATP输出的宏定义
* ----------------------------------------------------------------------------*/
#define PLAT_ATP_ID_TO_ATO              0x07000000                                      /*ATP至ATO的输出数据单元ID*/

#define PLAT_ATP_ID_TO_BOARD1   0x09010000                                      /*ATP发送给列车IO输出板1的数据单元ID*/
#define PLAT_ATP_ID_TO_BOARD2   0x09020000                                      /*ATP发送给列车IO输出板2的数据单元ID*/
#define PLAT_ATP_ID_TO_BOARD3   0x09030000                                      /*ATP发送给列车IO输出板3的数据单元ID*/

#define PLAT_ATP_ID_MESSAGEOUT1 0x0A010000                                      /*ATP的通信报文输出数据单元ID*/
#define PLAT_ATP_ID_MESSAGEOUT2 0x0A020000                                      /*ATP的通信报文输出数据单元ID*/

#define PLAT_ATP_ID_LINKCTRL1   0x0B010000                                      /*ATP的连接控制请求数据单元ID*/
#define PLAT_ATP_ID_LINKCTRL2   0x0B020000                                      /*ATP的连接控制请求数据单元ID*/

#define PLAT_ATP_ID_DBLINKCTRL  0x0E010000                                      /*ATP的请求DB信息数据单元ID  */

#define PLAT_LINK1_STATUS_TO_ATP 0x06010000                                     /*连接1状态数据toATP*/
#define PLAT_LINK2_STATUS_TO_ATP 0x06020000                                     /*连接2状态数据toATP*/


/*-----------------------------------------------------------------------------
* 平台用到的有关ZC输出的宏定义
* ----------------------------------------------------------------------------*/
#ifdef ZCDEFINE
#define ZC_ID_MESSAGEOUT	0x0A000000					/*ZC的通信报文输出数据单元ID*/
#endif

#endif