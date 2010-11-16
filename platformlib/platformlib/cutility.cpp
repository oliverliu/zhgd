#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "platform.h"
// #include "credis.h"
// #include "globle.h"
// #include "wintimer.h"

#include "globle.h"
#include "InterfaceDataStruct.h"
#include "cutility.h"

#include <stdarg.h>
//int msglevel = 3; /* the higher, the more messages... */
// 
// #if defined(NDEBUG) && defined(__GNUC__)
// #define pmesg(level, format, args...) ((void)0)
// #else
// void pmesg(int level, const char* format, ...) {
// #ifdef NDEBUG
// 	/* Empty body, so a good compiler will optimise calls
// 	   to pmesg away */
// #else
//         va_list args;
// 
//         if (level > msglevel)
//                 return;
// 
//         va_start(args, format);
//         vfprintf(stdout, format, args);
// 		//vfprintf(stderr, format, args);
//         va_end(args);
// #endif /* NDEBUG */
// #endif /* NDEBUG && __GNUC__ */
// }
//
// 
// void pFatal(const char* format, ...)   { return pmesg(0, format, ...); }
// void pWarning(const char* format, ...) { return pmesg(1, format, ...); }
// void pDebug(const char* format, ...)   { return pmesg(2, format, ...); }
// void pBig(const char* format, ...)     { return pmesg(5, format, ...); }

//#include <math.h>

#ifdef _PLAT_WIN

#endif

#ifdef _PLAT_LIUNX
#include <arpa/inet.h>
#endif

static bool isBigEndian()
{
   short word = 0x4321;
   if((*(char *)& word) != 0x21 )
     return true;
   else 
     return false;
}

static bool needSwap()
{
	static bool binit = false;
	static bool bneed = false;

	if (!binit)
	{
		bool bmsgBig = false;

	#ifdef _MESSAGE_BIG_ENDIAN
		bmsgBig = true;
	#endif

		bool blocalBig = isBigEndian();

		bneed = blocalBig != bmsgBig ? true :false;
		binit = true;
	}
	return bneed;
}

PLAT_UINT16 ByteSwap16 (PLAT_UINT16 nValue)
{
   return (((nValue>> 8)) | (nValue << 8));
}

PLAT_UINT32 ByteSwap32 (PLAT_UINT32 nLongNumber)
{
   return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
   ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

PLAT_UINT64 ByteSwap64 (PLAT_UINT64 nLongNumber)
{
	return (( (PLAT_UINT64) (ByteSwap32 ((PLAT_UINT32) ((nLongNumber << 32) >>32))) <<32) |
		ByteSwap32 ((PLAT_UINT32)(nLongNumber >> 32)) );
}

PLAT_UINT32 CUtility::BetoLe32(const PLAT_UINT32 val )
{
	return needSwap() ? ByteSwap32(val) : val;
}
PLAT_UINT16 CUtility::BetoLe16(const PLAT_UINT16 val)
{
	return needSwap() ? ByteSwap16(val) : val;
}
PLAT_UINT32 CUtility::LetoBe32(const PLAT_UINT32 val)
{
	return needSwap() ? ByteSwap32(val) : val;
}
PLAT_UINT16 CUtility::LetoBe16(const PLAT_UINT16 val)
{
	return needSwap() ? ByteSwap16(val) : val;
}

PLAT_UINT64 CUtility::BetoLe64(const PLAT_UINT64 val )
{
	return needSwap() ? ByteSwap64(val) : val;
}
PLAT_UINT64 CUtility::LetoBe64(const PLAT_UINT64 val)
{
	return needSwap() ? ByteSwap64(val) : val;
}

bool CUtility::isBitSet(PLAT_UINT32 val, PLAT_UINT32 bit)
{
	if ( bit < 0 || bit >= 32)
	{
		printf(" Warning: isBitSet parameter is wrong. bit = %d now", bit);
		return false;
	}
	return ((val) & (1 << (bit))) == 1 ? true : false;
}

PLAT_UINT32 CUtility::getBitsVal(const PLAT_UINT32 val, PLAT_UINT8 start, PLAT_UINT8 end)
{
	PLAT_UINT8 little = start > end ? end : start;
	PLAT_UINT8 big    = start < end ? end : start;

	//int tmp = (int) pow((float)2, big - little + 1) - 1;
	int tmp = (int) ((2 << (big - little)) - 1);
	return  (( tmp << little ) & val ) >> little;
}

PLAT_UINT32 CUtility::setBitsVal(const PLAT_UINT32 dst, PLAT_UINT8 start, PLAT_UINT8 end, PLAT_UINT32 val)
{	
	PLAT_UINT8 small = start >= end ? end : start;
	return dst | (val << small);
}

void CUtility::outputPackage(const PLAT_UBYTE * buf, FILE* fp)
{
	PLAT_UINT32 count = CUtility::getUnitCounts(buf);
	fprintf(fp, "package has %d unit\n", count);
	for (PLAT_UINT32 i = 0; i < count; i++)
	{
		PLAT_UBYTE * unit = CUtility::getUnitHead(buf, i);
		fprintf(fp, "NO.%2d package: unitId = %d, unitSize = %d, data:\n", 
			i, CUtility::getLittlePackUID(unit), CUtility::getLittlePackDataSize(unit));
		PLAT_UINT32 size = CUtility::getLittlePackSize(unit);
		for(PLAT_UINT32 j = 0;j < size; j++)
		{
			fprintf(fp, "%02x  ", unit[j]);
		}
		fprintf(fp, "\n");
	}
}

void CUtility::pushBackPack(PLAT_UBYTE* bigPackHead, PLAT_UBYTE * ppack)
{
	//Must ensure bigPackHead is the head of big package
	//ppack is a little package that data follows unitSize.
	T_DATA_INDEX idxData ;
	memcpy(&idxData, bigPackHead, sizeof(T_DATA_INDEX));
	
	PLAT_UINT32 count = BetoLe32 (idxData.regionUnitNum);
	if (count > 255 || count < 0)
	{
		printf("Warning: index regionUnitNum of big package ERROR, Now is %d\n", count);
		return;
	}

	PLAT_UBYTE* plast =  NULL;
	PLAT_UINT32 len = 0;

	if (count == 0)
	{
		//This is the first time to push a little package into big package.
		//now count is 0
		plast = bigPackHead + BetoLe32(idxData.unitAddrOffset[0]);
	}
	else
	{
		plast = bigPackHead + BetoLe32(idxData.unitAddrOffset[count  - 1]);
		len = getLittlePackSize(plast);
		plast += len;
	}
	
	len = getLittlePackSize(ppack);
	memcpy( plast, ppack, len);
	
	idxData.regionUnitNum = LetoBe32( (BetoLe32(idxData.regionUnitNum) + 1) );
	idxData.unitAddrOffset[count] = LetoBe32( plast - bigPackHead);
	
	updateBigPackIdx(bigPackHead, idxData);	
}

PLAT_UINT32 CUtility::getLittlePackUID(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;
	
	T_UNIT_HEAD unitHead;
	memcpy(&unitHead, littlepack , sizeof(T_UNIT_HEAD) );
	ret = BetoLe32(unitHead.unitId);
	return ret;
}

PLAT_UINT32 CUtility::getLittlePackSID(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;

	T_MESSAGE_HEAD messageHead;
	memcpy(&messageHead, littlepack + sizeof(T_UNIT_HEAD), sizeof(T_MESSAGE_HEAD) );
	ret = BetoLe32(messageHead.SID);

	return ret;
}

PLAT_UINT32 CUtility::getLittlePackDID(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;
	
	T_MESSAGE_HEAD messageHead;
	memcpy(&messageHead, littlepack + sizeof(T_UNIT_HEAD), sizeof(T_MESSAGE_HEAD) );
	ret = BetoLe32(messageHead.DID);
	
	return ret;
}

PLAT_UINT32 CUtility::getLittlePackDataSize(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;
	
	T_UNIT_HEAD unitHead;
	memcpy(&unitHead, littlepack , sizeof(T_UNIT_HEAD) );
	ret = BetoLe32(unitHead.unitSize) * sizeof(PLAT_UBYTE);

	return ret;
}

PLAT_UINT32 CUtility::getLittlePackSize(const PLAT_UBYTE* littlepack)
{
	return getLittlePackDataSize(littlepack) + sizeof(T_UNIT_HEAD);
}

bool CUtility::updateLittlePack(const PLAT_UBYTE* littpackSrc, PLAT_UBYTE* littlepackDst)
{
	PLAT_UINT32 slen = getLittlePackSize(littpackSrc);
	PLAT_UINT32 dlen = getLittlePackSize(littlepackDst);

	if ( slen != dlen )
	{
		printf("Warning:  update little pakcage source and destine size IS NOT equal!\n");
		return false;
	}
	memmove(littlepackDst, littpackSrc, slen);
	return true;
}

void CUtility::updateBigPackIdx(PLAT_UBYTE* bigPackHead, const T_DATA_INDEX & idxData)
{
	memcpy(bigPackHead,&idxData, sizeof(T_DATA_INDEX));
}

void CUtility::initBigPackIdx(PLAT_UBYTE* p)
{
	T_DATA_INDEX idx ;
	memset(&idx, 0x00, sizeof(T_DATA_INDEX));
	idx.platformHealth = LetoBe32(1);
	idx.platformStatus = LetoBe32(1);
	idx.regionUnitNum =  LetoBe32(0); //Range: 1~256
	idx.unitAddrOffset[0] = LetoBe32( sizeof(T_DATA_INDEX));

	updateBigPackIdx(p, idx);
}

PLAT_UINT32 CUtility::getUnitCounts(const PLAT_UBYTE* bigPackHead)
{
	T_DATA_INDEX idx;
	memcpy(&idx, bigPackHead, sizeof(T_DATA_INDEX));
	return BetoLe32(idx.regionUnitNum);
}

PLAT_UBYTE* CUtility::getUnitHead(const PLAT_UBYTE* bigPackHead, PLAT_UINT32 indx)
{
	T_DATA_INDEX idx;
	memcpy(&idx, bigPackHead, sizeof(T_DATA_INDEX));
	return  (PLAT_UBYTE* )bigPackHead +  BetoLe32(idx.unitAddrOffset[indx]) ;
}


///////////////////////////////////////////////////////////
// Big package encode
///////////////////////////////////////////////////////////
CBigpackParser::CBigpackParser(PLAT_UBYTE* p)
{
	m_phead = p;
    CUtility::initBigPackIdx(p);
}

CBigpackParser::~CBigpackParser()
{
}

void CBigpackParser::finished()
{
}


void CBigpackParser::pushPack(PLAT_UBYTE*  ppack)
{
	CUtility::pushBackPack(m_phead, ppack);
}

void CBigpackParser::pushPack(int type)
{
}

void CBigpackParser::pushPackzc2Broder()
{
}

void CBigpackParser::pushPackZc2Cir()
{
}

void CBigpackParser::pushPackZc2Atp()
{
}

void CBigpackParser::pushPackZc2Ato()
{
	//#define MT_ZC_TO_CC 0x00014101

	//T_UNIT_HEAD     unithead;
	//T_MESSAGE_HEAD  messagehead;
	//int i = 0;
	//unithead.unitId = ZC_ID_MESSAGEOUT;
	//unithead.unitSize = 2 + sizeof(messagehead);
	//
	//messagehead.sequenceNum =1;			//临时
	//messagehead.timeStamp =0;			//临时
	//messagehead.SID = ZC1;
	//messagehead.DID = ZC1;
	//messagehead.msgType = MT_ZC_TO_CC;
	//messagehead.msgLen = 2;

	//memcpy(m_pcurrent, &unithead, sizeof(unithead));
	//m_pcurrent +=sizeof(unithead);
	//memcpy(m_pcurrent, &messagehead, sizeof(messagehead));
	//m_pcurrent +=sizeof(messagehead);

	//char m[2] = {'a','b'};
	//memcpy(m_pcurrent, &m[0], sizeof(PLAT_UBYTE));
	//++m_pcurrent;
	//memcpy(m_pcurrent, &m[1], sizeof(PLAT_UBYTE));
}

//////////////////////////////////////////////////////
//
/////////////////////////////////
CLittlePack::CLittlePack(const PLAT_UINT32 fromID, const PLAT_UBYTE* p)
{
	m_pHeader = (PLAT_UBYTE*) p;
	m_fromID = fromID;
	init();	
}


CLittlePack::CLittlePack(const PLAT_UINT32 fromID, const PLAT_UBYTE* parentHead, const PLAT_UINT32 idx)
{
	m_pHeaderParent = (PLAT_UBYTE*)  parentHead;
	m_idx = idx;
	m_fromID = fromID;
	
	T_DATA_INDEX dataidx;
	memcpy( &dataidx, m_pHeaderParent, sizeof(T_DATA_INDEX));
	m_pHeader = (PLAT_UBYTE*) (parentHead + CUtility::BetoLe32(dataidx.unitAddrOffset[idx]) );
	init();
}

void CLittlePack::init()
{
	memcpy(&m_header, m_pHeader, sizeof(T_UNIT_HEAD) );
	m_header.unitId = CUtility::BetoLe32(m_header.unitId);
	m_header.unitSize = CUtility::BetoLe32(m_header.unitSize);
	m_pData =  m_pHeader + sizeof(T_UNIT_HEAD);
}

void CLittlePack::updateUID(PLAT_UINT32 _id)
{ 
	m_header.unitId = _id;
	PLAT_UINT32 id = CUtility::LetoBe32(_id);
	memcpy(m_pHeader, &id, sizeof(PLAT_UINT32));
}

PLAT_UINT32 CLittlePack::getDstID() const
{
	return CUtility::getLittlePackDID((PLAT_UBYTE*) this);

	//PLAT_UINT32 Tempreturn = 0;
	//PLAT_UINT32 fromID = m_fromID;
	//
	//switch( (fromID & 0xE0000000) >> 29 )
	//{
	//case ATS:
	//case CBI:
	//	break;
	//case ZC:
	//	/*若数据源为ZC，根据单元ID得到数据单元的目的地ID*/
	//	Tempreturn =ZC_GetDestID();		
	//	break;
	//case ATO:		
	//	Tempreturn =ATO_GetDestID();/*若数据源为ATO，根据单元ID得到数据单元的目的地ID*/					
	//	break;
	//case ATP:					
	//	Tempreturn =ATP_GetDestID();/*若数据源为ATP，根据单元ID得到数据单元的目的地ID*/		
	//	break;
	//case RS:					
	//	Tempreturn =RS_GetDestID();/*若数据源为RS，根据单元ID得到数据单元的目的地ID*/						
	//	break;		
	//default:
	//	break;
	//}
	//return Tempreturn;	
}

PLAT_UINT32 CLittlePack::getDstIDInternal() const
{
	PLAT_UINT32 ret = 0;
	if (hasMsgHeader())
	{
		T_MESSAGE_HEAD messageHead;
		memcpy(&messageHead, m_pData, sizeof(T_MESSAGE_HEAD));
		ret =  CUtility::BetoLe32(messageHead.DID);
	}
	return ret;
}


PLAT_UINT32 CLittlePack::getSrcID() const
{
	return CUtility::getLittlePackSID((PLAT_UBYTE*) this);

	//PLAT_UINT32 ret = 0;
	//if (hasMsgHeader())
	//{
	//	T_MESSAGE_HEAD messageHead;
	//	memcpy(&messageHead, m_pData, sizeof(T_MESSAGE_HEAD));
	//	ret = messageHead.SID;
	//}
	//return ret;
}


/*得到ATP发送数据包中各单元的目的地ID*/
PLAT_UINT32 CLittlePack::ATP_GetDestID()  const
{
	PLAT_UINT32 SRC = m_fromID;
	PLAT_UINT32 dest = 0;

#ifdef ATPDEFINE
	switch(m_header.unitId)
	{
	case ATP_ID_TO_ATO:													/*ATP至ATO*/
		dest =((SRC&0x1FFFFFFF)|0x40000000);
		break;
		
	case ATP_ID_MESSAGEOUT1:											/*ATP发送的通信报文*/
	case ATP_ID_MESSAGEOUT2:
		{
			dest = getDstIDInternal();
			break;
		}
	case ATP_ID_TO_BOARD1:												/*ATP至列车*/
	case ATP_ID_TO_BOARD2:												/*ATP至列车*/	
	case ATP_ID_TO_BOARD3:												/*ATP至列车*/
		dest =((SRC&0x1FFFFFFF)|0x80000000);	
		break;
		
	case ATP_ID_LINKCTRL1:												/*ATP至平台*/			
	case ATP_ID_LINKCTRL2:												/*ATP至平台*/
	case ATP_ID_DBLINKCTRL:												/*ATP至平台*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;		
	default:
		dest =0;			
	}
#endif

	return dest;
}

PLAT_UINT32 CLittlePack::RS_GetDestID() const /*得到RS发送数据包中各单元的目的地ID*/
{
	PLAT_UINT32 dest = 0;
	PLAT_UINT32 SRC = m_fromID;

 #ifdef RSDEFINE			
	switch(m_header.unitId)
	{
	case RS_ID_VELOCITYOUT:												/*列车至ATO*/	
		dest =((SRC&0x1FFFFFFF)|0x40000000);
		break;
		
	case RS_ID_TO_PLOT:													/*列车至现场*/
		dest =PLOT;
		break;
		
	case RS_ID_TO_ATP:													/*列车至ATP*/
	case RS_ID_BOARD1_TO_ATP:											/*列车至ATP*/
	case RS_ID_BOARD2_TO_ATP:											/*列车至ATP*/		
	case RS_ID_BOARD3_TO_ATP:											/*列车至ATP*/
		dest =((SRC&0x1FFFFFFF)|0x60000000);
		break;	
	default:

		dest =0;
	}
	#endif

	return dest;
}

PLAT_UINT32 CLittlePack::ZC_GetDestID() const
{
	PLAT_UINT32 dest;
	dest =0;
	
 #ifdef ZCDEFINE
	switch(m_header.unitId)
	{
		
	    case ZC_ID_MESSAGEOUT:
		{
			dest = getDstIDInternal();
			break;
		}
	   default:
		
		dest =0;
	}
#endif
		
	return dest;
}

bool CLittlePack::hasMsgHeader() const
{
	return true;
}

// id Bit 29 - Bit 24
// get package data type from uint id
PLAT_UINT32 CLittlePack::dataType() const
{
	return CUtility::getBitsVal(m_header.unitId, 24,29);
	//return m_header.unitId & 0x3F000000; 
}

//1010
bool CLittlePack::isMsgOut() const
{
	return dataType() == 0xa ? true : false;
}

// 1011
bool CLittlePack::isConnectControl() const
{
	return dataType() ==  0xB ? true : false;
}

bool CLittlePack::isBroad() const
{
	return getDstID()  == 0xffff ? true : false;
}

// 1010
bool CLittlePack::isConnectState() const
{
	return dataType() ==  0xA ? true : false;	
}

bool CLittlePack::isInputAppStatus() const
{
	return dataType() == 0xF ? true : false;
}

// get content of NO.$byteidx, start from 0
PLAT_UINT8 CLittlePack::getByteData(int byteidx)
{
	PLAT_UINT8 val;
	memcpy(&val, m_pData + byteidx * sizeof(PLAT_UINT8) + sizeof(T_MESSAGE_HEAD), sizeof(PLAT_UINT8));
	return val & 0xFF;
}

void CLittlePack::setByteData(int byteidx, PLAT_UINT8 val)
{
	PLAT_UINT8 tmp = val;
	memcpy(m_pData + byteidx * sizeof(PLAT_UINT8) + sizeof(T_MESSAGE_HEAD), &tmp, sizeof(PLAT_UINT8));
}

// get byte 0 data
PLAT_UINT8 CLittlePack::getConnectState()
{
	return getByteData(0);
}

void CLittlePack::setConnectState(PLAT_UINT8 val)
{
	setByteData(0, val);
}

PLAT_UINT8 CLittlePack::getConnectControl()
{
	return getByteData(0);
}


PLAT_UINT32 CLittlePack::ATO_GetDestID() const/*得到ATO发送数据包中各单元的目的地ID*/
{
	PLAT_UINT32 SRC = m_fromID;
	PLAT_UINT32 dest;
	dest =0;
	
#ifdef ATODEFINE
	switch(m_header.unitId)
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
			dest = getDstIDInternal();
			break;
		}
		
	default:		
		dest =0;
	}
#endif
	
	return dest;
}


/////////////////////////////////////////////////
//
// Zc related
//
/////////////////////////////////////////////////
bool CZc::updateNotifyTerminal(PLAT_UINT32 id, bool badd)
{
	if (badd)
		m_listTerminal.push_back(id);
	else
	{
		m_listTerminal.remove(id);
	}

	return true;
}

std::list<PLAT_UINT32> CZc::getNotifyTerminals()
{
	return m_listTerminal;
}




