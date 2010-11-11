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
};    

/*----------------------------------
* 定义的数据源ID或目的地ID
* ---------------------------------*/

#define ATS1   0x00000001        //ATS1的ID    

#define ATS2   0x00000002        //ATS2的ID  

#define ZC1    0x20000001        //ZC1的ID

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

#define PLOT   0xF0000001        //现场仿真的ID  

#endif