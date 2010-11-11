#ifndef PLATFORMHEAD_H
#define PLATFORMHEAD_H

/*-----------------------------------------------------------------------------
* ƽ̨�������������
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
* ƽ̨����ĸ��೤��ֵ
* ----------------------------------------------------------------------------*/ 
#define SIZE				2048						/*���ݰ���һ����Ԫ�ĳ���*/                         
#define NETSIZE				2048*4						/*���ݰ��ĳ���          */                       
#define CHARSIZE			SIZE*2+1
#define IDSIZE				9							/*���ݰ�ID�ĳ���        */
 
#define APP_DB_ADDR         0xc1000000					/*ƽ̨��·�����׵�ַ	*/  
#define GETSID		        0x3F						/*���������ȡԴID�ĳ���*/ 
/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ATO����ĺ궨��
* ----------------------------------------------------------------------------*/
#ifdef  ATODEFINE  
#define ATO_ID_IOOUT		0x09000000					/*��ʾIO������ݵ�ID	*/
#define ATO_ID_MESSAGEOUT	0x0A000000					/*��ʾͨ�ű���������ݵ�ID*/
#define ATO_ID_ATPOUT		0x08000000					/*��ʾATO��ATP���ݵ�ID*/
#define ATO_ID_COMLINKOUT	0x0B000000					/*��ʾͨ�����ӿ������ݵ�ID*/
#define ATO_ID_TRACKDBOUT	0x0E000000					/*��ʾDB���ݿ������ݵ�ID*/
#define ATO_ID_COOUT		0x0D000000					/*��ʾ����������ݵ�ID*/
#define ATO_ID_TMSOUT		0x0F000000					/*��ʾTMS������ݵ�ID*/
#define ATO_ID_EVENTOUT		0x0C000000					/*��ʾ�¼���¼������ݵ�ID*/
#define ATO_ID_SYSDATAOUT	0x10000000					/*��ʾ�м�״̬ͬ��������ݵ�ID*/
#define ATO_ID_ATSOUT		10							/*��ʾͨ�ű���ATO��ATS������ݵ�ID*/
#define ATO_ID_TODOUT		11							/*��ʾͨ�ű���ATO��TOD������ݵ�ID*/

#define ID_PLATLINK_TO_ATO  0x06000000					/*��ʾƽ̨ͨ�������������ID��Ŀ�ĵ�ATO*/ 
#define ID_PLATDB_TO_ATO    0x02000000					/*��ʾƽ̨DB�������ID��Ŀ�ĵ�ATO*/
#endif

/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�RS��������ĺ궨��
* ----------------------------------------------------------------------------*/
#ifdef    RSDEFINE
#define RS_ID_VELOCITYOUT	0x03000000					/*��ʾ�г��Ĳ����������ID��Ŀ�ĵ�ATO*/
#define RS_ID_TO_PLOT       0x09000000					/*��ʾ�г���λ���������ID��Ŀ�ĵع��Է���*/
#define RS_ID_TO_ATP        0x03010000 					/*��ʾ�г��Ĳ����������ID��Ŀ�ĵ�ATP*/
#define RS_ID_BOARD1_TO_ATP 0x04010000					/*��ʾ�г���IO�������ID��Ŀ�ĵ�ATP*/
#define RS_ID_BOARD2_TO_ATP 0x04020000					/*��ʾ�г���IO�������ID��Ŀ�ĵ�ATP*/
#define RS_ID_BOARD3_TO_ATP 0x04030000					/*��ʾ�г���IO�������ID��Ŀ�ĵ�ATP*/
#endif

/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ƽ̨��������ĺ궨��
* ----------------------------------------------------------------------------*/
//#ifdef  PLATDEFINE
//#define ID_PLATLINK_TO_ATO  0x06000000					/*��ʾƽ̨ͨ�������������ID��Ŀ�ĵ�ATO*/ 
//#define ID_PLATDB_TO_ATO    0x02000000					/*��ʾƽ̨DB�������ID��Ŀ�ĵ�ATO*/ 
//#define LINK1_STATUS_TO_ATP 0x06010000					/*����1״̬����toATP*/
//#define LINK2_STATUS_TO_ATP 0x06020000					/*����2״̬����toATP*/
//#endif
 
/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ATP����ĺ궨��
* ----------------------------------------------------------------------------*/
#ifdef ATPDEFINE
#define ATP_ID_TO_ATO 		0x07000000					/*ATP��ATO��������ݵ�ԪID*/

#define ATP_ID_TO_BOARD1	0x09010000					/*ATP���͸��г�IO�����1�����ݵ�ԪID*/
#define ATP_ID_TO_BOARD2	0x09020000					/*ATP���͸��г�IO�����2�����ݵ�ԪID*/
#define ATP_ID_TO_BOARD3	0x09030000					/*ATP���͸��г�IO�����3�����ݵ�ԪID*/

#define ATP_ID_MESSAGEOUT1	0x0A010000					/*ATP��ͨ�ű���������ݵ�ԪID*/
#define ATP_ID_MESSAGEOUT2	0x0A020000					/*ATP��ͨ�ű���������ݵ�ԪID*/

#define ATP_ID_LINKCTRL1	0x0B010000					/*ATP�����ӿ����������ݵ�ԪID*/
#define ATP_ID_LINKCTRL2	0x0B020000					/*ATP�����ӿ����������ݵ�ԪID*/

#define ATP_ID_DBLINKCTRL	0x0E010000					/*ATP������DB��Ϣ���ݵ�ԪID  */

#define LINK1_STATUS_TO_ATP 0x06010000					/*����1״̬����toATP*/
#define LINK2_STATUS_TO_ATP 0x06020000					/*����2״̬����toATP*/
#endif

#define ZCDEFINE
/*-----------------------------------------------------------------------------
* ƽ̨�õ����й�ZC����ĺ궨��
* ----------------------------------------------------------------------------*/
#ifdef ZCDEFINE

#define ZC_ID_MESSAGEOUT	0x0A000000					/*ZC��ͨ�ű���������ݵ�ԪID*/
#endif

#endif

extern  PLAT_UBYTE APP_READ_ADDR[SIZE];
extern  PLAT_UBYTE APP_WRITE_ADDR[SIZE];