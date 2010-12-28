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
}T_TYPE;

/*----------------------------------
* ���������ԴID��Ŀ�ĵ�ID
* ---------------------------------*/

#define ATS1   0x00000001        //ATS1��ID    

#define ATS2   0x00000002        //ATS2��ID  

#define ZC1    211        //ZC1��ID

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

#define PLAT_ID_MESSAGEOUT  0x0A000000          /*��ʾͨ�ű���������ݵ�ID*/
#define PLOT   0xF0000001        //�ֳ������ID  

/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ATO����ĺ궨��
* ----------------------------------------------------------------------------*/
#define PLAT_ATO_ID_IOOUT               0x09000000                                      /*��ʾIO������ݵ�ID    */
#define PLAT_ATO_ID_MESSAGEOUT  0x0A000000                                      /*��ʾͨ�ű���������ݵ�ID*/
#define PLAT_ATO_ID_ATPOUT              0x08000000                                      /*��ʾATO��ATP���ݵ�ID*/
#define PLAT_ATO_ID_COMLINKOUT  0x0B000000                                      /*��ʾͨ�����ӿ������ݵ�ID*/
#define PLAT_ATO_ID_TRACKDBOUT  0x0E000000                                      /*��ʾDB���ݿ������ݵ�ID*/
#define PLAT_ATO_ID_COOUT               0x0D000000                                      /*��ʾ����������ݵ�ID*/
#define PLAT_ATO_ID_TMSOUT              0x0F000000                                      /*��ʾTMS������ݵ�ID*/
#define PLAT_ATO_ID_EVENTOUT    0x0C000000                                      /*��ʾ�¼���¼������ݵ�ID*/
#define PLAT_ATO_ID_SYSDATAOUT  0x10000000                                      /*��ʾ�м�״̬ͬ��������ݵ�ID*/
#define PLAT_ATO_ID_ATSOUT              10                                                      /*��ʾͨ�ű���ATO��ATS������ݵ�ID*/
#define PLAT_ATO_ID_TODOUT              11                                                      /*��ʾͨ�ű���ATO��TOD������ݵ�ID*/

#define PLAT_ID_PLATLINK_TO_ATO  0x06000000                                     /*��ʾƽ̨ͨ�������������ID��Ŀ�ĵ�ATO*/ 
#define PLAT_ID_PLATDB_TO_ATO    0x02000000                                     /*��ʾƽ̨DB�������ID��Ŀ�ĵ�ATO*/

/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�RS��������ĺ궨��
* ----------------------------------------------------------------------------*/

#define PLAT_RS_ID_VELOCITYOUT  0x03000000                                      /*��ʾ�г��Ĳ����������ID��Ŀ�ĵ�ATO*/
#define PLAT_RS_ID_TO_PLOT       0x09000000                                     /*��ʾ�г���λ���������ID��Ŀ�ĵع��Է���*/
#define PLAT_RS_ID_TO_ATP        0x03010000                                     /*��ʾ�г��Ĳ����������ID��Ŀ�ĵ�ATP*/
#define PLAT_RS_ID_BOARD1_TO_ATP 0x04010000                                     /*��ʾ�г���IO�������ID��Ŀ�ĵ�ATP*/
#define PLAT_RS_ID_BOARD2_TO_ATP 0x04020000                                     /*��ʾ�г���IO�������ID��Ŀ�ĵ�ATP*/
#define PLAT_RS_ID_BOARD3_TO_ATP 0x04030000                                     /*��ʾ�г���IO�������ID��Ŀ�ĵ�ATP*/

/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ƽ̨��������ĺ궨��
* ----------------------------------------------------------------------------*/
//#ifdef  PLATDEFINE
//#define ID_PLATLINK_TO_ATO  0x06000000                                        /*��ʾƽ̨ͨ�������������ID��Ŀ�ĵ�ATO*/ 
//#define ID_PLATDB_TO_ATO    0x02000000                                        /*��ʾƽ̨DB�������ID��Ŀ�ĵ�ATO*/ 
//#define LINK1_STATUS_TO_ATP 0x06010000                                        /*����1״̬����toATP*/
//#define LINK2_STATUS_TO_ATP 0x06020000                                        /*����2״̬����toATP*/
//#endif
 
/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ATP����ĺ궨��
* ----------------------------------------------------------------------------*/
#define PLAT_ATP_ID_TO_ATO              0x07000000                                      /*ATP��ATO��������ݵ�ԪID*/

#define PLAT_ATP_ID_TO_BOARD1   0x09010000                                      /*ATP���͸��г�IO�����1�����ݵ�ԪID*/
#define PLAT_ATP_ID_TO_BOARD2   0x09020000                                      /*ATP���͸��г�IO�����2�����ݵ�ԪID*/
#define PLAT_ATP_ID_TO_BOARD3   0x09030000                                      /*ATP���͸��г�IO�����3�����ݵ�ԪID*/

#define PLAT_ATP_ID_MESSAGEOUT1 0x0A010000                                      /*ATP��ͨ�ű���������ݵ�ԪID*/
#define PLAT_ATP_ID_MESSAGEOUT2 0x0A020000                                      /*ATP��ͨ�ű���������ݵ�ԪID*/

#define PLAT_ATP_ID_LINKCTRL1   0x0B010000                                      /*ATP�����ӿ����������ݵ�ԪID*/
#define PLAT_ATP_ID_LINKCTRL2   0x0B020000                                      /*ATP�����ӿ����������ݵ�ԪID*/

#define PLAT_ATP_ID_DBLINKCTRL  0x0E010000                                      /*ATP������DB��Ϣ���ݵ�ԪID  */

#define PLAT_LINK1_STATUS_TO_ATP 0x06010000                                     /*����1״̬����toATP*/
#define PLAT_LINK2_STATUS_TO_ATP 0x06020000                                     /*����2״̬����toATP*/


/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ZC����ĺ궨��
* ----------------------------------------------------------------------------*/
#ifdef ZCDEFINE
#define ZC_ID_MESSAGEOUT	0x0A000000					/*ZC��ͨ�ű���������ݵ�ԪID*/
#endif

#endif