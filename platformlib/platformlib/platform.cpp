#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "platform.h"
#include "credis.h"
#include "globle.h"
#include "wintimer.h"
#include "cutility.h"
#include "packetprocess.h"

#include <stdarg.h>
int msglevel = 3; /* the higher, the more messages... */

#if defined(NDEBUG) && defined(__GNUC__)
#define pmesg(level, format, args...) ((void)0)
#else
void pmesg(int level, const char* format, ...) {
#ifdef NDEBUG
	/* Empty body, so a good compiler will optimise calls
	   to pmesg away */
#else
        va_list args;

        if (level > msglevel)
                return;

        va_start(args, format);
        vfprintf(stdout, format, args);
		//vfprintf(stderr, format, args);
        va_end(args);
#endif /* NDEBUG */
#endif /* NDEBUG && __GNUC__ */
}

// 
// void pFatal(const char* format, ...)   { return pmesg(0, format, ...); }
// void pWarning(const char* format, ...) { return pmesg(1, format, ...); }
// void pDebug(const char* format, ...)   { return pmesg(2, format, ...); }
// void pBig(const char* format, ...)     { return pmesg(5, format, ...); }


PLAT_UINT32 CAppInterface::srcID = 0;

CAppInterface::CAppInterface()
{
	readcount =0;									/*记录平台的读周期*/	
	writecount =0;									/*记录平台的写周期*/
	m_pzc = new CZc();
	m_pei = new packetprocess();
}

CAppInterface::~CAppInterface()
{
	if(m_pzc)
	{
		delete m_pzc;
		m_pzc = NULL;
	}
	if( m_pei )
	{
		delete m_pei;
		m_pei = NULL;
	}

}

//******************************************************************************
// Function: CAppInterface::AppClose
// Brief:   关闭平台接口 ？？
// Returns: PLAT_INT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_INT32 CAppInterface::AppClose()
{
	app_close();
	return 0;
}

//******************************************************************************
// Function: CAppInterface::OutputUint
// Brief:   输入、输出数据包打印函数 
// Returns: void
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
void CAppInterface::OutputUint(PLAT_UINT8* uout,int len)
{
	int i;	
	for(i =0;i <len;i++)
	{	
		if(!(uout[i] ==0))
		printf("%u  ",uout[i]);
	}	
	printf("\n");
}

//******************************************************************************
// Function: CAppInterface::AppInit
// Brief:   平台提供的初始化调用函数 
// Returns: PLAT_INT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************

PLAT_UBYTE APP_READ_ADDR[SIZE];
PLAT_UBYTE APP_WRITE_ADDR[SIZE];

const char * s_host = "192.168.20.100";

PLAT_INT32 CAppInterface::AppInit()
{
	send = APP_WRITE_ADDR;
	recv = APP_READ_ADDR;

	memset(send, 0, SIZE);
	memset(recv, 0, SIZE);
	memset(src, 0, IDSIZE);

	CUtility::initBigPackIdx(send);
	CUtility::initBigPackIdx(recv);
	CUtility::initBigPackIdx(platBuf);

	app_init(uintBuf,s_host);

	//srcID = 0;
	
// 	srcID =sid;
// 	memset(src, '\0', IDSIZE);
// 	sprintf(src, "%08x", sid);
// 	memset(send, '\0', NETSIZE);
// 	memset(recv, '\0', NETSIZE);

	return 0;
}

PLAT_INT32 CAppInterface::AppInit(PLAT_UINT8* s,PLAT_UINT8* r,PLAT_UINT32 sid,char *host)
{
	app_init(uintBuf,host);
	
	send = s;
	recv = r;
	
	srcID = sid;
	memset(src, '\0', IDSIZE);
	sprintf(src, "%08x", sid);
	memset(send, '\0', NETSIZE);
	memset(recv, '\0', NETSIZE);
	return 0;
}

//******************************************************************************
// Function: CAppInterface::AppWrite
// Brief:   平台提供的写数据包函数 
// Returns: PLAT_INT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_INT32 CAppInterface::AppWrite()
{
	int j;
	writecount ++;
    m_pei->pPlataddr = platBuf;
	m_pei->Decoder(send);								 /*平台对接收到的数据包进行解包，得到各单元的单元ID和单元大小*/
	
	
	if (srcID == 0)
	{
		PLAT_UBYTE *p = CUtility::getUnitHead(send, 0);
		srcID = CUtility::getLittlePackSID(p);
		sprintf(src, "%08x", srcID);		
	}

	if(app_display() ==1)
	{
		printf("平台的写周期为：%u\n", writecount);
		printf("SEND DATA BEGAN------------------------------SEND DATA BEGAN \n");
		if(app_displaybig() ==1)
		{
			printf("发送的大包内容为：\n");
			OutputUint(send, NETSIZE);
		}
		printf("发送数据包源ID：%x\n", srcID);
		printf("发送数据包中数据单元个数=%u\n", m_pei->index.regionUnitNum); 
	}	           

	for(j =0;j < m_pei->index.regionUnitNum;j++)		/*循环遍历数据包中各数据单元，得到各单元的目的地ID*/           
	{		
		PLAT_UINT8 * addr =send+m_pei->index.unitAddrOffset[j]; 
		memcpy(uintBuf, addr, sizeof(T_UNIT_HEAD)+m_pei->unitsize[j]);//include msg header

		dstID = CUtility::getLittlePackDID(addr);
		//dstID = m_pei->GetDestID(srcID, send, j, m_pei->id[j]);	
		if(app_display() ==1)
		{
			printf("发送数据包单元%d的单元ID：%x 目的地ID: %x\n", j,m_pei->id[j],dstID);
		}
		sprintf(dst,"%08x",dstID);
		
		procConnectControl(uintBuf);
		procBroadMsg(uintBuf);
		procInputAppStatus(uintBuf);

		if(app_display() ==1)
		{
			printf("发送数据包单元%d的数据为：\n",j);
			OutputUint(uintBuf, sizeof(T_UNIT_HEAD)+m_pei->unitsize[j]);
		}

		if((dstID&0x00000FF0)==0x00000FF0)				//说明目的地为CC，此时应该将对应的ATP和ATO中同时压入内容
		{
			dstID =(dstID&0x1000000F)|0x60000000;       //将目的地修改为ATP
			sprintf(dst,"%08x",dstID);
			app_rpush(dst);

			dstID =(dstID&0x1000000F)|0x40000000;        //将目的地修改为ATO
			sprintf(dst,"%08x",dstID);
			app_rpush(dst);
		}
		else
			app_rpush(dst);								/*将数据包中各数据单元压入相应目的地ID的缓冲区中*/                    	
	}//end of for

	if(app_display() ==1)
		printf("SEND DATA END------------------------------SEND DATA END \n");

	return 0;
}

void CAppInterface::procConnectState(PLAT_UBYTE* p)
{
	return;
	CLittlePack parser(srcID, p);
	if (parser.isConnectState())
	{
		PLAT_UINT8 type = parser.getConnectState();
		switch(type)
		{
		case 0x20:
			printf("Connect is not established!\n");
			break;
		case 0x21:
			printf("Connect is establishing ...\n");
			break;
		case 0x22:
			printf("Connect is removing ...\n");
			break;
		case 0x23:
			printf("Connect is runing normally\n");
			break;
		case 0x24:
			printf("Connect is timeout!\n");
			break;
		case 0x25:
			printf("Connect is failed!\n");
			break;
		default:
			break;
		}
		//update connect state to platform buffer
	}

}

void CAppInterface::unInitPackage(PLAT_UBYTE* _ppack)
{
	T_UNIT* ppack = (T_UNIT*) _ppack;
	if (ppack != NULL)
	{
		if (ppack->unitData != NULL)
		{
			delete []ppack->unitData;
			ppack->unitData = NULL;
		}
	}
}

//init unit package
void CAppInterface::initPackage(PLAT_UBYTE*  _ppack, int type)
{
	T_UNIT* ppack = (T_UNIT*) _ppack;
	ppack->unitData = new PLAT_BYTE[ppack->unitSize];
	memset(ppack->unitData, 0, ppack->unitSize * sizeof(PLAT_UBYTE));

	switch (type)
	{
	case 0: //create connect
		{
			ppack->unitId = 0;
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //0000110
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,0x23); //0x23
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
		
			ppack->unitSize = 1;
			
			*(ppack->unitData) = 0x23;
		}	
		break;
	case 1: //remove connect
		{
			ppack->unitId = 0;
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //0000110
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,0x20); //0x20
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
			
			ppack->unitSize = 1;

			*(ppack->unitData) = 0x20;
		}
		break;
	default:
		break;
	}
}

#define PLATUNITID(sid, did)  (sid & did << sizeof(PLAT_UINT32))

void CAppInterface::write2PlatBuffer(PLAT_UBYTE*  ppack)
{
	//ensure platBuf has index structure.
	CUtility::pushBackPack(platBuf, ppack);

	//m_vecPlatUnitID.push_back(PLATUNITID(CUtility::getLittlePackSID(ppack), CUtility::getLittlePackDID(ppack)));
}

void CAppInterface::updatePlatBuffer(PLAT_UBYTE*  ppack)
{
	PLAT_UINT64 id = PLATUNITID(CUtility::getLittlePackSID(ppack), CUtility::getLittlePackDID(ppack));

	PLAT_UINT32 count = CUtility::getUnitCounts(platBuf);
	for (int i = 0; i < count; i++)
	{
		PLAT_UBYTE * unit = CUtility::getUnitHead(platBuf, i);
		PLAT_UINT64 idsrc = PLATUNITID(CUtility::getLittlePackSID(unit), CUtility::getLittlePackDID(unit));
		if (idsrc == id)
		{
			//update the unit data
			CUtility::updateLittlePack((PLAT_UBYTE*)ppack, unit);
		}
	}

}

void CAppInterface::procInputAppStatus(PLAT_UBYTE* p)
{
	CLittlePack parser(srcID, p);
	if (parser.isInputAppStatus())
	{
		//recode input app status into platform buffer
		CUtility::pushBackPack(platBuf, p);
	}
}

void CAppInterface::procConnectControl(PLAT_UBYTE* p)
{
	CLittlePack parser(srcID, p);
	if (parser.isConnectControl())
	{
		PLAT_UINT32 dstID = parser.getDstID();

		PLAT_UINT8 type = parser.getConnectControl();
		switch(type)
		{
		case 0x10:
			{
				printf("Connect Add\n");
				//create connect command

				//update states to platform buffer
				T_UNIT  pack;
				initPackage((PLAT_UBYTE*)&pack, 0);
				write2PlatBuffer((PLAT_UBYTE*)&pack);
				unInitPackage((PLAT_UBYTE*)&pack);
				
				//update terminals state
				m_pzc->updateNotifyTerminal(dstID, true);
			}
			break;
		case 0x11:
			{
				printf("Connect Remove\n");

				T_UNIT  pack;
				initPackage((PLAT_UBYTE*)&pack, 1);
				updatePlatBuffer((PLAT_UBYTE*)&pack);
				unInitPackage((PLAT_UBYTE*)&pack);

				m_pzc->updateNotifyTerminal(dstID, false);
			}
			break;
		default:
			break;
		}
		//update connect control
	}
}

void CAppInterface::procBroadMsg(PLAT_UBYTE* p)
{
	CLittlePack parser(srcID, p);
	
	if (parser.isBroad())
	{
		std::list<PLAT_UINT32> dstIDlist = m_pzc->getNotifyTerminals();
		std::list<PLAT_UINT32>::iterator it;
		for( it = dstIDlist.begin(); it != dstIDlist.end(); ++it )
		{
			//push to db
			memset(&uintBuf, 0x00, SIZE);

			PLAT_UINT32 len = CUtility::getLittlePackSize(p);
			memcpy(&uintBuf, p, len );

			char tmp[IDSIZE];
			sprintf(dst,"%08x",*it);
			app_rpush(tmp);	
		}
	}
}

PLAT_UINT32 getPlatformID( PLAT_UINT32 srcid)
{
	return (srcid&0x1FFFFFFF)|0xC0000000;     
}
//******************************************************************************
// Function: CAppInterface::AppRead
// Brief:   平台提供的数据包读取函数 
// Returns: PLAT_INT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_INT32 CAppInterface::AppRead()
{	
	int len;
//	int platlen;
	int j;

	if (srcID == 0)
		return 0;
	else
	{
		sprintf(src, "%08x", srcID);
	}

	char srccc[11];
	sprintf(srccc,"%s",src);
	while((app_run() >0) && (app_step(srccc) ==0))
	{
		PlatformSleep(10);							/*休眠10ms*/      
	}
	
	readcount ++;
	if(app_display() == 1)
	{
		printf("平台的读周期为：%u\n", readcount);
		printf("RECV DATA BEGAN------------------------------RECV DATA BEGAN \n");
	}

	len = app_llen(src);							/*得到平台的数据源缓冲区中数据单元的个数    */
	if(app_display() ==1)
		printf("读取端缓冲区中单元个数：src : len = %s : %d\n",src,len);

	m_pei->paddr =0;
	for(j =0;j <len;j++)
	{
		memset(&uintBuf, 0x00, SIZE);
		app_lpop(src);								/*将平台的数据源缓冲区中数据单元从缓冲区中弹出*/ 

		if(app_display() ==1)
		{
		   T_UNIT_HEAD pophead;
		   memcpy(&pophead, uintBuf, sizeof(pophead));
		   printf("%s缓冲区中弹出的单元%d的单元ID：%x\n", src,j,pophead.unitId);
           printf("%s缓冲区中弹出的单元%d的数据为：\n", src,j);
		   OutputUint(uintBuf, sizeof(pophead)+pophead.unitSize);    /*将平台的数据源缓冲区中弹出的数据打印出来    */
		}
  	
		m_pei->Encoder(recv, uintBuf, j);               /*对平台的数据源缓冲区中弹出的数据单元进行组包*/      
	}

	if(!len ==0)									/*若数据源缓冲区不为空,将数据包写入指定的数据首地址处*/
	{
		//update index area data
		m_pei->index.regionUnitNum = len;
		memcpy(recv, &m_pei->index, sizeof(m_pei->index));
	}

	platID = getPlatformID(srcID);         /*得到数据源对应的平台ID						*/        
	sprintf(plat,"%08x",platID);

	//get platform data for read
	//read little package from platform buffer and push little package to big package
	PLAT_UINT32 unitCounts = CUtility::getUnitCounts(platBuf);
	if(app_display() ==1)
 		printf("读取的平台中单元个数：plat:platlen =%s:%d\n", plat, unitCounts);
	for (int idx = 0; idx < unitCounts; idx++)
	{
		PLAT_UBYTE * p = CUtility::getUnitHead(platBuf, idx);
		CUtility::pushBackPack(recv, p);

		if(app_display() ==1)
		{
			PLAT_UINT32 popid;
			memcpy(&popid, p, 4);
			printf("平台%s缓冲区中弹出的单元%d的单元ID：%x\n", plat,idx, popid);
			printf("平台%s缓冲区中弹出的单元%d的数据为：\n", plat, idx);     
			OutputUint(p, SIZE );
		}
	}
	
// 
//     platID = getPlatformID();         /*得到数据源对应的平台ID						*/        
//     sprintf(plat,"%08x",platID);
// 
// 	platlen =app_llen(plat);						/*得到平台缓冲区中数据单元的个数				*/
// 	if(app_display() ==1)
// 		printf("读取的平台中单元个数：plat:platlen =%s:%d\n", plat, platlen);
// 
// 	for(j =len; j <len+platlen; j++)
// 	{
// 		app_lpop(plat);								/*将平台缓冲区中的数据单元弹出					*/
//        
// 		if(app_display() ==1)
// 		{
// 			PLAT_UINT32 popid;
// 			memcpy(&popid, uintBuf, 4);
// 			printf("平台%s缓冲区中弹出的单元%d的单元ID：%x\n", plat, j-len, popid);
// 			printf("平台%s缓冲区中弹出的单元%d的数据为：\n", plat, j-len);     
// 			OutputUint(uintBuf, SIZE );
// 		}
// 		
// 		m_pei->HandlePlatData(srcID, uintBuf, platBuf);			/*根据平台缓冲区中数据单元内容得到平台待发送的内容*/
// 		if(app_display() ==1)
// 		{
// 			printf("平台%s返回给%s的数据为：\n", plat,src);
// 			OutputUint(platBuf, SIZE );
// 		}
// 
//         m_pei->Encoder(recv, platBuf, j);				/*将平台待发送的内容添加到待发送数据包中		  */		 
// 	}
// 	

	if(!len ==0)									/*若数据源缓冲区不为空,将数据包写入指定的数据首地址处*/
	{
		//update index area data
// 		m_pei->index.regionUnitNum = len+platlen;
// 		memcpy(recv, &m_pei->index, sizeof(m_pei->index));

		if(app_display() ==1)
		{
			if(app_displaybig() ==1)
			{
				printf("%s读取到大包数据为：\n",src);
				OutputUint(recv, NETSIZE);
			}
		    printf("%s读取到的大包中单元个数： %d\n", src, m_pei->index.regionUnitNum);
		}

		//only save region unit data for debug
		if(app_display() ==1)
		{
			for(j =0; j <m_pei->index.regionUnitNum; j++)
			{
				memcpy(&m_pei->unithead, recv+m_pei->index.unitAddrOffset[j], sizeof(m_pei->unithead));
				memcpy(&uintBuf, recv+m_pei->index.unitAddrOffset[j], sizeof(m_pei->unithead)+m_pei->unithead.unitSize);
		       		
				printf("%s读取到的单元%d的单元ID：%x 单元大小：%u\n", 
					src, j, m_pei->unithead.unitId,m_pei->unithead.unitSize);
				printf("%s读取到得单元%d的数据为：\n", src, j);
				OutputUint(uintBuf, sizeof(m_pei->unithead)+m_pei->unithead.unitSize);
			}
		}
	}
	
	if(app_display() ==1)
		printf("RECV DATA END------------------------------RECV DATA END \n");

	return 0;
}

//******************************************************************************
// Function: CAppInterface::AppWrite
// Brief:   为测试写功能添加的写函数 
// Returns: PLAT_INT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_INT32 CAppInterface::AppWrite(char *dst)
{
	memset(uintBuf,'a',100);
	app_rpush(dst);                                           
	return 0;
}

//******************************************************************************
// Function: CAppInterface::AppRead
// Brief:   为测试读功能添加的读函数 
// Returns: PLAT_INT32
// Author:  刘志盼
// Data:	2010/10/09
//******************************************************************************
PLAT_INT32 CAppInterface::AppRead(char *src)
{
	int i;
	app_lpop(src);
	for(i =0;i <SIZE;i++)
	{
		printf(" %u",uintBuf[i]);
	}
	printf("\n",uintBuf[i]);
	return 0;
}

