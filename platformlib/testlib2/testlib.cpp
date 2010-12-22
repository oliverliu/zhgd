// testlib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "platformhead.h"
#include "cutility.h"
#include "platforminterface.h"

class CPackUtility
{
public:
	CPackUtility(PLAT_UBYTE* );
	~CPackUtility();
	void pushPack(int type);

	void pushCC2ZC_RoutineMsg();
	void pushZC2CC_RoutinMsg();
	void pushZC2CC_TrackMsg();
	void pushZC2CC_SpeedRstMsg(); //speed restrict
	void pushCC2ZC_2ATSTrasMsg();
	void pushZC2CC_fromATSTrasMsg();
	void pushZC2CreateConnect(bool bcreate); 

    void pushZC2ATS_routineMsg(); 
    void pushZC2ATS_CC2ATS();

    void pushAtp2ZC_linkdataOutput();
	
	void finished();
private:
	void pushPackZc2Atp();
	void pushPackZc2Ato();


	void updateIdxSt();
private:
	PLAT_UBYTE * m_phead;
	//PLAT_UBYTE * m_pcurrent;
	//T_DATA_INDEX    m_index;
	//int  m_seqnum;
};

void CPackUtility::updateIdxSt()
{
	//m_seqnum++;
	//if ( m_seqnum > 255)
	//{
	//	printf("!!!!!!Waring package sum large than 256, now is %d", m_seqnum + 1); 
	//	return;
	//}

	//m_index.unitAddrOffset[m_seqnum] = m_pcurrent - m_phead;
	//m_index.regionUnitNum =  m_seqnum + 1;
}

void CPackUtility::pushCC2ZC_RoutineMsg()
{
	//updateIdxSt();

	//
	//T_UNIT_HEAD     unithead;
	//T_MESSAGE_HEAD  msghead;

	//unithead.unitId = 0x01;
	//unithead.unitSize = 0;

	//msghead.sequenceNum = 2;
	//msghead.timeStamp = 2;
	//msghead.SID = g_sid;
	//msghead.DID = g_did;
	//msghead.msgType = g_msgType;
 //   msghead.msgLen = 0;
	//
	//typedef struct _msgInter
	//{
	//	PLAT_UINT32 info1[14];
	//	PLAT_UINT8  info2[9];
	//	PLAT_UINT32 info3;
	//	PLAT_UINT8  info4[2];
	//	PLAT_UINT32 info5[2];
	//} stMsgInter;

	//stMsgInter inter;
	//inter.info1[0] = msghead.DID;
	//inter.info1[1] = msghead.SID;
	//inter.info1[2] = msghead.msgType;
	//inter.info1[3] = sizeof(stMsgInter ) - 4 * sizeof(PLAT_UINT32);

	////info1
	//for( int i = 4; i < 14; i++ )
	//	inter.info1[i] = 0x01;
	//
	////change special value
	//inter.info1[5] = 0x02;
	//inter.info1[6] = 0x10;

	////info2
	//for(int i = 0; i < 9; i++ )
	//	inter.info2[i] = 0xa0;
	//
	//inter.info2[0] = 0xAA;
	//inter.info2[4] = 0x8;

	////info3
	//inter.info3 = 0x11;

	////info4
	//for(int i = 0; i < 2; i++ )
	//	inter.info4[i] = 0xb0;

	////info5
	//for( int i = 0; i < 2; i++ )
	//	inter.info5[i] = 0xc0;
	//
	//msghead.msgLen = sizeof(stMsgInter);
	//unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	//memcpy(m_pcurrent, &unithead, sizeof(T_UNIT_HEAD));
	//m_pcurrent += sizeof(T_UNIT_HEAD);
	//
	//memcpy(m_pcurrent, &msghead, sizeof(T_MESSAGE_HEAD));
	//m_pcurrent += sizeof(T_MESSAGE_HEAD);

	//memcpy(m_pcurrent, &inter, sizeof(stMsgInter));
	//m_pcurrent += sizeof(stMsgInter);

	//printf( "msghead len = %d, unithead size = %d\n", msghead.msgLen, unithead.unitSize);
}

//#define toBig32(val) CUtility::ByteSwap32(val)
//#define toBig64(val) CUtility::ByteSwap64(val)
//#define toBig16(val) CUtility::ByteSwap16(val)
#define toBig32(val) (val)
#define toBig64(val) (val)
#define toBig16(val) (val)

void CPackUtility::pushZC2CreateConnect(bool bcreate)
{
    const PLAT_UINT32 did = 0x20000001;
    const PLAT_UINT32 sid = 0x60000001;

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
    cmd.ctrlReq = 0x10;
    cmd.sid =  sid;
    cmd.did = did;

	//updateIdxSt();
	PLAT_UBYTE pArray[1024*8];
	memset(pArray, 0, 1024*8); 
	PLAT_UBYTE *ptmpH = pArray;

	T_UNIT_HEAD     unithead;

	//01 001011, 0xaa, 0x00,0x00
	unithead.unitId = toBig32(0x4baa1234);
	unithead.unitSize = toBig32(sizeof(tLinkCtrl));

    memcpy(ptmpH, &unithead, sizeof(T_UNIT_HEAD));
	ptmpH += sizeof(T_UNIT_HEAD);

    memcpy(ptmpH, &cmd, sizeof(tLinkCtrl));
	ptmpH += sizeof(tLinkCtrl);

	CUtility::pushBackPack(m_phead,pArray); 
}

void CPackUtility::pushAtp2ZC_linkdataOutput()
{
    PLAT_UBYTE pArray[1024*8];
	memset(pArray, 0, 1024*8);
	PLAT_UBYTE *ptmpH = pArray;

	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  msghead;

    const PLAT_UINT32 sid = 0x60000001;
    const PLAT_UINT32 did = 0x20000001;

	//01 001010, 0xaa, 0x00,0x00
	unithead.unitId = toBig32(0x4aaa0000);//zc uid
	unithead.unitSize = toBig32(0);

	msghead.sequenceNum = toBig32(2);
	msghead.timeStamp = toBig64((long long) 2) ;//toBig64(2);
	msghead.SID = toBig32(sid);
	msghead.DID = toBig32(did);
	msghead.msgType = toBig32(0x4302); //0xff000001
    msghead.msgLen = toBig32(0);
	
    typedef struct _data 
    {
		PLAT_UINT32  seqNo;
		PLAT_UINT8   trainNum;
        PLAT_UINT32  trainID;
        PLAT_UINT8   maldir;
		PLAT_UINT32  malpossecId;
		PLAT_UINT32  malposOffset;
    } dataInter;
	typedef struct _msgInter
	{
		PLAT_UINT32  did;
        PLAT_UINT32  sid;
        PLAT_UINT32  msgtype;
        PLAT_UINT32  msglen;

		dataInter data;
	} stMsgInter;

	stMsgInter inter;
	inter.did = did;
	inter.sid = sid;
	inter.msgtype =  0x4302;
    inter.msglen = sizeof(dataInter);

	inter.data.seqNo =  toBig32(1);
	inter.data.trainNum =  toBig16(3);
	inter.data.trainID =  toBig16(0x1234de);
    inter.data.maldir = 0xaa;
    inter.data.malpossecId = 0x21;
    inter.data.malposOffset = 0x12;
	
	msghead.msgLen =  sizeof(stMsgInter);
	unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	printf( "msghead len = %d, unithead size = %d\n",
		msghead.msgLen, unithead.unitSize);

	msghead.msgLen = toBig32(msghead.msgLen);
	unithead.unitSize  = toBig32(unithead.unitSize );

	printf( "msghead len = %0x, unithead size = %0x, in memory, hex unit\n",
		msghead.msgLen, unithead.unitSize );


	memcpy(ptmpH, &unithead, sizeof(T_UNIT_HEAD));
	ptmpH += sizeof(T_UNIT_HEAD);
	
	memcpy(ptmpH, &msghead, sizeof(T_MESSAGE_HEAD));
	ptmpH += sizeof(T_MESSAGE_HEAD);

	memcpy(ptmpH, &inter, sizeof(stMsgInter));
	ptmpH += sizeof(stMsgInter);

	CUtility::pushBackPack(m_phead,pArray); 
}

void CPackUtility::pushZC2ATS_routineMsg()
{
    PLAT_UBYTE pArray[1024*8];
	memset(pArray, 0, 1024*8);
	PLAT_UBYTE *ptmpH = pArray;

	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  msghead;

    const PLAT_UINT32 did = 0x00000003;
    const PLAT_UINT32 sid = 0x00000002;

	//01 001010, 0xaa, 0x00,0x00
	unithead.unitId = toBig32(0x4aaa0000);//zc uid
	unithead.unitSize = toBig32(0);

	msghead.sequenceNum = toBig32(2);
	msghead.timeStamp = toBig64((long long) 2) ;//toBig64(2);
	msghead.SID = toBig32(sid);
	msghead.DID = toBig32(did);
	msghead.msgType = toBig32(0x4302); //0xff000001
    msghead.msgLen = toBig32(0);
	
    typedef struct _data 
    {
		PLAT_UINT32  seqNo;
		PLAT_UINT8   trainNum;
        PLAT_UINT32  trainID;
        PLAT_UINT8   maldir;
		PLAT_UINT32  malpossecId;
		PLAT_UINT32  malposOffset;
    } dataInter;
	typedef struct _msgInter
	{
		PLAT_UINT32  did;
        PLAT_UINT32  sid;
        PLAT_UINT32  msgtype;
        PLAT_UINT32  msglen;

		dataInter data;
	} stMsgInter;

	stMsgInter inter;
	inter.did = did;
	inter.sid = sid;
	inter.msgtype =  0x4302;
    inter.msglen = sizeof(dataInter);

	inter.data.seqNo =  toBig32(1);
	inter.data.trainNum =  toBig16(3);
	inter.data.trainID =  toBig16(0x1234de);
    inter.data.maldir = 0xaa;
    inter.data.malpossecId = 0x21;
    inter.data.malposOffset = 0x12;
	
	msghead.msgLen =  sizeof(stMsgInter);
	unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	printf( "msghead len = %d, unithead size = %d\n",
		msghead.msgLen, unithead.unitSize);

	msghead.msgLen = toBig32(msghead.msgLen);
	unithead.unitSize  = toBig32(unithead.unitSize );

	printf( "msghead len = %0x, unithead size = %0x, in memory, hex unit\n",
		msghead.msgLen, unithead.unitSize );


	memcpy(ptmpH, &unithead, sizeof(T_UNIT_HEAD));
	ptmpH += sizeof(T_UNIT_HEAD);
	
	memcpy(ptmpH, &msghead, sizeof(T_MESSAGE_HEAD));
	ptmpH += sizeof(T_MESSAGE_HEAD);

	memcpy(ptmpH, &inter, sizeof(stMsgInter));
	ptmpH += sizeof(stMsgInter);

	CUtility::pushBackPack(m_phead,pArray); 
}

void CPackUtility::pushZC2ATS_CC2ATS()
{
    PLAT_UBYTE pArray[1024*8];
	memset(pArray, 0, 1024*8);
	PLAT_UBYTE *ptmpH = pArray;

    const PLAT_UINT32 did = 0x00000003;
    const PLAT_UINT32 sid = 0x00000002;

	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  msghead;

	//01 001010, 0xaa, 0x00,0x00
	unithead.unitId = toBig32(0x4aaa0000);//zc uid
	unithead.unitSize = toBig32(0);

	msghead.sequenceNum = toBig32(2);
	msghead.timeStamp = toBig64((long long) 2) ;//toBig64(2);
	msghead.SID = toBig32(sid);
	msghead.DID = toBig32(did);
	msghead.msgType = toBig32(0x4302); //0xff000001
    msghead.msgLen = toBig32(0);
	
    typedef struct _data 
    {
		char  msg[128];
    } dataInter;
	typedef struct _msgInter
	{
		PLAT_UINT32  did;
        PLAT_UINT32  sid;
        PLAT_UINT32  msgtype;
        PLAT_UINT32  msglen;

		dataInter data;
	} stMsgInter;

	stMsgInter inter;
	inter.did = did;
	inter.sid = sid;
	inter.msgtype =  0x4302;

    inter.msglen = sizeof(dataInter);

    char *p = "test_abcd";
    memcpy(inter.data.msg,p, strlen(p));
    inter.msglen = strlen(inter.data.msg);
	
	msghead.msgLen =  sizeof(stMsgInter);
	unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	printf( "msghead len = %d, unithead size = %d\n",
		msghead.msgLen, unithead.unitSize);

	msghead.msgLen = toBig32(msghead.msgLen);
	unithead.unitSize  = toBig32(unithead.unitSize );

	printf( "msghead len = %0x, unithead size = %0x, in memory\n",
		msghead.msgLen, unithead.unitSize );


	memcpy(ptmpH, &unithead, sizeof(T_UNIT_HEAD));
	ptmpH += sizeof(T_UNIT_HEAD);
	
	memcpy(ptmpH, &msghead, sizeof(T_MESSAGE_HEAD));
	ptmpH += sizeof(T_MESSAGE_HEAD);

	memcpy(ptmpH, &inter, sizeof(stMsgInter));
	ptmpH += sizeof(stMsgInter);

	CUtility::pushBackPack(m_phead,pArray); 
}

//to ato
void CPackUtility::pushZC2CC_RoutinMsg()
{
	 PLAT_UBYTE pArray[1024*8];
	memset(pArray, 0, 1024*8);
	PLAT_UBYTE *ptmpH = pArray;

    const PLAT_UINT32 did = 0xf0000001;//to ato
    const PLAT_UINT32 sid = 0x20000001;

	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  msghead;

	//01 001010, 0xaa, 0x00,0x00
	unithead.unitId = toBig32(0x4aaa0000);//zc uid
	unithead.unitSize = toBig32(0);

	msghead.sequenceNum = toBig32(2);
	msghead.timeStamp = toBig64((long long) 2) ;//toBig64(2);
	msghead.SID = toBig32(sid);
	msghead.DID = toBig32(did);
	msghead.msgType = toBig32(0x4101); //0xff000001
    msghead.msgLen = toBig32(0);
	
	typedef struct _msgInter
	{
		PLAT_UINT32  seqno;
        PLAT_UINT32  trainid;
        PLAT_UINT8   ebcmd;
        PLAT_UINT8   stopassreq;
        PLAT_UINT32  stopsigid;

        PLAT_UINT8  maldir;

        PLAT_UINT32   malsecid;
        PLAT_UINT32   maloffset;
        PLAT_UINT8   malendtype;
	} stMsgInter;

	stMsgInter inter;
	inter.seqno = 1;
	inter.trainid = 0x12;
    inter.ebcmd = 0xaa;
	inter.stopassreq = 0x55;
    inter.stopsigid = 1;
	inter.maldir = 0xaa;
    inter.malsecid = 2;
	inter.maloffset = 1000;
	inter.malendtype =  3;
	
	msghead.msgLen =  sizeof(stMsgInter);
	unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	printf( "msghead len = %d, unithead size = %d\n",
		msghead.msgLen, unithead.unitSize);

	msghead.msgLen = toBig32(msghead.msgLen);
	unithead.unitSize  = toBig32(unithead.unitSize );

	printf( "msghead len = %0x, unithead size = %0x, in memory\n",
		msghead.msgLen, unithead.unitSize );


	memcpy(ptmpH, &unithead, sizeof(T_UNIT_HEAD));
	ptmpH += sizeof(T_UNIT_HEAD);
	
	memcpy(ptmpH, &msghead, sizeof(T_MESSAGE_HEAD)); 
	ptmpH += sizeof(T_MESSAGE_HEAD);

	memcpy(ptmpH, &inter, sizeof(stMsgInter));
	ptmpH += sizeof(stMsgInter);

	CUtility::pushBackPack(m_phead,pArray); 
}

void CPackUtility::pushZC2CC_TrackMsg()
{
	//updateIdxSt();

}

void CPackUtility::pushZC2CC_SpeedRstMsg() //speed restrict
{
	//updateIdxSt();

}

void CPackUtility::pushCC2ZC_2ATSTrasMsg()
{
	//updateIdxSt();
}

void CPackUtility::pushZC2CC_fromATSTrasMsg()
{
	//updateIdxSt();
}

CPackUtility::CPackUtility(PLAT_UBYTE* p)
{
	m_phead = p;


	CUtility::initBigPackIdx(p);
}

CPackUtility::~CPackUtility()
{
  // finished();
}

void CPackUtility::finished()
{
	//memcpy(m_phead, &m_index, sizeof(T_DATA_INDEX));	
}

enum packType
{
	zc2Cir = 0,
	zc2Broder,
	zc2Atp,
	zc2Ato,
	zc2Ci
};

void CPackUtility::pushPack(int type)
{
	//updateIdxSt();

	//switch(type)
	//{
	//case zc2Cir:
	//	break;
	//case zc2Broder:
	//	break;
	//case zc2Atp:
	//	break;
	//case zc2Ato:
	//	pushPackZc2Ato();
	//	break;
	//case zc2Ci:
	//	break;
	//default:
	//	break;
	//}
}

void CPackUtility::pushPackZc2Atp()
{
}

void CPackUtility::pushPackZc2Ato()
{
	
}

void createPacakge()
{
}

//zc won't create link initiatively
void createTestZc()
{
    unsigned char* dstBuf = APP_WRITE_ADDR;

	CPackUtility	packUtil(dstBuf); 
    printf("Pre: ZC2CC_RoutinMsg\n");
    packUtil.pushZC2CC_RoutinMsg();
    //packUtil.pushZC2ATS_CC2ATS();
    //packUtil.pushZC2ATS_routineMsg();
   
    if(CUtility::needSwap())
        CUtility::bigPackToBE(dstBuf);
}

void createTestAtp()
{
    unsigned char* dstBuf = APP_WRITE_ADDR;

	CPackUtility	packUtil(dstBuf);
    printf("Pre: create connect\n");
    static bool binit = false;
    if ( binit )
    {
         printf("ATP Send data to ZC\n");
         packUtil.pushAtp2ZC_linkdataOutput();
    }

    if (!binit)
    {
        printf("ATP Connect ZC\n");
        packUtil.pushZC2CreateConnect(true);//create link
        binit = true;
    }
    //packUtil.pushZC2CreateConnect(false);//delete link
     //通信报文输出
    

    if(CUtility::needSwap())
        CUtility::bigPackToBE(dstBuf);
}

//ato won't create link, it using atp's link
void createTestAto()
{
    unsigned char* dstBuf = APP_WRITE_ADDR;

	CPackUtility	packUtil(dstBuf);
    printf("Pre: ato 2 zc msg is not support for now\n");

     //通信报文输出
    packUtil.pushAtp2ZC_linkdataOutput();

    if(CUtility::needSwap())
        CUtility::bigPackToBE(dstBuf);
}


PLAT_UBYTE APP_READ_ADDR_SWAP[NETSIZE];
PLAT_UBYTE APP_WRITE_ADDR_SWAP[NETSIZE];

#include <string>
#include <stdio.h>
using namespace std;

int _tmain(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: testlib type\n"
            "Support type: zc, ato, atp for now.\n"
              );
        return -1;
    }
    
    string type = "zc";

    FILE *fp = NULL;
	fp = fopen("test.txt", "a+");
	
	//app.AppInit();
	InitPlatformInterface();
    
    if (argc > 1)
    {
        type = argv[1];
        printf("Now type is %s\n",type.c_str());
    }

    int i = 0;
    char str[64] = "n";
    do{
        fprintf(fp, "\n\nTest NO.%d\n", i++);
	    ReadFormPlatform(); 
    
        memset(APP_READ_ADDR_SWAP,0, NETSIZE);
        memcpy(APP_READ_ADDR_SWAP, APP_READ_ADDR, NETSIZE);
        if(CUtility::needSwap())
            CUtility::bigPackToLE(APP_READ_ADDR_SWAP);
        fprintf(fp, "READ: \n");
	    CUtility::outputPackage(APP_READ_ADDR_SWAP,fp); 
	
	    if ( type == "zc")
            createTestZc();
        else if ( type == "ato")
            createTestAto();
        else if ( type == "atp")
            createTestAtp();

	    //create a package
	    //createPacakge();

        memset(APP_WRITE_ADDR_SWAP,0, NETSIZE);
        memcpy(APP_WRITE_ADDR_SWAP, APP_WRITE_ADDR, NETSIZE);
        if(CUtility::needSwap())
            CUtility::bigPackToLE(APP_WRITE_ADDR_SWAP);
	    fprintf(fp, "Write: \n");
	    CUtility::outputPackage(APP_WRITE_ADDR_SWAP, fp);

        WriteToPlatform();

        fflush(fp);

        puts ("If want to continue do again, please input 'y'. Otherwise quit\n");
        gets(str);
    
    }while(std::string(str) == std::string("y") );

    fclose(fp);

    printf("Quit testlib process\n");
    getchar();
	return 0;
}

