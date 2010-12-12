#ifndef PLATFORMHEAD_H
#define PLATFORMHEAD_H

/*-----------------------------------------------------------------------------
* ƽ̨�������������
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
* ƽ̨����ĸ��೤��ֵ
* ----------------------------------------------------------------------------*/ 
#define SIZE				2048						/*���ݰ���һ����Ԫ�ĳ���*/                         
#define NETSIZE				2048*4						/*���ݰ��ĳ���          */                       
#define CHARSIZE			SIZE*2+1
#define IDSIZE				9							/*���ݰ�ID�ĳ���        */
 
#define APP_DB_ADDR         0xc1000000					/*ƽ̨��·�����׵�ַ	*/  
#define GETSID		        0x3F						/*���������ȡԴID�ĳ���*/ 



#endif

