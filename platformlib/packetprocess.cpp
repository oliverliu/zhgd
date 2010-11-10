#include "packetprocess.h"
#include "stdio.h"
#include "string.h"
#include "globle.h"
#include "platformhead.h"

//******************************************************************************
// Function: packetprocess::packetprocess
// Brief:    包处理类的构造函数，完成一些初始化工作 
// Returns:	 无
// Author:   刘志盼
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
// Brief:    包处理类的析构函数
// Returns:  无
// Author:   刘志盼
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
// Brief:    平台解包程序，得到接收数据包中各单元的单元ID和单元大小 
// Returns:  void
// Author:	 刘志盼
// Data:	 2010/10/09
//******************************************************************************
void packetprocess::Decoder(PLAT_UINT8* uin)
{
	int i, len;
	//clear arrays data?

	memcpy(&index, uin, sizeof(index));
	len =index.regionUnitNum;										/*得到大数据包中单元的个数*/					
	if(len ==0)														/*若单元个数为0，返回不处理*/
	{
		printf("Data is NULL\n");
		return;
	}

	for(i =0; i <index.regionUnitNum; i++)
	{
		paddr =uin+index.unitAddrOffset[i];							/*根据各单元的偏移量求单元首地址*/
		memcpy(&id[i], paddr, sizeof(id[i]));						/*得到数据包中各单元的单元ID*/

        paddr +=sizeof(id[i]);                       
		memcpy(&unitsize[i], paddr, sizeof(unitsize[i]));			/*得到数据包中各单元的数据区大小*/
	}
}

//******************************************************************************
// Function: packetprocess::Encoder
// Brief:   平台打包程序，将数据源缓冲区中的数据打包成指定结构形式的数据包  
// Returns: void
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
void packetprocess::Encoder(PLAT_UINT8* last, PLAT_UINT8* add, int iunitoder)         
{
    index.platformStatus =1;
	index.platformHealth =1;
	int enlen;														/*用来保存各单元的单元大小*/

	if(iunitoder ==0)
	{
		index.unitAddrOffset[0] =sizeof(index);						/*设定待打数据包中第一个单元的偏移量*/
		paddr =last+index.unitAddrOffset[0];						/*得到待打数据包第一个数据单元的首地址*/
		memcpy(&unithead, add, sizeof(unithead));					

		enlen =sizeof(unithead)+unithead.unitSize;					/*得到待打数据包第一个数据单元的单元大小*/		
		memcpy(paddr, add, enlen);									/*将第一个数据单元存放到其对应的地址处去*/

		paddr +=enlen;												/*存放数据单元的指针移位*/
	}
	else
	{
		index.unitAddrOffset[iunitoder] =paddr-last;				/*设定待打数据包中其它单元的偏移量*/	
		memcpy(&unithead, add, sizeof(unithead));
	
		enlen =sizeof(unithead)+unithead.unitSize;					/*得到待打数据包其它数据单元的单元大小*/
		memcpy(paddr, add, enlen);									/*将其它数据单元存放到其对应的地址处去*/

		paddr +=enlen;												/*存放数据单元的指针移位*/
	}   
}

//******************************************************************************
// Function: packetprocess::GetDestID
// Brief:   根据数据源缓冲区中的各单元ID得到其目的地ID 
// Returns: PLAT_UINT32
// Author:  刘志盼
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
		/*若数据源为ZC，根据单元ID得到数据单元的目的地ID*/
		Tempreturn =ZC_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);		
		break;
	case ATO:		
		Tempreturn =ATO_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);/*若数据源为ATO，根据单元ID得到数据单元的目的地ID*/					
		break;
	case ATP:					
		Tempreturn =ATP_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);/*若数据源为ATP，根据单元ID得到数据单元的目的地ID*/		
		break;
	case RS:					
		Tempreturn =RS_GetDestID(SRC, Unitbuf, iUnitOder, UnitID);/*若数据源为RS，根据单元ID得到数据单元的目的地ID*/						
		break;		
	default:
		break;
	}
	return Tempreturn;	
}

//******************************************************************************
// Function: packetprocess::ATO_GetDestID
// Brief:   得到ATO发送的数据包各数据单元的目的地ID
// Returns: PLAT_UINT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::ATO_GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID) /*得到ATO发送数据包中各单元的目的地ID*/
{

	PLAT_UINT32 dest;
	dest =0;

	#ifdef ATODEFINE
	switch(UnitID)
	{
	case ATO_ID_COOUT:													/*ATO至列车的电流输出信息*/	
		dest =((SRC&0x1FFFFFFF)|0x80000000);
		break;		
	case ATO_ID_TMSOUT:													/*ATO至列车的TMS输出信息*/
		dest =((SRC&0x1FFFFFFF)|0x80000000);
		break;
	case ATO_ID_IOOUT:													/*ATO至列车的IO输出信息*/
		dest =((SRC&0x1FFFFFFF)|0x80000000);
		break;
	case ATO_ID_ATPOUT:													/*ATO至ATP的输出信息*/
		dest =((SRC&0x1FFFFFFF)|0x60000000);
		break;
	case ATO_ID_SYSDATAOUT:												/*ATO至平台的中间状态同步输出信息*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_EVENTOUT:												/*ATO至平台的事件记录输出信息*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_COMLINKOUT:												/*ATO至平台的通信连接控制信息*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_TRACKDBOUT:												/*ATO至平台的DB数据控制信息*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;
	case ATO_ID_MESSAGEOUT:												/*ATO发送的通信报文*/
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
// Brief:   得到ATP发送数据包各数据单元的目的地ID
// Returns: PLAT_UINT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::ATP_GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID) /*得到ATP发送数据包中各单元的目的地ID*/
{
	PLAT_UINT32 dest;
	dest =0;

	#ifdef ATPDEFINE
	switch(UnitID)
	{
	case ATP_ID_TO_ATO:													/*ATP至ATO*/

		dest =((SRC&0x1FFFFFFF)|0x40000000);

		break;
		
	case ATP_ID_MESSAGEOUT1:											/*ATP发送的通信报文*/
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
	case ATP_ID_TO_BOARD1:												/*ATP至列车*/

		dest =((SRC&0x1FFFFFFF)|0x80000000);

		break;
		
	case ATP_ID_TO_BOARD2:												/*ATP至列车*/

		dest =((SRC&0x1FFFFFFF)|0x80000000);
		
		break;
		
	case ATP_ID_TO_BOARD3:												/*ATP至列车*/

		dest =((SRC&0x1FFFFFFF)|0x80000000);
		
		break;
		
	case ATP_ID_LINKCTRL1:												/*ATP至平台*/

		dest =((SRC&0x1FFFFFFF)|0xC0000000);

		break;		
		
	case ATP_ID_LINKCTRL2:												/*ATP至平台*/

		dest =((SRC&0x1FFFFFFF)|0xC0000000);

		break;		
		
	case ATP_ID_DBLINKCTRL:												/*ATP至平台*/

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
// Brief:   得到列车运行仿真发送数据包各单元的目的地ID
// Returns: PLAT_UINT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_UINT32 packetprocess::RS_GetDestID(PLAT_UINT32 SRC,PLAT_UINT8* Unitbuf, int iUnitOder,PLAT_UINT32 UnitID)  /*得到RS发送数据包中各单元的目的地ID*/
{
	PLAT_UINT32 dest;
	dest =0;

    #ifdef RSDEFINE			
	switch(UnitID)
	{
	case RS_ID_VELOCITYOUT:												/*列车至ATO*/
		
		dest =((SRC&0x1FFFFFFF)|0x40000000);

		break;
		
	case RS_ID_TO_PLOT:													/*列车至现场*/
		
		dest =PLOT;

		break;
		
	case RS_ID_TO_ATP:													/*列车至ATP*/
		
		dest =((SRC&0x1FFFFFFF)|0x60000000);

		break;
		
	case RS_ID_BOARD1_TO_ATP:											/*列车至ATP*/
		
		dest =((SRC&0x1FFFFFFF)|0x60000000);

		break;
		
	case RS_ID_BOARD2_TO_ATP:											/*列车至ATP*/
			
		dest =((SRC&0x1FFFFFFF)|0x60000000);

		break;
		
	case RS_ID_BOARD3_TO_ATP:											/*列车至ATP*/
		
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
// Brief:   得到ZC发送数据包各单元的目的地ID
// Returns: PLAT_UINT32
// Author:  刘志盼
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
// Brief:   平台对发送至平台的数据单元的处理函数 
// Returns: void
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
void  packetprocess::HandlePlatData(PLAT_UINT32 SRC, PLAT_UINT8 *bufscr, PLAT_UINT8 *bufdes)
{
	pPlataddr =bufdes;
    memcpy(&unithead.unitId, bufscr, sizeof(unithead.unitId));
	
	switch((SRC&0xE0000000)>>29)
	{
	case ATS:														/*对ATS发送至平台的数据包的处理*/
		break;
	case ZC:														/*对ZC发送至平台的数据包的处理*/
		break;
	case ATO:														/*对ATO发送至平台的数据包的处理*/		
		{
#ifdef ATODEFINE
			switch(unithead.unitId)
			{
			case ATO_ID_SYSDATAOUT:       
			case ATO_ID_EVENTOUT:       
				break;
			case ATO_ID_COMLINKOUT:									/*对ATO的连接请求进行处理，返回连接状态信息	*/
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
				
			case ATO_ID_TRACKDBOUT:									/*对ATO的数据库更新请求进行处理，返回数据库更新信息*/
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
	case ATP:													/*对ATP发送至平台的数据包的处理*/
		{
#ifdef ATPDEFINE
			switch(unithead.unitId)
			{
				
			case ATP_ID_LINKCTRL1:								/*对ATP的连接请求进行处理，返回连接状态信息	*/
			case ATP_ID_LINKCTRL2:
			case ATP_ID_DBLINKCTRL:								/*对ATP的数据库更新请求进行处理，返回数据库更新信息*/
			default:
				break;
			}
#endif
		}		
		break;
		
	case CBI:													/*对CBI发送至平台的数据包的处理*/
		break;
	default:
		break;
	}
}