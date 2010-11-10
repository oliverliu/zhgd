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

//#include <stdarg.h>
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
// void pFatal(const char* format, ...)   { return pmesg(0, format, ...); }
// void pWarning(const char* format, ...) { return pmesg(1, format, ...); }
// void pDebug(const char* format, ...)   { return pmesg(2, format, ...); }
// void pBig(const char* format, ...)     { return pmesg(5, format, ...); }

#include <math.h>
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
	if ( end < start)
		printf("!!!! Warning %s parameter error", "getBitsVal");
	int tmp = (int) pow(2, end - start + 1) - 1;
	return  ( tmp << start ) & val;
}

PLAT_UINT32 CUtility::setBitsVal(const PLAT_UINT32 dst, PLAT_UINT8 start, PLAT_UINT8 end, PLAT_UINT32 val)
{	
	char small = start >= end ? end : start;
	return dst | (val << small);
}

void CUtility::pushBackPack(PLAT_UBYTE* bigPackHead, PLAT_UBYTE * ppack)
{
	//Must ensure bigPackHead is the head of big package
	PLAT_UBYTE* m_phead = bigPackHead;
	T_DATA_INDEX idxData ;
	memcpy(&idxData, bigPackHead, sizeof(T_DATA_INDEX));
	
	PLAT_UINT32 count = idxData.regionUnitNum;
	PLAT_UBYTE* plast = bigPackHead + idxData.unitAddrOffset[count - 1 ];
	PLAT_UINT32 len = getLittlePackSize(plast);
	plast += len;
	
	len = getLittlePackSize(ppack);
	memcpy( plast, ppack, len);
	
	idxData.regionUnitNum++;
	idxData.unitAddrOffset[count] =  plast - bigPackHead;
	
	updateBigPackIdx(bigPackHead, idxData);	
}

PLAT_UINT32 CUtility::getLittlePackUID(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;
	
	T_UNIT_HEAD unitHead;
	memcpy(&unitHead, littlepack , sizeof(T_UNIT_HEAD) );
	ret = unitHead.unitId;
	return ret;
}

PLAT_UINT32 CUtility::getLittlePackUSize(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;
	
	T_UNIT_HEAD unitHead;
	memcpy(&unitHead, littlepack , sizeof(T_UNIT_HEAD) );
	ret = unitHead.unitSize;
	return ret;
}

PLAT_UINT32 CUtility::getLittlePackSID(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;

	T_MESSAGE_HEAD messageHead;
	memcpy(&messageHead, littlepack + sizeof(T_UNIT_HEAD), sizeof(T_MESSAGE_HEAD) );
	ret = messageHead.SID;

	return ret;
}

PLAT_UINT32 CUtility::getLittlePackDID(const PLAT_UBYTE * littlepack)
{
	PLAT_UINT32 ret = 0;
	
	T_MESSAGE_HEAD messageHead;
	memcpy(&messageHead, littlepack + sizeof(T_UNIT_HEAD), sizeof(T_MESSAGE_HEAD) );
	ret = messageHead.DID;
	
	return ret;
}


PLAT_UINT32 CUtility::getLittlePackSize(const PLAT_UBYTE* littlepack)
{
	T_UNIT_HEAD unitHeadtmp;
	memcpy(&unitHeadtmp, littlepack, sizeof(T_UNIT_HEAD));
	return sizeof(T_UNIT_HEAD) + unitHeadtmp.unitSize * sizeof(PLAT_UBYTE);
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
	//memmove(littlepackDst, littpackSrc, slen);
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
	idx.platformHealth = 1;
	idx.platformStatus = 1;
	idx.regionUnitNum = 0;
	updateBigPackIdx(p, idx);
}

PLAT_UINT32 CUtility::getUnitCounts(const PLAT_UBYTE* bigPackHead)
{
	T_DATA_INDEX idx;
	memcpy(&idx, bigPackHead, sizeof(T_DATA_INDEX));
	return idx.regionUnitNum;
}

PLAT_UBYTE* CUtility::getUnitHead(const PLAT_UBYTE* bigPackHead, PLAT_UINT32 indx)
{
	T_DATA_INDEX idx;
	memcpy(&idx, bigPackHead, sizeof(T_DATA_INDEX));
	return  (PLAT_UBYTE* )bigPackHead + idx.unitAddrOffset[indx] ;
}


///////////////////////////////////////////////////////////
// Big package encode
///////////////////////////////////////////////////////////
CBigpackParser::CBigpackParser(PLAT_UBYTE* p)
{
	m_phead = p;
	m_pcurrent = p + sizeof(T_DATA_INDEX);
	memset(&m_index, 0x00, sizeof(T_DATA_INDEX));
	m_index.platformHealth = 1;
	m_index.platformStatus = 1;
	m_index.regionUnitNum = 0;
	m_seqnum = -1;
}

CBigpackParser::~CBigpackParser()
{
   finished();
}

void CBigpackParser::finished()
{
	memcpy(m_phead, &m_index, sizeof(T_DATA_INDEX));	
}


void CBigpackParser::pushPack(T_UNIT*  ppack)
{
	m_seqnum++;
	if ( m_seqnum > 255) 
	{
		printf("!!!!!!Waring package sum large than 256, now is %d", m_seqnum + 1); 
		return;
	}
	m_index.unitAddrOffset[m_seqnum] = m_pcurrent - m_phead;
	m_index.regionUnitNum =  m_seqnum + 1;

	PLAT_UINT32 len = ppack->unitSize;
	len += sizeof(T_UNIT_HEAD);

	memcpy(m_pcurrent, ppack, len);
	m_pcurrent +=len;
}

void CBigpackParser::pushPack(int type)
{
	m_seqnum++;
	if ( m_seqnum > 255) 
	{
		printf("!!!!!!Waring package sum large than 256, now is %d", m_seqnum + 1); 
		return;
	}
	m_index.unitAddrOffset[m_seqnum] = m_pcurrent - m_phead;
	m_index.regionUnitNum =  m_seqnum + 1;

	switch(type)
	{
		case zc2Cir:
			pushPackZc2Cir();
		break;
		case zc2Broder:
			pushPackzc2Broder();
		break;
		case zc2Atp:
			pushPackZc2Atp();
		break;
		case zc2Ato:
			pushPackZc2Ato();
		break;
		case zc2Ci:
		break;
		default:
		break;
	}
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
	#define MT_ZC_TO_CC 0x00014101

	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  messagehead;
	int i = 0;
	unithead.unitId = ZC_ID_MESSAGEOUT;
	unithead.unitSize = 2 + sizeof(messagehead);
	
	messagehead.sequenceNum =1;			//��ʱ
	messagehead.timeStamp =0;			//��ʱ
	messagehead.SID = ZC1;
	messagehead.DID = ZC1;
	messagehead.msgType = MT_ZC_TO_CC;
	messagehead.msgLen = 2;

	memcpy(m_pcurrent, &unithead, sizeof(unithead));
	m_pcurrent +=sizeof(unithead);
	memcpy(m_pcurrent, &messagehead, sizeof(messagehead));
	m_pcurrent +=sizeof(messagehead);

	char m[2] = {'a','b'};
	memcpy(m_pcurrent, &m[0], sizeof(PLAT_UBYTE));
	++m_pcurrent;
	memcpy(m_pcurrent, &m[1], sizeof(PLAT_UBYTE));
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
	m_pHeader = (PLAT_UBYTE*) (parentHead + dataidx.unitAddrOffset[idx] );
	init();
}

void CLittlePack::init()
{
	memcpy(&m_header, m_pHeader, sizeof(T_UNIT_HEAD) );
	m_pData =  m_pHeader + sizeof(T_UNIT_HEAD);
}

PLAT_UINT32 CLittlePack::getDstID() const
{
	PLAT_UINT32 Tempreturn = 0;
	PLAT_UINT32 fromID = m_fromID;
	
	switch( (fromID & 0xE0000000) >> 29 )
	{
	case ATS:
	case CBI:
		break;
	case ZC:
		/*������ԴΪZC�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/
		Tempreturn =ZC_GetDestID();		
		break;
	case ATO:		
		Tempreturn =ATO_GetDestID();/*������ԴΪATO�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/					
		break;
	case ATP:					
		Tempreturn =ATP_GetDestID();/*������ԴΪATP�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/		
		break;
	case RS:					
		Tempreturn =RS_GetDestID();/*������ԴΪRS�����ݵ�ԪID�õ����ݵ�Ԫ��Ŀ�ĵ�ID*/						
		break;		
	default:
		break;
	}
	return Tempreturn;	
}

PLAT_UINT32 CLittlePack::getDstIDInternal() const
{
	PLAT_UINT32 ret = 0;
	if (hasMsgHeader())
	{
		T_MESSAGE_HEAD messageHead;
		memcpy(&messageHead, m_pData, sizeof(T_MESSAGE_HEAD));
		ret = messageHead.DID;
	}
	return ret;
}


PLAT_UINT32 CLittlePack::getSrcID() const
{
	PLAT_UINT32 ret = 0;
	if (hasMsgHeader())
	{
		T_MESSAGE_HEAD messageHead;
		memcpy(&messageHead, m_pData, sizeof(T_MESSAGE_HEAD));
		ret = messageHead.SID;
	}
	return ret;
	
}


/*�õ�ATP�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/
PLAT_UINT32 CLittlePack::ATP_GetDestID()  const
{
	PLAT_UINT32 SRC = m_fromID;
	PLAT_UINT32 dest = 0;

#ifdef ATPDEFINE
	switch(m_header.unitId)
	{
	case ATP_ID_TO_ATO:													/*ATP��ATO*/
		dest =((SRC&0x1FFFFFFF)|0x40000000);
		break;
		
	case ATP_ID_MESSAGEOUT1:											/*ATP���͵�ͨ�ű���*/
	case ATP_ID_MESSAGEOUT2:
		{
			dest = getDstIDInternal();
			break;
		}
	case ATP_ID_TO_BOARD1:												/*ATP���г�*/
	case ATP_ID_TO_BOARD2:												/*ATP���г�*/	
	case ATP_ID_TO_BOARD3:												/*ATP���г�*/
		dest =((SRC&0x1FFFFFFF)|0x80000000);	
		break;
		
	case ATP_ID_LINKCTRL1:												/*ATP��ƽ̨*/			
	case ATP_ID_LINKCTRL2:												/*ATP��ƽ̨*/
	case ATP_ID_DBLINKCTRL:												/*ATP��ƽ̨*/
		dest =((SRC&0x1FFFFFFF)|0xC0000000);
		break;		
	default:
		dest =0;			
	}
#endif

	return dest;
}

PLAT_UINT32 CLittlePack::RS_GetDestID() const /*�õ�RS�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/
{
	PLAT_UINT32 dest = 0;
	PLAT_UINT32 SRC = m_fromID;

 #ifdef RSDEFINE			
	switch(m_header.unitId)
	{
	case RS_ID_VELOCITYOUT:												/*�г���ATO*/	
		dest =((SRC&0x1FFFFFFF)|0x40000000);
		break;
		
	case RS_ID_TO_PLOT:													/*�г����ֳ�*/
		dest =PLOT;
		break;
		
	case RS_ID_TO_ATP:													/*�г���ATP*/
	case RS_ID_BOARD1_TO_ATP:											/*�г���ATP*/
	case RS_ID_BOARD2_TO_ATP:											/*�г���ATP*/		
	case RS_ID_BOARD3_TO_ATP:											/*�г���ATP*/
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
	return m_header.unitId & 0x3F000000; 
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


PLAT_UINT32 CLittlePack::ATO_GetDestID() const/*�õ�ATO�������ݰ��и���Ԫ��Ŀ�ĵ�ID*/
{
	PLAT_UINT32 SRC = m_fromID;
	PLAT_UINT32 dest;
	dest =0;
	
#ifdef ATODEFINE
	switch(m_header.unitId)
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




