#include "packetprocess.h"
#include "stdio.h"
#include "string.h"
#include "globle.h"
#include "platformhead.h"

//******************************************************************************
// Function: packetprocess::packetprocess
// Brief:    ��������Ĺ��캯�������һЩ��ʼ������ 
// Returns:	 ��
// Author:   ��־��
// Data:	2010/10/09
//******************************************************************************
packetprocess::packetprocess()       
{
	int i;
	paddr =NULL;
	memset(&index, 0x00, sizeof(index)	);
	memset(&unithead, 0x00, sizeof(unithead));
	for(i =0; i <256; i++)
	{
		id[i] =0;
		unitsize[i] =0;
	}
	memset(&messageHeadOut, 0x00, sizeof(messageHeadOut));
}

//******************************************************************************
// Function: packetprocess::~packetprocess
// Brief:    �����������������
// Returns:  ��
// Author:   ��־��
// Data:	2010/10/09
//******************************************************************************
packetprocess::~packetprocess()
{
	
}

bool packetprocess::hasMsgHeader()
{
	return true;
}
//******************************************************************************
// Function: packetprocess::Decoder
// Brief:    ƽ̨������򣬵õ��������ݰ��и���Ԫ�ĵ�ԪID�͵�Ԫ��С 
// Returns:  void
// Author:	 ��־��
// Data:	 2010/10/09
//******************************************************************************
void packetprocess::Decoder(PLAT_UINT8* uin)
{
	int i, len;
	//clear arrays data?

	memcpy(&index, uin, sizeof(index));
	len =index.regionUnitNum;										/*�õ������ݰ��е�Ԫ�ĸ���*/					
	if(len ==0)														/*����Ԫ����Ϊ0�����ز�����*/
	{
		printf("Data is NULL\n");
		return;
	}

	for(i =0; i <index.regionUnitNum; i++)
	{
		paddr =uin+index.unitAddrOffset[i];							/*���ݸ���Ԫ��ƫ������Ԫ�׵�ַ*/
		memcpy(&id[i], paddr, sizeof(id[i]));						/*�õ����ݰ��и���Ԫ�ĵ�ԪID*/

        paddr +=sizeof(id[i]);                       
		memcpy(&unitsize[i], paddr, sizeof(unitsize[i]));			/*�õ����ݰ��и���Ԫ����������С*/
	}
}

//******************************************************************************
// Function: packetprocess::Encoder
// Brief:   ƽ̨������򣬽�����Դ�������е����ݴ����ָ���ṹ��ʽ�����ݰ�  
// Returns: void
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
void packetprocess::Encoder(PLAT_UINT8* last, PLAT_UINT8* add, int iunitoder)         
{
    index.platformStatus =1;
	index.platformHealth =1;
	int enlen;														/*�����������Ԫ�ĵ�Ԫ��С*/

	if(iunitoder ==0)
	{
		index.unitAddrOffset[0] =sizeof(index);						/*�趨�������ݰ��е�һ����Ԫ��ƫ����*/
		paddr =last+index.unitAddrOffset[0];						/*�õ��������ݰ���һ�����ݵ�Ԫ���׵�ַ*/
		memcpy(&unithead, add, sizeof(unithead));					

		enlen =sizeof(unithead)+unithead.unitSize;					/*�õ��������ݰ���һ�����ݵ�Ԫ�ĵ�Ԫ��С*/		
		memcpy(paddr, add, enlen);									/*����һ�����ݵ�Ԫ��ŵ����Ӧ�ĵ�ַ��ȥ*/

		paddr +=enlen;												/*������ݵ�Ԫ��ָ����λ*/
	}
	else
	{
		index.unitAddrOffset[iunitoder] =paddr-last;				/*�趨�������ݰ���������Ԫ��ƫ����*/	
		memcpy(&unithead, add, sizeof(unithead));
	
		enlen =sizeof(unithead)+unithead.unitSize;					/*�õ��������ݰ��������ݵ�Ԫ�ĵ�Ԫ��С*/
		memcpy(paddr, add, enlen);									/*���������ݵ�Ԫ��ŵ����Ӧ�ĵ�ַ��ȥ*/

		paddr +=enlen;												/*������ݵ�Ԫ��ָ����λ*/
	}   
}

//******************************************************************************
// Function: packetprocess::GetDestID
// Brief:   ��������Դ�������еĸ���ԪID�õ���Ŀ�ĵ�ID 
// Returns: PLAT_UINT32
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID)
{   
	PLAT_UINT32 Tempreturn = 0;

	switch((SRC&0xE0000000)>>29)
	{
	case ATS:
	case CBI:
		break;
	case ZC:
		/*������ԴΪZC�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/
		Tempreturn =ZC_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);		
		break;
	case ATO:		
		Tempreturn =ATO_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);/*������ԴΪATO�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/					
		break;
	case ATP:					
		Tempreturn =ATP_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);/*������ԴΪATP�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/		
		break;
	case RS:					
		Tempreturn =RS_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);/*������ԴΪRS�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/						
		break;		
	default:
		break;
	}
	return Tempreturn;	
}

//******************************************************************************
// Function: packetprocess::ATO_GetDestID
// Brief:   �õ�ATO���͵����ݰ������ݵ�Ԫ��Ŀ�ĵ�ID
// Returns: PLAT_UINT32
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::ATO_GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID) /*�õ�ATO�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/
{

	PLAT_UINT32 dest;
	dest =0;

	#ifdef ATODEFINE
	switch(UnitID)
	{
	case ATO_ID_COOUT:													/*ATO���г��ĵ��������Ϣ*/	
		dest =((SRC&0x1FFFFFFF)|0x80000000);
		break;		
	case ATO_ID_TMSOUT:													/*ATO���г���TMS�����Ϣ*/
		dest =((SRC&0x1FFFFFFF)|0x80000000);
		break;
	case ATO_ID_IOOUT:													/*ATO���г���IO�����Ϣ*/
		dest =((SRC&0x1FFFFFFF)|0x80000000);
		break;
	case ATO_ID_ATPOUT:													/*ATO��ATP�������Ϣ*/
		dest =((SRC&0x1FFFFFFF)|0x60000000);
		break;
	case ATO_ID_SYSDATAOUT:												/*ATO��ƽ̨���м�״̬ͬ�������Ϣ*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_EVENTOUT:												/*ATO��ƽ̨���¼���¼�����Ϣ*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_COMLINKOUT:												/*ATO��ƽ̨��ͨ�����ӿ�����Ϣ*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_TRACKDBOUT:												/*ATO��ƽ̨��DB���ݿ�����Ϣ*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_MESSAGEOUT:												/*ATO���͵�ͨ�ű���*/
		{
			memcpy(&messageHeadOut, (Unitbuf+index.unitAddrOffset[iUnitOder]
				+sizeof(T_UNIT_HEAD)), sizeof(messageHeadOut));
			
			dest =messageHeadOut.DID;

			break;
		}
		
	default:
		
		dest =0;
	}
	#endif

	return dest;
}

//******************************************************************************
// Function: packetprocess::ATP_GetDestID
// Brief:   �õ�ATP�������ݰ������ݵ�Ԫ��Ŀ�ĵ�ID
// Returns: PLAT_UINT32
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::ATP_GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID) /*�õ�ATP�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/
{
	PLAT_UINT32 dest;
	dest =0;

	#ifdef ATPDEFINE
	switch(UnitID)
	{
	case ATP_ID_TO_ATO:													/*ATP��ATO*/

		dest =((SRC&0x1FFFFFFF)|0x40000000);

		break;
		
	case ATP_ID_MESSAGEOUT1:											/*ATP���͵�ͨ�ű���*/
		{
			memcpy(&messageHeadOut, (Unitbuf+index.unitAddrOffset[iUnitOder]
				+sizeof(T_UNIT_HEAD)), sizeof(messageHeadOut));
			
			dest =messageHeadOut.DID;

			break;
		}
	case ATP_ID_MESSAGEOUT2:
		{
			memcpy(&messageHeadOut, (Unitbuf+index.unitAddrOffset[iUnitOder]
				+sizeof(T_UNIT_HEAD)), sizeof(messageHeadOut));
			
			dest =messageHeadOut.DID;

			break;
		}
	case ATP_ID_TO_BOARD1:												/*ATP���г�*/

		dest =((SRC&0x1FFFFFFF)|0x80000000);

		break;
		
	case ATP_ID_TO_BOARD2:												/*ATP���г�*/

		dest =((SRC&0x1FFFFFFF)|0x80000000);
		
		break;
		
	case ATP_ID_TO_BOARD3:												/*ATP���г�*/

		dest =((SRC&0x1FFFFFFF)|0x80000000);
		
		break;
		
	case ATP_ID_LINKCTRL1:												/*ATP��ƽ̨*/

		dest =((SRC&0x1FFFFFFF)|0xC0000000);

		break;		
		
	case ATP_ID_LINKCTRL2:												/*ATP��ƽ̨*/

		dest =((SRC&0x1FFFFFFF)|0xC0000000);

		break;		
		
	case ATP_ID_DBLINKCTRL:												/*ATP��ƽ̨*/

		dest =((SRC&0x1FFFFFFF)|0xC0000000);

		break;	
		
	default:

		dest =0;			
	}
	#endif

	return dest;
}


//******************************************************************************
// Function: packetprocess::RS_GetDestID
// Brief:   �õ��г����з��淢�����ݰ�����Ԫ��Ŀ�ĵ�ID
// Returns: PLAT_UINT32
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::RS_GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID)  /*�õ�RS�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/
{
	PLAT_UINT32 dest;
	dest =0;

    #ifdef RSDEFINE			
	switch(UnitID)
	{
	case RS_ID_VELOCITYOUT:												/*�г���ATO*/
		
		dest =((SRC&0x1FFFFFFF)|0x40000000);

		break;
		
	case RS_ID_TO_PLOT:													/*�г����ֳ�*/
		
		dest =PLOT;

		break;
		
	case RS_ID_TO_ATP:													/*�г���ATP*/
		
		dest =((SRC&0x1FFFFFFF)|0x60000000);

		break;
		
	case RS_ID_BOARD1_TO_ATP:											/*�г���ATP*/
		
		dest =((SRC&0x1FFFFFFF)|0x60000000);

		break;
		
	case RS_ID_BOARD2_TO_ATP:											/*�г���ATP*/
			
		dest =((SRC&0x1FFFFFFF)|0x60000000);

		break;
		
	case RS_ID_BOARD3_TO_ATP:											/*�г���ATP*/
		
		dest =((SRC&0x1FFFFFFF)|0x60000000);

		break;
		
	default:

		dest =0;
	}
	#endif

	return dest;
}

//******************************************************************************
// Function: packetprocess::ZC_GetDestID
// Brief:   �õ�ZC�������ݰ�����Ԫ��Ŀ�ĵ�ID
// Returns: PLAT_UINT32
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::ZC_GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID) 
{
	PLAT_UINT32 dest;
	dest =0;
	
 #ifdef ZCDEFINE
	switch(UnitID)
	{
		
	    case ZC_ID_MESSAGEOUT:
		{
			memcpy(&messageHeadOut, (Unitbuf+index.unitAddrOffset[iUnitOder]
				+sizeof(T_UNIT_HEAD)), sizeof(messageHeadOut));
			dest = messageHeadOut.DID;
			break;
		}
	   default:
		
		dest =0;
	}
#endif
		
	return dest;
}

//******************************************************************************
// Function: packetprocess::HandlePlatData
// Brief:   ƽ̨�Է�����ƽ̨�����ݵ�Ԫ�Ĵ����� 
// Returns: void
// Author:  ��־��
// Data:	2010/10/09
//******************************************************************************
void  packetprocess::HandlePlatData(PLAT_UINT32 SRC, PLAT_UINT8 *bufscr, PLAT_UINT8 *bufdes)
{
	pPlataddr =bufdes;
    memcpy(&unithead.unitId, bufscr, sizeof(unithead.unitId));
	
	switch((SRC&0xE0000000)>>29)
	{
	case ATS:														/*��ATS������ƽ̨�����ݰ��Ĵ���*/
		break;
	case ZC:														/*��ZC������ƽ̨�����ݰ��Ĵ���*/
		break;
	case ATO:														/*��ATO������ƽ̨�����ݰ��Ĵ���*/		
		{
#ifdef ATODEFINE
			switch(unithead.unitId)
			{
			case ATO_ID_SYSDATAOUT:       
			case ATO_ID_EVENTOUT:       
				break;
			case ATO_ID_COMLINKOUT:									/*��ATO������������д�����������״̬��Ϣ	*/
				{
					T_UNIT_HEAD linkinhead;
					linkinhead.unitId = ID_PLATLINK_TO_ATO;         
					
					T_COM_LINK_IN  linkin;
					linkin.CtrlRequest =0x23;
					linkinhead.unitSize =sizeof(linkin);
					
					memcpy(pPlataddr, &linkinhead, sizeof(linkinhead));
					pPlataddr +=sizeof(linkinhead);
					memcpy(pPlataddr, &linkin, sizeof(linkin));
				}
				break;
				
			case ATO_ID_TRACKDBOUT:									/*��ATO�����ݿ����������д����������ݿ������Ϣ*/
				{
					T_UNIT_HEAD dbinhead;
					dbinhead.unitId =ID_PLATDB_TO_ATO;
					
					T_TRACK_DB_IN  dbin;
					dbin.addr =APP_DB_ADDR;
					dbin.size =6024;
					dbin.version =1111;
					dbin.state =0x50;
					dbinhead.unitSize =sizeof(dbin);
					
					memcpy(pPlataddr, &dbinhead, sizeof(dbinhead));
					pPlataddr +=sizeof(dbinhead);
					memcpy(pPlataddr, &dbin, sizeof(dbin));
				}
				break;
			default:
				break;
			}
#endif
		}
		break;
	case ATP:													/*��ATP������ƽ̨�����ݰ��Ĵ���*/
		{
#ifdef ATPDEFINE
			switch(unithead.unitId)
			{
				
			case ATP_ID_LINKCTRL1:								/*��ATP������������д�����������״̬��Ϣ	*/
			case ATP_ID_LINKCTRL2:
			case ATP_ID_DBLINKCTRL:								/*��ATP�����ݿ����������д����������ݿ������Ϣ*/
			default:
				break;
			}
#endif
		}		
		break;
		
	case CBI:													/*��CBI������ƽ̨�����ݰ��Ĵ���*/
		break;
	default:
		break;
	}
}