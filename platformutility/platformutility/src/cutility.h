#ifndef _CUTILITY_H
#define _CUTILITY_H

/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
 * Create Data: 2010-11-4
 * Version: 0.1
 * Author: Liuning
 * Contact: liuning@zdwxgd.com
 * Description: for platform simulator network layer
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 */
#include <list>
#include "platformhead.h"
//#include "InterfaceDataStruct.h"
#include "tpackage.h"
#pragma pack(1)                                                                                                                                         /*设定为1字节对齐*/

//#define _CRT_SECURE_NO_WARNINGS 
// The output / input data for platform are big endian sequence
// When enter our loop, transfer them to little endian and leave transfer them to big endian
class CUtility
{
public:
	//start from little bit with 0
	static bool isBitSet(PLAT_UINT32 val, PLAT_UINT32 bit);

	static  PLAT_UINT32 getBitsVal(const PLAT_UINT32 val, PLAT_UINT8 start, PLAT_UINT8 end);
	static  PLAT_UINT32 setBitsVal(const PLAT_UINT32 dst, PLAT_UINT8 start, PLAT_UINT8 end, PLAT_UINT32 val);

	static void outputPackage(const PLAT_UBYTE * buf, FILE* fp);
    static void outputLittlepack(const unsigned char * buffer);

	static PLAT_UINT16      ByteSwap16 (PLAT_UINT16 nValue);
	static PLAT_UINT32      ByteSwap32 (PLAT_UINT32 nLongNumber);
	static PLAT_UINT64      ByteSwap64 (PLAT_UINT64 nLongNumber);
	static int             needSwap(); //need: 1, Not: 0

	static void bigPackToLE(PLAT_UBYTE* bigPackHead) ;
	static void bigPackToBE(PLAT_UBYTE* bigPackHead)  ;
	static void littlePackToLE(PLAT_UBYTE* bigPackHead);
	static void littlePackToBE(PLAT_UBYTE* bigPackHead);

	//
	static void         pushBackPack(PLAT_UBYTE* bigPackHead, PLAT_UBYTE * ppack);

    //Actual SID map to internal id for determine ATO/ATP
	static  PLAT_UINT32 getSrcIDInternal();
		
	static bool        hasMsgHead(const PLAT_UBYTE * littlepack);
	static PLAT_UINT32 getLittlePackDataType(const PLAT_UBYTE * littlepack);
	static PLAT_UINT32 getLittlePackDataInfo(const PLAT_UBYTE * littlepack);
	static PLAT_UINT32 getLittlePackUID(const PLAT_UBYTE * littlepack);
	static PLAT_UINT32 getLittlePackSID(const PLAT_UBYTE * littlepack);
	static PLAT_UINT32 getLittlePackDID(const PLAT_UBYTE * littlepack);
	static PLAT_UINT32 getLittlePackDataSize(const PLAT_UBYTE * littlepack);
	static PLAT_UINT32 getLittlePackSize(const PLAT_UBYTE* littlepack);//including head
	static bool updateLittlePack(const PLAT_UBYTE* littpackSrc, PLAT_UBYTE* littlepackDst);

    static PLAT_UINT32 getLinkStateDID(PLAT_UBYTE*  plinkstatePack);
    static PLAT_UINT32 getLinkStateSID(PLAT_UBYTE*  plinkstatePack);
	//Big package
	static PLAT_UINT32 getUnitCounts(const PLAT_UBYTE* bigPackHead);
	static PLAT_UBYTE* getUnitHead(const PLAT_UBYTE* bigPackHead, PLAT_UINT32 indx); //start from 0;
	static void        initBigPackIdx(PLAT_UBYTE* p);
	static void        updateBigPackIdx(PLAT_UBYTE* bigPackHead, const T_DATA_INDEX & idxData);     

private:
    // isBigEndianP means bigPackHead is big endian, when get unit count form index.
        static void         swapBigpackage(PLAT_UBYTE* bigPackHead, bool isBigEndianP);
        static void         swapIdx(PLAT_UBYTE* bigPackHead);
    // isBigEndianP means bigPackHead is big endian, when get unit id form unit head.
    //unit id determine if it has msg head.
        static void         swapLittlePackage(PLAT_UBYTE * littlepack, bool isBigEndianP);
        static void         swapUnitHead(PLAT_UBYTE * littlepack);
        static void         swapUnitMsgHead(PLAT_UBYTE * littlepack);

};


// Utility for encode big package
// HAS NOT message header
// -------------------------------------
// |      Data index                                                                                                                    | Little Package1~256  |
// | Region unit number | platform state | platform health | offset unit[1~256] |                      |
// |      4 Bytes       |     4 Bytes    |     4 Bytes     |    4*256 Bytes     |                      |
// -------------------------------------

// In construct function, please give big package start address.
// In destruct function or finish function will update Data index area
// The class will change the data of address p and subsequent area.

//this is not used for now. 20101116 by liuning
class CBigpackParser
{
public:
        // Parameter p is big package start address.
        CBigpackParser(PLAT_UBYTE* p );
        ~CBigpackParser();

        //sequence push package to build a package
        void pushPack(int type);
        void pushPack(PLAT_UBYTE*  littlepack);

        //at end push package at end of big package

        void finished();

public:
        enum packType
        {
                zc2Cir = 0,
                        zc2Broder,
                        zc2Atp,
                        zc2Ato,
                        zc2Ci
        };

private:
        void pushPackZc2Atp();
        void pushPackZc2Ato();
        void pushPackZc2Cir();
        void pushPackzc2Broder();
private:
        PLAT_UBYTE * m_phead; //Big package start address.
};


// Using for process little package (unit), including parse and set values 
// HAS NOT message header
// -------------------------------------
// |      UnitHead      | UnitData    |
// | UnitID  | UnitSize |             |
// length
// | 4 Bytes | 4 Bytes  | Sizes Bytes |
// -------------------------------------

// HAS message header
// -------------------------------------
// |      UnitHead      |  MsgHead    |  UnitData    |
// | UnitID  | UnitSize |                            |
// length
// | 4 Bytes | 4 Bytes  | Sizes Bytes                |
// -------------------------------------

//UnitID struct
// -------------------------------------
// | Bit31 ~ bit30 | Bit29 ~ Bit24 | bit23 ~ Bit16 | Bit15 ~ Bit0 |
// | data source   | data type     | data info     | data ID      |
// -------------------------------------
class CLittlePack
{
public:
        CLittlePack(const PLAT_UINT32 fromID, const PLAT_UBYTE* p);
        CLittlePack(const PLAT_UINT32 fromID, const PLAT_UBYTE* parentHead, const PLAT_UINT32 idx);

        bool isConnectControl() const;
        bool isMsgOut() const;
        bool isInputAppStatus() const;
        bool isBroad() const;
        bool isConnectState() const;
        
        PLAT_UINT8 getConnectState();
        PLAT_UINT8 getConnectControl();

        PLAT_UINT32 getUnitID() const {return m_header.unitId;}
        PLAT_UBYTE* getDataHeader() const { return m_pData;}
        void        updateUID(PLAT_UINT32 id) ;

        PLAT_UINT32 getDstID() const;
        PLAT_UINT32 getSrcID() const;


        void setConnectState(PLAT_UINT8 val);

private:        
        PLAT_UINT32 ATO_GetDestID() const; /*得到ATO发送数据包中各单元的目的地ID*/
        PLAT_UINT32 ATP_GetDestID() const; /*得到ATP发送数据包中各单元的目的地ID*/
        PLAT_UINT32 RS_GetDestID() const;  /*得到RS发送数据包中各单元的目的地ID*/
        PLAT_UINT32 ZC_GetDestID() const;  /*得到ZC发送数据包中各单元的目的地ID*/

        void init();
        PLAT_UINT32 dataType() const;
        bool hasMsgHeader() const;
        PLAT_UINT8 getByteData(int byteidx);
        void setByteData(int byteidx, PLAT_UINT8 val);
        PLAT_UINT32 getDstIDInternal() const;
private:
        PLAT_UINT32 m_fromID;
        PLAT_UBYTE* m_pHeader; //unit head address
        PLAT_UBYTE* m_pData; // msg header address if msg header exist
        T_UNIT_HEAD m_header;// the content has been change to little-endian
        PLAT_UINT32 m_idx; // its index in composited package (big package), start from 0
    PLAT_UBYTE* m_pHeaderParent;
};

class CZc
{
public:
        std::list<PLAT_UINT32 > getNotifyTerminals();
        bool updateNotifyTerminal( PLAT_UINT32 id, bool badd); 
private:
        std::list<PLAT_UINT32 > m_listTerminal;
};

#endif
