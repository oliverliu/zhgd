#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "zsocket.h"
#include "wintimer.h"
#include "platform.h"
#include "cutility.h"
#include "zredis.h"
#include "wintimer.h"
#include "INIReader.h"

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

#define  APP_READ_ADDR    PLAT2APP_DATA
#define  APP_WRITE_ADDR   APP2PLAT_DATA

PLAT_UBYTE APP_READ_ADDR[NETSIZE];
PLAT_UBYTE APP_WRITE_ADDR[NETSIZE];

//const char * s_host = "192.168.20.100";
static const char* s_dbip = "127.0.0.1";

CAppInterface::CAppInterface()
{
    m_bUseP1 = false;
    m_pzc = new CZc();
    //m_pei = new packetprocess();
    m_pRedis = new ZRedis();
    m_pSockClient = new  ZSocketClient();
    m_pSockClient->setLog("agent_client.log");

    setLog("fromTerminalLog_liboutput.txt");
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
}

PLAT_INT32 CAppInterface::AppClose()
{
    return 0;
}

PLAT_INT32 CAppInterface::AppInit()
{
    PLAT_INT32 ret = 0;
    send = APP_WRITE_ADDR;
    recv = APP_READ_ADDR;

    memset(send, 0, SIZE);
    memset(recv, 0, SIZE);
    memset(src, 0, IDSIZE);

    CUtility::initBigPackIdx(send);
    CUtility::initBigPackIdx(recv);
    CUtility::initBigPackIdx(platBuf);
    CUtility::initBigPackIdx(m_dbBuf);

    std::string strDbvalue = "";
    std::string strAgentServer = "";
    INIReader reader("sid.config");
    if (reader.ParseError() < 0) {
        plog( "Can't load 'sid.config'\n");
        return -1;
    }
    strAgentServer = reader.Get("self","agentip","127.0.0.1");
    strDbvalue = reader.Get("self","dbip","127.0.0.1");
     
    m_srcID = reader.GetInteger("self", "SID", 0);

    //default use p1
    m_bUseP1 = reader.GetInteger("self","usep1", 1) == 1 ? true : false;
    plog("Using p1 protocol: %s\n", m_bUseP1 ? "true" : "false");
        
    //connect db
    if (m_pRedis->connect(strDbvalue.c_str()) != true )
    {
        plog("Failed: terminal connect db! Should stop run down!\n");
        ret = -1;
    }

    plog("Address: database = %s, agentip = %s, self SID = %0x\n",
        strDbvalue.c_str(), strAgentServer.c_str(), m_srcID);

    //connect server agent
    if( m_bUseP1 && m_pSockClient->connectServer(strAgentServer.c_str()) < 0)
    {
        plog("Failed: Socket client connect agent server\n");
        ret = -1;
    }
    //app_init(uintBuf,s_host);

    return ret;
}

PLAT_INT32 CAppInterface::AppInit(PLAT_UINT8* s,PLAT_UINT8* r,PLAT_UINT32 sid,char *host)
 {
    send = s;
    recv = r;

    memset(send, 0, SIZE);
    memset(recv, 0, SIZE);
    memset(src, 0, IDSIZE);

    CUtility::initBigPackIdx(send);
    CUtility::initBigPackIdx(recv);
    CUtility::initBigPackIdx(platBuf);
    CUtility::initBigPackIdx(m_dbBuf);

    std::string strDbvalue = "";
    std::string strAgentServer = "";
    INIReader reader("sid.config");
    if (reader.ParseError() < 0) {
        plog( "Can't load 'sid.config'\n");
        return -1;
    }
    strAgentServer = reader.Get("self","agentip","127.0.0.1");
    strDbvalue = reader.Get("self","dbip","127.0.0.1");

    m_srcID = reader.GetInteger("self", "SID", 0);
        
    m_pRedis->connect(strDbvalue.c_str());

     //default use p1
    m_bUseP1 = reader.GetInteger("self","usep1", 1) == 1 ? true : false;

    plog("Address: database = %s, agentip = %s, self SID = %0x\n",
        strDbvalue.c_str(), strAgentServer.c_str(), m_srcID);
    if(m_bUseP1 && m_pSockClient->connectServer(strAgentServer.c_str()) < 0)
    {
        plog("Failed: Socket client connect agent server\n");
    }

    return 0;
}
//The input buf MUST be little endian and is big package pointer.
void CAppInterface::outputPackage(const PLAT_UBYTE * bigpack )
{
    const PLAT_UBYTE * buf = bigpack;
    PLAT_UINT32 count = CUtility::getUnitCounts(buf);
    plog( "package has %d = %0x(16)unit, bigpack addr = %0x\n", count,count, bigpack);
    for (PLAT_UINT32 i = 0; i < count; i++)
    {
        plog("NO.%2d package: ", i);
        PLAT_UBYTE * unit = CUtility::getUnitHead(buf, i);
        outputLittlepack((const unsigned char*) unit);
    }
}

void CAppInterface::outputLittlepack(const unsigned char * buffer)
{
   //print out little package info
   PLAT_UINT32 size = CUtility::getLittlePackSize(buffer);
   
   plog("Now package: unitId = %08x, unitSize = %d, data:\n", 
           CUtility::getLittlePackUID(buffer), size);
   
   PLAT_UINT32 j = 0;
   for(;j < size; j++)
   {
           plog("%02x  ", buffer[j]);
   }
   plog("print out %d \n", j);
}

PLAT_INT32 CAppInterface::AppWrite()
{
   if(CUtility::needSwap())
    {
        //big endian to little
        CUtility::bigPackToLE(send);
    }

    //debug client to platform data
    plog("Write data: write Addr: %0x\n",send );
    outputPackage(send);


    if (m_srcID == 0)
    {
        plog("Warning: the client SID is 0, now get it from little package\n");
        PLAT_UBYTE *p = CUtility::getUnitHead(send, 0);
        m_srcID = CUtility::getLittlePackSID(p);
    }   
    //sprintf(src, "%08x", m_srcID);  
    sprintf(src, "%08x", m_srcID);  


    int count = CUtility::getUnitCounts(send);
     
    int boardId = 1;
    for(int j = 0;j < count;j++)                  
    {       
        PLAT_UINT8 * addr = CUtility::getUnitHead(send, j); // send+m_pei->index.unitAddrOffset[j]; 
        //memcpy(uintBuf, addr, sizeof(T_UNIT_HEAD)+m_pei->unitsize[j]);//include msg header
        memcpy(uintBuf, addr, CUtility::getLittlePackSize(addr) );//include msg header

        dstID = CUtility::getLittlePackDID(addr);
        sprintf(dst,"%08x",dstID);
        plog("dstID = %s in No.%d package\n", dst, j);
        
        int len = CUtility::getLittlePackSize(uintBuf);
        //maybe update data in platform buffer  where the data is little endian
        CLittlePack parser(m_srcID, uintBuf);
        if (parser.isBroad()) // not supported for now temporary.
        {
            //need transfer, DID process need specially process.
            // from message, DID can not be gotten, it always ffff for broadcast.
            // so need parameter DID that means msg format is selftransfer+did+msg content.
            procBroadMsg(uintBuf);
            continue;
        }
       
         PLAT_UINT32 datatype =  CUtility::getLittlePackDataType(uintBuf);
         switch (datatype)
         {
            case 0x2://0010; DB data
            {
              //fprintf(m_fpFromTerminalLog, "Message DB data\n");
              plog( "Message DB data\n");
            }
            break;
            case 0x7://0111; atp 2 ato
            {
             // fprintf(m_fpFromTerminalLog, "Message atp to ato\n");
              plog("Message atp to ato\n");
              PLAT_UINT did = CUtility::getLittlePackDID(uintBuf);
              m_pRedis->app_rpush(did, uintBuf);
             
              continue;
            }
            break;
            case 0x8://1000; ato 2 atp
            {
              plog("Message ato to atp\n");
              PLAT_UINT did = CUtility::getLittlePackDID(uintBuf);
              m_pRedis->app_rpush(did, uintBuf);
             
              continue;
            }
            break;
            case 0x9://1001; output board data
            {
                static int count = 0;
                static PLAT_UINT8 pctrlBuf[SIZE] = "\0";
                static PLAT_UINT8* pbk = pctrlBuf;
                
                plog("Message output board data, this is No.%d\n",count+1);
                memcpy(pbk, uintBuf, sizeof(uintBuf));
                pbk += sizeof(uintBuf);

                if (count < 3)
                {
                    count++;
                }
                else //count == 3
                {
                    char ctrl[IDSIZE+5] = "\0";
                    sprintf(ctrl, "%satoioctl", dst);
                    m_pRedis->app_set(ctrl, pctrlBuf);

                    pbk = pctrlBuf ;
                    memset(pctrlBuf, 0, SIZE);
                    count = 0;
                }

                //plog("Message output board data, this is No.%d\n",count+1);
                //char ctrl[IDSIZE+5] = "\0";
                //sprintf(ctrl, "%satoioctl", dst);
                //m_pRedis->app_set(ctrl, uintBuf);
               
                //PLAT_UINT32 datainfo = getLittlePackDataInfo(unitBuf);
                //if (datainfo <= 0x1f)
                //{
                //    plog("This is VIO board output data\n");
                //    char ctrl[IDSIZE+5] = "\0";
                //    sprintf(ctrl, "%sioctlvio", dst);

                //    PLAT_UINT8 ctrlbuf[SIZE] = "\0";                 
                //    memcpy(ctrlbuf, uintBuf, sizeof(uintBuf));
                //    m_pRedis->app_set(ctrl, ctrlbuf);
                //}
                //else if ( datainfo <= 0x3f)
                //{
                //    plog("This is NVIO board output data\n");
                //    char ctrl[IDSIZE+5] = "\0";
                //    sprintf(ctrl, "%sioctlnvio", dst);

                //    PLAT_UINT8 ctrlbuf[SIZE] = "\0";                 
                //    memcpy(ctrlbuf, uintBuf, sizeof(uintBuf));
                //    m_pRedis->app_set(ctrl, ctrlbuf);
                //}
                //else
                //{
                //   plog("This is Error, datainfo = %0x, its max value is 
                //   0x3f\n",  datainfo);
                //}
                continue;
            }
            break;
            case 0x0a: //1010; msge output data
            {
                plog("Msg output data, we change it to msg in\n");
                 // msg status changed from out to in, 
                // for value its vaule from 5 to 10
                procMsgOut(uintBuf);//need transfer it

                plog("Want to transfer data ");
                outputLittlepack((const unsigned char*)uintBuf);
                PLAT_UINT did = CUtility::getLittlePackDID(uintBuf);

                if (CUtility::needSwap())
                {
                    //little to big endian for output to db
                    CUtility::littlePackToBE(uintBuf);
                }
                
                if (did >= 10000 && did < 100000) 
                {
                    plog("This message send to CBI, just push it\n");
                }
                else
                {
                   if ( m_bUseP1 ) m_pSockClient->transferTerminal((const char*) uintBuf,len);
                }
                m_pRedis->app_rpush(dstID, uintBuf);   
                continue;
            }
            break;
            case 0xB: //1011; connection control data
            {
               plog("Msg link control data\n");
                 //not need transfer
                // connect / disconnet did, and recode result into platform buffer
                procConnectControl(uintBuf); 
                continue;
            }
            case 0xC://1100; Event record data
            {
              plog("Message Event record data\n");
              //fprintf(m_fpFromTerminalLog, "Message Event record data\n");
            }
            break;
            case 0xD: //1101; broad cast data
            {
                plog("Message broadcast data\n");
                plog("For now ignore it, do nothing for this type msg\n");
                //procBroadMsg(uintBuf);
                continue;
            }
            case 0xE://1110; DB control data
            {
              //fprintf(m_fpFromTerminalLog, "Message DB control data\n");
              plog( "Message DB control data\n");
            }
            break;
            case 0xF: //1111; internal program state sync input data
            {
                plog("Msg internal program state sync input data\n");
                 //just only recode it into platform buffer
                procInputAppStatus(uintBuf);//not need transfer
                continue;
            }
            break;
            case 0x10 ://10000; analog output data
            {
                plog("Message analog output data\n");
                 char ctrl[IDSIZE+5];
                 memset(ctrl, 0, IDSIZE+5);
                 //If destination is train simulation RS, then it is ATO, pull 
                // contrl command, using app_set function    
                sprintf(ctrl, "%sauctl", dst);
                m_pRedis->app_set(ctrl, uintBuf);
                continue;
            }
            break;
            case 0x11://10001; TMS communication data
            {
                plog("Message TMS communication data\n");
            }
            break;
            default: 
            break;
         }

         //other type message
        if (CUtility::needSwap())
        {
            //little to big endian for output to db
            CUtility::littlePackToBE(uintBuf);
        }
        
        
        //If destination terminal is CC, Now push little package into ATP and 
        // ATO respectively
        if ( (dstID &0xF0000000) == 0xf0000000)
        //if((dstID&0x00000FF0)==0x00000FF0)
        {
            plog("This is destination is CC\n");
            //ATP
            m_pRedis->app_rpush( (dstID & 0x1fffffff) | 0x60000000, uintBuf);//atp
            m_pRedis->app_rpush( (dstID & 0x1fffffff) | 0x40000000,uintBuf);//ato
                   
            //dstID =(dstID&0x1000000F)|0x60000000;       
            //m_pRedis->app_rpush(dstID, uintBuf);

            //ATO
            //dstID =(dstID&0x1000000F)|0x40000000; 
            //m_pRedis->app_rpush(dstID, uintBuf); 
        }
        else
        {
            plog("Default proc for other type msg, just push it and transfer it\n");
            plog("push to db key = %08x\n", dstID);
            //push little package into correspond destination ID buffer in DB
            m_pRedis->app_rpush(dstID, uintBuf); 
        }
        if ( m_bUseP1 ) m_pSockClient->transferTerminal((const char*) uintBuf,len);    
    }//end of for
 
    //restore write buffer data
    if(CUtility::needSwap())
    {
        // to big endian
        CUtility::bigPackToBE(send);
    }
    
    return 0;
}

void CAppInterface::procConnectState(PLAT_UBYTE* p)
{
    return;
    CLittlePack parser(m_srcID, p);
    if (parser.isConnectState())
    {
        PLAT_UINT8 type = parser.getConnectState();
        switch(type)
        {
            case 0x20:
            plog("Connect is not established!\n");
            break;
            case 0x21:
            plog("Connect is establishing ...\n");
            break;
            case 0x22:
            plog("Connect is removing ...\n");
            break;
            case 0x23:
            plog("Connect is runing normally\n");
            break;
            case 0x24:
            plog("Connect is timeout!\n");
            break;
            case 0x25:
            plog("Connect is failed!\n");
            break;
            default:
            break;
        }
    //update connect state to platform buffer
    }

}

//void CAppInterface::unInitPackage(PLAT_UBYTE* _ppack)
// {
    //T_UNIT* ppack = (T_UNIT*) _ppack;
    //if (ppack != NULL)
    //{
        //if (ppack->unitData != NULL)
        //{
        //    delete []ppack->unitData;
        //   ppack->unitData = NULL;
        //}
    //}
//}


//init connect state unit package
//void CAppInterface::initPackage(PLAT_UBYTE*  _ppack, PLAT_UINT did, int type, int connectvalue)
//{
//    typedef struct connectStateData
//    {
//        PLAT_UBYTE result;
//        PLAT_UBYTE resver1[3];
//        PLAT_UINT sid;
//        PLAT_UINT did;
//        PLAT_UINT  resver2[2];
//    }tConStateData;
//
//    T_UNIT* ppack = (T_UNIT*) _ppack;
//    tConStateData _connectStateData;
//    memset(&_connectStateData, 0, sizeof(tConStateData));
//
//    ppack->unitSize = sizeof(tConStateData);
//    ppack->unitData = ( PLAT_BYTE* )(_ppack + sizeof(PLAT_BYTE) * sizeof(T_UNIT_HEAD));
//
//    char cv = 0xaa;
//    if (connectvalue == 1) //connect successful
//        cv = 0x55;
//
//    _connectStateData.result = cv;
//    _connectStateData.sid = m_srcID;
//    _connectStateData.did = did;
//
//    memcpy(ppack->unitData, (const char*)&_connectStateData, sizeof(tConStateData));
//
//    switch (type)
//    {
//    case 0: //create connect
//        {
//            ppack->unitId = 0;
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //000110
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //0xaa or 0x55
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
//            ppack->unitId = ppack->unitId;
//        }
//    break;
//    case 1: //remove connect
//    {
//        ppack->unitId = 0;
//        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
//        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //000110
//        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //0xaa or 0x55
//        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
//
//        ppack->unitId = ppack->unitId;
//    }
//    break;
//    default:
//    break;
//    }
//}

//init unit package
//void CAppInterface::initPackage(PLAT_UBYTE*  _ppack, PLAT_UINT len, int type, int value)
//{
//        T_UNIT* ppack = (T_UNIT*) _ppack;
//        ppack->unitSize = len;
//        ppack->unitData = ( PLAT_BYTE* )(_ppack + sizeof(PLAT_BYTE) * sizeof(T_UNIT_HEAD));
//
//        switch (type)
//        {
//        case 0: //create connect
//            {
//                char cv = 0x25;
//                if (value == 1) //connect successful
//                    cv = 0x23;
//
//                ppack->unitId = 0;
//                ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
//                ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //0000110
//                ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //0x23
//                ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
//
//                ppack->unitId = ppack->unitId;
//                *(ppack->unitData) = cv;
//            }
//            break;
//        case 1: //remove connect
//        {
//            char cv = 0x22;
//            if (value == 1) //connect successful
//                cv = 0x20;
//            ppack->unitId = 0;
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //0000110
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //0x20
//            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
//
//            ppack->unitId = ppack->unitId;
//            *(ppack->unitData) = cv;
//        }
//        break;
//             default:
//        break;
//        }
//}

//void CAppInterface::write2PlatBuffer(PLAT_UBYTE*  ppack)
//{
//    //ensure platBuf has index structure. Ensure the data pointer is after size pointer!
//    CUtility::pushBackPack(platBuf, ppack);
//
//    //m_vecPlatUnitID.push_back(PLATUNITID(CUtility::getLittlePackSID(ppack), CUtility::getLittlePackDID(ppack)));
//}
//
//void CAppInterface::write2DbBuffer(PLAT_UBYTE*  ppack)
//{
//    //ensure platBuf has index structure. Ensure the data pointer is after size pointer!
//    CUtility::pushBackPack(m_dbBuf, ppack);
//
//    //m_vecPlatUnitID.push_back(PLATUNITID(CUtility::getLittlePackSID(ppack), CUtility::getLittlePackDID(ppack)));
//}
//
//void CAppInterface::updatePlatBuffer(PLAT_UBYTE*  ppack)
//{
//        PLAT_UINT64 id = PLATUNITID(CUtility::getLittlePackSID(ppack),
//                               CUtility::getLittlePackDID(ppack));
//
//        PLAT_UINT32 count = CUtility::getUnitCounts(platBuf);
//        for (int i = 0; i < count; i++)
//        {
//            PLAT_UBYTE * unit = CUtility::getUnitHead(platBuf, i);
//            PLAT_UINT64 idsrc = PLATUNITID(CUtility::getLittlePackSID(unit), 
//                                 CUtility::getLittlePackDID(unit));
//            if (idsrc == id)
//            {
//                //update the unit data
//                CUtility::updateLittlePack((PLAT_UBYTE*)ppack, unit);
//            }
//        }
//
//}

//void CAppInterface::updateDbBuffer(PLAT_UBYTE*  ppack)
//{
//    PLAT_UINT32 sid = CUtility::getLinkStateSID(ppack);
//
//    if (sid != m_srcID)
//    {
//        plog("Warning: the sid = %0x in link state result is NOT equal m_srcID = %0x\n",
//            sid, m_srcID);
//    }
//
//    PLAT_UINT64 id = PLATUNITID(sid,
//            CUtility::getLinkStateDID(ppack));
//
//    PLAT_UINT32 count = CUtility::getUnitCounts(m_dbBuf);
//    for (int i = 0; i < count; i++)
//    {
//        PLAT_UBYTE * unit = CUtility::getUnitHead(m_dbBuf, i);
//
//        //get link control did
//        PLAT_UINT64 idsrc = PLATUNITID(CUtility::getLinkStateSID(unit),
//            CUtility::getLinkStateDID(unit));
//        if (idsrc == id)
//        {
//            //update the unit data
//            CUtility::updateLittlePack((PLAT_UBYTE*)ppack, unit);
//        }
//    }
//
//}

void CAppInterface::procInputAppStatus(PLAT_UBYTE* p)
{
    CLittlePack parser(m_srcID, p);
    //if (parser.isInputAppStatus())
    {
        //recode input app status into platform buffer
        CUtility::pushBackPack(platBuf, p);
    }
}

void CAppInterface::procMsgOut(PLAT_UBYTE* p)
{
    plog("Before: Uid input is %0x\n", CUtility::getLittlePackUID(p));
    //outputLittlepack(p);
    
    PLAT_UINT32 newUid = CUtility::setBitsVal(CUtility::getLittlePackUID(p), 
        24,29, 5);//0000110
    memcpy(p, &newUid, sizeof(PLAT_UINT32));

    plog("After: Uid output is %0x\n", CUtility::getLittlePackUID(p));
    //outputLittlepack(p);
}

//for doc v6, 2010.12
void CAppInterface::procConnectControl(PLAT_UBYTE* p)
{
    typedef struct _linkCtrl
    {
        PLAT_UBYTE ctrlReq;
        PLAT_UBYTE resv1[3];
        PLAT_UINT sid;
        PLAT_UINT did;
        PLAT_UINT msgype;
        PLAT_UINT msglen;
    }tLinkCtrl;

    tLinkCtrl cmd;
    memset(&cmd, 0, sizeof(tLinkCtrl));

    PLAT_UBYTE* pdata = p + sizeof(T_UNIT_HEAD);
    memcpy((void*) &cmd, pdata, sizeof(tLinkCtrl));
    
    PLAT_UBYTE pack[1024];
    memset(pack, 0, 1024);

    PLAT_UINT32 dstID = cmd.did;
    PLAT_UINT8 type = cmd.ctrlReq;
    switch(type)
    {
        case 0x10:
        {
            plog("Connect Add\n");
            //create connect command
            int ret = 1;
            if ( m_bUseP1 ) 
                ret = m_pSockClient->connectTermianl(dstID) == 1 ? 1 : 0;
                    
           // initPackage(pack,dstID, 0,ret);
           // CUtility::pushBackPack(m_dbBuf, pack);
        }
        break;
        case 0x11:
        {
            plog("Connect Remove\n");
            int ret = 1 ;
            if ( m_bUseP1 ) ret = m_pSockClient->disconnectTermianl(dstID) == 1 ? 1 : 0;;

           // initPackage(pack,dstID, 1,ret);
           // updateDbBuffer(pack);
            //CUtility::pushBackPack(m_dbBuf, pack);
            //m_pzc->updateNotifyTerminal(dstID, false);
        }
        break;
        default:
        break;
    }

    //update connect control
    //control state save in db using app_set
    //content shoule be little-endian
    //CUtility::pushBackPack(m_dbBuf, pack);
   // m_pRedis->app_set("linkstate", m_dbBuf);
    //CUtility::pushBackPack(recv, uintBuf);

    //update terminals state
    m_pzc->updateNotifyTerminal(dstID, true);
}

//this is for tengguodong, doc v5, 2010.11
//void CAppInterface::procConnectControl(PLAT_UBYTE* p)
//{
//    PLAT_UBYTE pack[1024];
//    memset(pack, 0, 1024);
//    CLittlePack parser(m_srcID, p);
//    
//    PLAT_UINT32 dstID = parser.getDstID();
//
//    PLAT_UINT8 type = parser.getConnectControl();
//    switch(type)
//    {
//        case 0x10:
//        {
//            plog("Connect Add\n");
//            //create connect command
//            int ret = 1;
//            if ( m_bUseP1 ) 
//                ret = m_pSockClient->connectTermianl(dstID) == -1 ? 0 : 1;
//                    
//            PLAT_UINT len = 1;
//            //if ( len >= 1024) 
//            //{
//            //    plog("Warning: little package size > 1024, it is now %d.", len);
//            //    plog("Warning: Create Add failed!");
//            //    return;
//            //}
//            initPackage(pack,len, 0,ret);
//            //updatePackage(pack, ret);
//            write2DbBuffer(pack);
//            //write2PlatBuffer(pack);
//            //unInitPackage((PLAT_UBYTE*)&pack);
//        }
//        break;
//        case 0x11:
//        {
//            plog("Connect Remove\n");
//            int ret = 1 ;
//            if ( m_bUseP1 ) ret = m_pSockClient->disconnectTermianl(dstID);
//
//            //update states to platform buffer
//            PLAT_UINT len = 1;
//            //if ( len >= 1024) 
//            //{
//            //    plog("Warning: little package size > 1024, it is now %d.\n", len);
//            //    plog("Warning: Create Remove failed!\n");
//            //    return;
//            //}
//            initPackage(pack,len, 1,ret);
//            //update connect control
//            //updatePackage(pack, ret);
//            //write2PlatBuffer(pack);
//            write2DbBuffer(pack);
//            //unInitPackage((PLAT_UBYTE*)&pack);
//
//            //m_pzc->updateNotifyTerminal(dstID, false);
//        }
//        break;
//        default:
//        break;
//    }
//
//    //update connect control
//    //control state save in db using app_set
//    //content shoule be little-endian
//    //CUtility::pushBackPack(m_dbBuf, pack);
//    m_pRedis->app_set("linkstate", m_dbBuf);
//    //CUtility::pushBackPack(recv, uintBuf);
//
//    //update terminals state
//    m_pzc->updateNotifyTerminal(dstID, true);
//}

void CAppInterface::procBroadMsg(PLAT_UBYTE* p)
{
    CLittlePack parser(m_srcID, p);
       
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
        sprintf(dst,"%08x",*it);
        m_pRedis->app_rpush(tmp, uintBuf);
        
        //transfer it, broadcast message DID can not gotten from little packe 
        // which DID is ffff
        if ( m_bUseP1 ) m_pSockClient->transferTerminal(*it, (const char*) p); 
            
     }
}

PLAT_UINT32 CAppInterface::getPlatformID( PLAT_UINT32 srcid)
{
        return (srcid&0x1FFFFFFF)|0xC0000000;     
}


//void CAppInterface::InitSrcId()
//{
//     srcID = 0x21000002;
//}
//
//PLAT_INT32 CAppInterface::GetSrcId()
//{
//      return srcID;
//}

PLAT_INT32 CAppInterface::AppRead()
{
    CUtility::initBigPackIdx(recv);
    plog("Now srcID = %08x start Read\n", m_srcID);

    if (m_srcID == 0)
        return 0;
    
    sprintf(src, "%08x", m_srcID);
    
    char srccc[11];
    sprintf(srccc,"%s",src);
    while((m_pRedis->app_run() >0) && (m_pRedis->app_step(srccc) ==0))
    {
        /*休眠10ms*/      
        PlatformSleep(10);                         
    }
    
    int len = m_pRedis->app_llen(src);
    for(int j =0;j <len;j++)
    {
        memset(&uintBuf, 0x00, SIZE);
         m_pRedis->app_lpop(m_srcID, uintBuf);

        if(CUtility::needSwap())
        {
            //big endian to little endian for little package
            CUtility::littlePackToLE(uintBuf);
        }

        CUtility::pushBackPack(recv, uintBuf);
    }
    plog("Now db %s count = %d\n", src, len);

    //------------------------------
    //get platform data for read
    //read little package from platform buffer and push little package to big package
    PLAT_UINT32 unitCounts = CUtility::getUnitCounts(platBuf);
    for (unsigned int idx = 0; idx < unitCounts; idx++)
    {
        PLAT_UBYTE * p = CUtility::getUnitHead(platBuf, idx);
        CUtility::pushBackPack(recv, p);
    }

    //------------------------------
    //get db data that is through app_set  in db, now get it.
    CUtility::initBigPackIdx(m_dbBuf);
    m_pRedis->app_get("linkstate", m_dbBuf);//Maybe m_dbBuf can not through db channel.
    unitCounts = CUtility::getUnitCounts(m_dbBuf);
    for (unsigned int idx = 0; idx < unitCounts; idx++)
    {
        PLAT_UBYTE * p = CUtility::getUnitHead(m_dbBuf, idx);
        CUtility::pushBackPack(recv, p);
    }

    //Specially process for special terminal
   // //11-11添加，若是ATP读时，调用计算函数[要根据驾驶模式来区分算法]
   if((m_srcID & 0xE0000000)==0x60000000)           
   {
        plog ("Now is m_srcID & 0xE000000 == 0x60000000, This is atp\n");
        PLAT_UINT8  RstoATPbuf[SIZE]  = "\0";
        PLAT_UINT8  tempBuf[SIZE] = "\0";
        char ctrl[IDSIZE+5] = "\0";

        //SIMRS set the key's value
        sprintf(ctrl, "%08x", ((m_srcID&0x1FFFFFFF)|0x80000000));
        sprintf(ctrl, "%stoatp", ctrl);  
        //得到RS发送给ATP的包含三块IO板输出数据和速度脉冲包数据信息
        m_pRedis->app_get (ctrl, RstoATPbuf);

        //得到第一块IO板输出数据，包含unithead头
        memcpy(tempBuf, RstoATPbuf, 10);                        
        CUtility::pushBackPack(recv, tempBuf);

        //得到第二块IO板输出数据，包含unithead头
        memcpy(tempBuf, RstoATPbuf+10, 10);                    
        CUtility::pushBackPack(recv, tempBuf);

        //得到第三块IO板输出数据，包含unithead头
        memcpy(tempBuf, RstoATPbuf+20, 10);                    
        CUtility::pushBackPack(recv, tempBuf);

        //得到RS的速度脉冲包输出数据，包含unithead头，190为包的长度
        memcpy(tempBuf, RstoATPbuf+30, 202);          
        CUtility::pushBackPack(recv, tempBuf);                                  
   }
    //11-11添加，若是ATO读时，调用计算函数
   else if((m_srcID&0xE0000000)==0x40000000)                      
   {
        plog ("Now is m_srcID & 0xE000000 == 0x40000000, this is ato\n");
        PLAT_UINT8  RstoATObuf[SIZE] = "\0";
        char ctrl[IDSIZE+5]  = "\0";
        PLAT_UINT8  tempBuf[SIZE] = "\0";

        //SIMRS set the key's value
        sprintf(ctrl, "%08xtoato", ((m_srcID&0x1FFFFFFF)|0x80000000));
        //sprintf(ctrl, "%stoato", ctrl);
        //m_pRedis->app_get (ctrl, RstoATOcharbuf);
        //app_get(ctrl, RstoATOcharbuf);
        //app_2_uint(RstoATOcharbuf, RstoATObuf);     
        //得到RS发送-%0x给ATO的包含速度脉冲包数据信息
        plog("get %s - %0x\n", ctrl, m_srcID);
        m_pRedis->app_get (ctrl, RstoATObuf);

        //得到RS的速度脉冲包输出数据，包含unithead头，42为包的长度
        memcpy(tempBuf, RstoATObuf, 202);                 
        CUtility::pushBackPack(recv, tempBuf);

        memcpy(tempBuf, RstoATObuf+202, 88);
        CUtility::pushBackPack(recv, tempBuf);
   }

    //debug
    plog("Read data Finished: read Addr: %0x\n", recv);
    outputPackage(recv);

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
        plog(" %u",uintBuf[i]);
    }
    plog("\n",uintBuf[i]);
    return 0;
}

void CAppInterface::setLog(const char* file) 
{
    if ( file != NULL)
    {
        m_strfileLog = std::string(file);
        FILE *  fd = fopen (file, "w"); 
        fclose(fd);
    }
}

void CAppInterface::plog(const char* format, ...)
{
    FILE *  fd = stdout;
    bool bopen = false;
    if (m_strfileLog.length() != 0)
    {
      fd = fopen (m_strfileLog.c_str(), "a+"); 
      bopen = true;
    }

    va_list args;
    va_start(args, format);
    vfprintf(fd, format, args);
    
    fflush(fd);

    if( bopen)
        fclose(fd);
    va_end(args);
}
