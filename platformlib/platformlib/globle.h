#ifndef GLOBLE_H
#define GLOBLE_H

/*----------------------------------
* ���������Դ����
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
* ���������ԴID��Ŀ�ĵ�ID
* ---------------------------------*/

#define ATS1   0x00000001        //ATS1��ID    

#define ATS2   0x00000002        //ATS2��ID  

#define ZC1    0x20000001        //ZC1��ID

#define ZC2    0x20000002        //ZC2��ID

#define CC1    0x00000FF1        //�ݶ�ZC���͸�CC1�����ݰ���Ŀ�ĵ�ID

#define CC2    0x00000FF2        //�ݶ�ZC���͸�CC2�����ݰ���Ŀ�ĵ�ID

#define ATO1   0x40000001        //ATO1��ID

#define ATO2   0x40000002        //ATO2��ID

#define ATP1   0x60000001        //ATP1��ID

#define ATP2   0x60000002        //ATP2��ID

#define RS1    0x80000001        //RS1��ID

#define RS2    0x80000002        //RS2��ID

#define CBI1   0xA0000001        //CBI1��ID

#define CBI2   0xA0000002        //CBI2��ID

#define PLAT1  0xC0000001        //ƽ̨1��ID

#define PLAT2  0xC0000002        //ƽ̨2��ID

#define TOD1   0xE0000001        //TOD1��ID

#define TOD2   0xE0000002        //TOD2��ID

#define PLOT   0xF0000001        //�ֳ������ID  

#endif