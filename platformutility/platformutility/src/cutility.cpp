#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cutility.h"
#include "INIReader.h"

#include <stdarg.h>
//int msglevel = 3; /* the higher, the more messages... */
// 
// #if defined(NDEBUG) && defined(__GNUC__)
// #define pmesg(level, format, args...) ((void)0)
// #else
// void pmesg(int level, const char* format, ...) {
// #ifdef NDEBUG
//      /* Empty body, so a good compiler will optimise calls
//         to pmesg away */
// #else
//         va_list args;
// 
//         if (level > msglevel)
//                 return;
// 
//         va_start(args, format);
//         vfprintf(stdout, format, args);
//              //vfprintf(stderr, format, args);
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

//#define _MESSAGE_BIG_ENDIAN

int CUtility::needSwap()
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
        printf("Need swap endian: %d\n", bneed);
    }
    return bneed;
}

PLAT_UINT16 CUtility::ByteSwap16 (PLAT_UINT16 nValue)
{
   return (((nValue>> 8)) | (nValue << 8));
}

PLAT_UINT32 CUtility::ByteSwap32 (PLAT_UINT32 nLongNumber)
{
   return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
   ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

PLAT_UINT64 CUtility::ByteSwap64 (PLAT_UINT64 nLongNumber)
{
    return (( (PLAT_UINT64) (ByteSwap32 ((PLAT_UINT32) ((nLongNumber << 32) >>32))) <<32) |
            ByteSwap32 ((PLAT_UINT32)(nLongNumber >> 32)) );
}

void CUtility::swapIdx(PLAT_UBYTE* bigPackHead)
{
    T_DATA_INDEX idx ;
    memset(&idx, 0x00, sizeof(T_DATA_INDEX));

    T_DATA_INDEX idxsrc ;
    memcpy(&idxsrc, bigPackHead, sizeof(T_DATA_INDEX));

    idx.platformHealth = ByteSwap32(idxsrc.platformHealth);
    idx.platformStatus = ByteSwap32(idxsrc.platformStatus);
    idx.regionUnitNum =  ByteSwap32(idxsrc.regionUnitNum); //Range: 1~256

    for (int i = 0; i < 256; i++)
            idx.unitAddrOffset[i] = ByteSwap32(idxsrc.unitAddrOffset[i]);

    updateBigPackIdx(bigPackHead, idx);

}

void  CUtility::swapLittlePackage(PLAT_UBYTE * littlepack,bool isBigEndianP)
{
    if (isBigEndianP)
        swapUnitHead(littlepack);

    if (hasMsgHead(littlepack))
        swapUnitMsgHead(littlepack);

    if (!isBigEndianP)
        swapUnitHead(littlepack);
}

void CUtility:: swapUnitHead(PLAT_UBYTE * littlepack)
{
    T_UNIT_HEAD src, dst;
    memset(&dst, 0, sizeof(T_UNIT_HEAD));
    memcpy(&src, littlepack , sizeof(T_UNIT_HEAD) );

    dst.unitId = ByteSwap32(src.unitId);
    dst.unitSize = ByteSwap32(src.unitSize);

    memcpy(littlepack, &dst, sizeof(T_UNIT_HEAD));
}

void CUtility:: swapUnitMsgHead(PLAT_UBYTE * littlepack)
{
    T_MESSAGE_HEAD src,dst;
    memcpy(&src, littlepack + sizeof(T_UNIT_HEAD), sizeof(T_MESSAGE_HEAD) );
    memset(&dst, 0, sizeof(T_MESSAGE_HEAD) );

    //PLAT_UINT32 sequenceNum;      /*通信报文序列号*/
    //PLAT_UINT64 timeStamp;                /*通信报文时间戳*/
    //PLAT_UINT32 SID;                      /*通信报文源地址*/
    //PLAT_UINT32 DID;                      /*通信报文目的地址*/
    //PLAT_UINT32 msgType;          /*通信报文信息类型*/
    //PLAT_UINT32 msgLen;                   /*通信报文数据大小*/

    dst.sequenceNum = ByteSwap32(src.sequenceNum);
    dst.timeStamp = ByteSwap64(src.timeStamp);
    dst.SID = ByteSwap32(src.SID);
    dst.DID = ByteSwap32(src.DID);
    dst.msgType = ByteSwap32(src.msgType);
    dst.msgLen = ByteSwap32(src.msgLen);

    memcpy(littlepack + sizeof(T_UNIT_HEAD), &dst, sizeof(T_MESSAGE_HEAD));
}

void CUtility::bigPackToLE(PLAT_UBYTE* bigPackHead)
{
    swapBigpackage(bigPackHead, true);
}
void CUtility::bigPackToBE(PLAT_UBYTE* bigPackHead)
{
    swapBigpackage(bigPackHead, false);
}

void CUtility::littlePackToLE(PLAT_UBYTE* bigPackHead)
{
    swapLittlePackage(bigPackHead, true);
}
void CUtility::littlePackToBE(PLAT_UBYTE* bigPackHead)
{
    swapLittlePackage(bigPackHead, false);
}

void  CUtility::swapBigpackage(PLAT_UBYTE* bigPackHead, bool isBigEndianP)
{
    if ( isBigEndianP )
    {
        //big endian to little endian, because local is little endian
            swapIdx(bigPackHead);
    }

    PLAT_UINT32 count = getUnitCounts(bigPackHead);
    for (int i = 0; i < count; i++)
    {
        PLAT_UBYTE * plittle = getUnitHead(bigPackHead, i);
        swapLittlePackage(plittle, isBigEndianP);
    }

    if ( !isBigEndianP )
    {
        swapIdx(bigPackHead);
    }
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
    PLAT_UINT8 big = start >= end ? start : end;
    PLAT_UINT32 h = dst >>(big+1) << (big+1);
    PLAT_UINT32 l =  dst & ((1 << (small + 1)) - 1);
    return (h | l) | (val << small);
}

 void CUtility::outputLittlepack(const unsigned char * buffer)
 {
    //print out little package info
    printf("Now package: unitId = %08x, unitSize = %d, data:\n", 
            CUtility::getLittlePackUID(buffer), CUtility::getLittlePackDataSize(buffer));
    PLAT_UINT32 size = CUtility::getLittlePackSize(buffer);
    for(PLAT_UINT32 j = 0;j < size; j++)
    {
            printf("%02x  ", buffer[j]);
    }
    printf("\n");
 }

//The input buf MUST be little endian and is big package pointer.
void CUtility::outputPackage(const PLAT_UBYTE * buf,FILE* fp)
{
    PLAT_UINT32 count = CUtility::getUnitCounts(buf);
    fprintf(fp, "package has %d unit\n", count);
    for (PLAT_UINT32 i = 0; i < count; i++)
    {
        PLAT_UBYTE * unit = CUtility::getUnitHead(buf, i);
        fprintf(fp, "NO.%2d package: unitId = %08x, unitSize = %d, data:\n", 
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
        
    PLAT_UINT32 count = idxData.regionUnitNum;
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
            plast = bigPackHead + idxData.unitAddrOffset[0];
    }
    else
    {
            plast = bigPackHead + idxData.unitAddrOffset[count  - 1];
            len = getLittlePackSize(plast);
            plast += len;
    }
        
    len = getLittlePackSize(ppack);
    memcpy( plast, ppack, len);
        
    idxData.regionUnitNum = idxData.regionUnitNum + 1;
    idxData.unitAddrOffset[count] = plast - bigPackHead;
        
    updateBigPackIdx(bigPackHead, idxData); 
}

PLAT_UINT32 CUtility::getSrcIDInternal()
{
    INIReader reader("sid.config");
         
    if (reader.ParseError() < 0) {
       printf( "Can't load 'private.config'\n");
       return 0;
    }
    long value = reader.GetInteger("self","SID",0);
   // long value = reader.GetInteger("SID2Internal",key,0);

    return value;
}


// id Bit 29 - Bit 24
// get package data type from uint id
PLAT_UINT32 CUtility::getLittlePackDataType(const PLAT_UBYTE * littlepack)
{
    PLAT_UINT32 uid = getLittlePackUID(littlepack);
    return getBitsVal(uid, 24,29);
    //return m_header.unitId & 0x3F000000; 
}

// id Bit 23 - Bit 16
// get package data info from uint id
PLAT_UINT32 CUtility::getLittlePackDataInfo(const PLAT_UBYTE * littlepack)
{
    PLAT_UINT32 uid = getLittlePackUID(littlepack);
    return getBitsVal(uid, 16,23);
}

// littlepack is little endian
// Only communication msg out has msg head
bool CUtility::hasMsgHead(const PLAT_UBYTE * littlepack)
{
   bool ret = false;
   PLAT_UINT32 dtype = getLittlePackDataType(littlepack);

    switch(dtype)
    {
        case 6://000110 link state data
        case 0xb://001011 link control data
        {
            ret = false;
        }
        break;
       case 0xa://001010 communication msg out
       case 5://000101 communication msg in
            ret = true;
        break;
        default:
            break;
    }
   return ret;
}


PLAT_UINT32 CUtility::getLinkStateDID(PLAT_UBYTE*  plinkstatePack)
{
    typedef struct connectStateData
    {
        PLAT_UBYTE result;
        PLAT_UBYTE resver1[3];
        PLAT_UINT sid;
        PLAT_UINT did;
        PLAT_UINT  resver2[2];
    }tConStateData;

    tConStateData st;
    memcpy(&st, plinkstatePack, sizeof(tConStateData));
    return st.did;

}

PLAT_UINT32 CUtility::getLinkStateSID(PLAT_UBYTE*  plinkstatePack)
{
    typedef struct connectStateData
    {
        PLAT_UBYTE result;
        PLAT_UBYTE resver1[3];
        PLAT_UINT sid;
        PLAT_UINT did;
        PLAT_UINT  resver2[2];
    }tConStateData;

    tConStateData st;
    memcpy(&st, plinkstatePack, sizeof(tConStateData));
    return st.sid;
}

PLAT_UINT32 CUtility::getLittlePackUID(const PLAT_UBYTE * littlepack)
{
    PLAT_UINT32 ret = 0;
        
    T_UNIT_HEAD unitHead;
    memcpy(&unitHead, littlepack , sizeof(T_UNIT_HEAD) );
    ret = unitHead.unitId;
    return ret;
}

PLAT_UINT32 CUtility::getLittlePackSID(const PLAT_UBYTE * littlepack)
{
    PLAT_UINT32 ret = 0;

    if ( hasMsgHead(littlepack))
    {
        T_MESSAGE_HEAD messageHead;
        memcpy(&messageHead, littlepack + sizeof(T_UNIT_HEAD), sizeof(T_MESSAGE_HEAD) );
        ret = messageHead.SID;
    }
    else
    {
       //littlepack has not message heade
       //using terminal's ID as its SID
       ret = getSrcIDInternal();
    }

    return ret;
}

PLAT_UINT32 CUtility::getLittlePackDID(const PLAT_UBYTE * littlepack)
{
    PLAT_UINT32 ret = 0;
        
    if ( hasMsgHead(littlepack))//datatyep = 0x5 or 0xa
    {
        T_MESSAGE_HEAD messageHead;
        memcpy(&messageHead, littlepack + sizeof(T_UNIT_HEAD), sizeof(T_MESSAGE_HEAD) );
        ret = messageHead.DID;
        }
        else
        {
                //get did from datatye and rules
            //Rules: ATO1: 0x40000001, its correspond ATP1 ID is 0x60000001
            //       ATO2: 0x40000002, its correspond ATP2 ID is 0x60000002
            // The rules is ato/atp
            PLAT_UINT32 datatype =  CUtility::getLittlePackDataType(littlepack);
            PLAT_UINT32 srcId    =  CUtility::getLittlePackSID(littlepack);
            switch (datatype)
            {
                    case 0x10: //analog output data
                    case 0x11://TMS communication data
                    case 0x9://output board data
                        ret = srcId & 0xffffffff | 0x80000000;
                    break;

                    case 7://000111; atp 2 ato data; ato is 0x40000x
                        ret = srcId & 0x0fffffff | 0x40000000;
                    break;

                    case 8: //001000; ato 2 atp data; atp is 0x6000000X
                        ret = srcId & 0x0fffffff | 0x60000000;
                    break;

                    case 0xf:// 1111; program internal stata sync data
                    case 0xc://1100;event record data
                    //case 0xb://1011; connect control data
                    case 0xe://1110; db control command
                           ret = srcId & 0xffffffff | 0xc0000000;
                    break;
                    
                    default:
                    break;
            }
        }

    return ret;
}

PLAT_UINT32 CUtility::getLittlePackDataSize(const PLAT_UBYTE * littlepack)
{
    PLAT_UINT32 ret = 0;
        
    T_UNIT_HEAD unitHead;
    memcpy(&unitHead, littlepack , sizeof(T_UNIT_HEAD) );
    ret = unitHead.unitSize * sizeof(PLAT_UBYTE);

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
    idx.platformHealth = 1;
    idx.platformStatus = 1;
    idx.regionUnitNum =  0; //Range: 1~256
    idx.unitAddrOffset[0] = sizeof(T_DATA_INDEX);

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
    return  (PLAT_UBYTE* )bigPackHead +  idx.unitAddrOffset[indx] ;
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
    //messagehead.sequenceNum =1;                   //临时
    //messagehead.timeStamp =0;                     //临时
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
    m_pHeader = (PLAT_UBYTE*) (parentHead + dataidx.unitAddrOffset[idx] );
    init();
}

void CLittlePack::init()
{
    memcpy(&m_header, m_pHeader, sizeof(T_UNIT_HEAD) );
    m_header.unitId = m_header.unitId;
    m_header.unitSize = m_header.unitSize;
    m_pData =  m_pHeader + sizeof(T_UNIT_HEAD);
}

void CLittlePack::updateUID(PLAT_UINT32 id)
{ 
    m_header.unitId = id;
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
    //      break;
    //case ZC:
    //      /*若数据源为ZC，根据单元ID得到数据单元的目的地ID*/
    //      Tempreturn =ZC_GetDestID();             
    //      break;
    //case ATO:             
    //      Tempreturn =ATO_GetDestID();/*若数据源为ATO，根据单元ID得到数据单元的目的地ID*/                                 
    //      break;
    //case ATP:                                     
    //      Tempreturn =ATP_GetDestID();/*若数据源为ATP，根据单元ID得到数据单元的目的地ID*/         
    //      break;
    //case RS:                                      
    //      Tempreturn =RS_GetDestID();/*若数据源为RS，根据单元ID得到数据单元的目的地ID*/                                           
    //      break;          
    //default:
    //      break;
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
            ret =  messageHead.DID;
    }
    return ret;
}


PLAT_UINT32 CLittlePack::getSrcID() const
{
    return CUtility::getLittlePackSID((PLAT_UBYTE*) this);

    //PLAT_UINT32 ret = 0;
    //if (hasMsgHeader())
    //{
    //      T_MESSAGE_HEAD messageHead;
    //      memcpy(&messageHead, m_pData, sizeof(T_MESSAGE_HEAD));
    //      ret = messageHead.SID;
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
    case ATP_ID_TO_ATO:                                                                                                     /*ATP至ATO*/
            dest =((SRC&0x1FFFFFFF)|0x40000000);
            break;
                
    case ATP_ID_MESSAGEOUT1:                                                                                        /*ATP发送的通信报文*/
    case ATP_ID_MESSAGEOUT2:
            {
                    dest = getDstIDInternal();
                    break;
            }
    case ATP_ID_TO_BOARD1:                                                                                          /*ATP至列车*/
    case ATP_ID_TO_BOARD2:                                                                                          /*ATP至列车*/   
    case ATP_ID_TO_BOARD3:                                                                                          /*ATP至列车*/
            dest =((SRC&0x1FFFFFFF)|0x80000000);    
            break;
                
    case ATP_ID_LINKCTRL1:                                                                                          /*ATP至平台*/                   
    case ATP_ID_LINKCTRL2:                                                                                          /*ATP至平台*/
    case ATP_ID_DBLINKCTRL:                                                                                         /*ATP至平台*/
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
    case RS_ID_VELOCITYOUT:                                                                                         /*列车至ATO*/   
            dest =((SRC&0x1FFFFFFF)|0x40000000);
            break;
                
    case RS_ID_TO_PLOT:                                                                                                     /*列车至现场*/
            dest =PLOT;
            break;
                
    case RS_ID_TO_ATP:                                                                                                      /*列车至ATP*/
    case RS_ID_BOARD1_TO_ATP:                                                                                       /*列车至ATP*/
    case RS_ID_BOARD2_TO_ATP:                                                                                       /*列车至ATP*/           
    case RS_ID_BOARD3_TO_ATP:                                                                                       /*列车至ATP*/
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

//1010= 0xa
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
    if ( CUtility::hasMsgHead(m_pHeader) )
    {
	    memcpy(&val, m_pData + byteidx * sizeof(PLAT_UINT8) + sizeof(T_MESSAGE_HEAD),
            sizeof(PLAT_UINT8));
    }
    else
    {
      memcpy(&val, m_pData + byteidx * sizeof(PLAT_UINT8), 
          sizeof(PLAT_UINT8));
    }
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
    case ATO_ID_COOUT:                                                                                                      /*ATO至列车的电流输出信息*/     
            dest =((SRC&0x1FFFFFFF)|0x80000000);
            break;          
    case ATO_ID_TMSOUT:                                                                                                     /*ATO至列车的TMS输出信息*/
            dest =((SRC&0x1FFFFFFF)|0x80000000);
            break;
    case ATO_ID_IOOUT:                                                                                                      /*ATO至列车的IO输出信息*/
            dest =((SRC&0x1FFFFFFF)|0x80000000);
            break;
    case ATO_ID_ATPOUT:                                                                                                     /*ATO至ATP的输出信息*/
            dest =((SRC&0x1FFFFFFF)|0x60000000);
            break;
    case ATO_ID_SYSDATAOUT:                                                                                         /*ATO至平台的中间状态同步输出信息*/
            dest =((SRC&0x1FFFFFFF)|0xC0000000);
            break;
    case ATO_ID_EVENTOUT:                                                                                           /*ATO至平台的事件记录输出信息*/
            dest =((SRC&0x1FFFFFFF)|0xC0000000);
            break;
    case ATO_ID_COMLINKOUT:                                                                                         /*ATO至平台的通信连接控制信息*/
            dest =((SRC&0x1FFFFFFF)|0xC0000000);
            break;
    case ATO_ID_TRACKDBOUT:                                                                                         /*ATO至平台的DB数据控制信息*/
            dest =((SRC&0x1FFFFFFF)|0xC0000000);
            break;
    case ATO_ID_MESSAGEOUT:                                                                                         /*ATO发送的通信报文*/
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




