#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "zsocket.h"
#include "wintimer.h"
#include "platform.h"
#include "cutility.h"
#include "zredis.h"




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

PLAT_UBYTE APP_READ_ADDR[NETSIZE];
PLAT_UBYTE APP_WRITE_ADDR[NETSIZE];
const char * g_fromTerminalLog = "fromTerminalLog.txt";

//const char * s_host = "192.168.20.100";
static const char* s_dbip = "127.0.0.1";

CAppInterface::CAppInterface()
{
	m_pzc = new CZc();
	//m_pei = new packetprocess();
    m_pRedis = new ZRedis();
    m_pSockClient = new  ZSocketClient();
    m_pSockClient->setLog("agent_client.log");

	m_fpFromTerminalLog = NULL;
	int ret = fopen_s(&m_fpFromTerminalLog,g_fromTerminalLog, "w");
    fclose(m_fpFromTerminalLog);

	InitSrcId();
}

CAppInterface::~CAppInterface()
{
    if (m_pSockClient)
    {
        delete m_pSockClient;
        m_pSockClient = NULL;
    }

     if ( m_pRedis )
     {
         delete m_pRedis;
         m_pRedis = NULL;
     }

    if(m_pzc)
	{
		delete m_pzc;
		m_pzc = NULL;
	}

	if(m_fpFromTerminalLog)
    {
		fclose(m_fpFromTerminalLog);
    }

}

PLAT_INT32 CAppInterface::AppClose()
{
	return 0;
}

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

    int err = fopen_s(&m_fpFromTerminalLog, g_fromTerminalLog, "a+");

    m_pRedis->connect(s_dbip);

    if( m_pSockClient->connectServer("127.0.0.1") < 0)
    {
        printf("Failed: Socket client connect agent server\n");
    }


    //app_init(uintBuf,s_host);

	return 0;
}

PLAT_INT32 CAppInterface::AppInit(PLAT_UINT8* s,PLAT_UINT8* r,PLAT_UINT32 sid,char *host)
{
	//m_pRedis->app_init(s_host);
	//
	//send = s;
	//recv = r;
	//
	//srcID = sid;
	//memset(src, '\0', IDSIZE);
	//sprintf(src, "%08x", sid);
	//memset(send, '\0', NETSIZE);
	//memset(recv, '\0', NETSIZE);

	//CUtility::initBigPackIdx(send);
	//CUtility::initBigPackIdx(recv);
	return 0;
}

PLAT_INT32 CAppInterface::AppWrite()
{
	if(CUtility::needSwap())
	{
		//big endian to little
        CUtility::bigPackToLE(send);
	}

    //debug client to platform data
    fprintf(m_fpFromTerminalLog, "Write data: \n");
    CUtility::outputPackage(APP_WRITE_ADDR,m_fpFromTerminalLog);

	if (srcID == 0)
	{
		PLAT_UBYTE *p = CUtility::getUnitHead(send, 0);
		srcID = CUtility::getLittlePackSID(p);
	}   
	//sprintf(src, "%08x", srcID);	
    sprintf_s(src, "%08x", srcID);	


	int count = CUtility::getUnitCounts(send);
	/*ѭ���������ݰ��и����ݵ�Ԫ���õ�����Ԫ��Ŀ�ĵ�ID*/ 
	for(int j = 0;j < count;j++)		          
	{		
		PLAT_UINT8 * addr = CUtility::getUnitHead(send, j); // send+m_pei->index.unitAddrOffset[j]; 
		//memcpy(uintBuf, addr, sizeof(T_UNIT_HEAD)+m_pei->unitsize[j]);//include msg header
		memcpy(uintBuf, addr, CUtility::getLittlePackSize(addr) );//include msg header

		dstID = CUtility::getLittlePackDID(addr);
		sprintf_s(dst,"%08x",dstID);
		
        //maybe update data in platform buffer  where the data is little endian
        CLittlePack parser(srcID, uintBuf);
	    if (parser.isMsgOut())
        {
            // msg status changed from out to in, 
            // for value its vaule from 5 to 10
		    procMsgOut(uintBuf);//need transfer it

            if (CUtility::needSwap())
		    {
			    //little to big endian for output to db
                CUtility::littlePackToBE(uintBuf);
		    }
            m_pRedis->app_rpush(dstID, uintBuf);   
            m_pSockClient->transferTerminal((const char*) uintBuf);
        }
        else if (parser.isConnectControl())
        {
            //not need transfer
            // connect / disconnet did, and recode result into platform buffer
		    procConnectControl(uintBuf); 
        }
        else if (parser.isBroad()) // not supported for now temporary.
        {
            //need transfer, DID process need specially process.
            // from message, DID can not be gotten, it always ffff for broadcast.
            // so need parameter DID that means msg format is selftransfer+did+msg content.
		    procBroadMsg(uintBuf);
        }
        else if (parser.isInputAppStatus())
        {
            //just only recode it into platform buffer
            procInputAppStatus(uintBuf);//not need transfer
        }

        //other type message
		if (CUtility::needSwap())
		{
			//little to big endian for output to db
            CUtility::littlePackToBE(uintBuf);
		}

		if((dstID&0x00000FF0)==0x00000FF0)				//˵��Ŀ�ĵ�ΪCC����ʱӦ�ý���Ӧ��ATP��ATO��ͬʱѹ������
		{
			dstID =(dstID&0x1000000F)|0x60000000;       //��Ŀ�ĵ��޸�ΪATP
			//sprintf(dst,"%08x",dstID);
            m_pRedis->app_rpush(dstID, uintBuf);
			//app_rpush(dst);

			dstID =(dstID&0x1000000F)|0x40000000;        //��Ŀ�ĵ��޸�ΪATO
			//sprintf(dst,"%08x",dstID);
            m_pRedis->app_rpush(dstID, uintBuf); 
            m_pSockClient->transferTerminal((const char*) uintBuf);
		}
        else
        {
            /*�����ݰ��и����ݵ�Ԫѹ����ӦĿ�ĵ�ID�Ļ�������*/   
            m_pRedis->app_rpush(dstID, uintBuf); 
            m_pSockClient->transferTerminal((const char*) uintBuf);
        }
	}//end of for
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
	//T_UNIT* ppack = (T_UNIT*) _ppack;
	//if (ppack != NULL)
	//{
	//	if (ppack->unitData != NULL)
	//	{
	//		delete []ppack->unitData;
	//		ppack->unitData = NULL;
	//	}
	//}
}

//init unit package
void CAppInterface::initPackage(PLAT_UBYTE*  _ppack, PLAT_UINT len, int type, int value)
{
	T_UNIT* ppack = (T_UNIT*) _ppack;
	ppack->unitSize = len;
	ppack->unitData = ( PLAT_BYTE* )(_ppack + sizeof(PLAT_BYTE) * sizeof(T_UNIT_HEAD));

	switch (type)
	{
	case 0: //create connect
		{
			ppack->unitId = 0;
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //0000110
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,value); //0x23
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
			
			ppack->unitId = ppack->unitId;
			*(ppack->unitData) = value;
		}	
		break;
	case 1: //remove connect
		{
			ppack->unitId = 0;
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //0000110
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,value); //0x20
			ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff

			ppack->unitId = ppack->unitId;
			*(ppack->unitData) = value;
		}
		break;
	default:
		break;
	}
}

#define PLATUNITID(sid, did)  (sid & did << sizeof(PLAT_UINT32))

void CAppInterface::write2PlatBuffer(PLAT_UBYTE*  ppack)
{
	//ensure platBuf has index structure. Ensure the data pointer is after size pointer!
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
	//if (parser.isInputAppStatus())
	{
		//recode input app status into platform buffer
		CUtility::pushBackPack(platBuf, p);
	}
}


void CAppInterface::procMsgOut(PLAT_UBYTE* p)
{
	CLittlePack parser(srcID, p);
	//if (parser.isMsgOut())
	{
		PLAT_UINT32 val = CUtility::setBitsVal(parser.getUnitID(), 24,29, 5);//0000110
		parser.updateUID(val);
		//update connect control
	}
}


void CAppInterface::procConnectControl(PLAT_UBYTE* p)
{
	CLittlePack parser(srcID, p);
//  if (parser.isConnectControl())
	{
		PLAT_UINT32 dstID = parser.getDstID();

		PLAT_UINT8 type = parser.getConnectControl();
		switch(type)
		{
		case 0x10:
			{
				printf("Connect Add\n");
				//create connect command
                int ret = 0;//m_pSockClient->connectTermianl(dstID);


				//update states to platform buffer
				//content shoule be big-endian
				PLAT_UBYTE pack[1024];
				memset(pack, 0, 1024);
				PLAT_UINT len = 1;
				if ( len >= 1024) 
				{	
					printf("Warning: little package size > 1024, it is now %d.", len);
					printf("Warning: Create Add failed!");
					return;
				}
				initPackage(pack,len, 0,ret);
                //update connect control
                //updatePackage(pack, ret);
				write2PlatBuffer(pack);
				//unInitPackage((PLAT_UBYTE*)&pack);
				
				//update terminals state
				m_pzc->updateNotifyTerminal(dstID, true);
			}
			break;
		case 0x11:
			{
				printf("Connect Remove\n");
                int ret = 0 ;//m_pSockClient->disconnectTermianl(dstID);

				//update states to platform buffer
				PLAT_UBYTE pack[1024];
				PLAT_UINT len = 1;
				if ( len >= 1024) 
				{
					printf("Warning: little package size > 1024, it is now %d.\n", len);
					printf("Warning: Create Remove failed!\n");
					return;
				}
				initPackage(pack,len, 1,ret);
                //update connect control
                //updatePackage(pack, ret);
				write2PlatBuffer(pack);
				//unInitPackage((PLAT_UBYTE*)&pack);

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
	
	//if (parser.isBroad())
	{
		std::list<PLAT_UINT32> dstIDlist = m_pzc->getNotifyTerminals();
		std::list<PLAT_UINT32>::iterator it;
		for( it = dstIDlist.begin(); it != dstIDlist.end(); ++it )
		{
			//push to db
			memset(&uintBuf, 0x00, SIZE);

			PLAT_UINT32 len = CUtility::getLittlePackSize(p);
            PLAT_UINT32 did = CUtility::getLittlePackDID(p); //should equal to ffff
            if (CUtility::needSwap())
		    {
			    //little to big endian for output to db
                CUtility::littlePackToBE(p);
		    }

			memcpy(&uintBuf, p, len );

			char tmp[IDSIZE];
			sprintf_s(dst,"%08x",*it);
			m_pRedis->app_rpush(tmp, uintBuf);	

            //transfer it, broadcast message DID can not gotten from little packe 
            // which DID is ffff
            m_pSockClient->transferTerminal(*it, (const char*) p); 
		}
	}
}

PLAT_UINT32 getPlatformID( PLAT_UINT32 srcid)
{
	return (srcid&0x1FFFFFFF)|0xC0000000;     
}


void CAppInterface::InitSrcId()
{
	srcID = 0x21000002;
}

PLAT_INT32 CAppInterface::GetSrcId()
{
	return srcID;
}

PLAT_INT32 CAppInterface::AppRead()
{	
	int len;
	int j;

    CUtility::initBigPackIdx(recv);

	if (srcID == 0)
		return 0;
	else
	{
		sprintf_s(src, "%08x", srcID);
	}

	char srccc[11];
	sprintf_s(srccc,"%s",src);
	while((m_pRedis->app_run() >0) && (m_pRedis->app_step(srccc) ==0))
	{
		PlatformSleep(10);							/*����10ms*/      
	}
	
	len = m_pRedis->app_llen(src);
	for(j =0;j <len;j++)
	{
		memset(&uintBuf, 0x00, SIZE);
		/*��ƽ̨������Դ�����������ݵ�Ԫ�ӻ������е���*/
		// m_pRedis->app_lpop(src, uintBuf);	
          m_pRedis->app_lpop(srcID, uintBuf);

		if(CUtility::needSwap())
		{
			//big endian to little endian for little package
            CUtility::littlePackToLE(uintBuf);
		}

		CUtility::pushBackPack(recv, uintBuf);
  		/*��ƽ̨������Դ�������е��������ݵ�Ԫ�������*/
		//m_pei->Encoder(recv, uintBuf, j);                     
	}

	platID = getPlatformID(srcID);         /*�õ�����Դ��Ӧ��ƽ̨ID						*/        
	sprintf_s(plat,"%08x",platID);

	//------------------------------
	//get platform data for read
	//read little package from platform buffer and push little package to big package
	PLAT_UINT32 unitCounts = CUtility::getUnitCounts(platBuf);
	for (unsigned int idx = 0; idx < unitCounts; idx++)
	{
		PLAT_UBYTE * p = CUtility::getUnitHead(platBuf, idx);
		CUtility::pushBackPack(recv, p);
	}

    //debug
	fprintf(m_fpFromTerminalLog, "Read data: \n");
	CUtility::outputPackage(APP_READ_ADDR,m_fpFromTerminalLog);
	//CUtility::outputPackage(platBuf,m_fpFromTerminalLog);

	if(CUtility::needSwap())
	{
        //to big endian
        CUtility::bigPackToBE(recv);
	}

	return 0;
}

PLAT_INT32 CAppInterface::AppWrite(char *dst)
{
	memset(uintBuf,'a',100);
	m_pRedis->app_rpush(dst, uintBuf);                                           
	return 0;
}

PLAT_INT32 CAppInterface::AppRead(char *src)
{
	int i;
	m_pRedis->app_lpop(src, uintBuf);
	for(i =0;i <SIZE;i++)
	{
		printf(" %u",uintBuf[i]);
	}
	printf("\n",uintBuf[i]);
	return 0;
}

