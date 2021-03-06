#include <stdio.h>
#include <stddef.h>
#include <vector>
#include <string.h>
#include <stdarg.h>

#ifndef  Q_WS_WIN
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "zsocket.h"
#include "INIReader.h"
#include "platformhead.h"
#include "cutility.h"
#include "zredis.h"


using namespace std;

#define ASSERT_SOCKID(id) {if(id <= 0) return false;}

 string toString(long long_address)
 {
    string ret;
    struct in_addr addr;
    addr.s_addr = long_address;
    char *dot_ip = inet_ntoa(addr);
    ret = string(dot_ip);
    return ret;        
 } 

 // buffer is little endian
 void ZSocket::outputLittlepack(const unsigned char * buffer)
 {
    PLAT_UINT32 size = CUtility::getLittlePackSize(buffer);
    //print out little package info
    plog("Now package: unitId = %08x, unitSize = %d, data:\n", 
            CUtility::getLittlePackUID(buffer),size);

    PLAT_UINT32 j = 0;
    for(j = 0;j < size; j++)
    {
            plog("%02x  ", buffer[j]);
    }
    plog("print out %d bytes for debug.\n",j);
 }

ZSocket::ZSocket() 
{
    memset((char *) &m_connectionList, 0, sizeof(m_connectionList));

    m_pRedis = NULL;
    m_bInit  = false;
    m_idListen = -1;
    m_strfileLog = "";

    if (!init())
    {
      plog("Failed: safe socket start socket!");
    }

}

ZSocket::~ZSocket()
{
    unInit();
    if ( m_pRedis )
    {
        delete m_pRedis;
        m_pRedis = NULL;
    }
}

bool ZSocket::init()
{
    //get from agent srcID
    INIReader reader("sid.config");
    if (reader.ParseError() < 0) {
        printf( "Can't load 'private.config'\n");
        return false ;
    }
    m_selfSID = reader.GetInteger("self","SID",0);

    m_bEnableLog = reader.GetInteger("self","enablelog",0) != 0 ? true : false;
     if ( m_bEnableLog )
    {
         std::string strlog = reader.Get("self","agentlog","");
         if (strlog.length()  != 0)
         {
             setLog(strlog.c_str());
             printf("Com board log is saved into file %s\n", strlog.c_str());
         }
    }

    CUtility::initBigPackIdx(m_dbBuf);

    TSafeStruct_CfgData pa ;
    memset(&pa, 0, sizeof(TSafeStruct_CfgData));
 
    bool ret = true;
    /* start safe socket */
    if (safeSocket_startSocket(&pa) == -1)
    {
            plog ("safe socket failed to be started!\n");
            ret = false;
    }
    m_bInit = ret;
    return ret;
}

bool ZSocket::unInit()
{
    if ( m_bInit )
    {
        safeSocket_stopSocket ();
        m_bInit = false;
    }
    return true;
}

int ZSocket::write(int id, const char* data, int byteRead, int flag)
{
    int ret = safeSocket_send (id, data, byteRead, flag);
    plog("Write to socket %d, result len = %d, given len = %d\n", id, ret, byteRead);
    return ret;
}

int ZSocket::read(int sockfd, void *buf, size_t len, int pflags)
{
    int ret = safeSocket_recv(sockfd, buf, len, pflags);
    plog("Read from socket %d, result len = %d, given len = %d\n", sockfd, ret, len);
    plog("Data from network: \n");

    PLAT_UBYTE* _p = (PLAT_UBYTE*) buf;
    int i = 0;
    for ( ;i < ret; i++)
    {
        plog("%02x  ",_p[i]);//(PLAT_UBYTE*)(buf + i));
    }
    plog("\nGet %d bytes from network. \n", i);
    return ret;
}
 
int ZSocket::connectServer(const char *ip,int port,int seconds)
{
    int sockid = create();
    if( sockid < 0)
    {
        plog("Failed: create sockid, now is =%d\n", sockid);
        return -1;
    }
    int ret = connect(sockid, port, ip,seconds);   

    TSafeSocket_FdSet  exceptSet;
    SS_FD_ZERO (&exceptSet);
    SS_FD_SET (sockid, &exceptSet);

    struct timeval timeout;
    timeout.tv_sec = seconds;//3s
    timeout.tv_usec = 0;
    
    int  errNumber ;
    if ( seconds > 0)
    {
        plog ("Wait connect result %d seconds\n", seconds);
        errNumber = safeSocket_select(0, NULL,NULL, &exceptSet,&timeout);
    }
    else
        errNumber = safeSocket_select (0, NULL,NULL, &exceptSet, NULL);

    if (errNumber == 0)
    {
        plog ("Error: select connection hasn't been established! ip = %s,port=%d\n",ip,port);
        return -1;
    }

    ret = -1;
    if (SS_FD_ISSET(sockid, &exceptSet))
            ret = sockid;
        
    return ret;

}

bool ZSocket::canWriteToDid(unsigned int did)
{
    bool ret = false;
    int sockid = getSockIDFromID(did);
    if ( sockid != -1)
    {
       ret = canWrite(sockid,3);
    }
    return ret;
}

void ZSocket::addMapLinkInfo(const int sockid, const unsigned int did,
    const unsigned int sid, const std::string ip)
{
    s_linkInfo info;
    memset(&info, 0, sizeof(s_linkInfo));
    info.did = did;
    info.sid = sid;
    info.ip = ip;

    m_mapSockId2Info.insert(pair<int,s_linkInfo>(sockid, info) );
    return;
}

void ZSocket::delMapLinkInfo(const int sockid)
{
    map<int,s_linkInfo>::iterator it = m_mapSockId2Info.find(sockid);
    if ( it != m_mapSockId2Info.end())
    {
        m_mapSockId2Info.erase(it);
    }
}

int ZSocket::connectTerminal(unsigned int did,int port,int timeout_nouse)
{
    if ( canWriteToDid(did) )
    {
        plog("Connection with %08x socket can write! Need not to connect %08x\n",did,did);
        plog("Terminal should not re-connnect when link is successful\n");
        return 1;
    }

    string ip = getIPFromID(did);
    if (ip.empty())
    {
        plog("Failed: Find ip from configure file for id=0x%0x.in ZSocket::connectTerminal \n",
            did);
        return -1;
    }

    int sockid = create();
    if( sockid < 0)
    {
        plog("Failed: create sockid, now is =%d\n", sockid);
        return -1;
    }

    plog("Info: connect server %s try!\n", ip.c_str());
    int ret = connect(sockid, port, ip.c_str(),timeout_nouse);

    addMapLinkInfo(sockid, did,m_selfSID, ip);

//for test

    // sockid = ret;
    //// add safeSock to except set 
    // TSafeSocket_FdSet  exceptSet;
    // SS_FD_ZERO (&exceptSet);
    // SS_FD_SET (sockid, &exceptSet);

    //int seconds = 160;
    // struct timeval timeout;
    // timeout.tv_sec = seconds;//3s
    // timeout.tv_usec = 0;
    // 
    // int  errNumber ;
    // plog("Now wait %d seconds for connect ---------- \n",seconds);
    // if ( seconds > 0)
    // {
    //     plog ("Wait connect result %d seconds\n", seconds);
    //     errNumber = safeSocket_select(0, NULL,NULL, &exceptSet,&timeout);
    // }
    // else
    //     errNumber = safeSocket_select (0, NULL,NULL, &exceptSet, NULL);
    //
    // if (errNumber == 0)
    // {
    //     plog ("Error: select connection hasn't been established! ip = %s,port=%d\n",ip,port);
    //     return -1;
    // }
    //
    // ret = -1;
    // if (SS_FD_ISSET(sockid, &exceptSet))
    //         ret = sockid;

//test end    

    if( ret != -1)
    {
        plog("Info: Add to monitor array for READ and CREATED ip=%s sockid = %d\n",
            ip.c_str(), sockid);
        long long_address = inet_addr ( ip.c_str());
        bool badd = addConnection(long_address, sockid);
        if ( !badd )
        {
            plog("Warning: try connect , but add monitor failed!!!! That is unnormal!\n");
        }
    }
    else
    {
        plog("Warning: connect server %s failed!\n", ip.c_str());
        return -1;
    }

    //debug network
     //safeSocket_getSockopt(int safeSock, int level, int optname, void* optval, socklen_t *optlen);
    //int d = 0;
    //int optval = 0;
    //safeSocket_getSockopt(sockid,SOL_SAFESOCKET,SS_LINK_STATUS, (void*) &optval, &d);
    //plog("---------- safeSocket_getSockopt optval = %d, optlen = %d\n", optval, d);

///////////////////////////////////////////
    //Now we do not do anthing about create connect for linkstate
    //plog("Set connect result is FAILED for temporarily, the value will be "
    //       "changed when Except_set is set later.\n");
    //PLAT_UBYTE pack[1024] = "\0" ;
    //initPackage(pack,did, 0,0);
    //updateDbBuffer(pack);
    ////CUtility::pushBackPack(m_dbBuf, pack);

    //plog("Write to DB value is in little endian\n");
    //outputLittlepack(pack);

    //unsigned char dbBuf[NETSIZE] = "\0";
    //memcpy(dbBuf, m_dbBuf, NETSIZE);
    //if (CUtility::needSwap())
    //{
    //    CUtility::bigPackToBE(dbBuf);
    //}
    //m_pRedis->app_set("linkstate", dbBuf);

    return ret;
}

//init connect state unit package
//type: 0: create connect
//      1: remove connect
//connectvalue: 1: successful
//          other: failed
void ZSocket::initPackage(unsigned char *  _ppack, PLAT_UINT did,
                                           int type, int connectvalue)
{
    typedef struct connectStateData
    {
        PLAT_UBYTE result;
        PLAT_UBYTE resver1[3];
        PLAT_UINT sid;
        PLAT_UINT did;
        PLAT_UINT  resver2[2];
    }tConStateData;

    T_UNIT* ppack = (T_UNIT*) _ppack;
    tConStateData _connectStateData;
    memset(&_connectStateData, 0, sizeof(tConStateData));

    ppack->unitSize = sizeof(tConStateData);
    ppack->unitData = ( PLAT_BYTE* )(_ppack + sizeof(PLAT_BYTE) * sizeof(T_UNIT_HEAD));

   static PLAT_UINT srcID = m_selfSID;
   
    char cv = 0xaa;
    if (connectvalue == 1) //connect successful
        cv = 0x55;

    _connectStateData.result = cv;
    _connectStateData.sid = srcID;
    _connectStateData.did = did;

    memcpy(ppack->unitData, (const char*)&_connectStateData, sizeof(tConStateData));

    switch (type)
    {
    case 0: //create connect
        {
            ppack->unitId = 0;
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //000110
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //0x23
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
            ppack->unitId = ppack->unitId;
        }
    break;
    case 1: //remove connect
    {
        ppack->unitId = 0;
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //000110
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //0x20
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff

        ppack->unitId = ppack->unitId;
    }
    break;
    default:
    break;
    }
}


void ZSocket::updateDbBuffer(unsigned char *  ppack)
{
    bool bfind = false;
    PLAT_UINT32 sid = CUtility::getLinkStateSID(ppack);
    
    PLAT_UINT64 id = PLATUNITID(sid,
            CUtility::getLinkStateDID(ppack));

    PLAT_UINT32 count = CUtility::getUnitCounts(m_dbBuf);
    for (int i = 0; i < count; i++)
    {
        PLAT_UBYTE * unit = CUtility::getUnitHead(m_dbBuf, i);

        //get link control did
        PLAT_UINT64 idsrc = PLATUNITID(CUtility::getLinkStateSID(unit),
            CUtility::getLinkStateDID(unit));
        if (idsrc == id)
        {
            //update the unit data
            CUtility::updateLittlePack((PLAT_UBYTE*)ppack, unit);
            bfind = true;
        }
    }

    if (!bfind)
        CUtility::pushBackPack(m_dbBuf, ppack);
}

int ZSocket::disconnectServer(char* ip)
{
    return -1;
}

int ZSocket::disconnectTerminal(int did)
{
    int bret = 0;
    int sockid = getSockIDFromID(did);
    string value = getIPFromID(did);

    plog("Try disconnect with %s connection.\n", value.c_str());
    if (sockid < 0)
        bret = -1;
    else
        bret = safeSocket_close(sockid);

    if( bret != -1)
    {
        if (value.empty())
        {
            plog("Failed: Find ip from configure file for id=0x%0x.in ZSocket::procDisconnectCtrl\n",
                   did);
            return -1;
        }
        long long_address = inet_addr (value.c_str()) ;
        bool bdel = delConnection(long_address);
        if ( !bdel )
        {
            plog("Warning: disconnect successful, but del monitor failed!!!! That is unnormal!\n");
        }

        delMapLinkInfo(sockid);       
    }
    else
    {
        plog("%0x has not connect agent, it need not disconnect\n", did);
    }

    PLAT_UBYTE pack[1024] = "\0" ;
    initPackage(pack,did, 1,1);
    updateDbBuffer(pack);
    
    plog("Update data info in DB for conect with %s for delete connection.\n", value.c_str());
    unsigned char dbBuf[NETSIZE] = "\0";
    memcpy(dbBuf, m_dbBuf, NETSIZE);
    if (CUtility::needSwap())
    {
        CUtility::bigPackToBE(dbBuf);
    }
    setLinkstate(dbBuf);
}

//int ZSocket::getWriteSocket(const char *ip,int port,int timeout)
//{
//    int sockid = create();
//    return connect(sockid, port, ip,timeout);   
//}

int ZSocket::create()
{
    int ret = -1;
   
    /* create safe socket */
    ret =  safeSocket_socket (SS_PROTOCOL_UDP, SS_WITHCONNECTION);
    if (ret == SS_FAILED)
    {
            plog ("Failed: safeSockID created!\n");
            return -1;
    }
    return ret;
}

bool ZSocket::bind(int _port, const char *ip )
{
    unsigned short port = _port;

    /* bind safeSockID to listen connection request from client */
   // SOCKADDR_IN   localAddr;
    struct sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port);
    if ( ip != NULL)
    {
        //localAddr.sin_addr.s_addr =  inet_addr ("127.0.0.1");
        localAddr.sin_addr.s_addr = inet_addr (ip);
    }
    else
    {
         localAddr.sin_addr.s_addr = INADDR_ANY; // inet_addr ("127.0.0.1");
    }

    if (safeSocket_bind (m_idListen, (const sockaddr*) & localAddr, sizeof (struct sockaddr_in)) 
        == SS_FAILED)
    {
        int errNumber = safeSocket_getErrorNum (m_idListen);
        plog ("bind error, errNumber: %u\n", errNumber);
        return false;
    }

    plog("bind finished port=%d, listenid = %d \n", port, m_idListen);
    return true;

}

bool ZSocket::listen()
{
    /* set safeSockID to listen state */
    //if (safeSocket_listen (m_idListen, MAX_CNT_COUNT) == SS_FAILED) // the parameter value 3 means?
    if (safeSocket_listen (m_idListen, 3) == SS_FAILED) // the parameter value 3 means?
    {
            int errNumber = safeSocket_getErrorNum (m_idListen);
            plog ("Failed: socket to be set to listen state, errNumber: %u\n", errNumber);
            return false;
    }
    return true;
}

//seconds is not used for now
int ZSocket::connect(int sockid, int port, const char *ip, int seconds)
{
    if( strlen(ip) <= 0)
    {
        plog("ip address is empty\n");
        return -1;
    }
    
        /* bind safeSockID to listen connection request from client */
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(struct sockaddr_in));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port);
    localAddr.sin_addr.s_addr = inet_addr (ip);
   
    /* connect to server */
    plog ("Connect ip %s\n", ip);
    if (safeSocket_connect (sockid, (const struct sockaddr*) & localAddr, sizeof (sockaddr_in)) 
              == SS_FAILED)
    {
       int errNumber = safeSocket_getErrorNum (sockid);
       plog ("connect failed, errNumber = %u\n", errNumber);
       
        #ifdef Q_WS_WIN
        closesocket (sockid);
        #else
        close(sockid);
        #endif
        
       return -1;
    }

    return sockid;
    
    //for now do not test fd_set, that move to procLoop select
     // add safeSock to except set 
    //TSafeSocket_FdSet  exceptSet;
    //SS_FD_ZERO (&exceptSet);
    //SS_FD_SET (sockid, &exceptSet);

    //struct timeval timeout;
    //timeout.tv_sec = seconds;//3s
    //timeout.tv_usec = 0;
    //
    //int  errNumber ;
    //if ( seconds > 0)
    //{
    //    plog ("Wait connect result %d seconds\n", seconds);
    //    errNumber = safeSocket_select(0, NULL,NULL, &exceptSet,&timeout);
    //}
    //else
    //    errNumber = safeSocket_select (0, NULL,NULL, &exceptSet, NULL);

    //if (errNumber == 0)
    //{
    //    plog ("Error: select connection hasn't been established! ip = %s,port=%d\n",ip,port);
    //    return -1;
    //}

    //int ret = -1;
    //if (SS_FD_ISSET(sockid, &exceptSet))
    //        ret = sockid;
    //    
    //return ret;
}

 void ZSocket::preSockSet()
 {
    m_maxSetNums = 0;
    SS_FD_ZERO (&m_excepSet);
    SS_FD_ZERO(&m_readSet);

    SS_FD_SET (m_idListen, &m_excepSet);
    for (int listnum = 0; listnum < MAX_CNT_COUNT; listnum++)
    {
            if (m_connectionList[listnum] != 0) 
            {
                    SS_FD_SET(m_connectionList[listnum],&m_readSet);
                    SS_FD_SET (m_connectionList[listnum], &m_excepSet);
                    m_maxSetNums++;
            }
    }
 }

bool ZSocket::initServer(int port)
{
    m_idListen = create(); 

    if ( m_idListen == -1 || !bind(port) || !listen() )
    {
        return false;
    }
    return true;
}

 bool ZSocket::delConnection(long long_address)
{
    bool bret = false;
    map<long,int>::iterator it = m_mapLongIP2SockId.find(long_address);
    if ( it != m_mapLongIP2SockId.end())
    {
        int sockid = it->second;
        //update connection list
        for (int i = 0; i < MAX_CNT_COUNT; i ++)
        {
            if (m_connectionList[i] == sockid)
            {
                m_connectionList[i] = 0;
                break;
            }
        }
        //erase from map
        m_mapLongIP2SockId.erase(it);
        bret = true;
    }
    else
    {
        plog("Found Failed: ip %0x not in map.\n", long_address);
    }
    return bret;
}

bool ZSocket::addConnection(long longip,int sockid )
{
    bool bret = false;
    for (int i = 0; i < MAX_CNT_COUNT; i ++)
    {
        if (m_connectionList[i] == 0) 
        {
            m_connectionList[i] = sockid;
            m_mapLongIP2SockId.insert(pair<long,int>(longip, sockid) );
              
            plog("Connection: add tomap: ip2sockid, sockid =%d, ip=%s,index=%d\n", 
                sockid, toString(longip).c_str(),i);
            bret = true;
            break;
         }
    }
    return bret;
}

void ZSocket::handleNewConnection() 
{
    struct sockaddr_in clientAddr;
#ifndef Q_WS_WIN
    socklen_t clientAddrLen;
#else
    int       clientAddrLen;
#endif

    /* Socket file descriptor for incoming connections */
    int connection = safeSocket_accept(m_idListen,(struct sockaddr*) &clientAddr, & clientAddrLen);
    if (connection < 0) 
    {
        plog("Error: accept listenid=%d\n",m_idListen);
        return;
    }

    //update linkstae
    std::string ip = std::string(inet_ntoa(clientAddr.sin_addr));
    PLAT_UINT did = getIDFromIP(ip);
    addMapLinkInfo(connection, did,m_selfSID, ip);

    PLAT_UBYTE pack[1024] = "\0" ;
    initPackage(pack,did, 0,1);
    updateDbBuffer(pack);
    
    plog("Update data info in DB for conect with %s for accept connection.\n", ip.c_str());
    unsigned char dbBuf[NETSIZE] = "\0";
    memcpy(dbBuf, m_dbBuf, NETSIZE);
    if (CUtility::needSwap())
    {
        CUtility::bigPackToBE(dbBuf);
    }
    setLinkstate(dbBuf);
    //end of update linkstate

        
    bool badd =  addConnection(clientAddr.sin_addr.s_addr, connection);
    if (!badd) 
    {
        /* No room left in the queue! */
        plog("\nNo room left for new client.\n");
        char data[102] = "Sorry, this server is too busy. Try again later!\n";
        write (connection, data, strlen(data) , 0);
                
        //close socket
        safeSocket_close (connection);
    }
}

int ZSocket::connectDb()
{
    m_pRedis = new ZRedis();
    
    INIReader reader("sid.config");
    if (reader.ParseError() < 0) {
        plog( "Can't load 'sid.config'\n");
        return false;
    }
    std::string value = reader.Get("self","dbip","127.0.0.1");

    plog("Address: database = %s \n",
        value.c_str());

//    int ret = m_pRedis->connect_test(value.c_str());// const char *host);
    return m_pRedis->connect(value.c_str());
    
}



bool ZSocket::canWrite(int sockId,int microseconds )
{
    ASSERT_SOCKID(sockId);
    bool bret = false;
    TSafeSocket_FdSet set;
    SS_FD_ZERO (&set);
    SS_FD_SET (sockId, &set);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = microseconds;
    
    int  socks ;
    if ( microseconds > 0)
        socks = safeSocket_select (0, NULL, &set, NULL,  &timeout);
    else
        socks = safeSocket_select (0, NULL, &set, NULL,  NULL);
    if (socks > 0)
        bret = SS_FD_ISSET(sockId,& set);
    return bret;
}

bool ZSocket::canRead(int sockId,int microseconds )
{
    ASSERT_SOCKID(sockId);
    bool bret = false;
    TSafeSocket_FdSet set;
    SS_FD_ZERO (&set);
    SS_FD_SET (sockId, &set);
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = microseconds;
   
    int  socks ;
    if ( microseconds > 0)
        socks = safeSocket_select (0, &set, NULL, NULL,  &timeout);
    else
        socks = safeSocket_select (0, &set, NULL, NULL,  NULL);
    
    if (socks > 0)
        bret = SS_FD_ISSET(sockId,& set);
    return bret;
}

//get did of ip in link map
PLAT_UINT ZSocket::getIDFromIP(std::string ip)
{
    //ip->DID
    //PLAT_UINT value = 0;
    //map<int,s_linkInfo>::iterator it = m_mapSockId2Info.begin();
    //for(; it != m_mapSockId2Info.end(); it++)
    //{
    //    s_linkInfo st = it->second;
    //    if (st.ip == ip)
    //        value = st.did;
    //}

    ////ip->DID
    ZINIReader reader("sid.config");

    if (reader.ParseError() < 0) {
        plog( "Can't load 'sid.config'\n");
        return 0;
    }
    PLAT_UINT value = reader.GetIDFromIp( "ip",ip, 0);

    if ( CUtility::isCCType(value))
    {
        value = CUtility::getCCID(value);
    }
    plog("Get IP from ID in configure file, id = %08x, for ip = %s\n",value, ip );
    return value;
}

string ZSocket::getIPFromID(int id)
{
    char key[48];
    memset(key, 0 ,48);
    key[0]='0';
    key[1]='x';
    sprintf(key+2, "%08x", id);

    string value = "";

    //id->longip
    INIReader reader("sid.config");

    if (reader.ParseError() < 0) {
        plog( "Can't load 'sid.config'\n");
        return "";
    }
    value = reader.Get("ip",key,"");
    //plog("Get IP from ID in configure file, id = %s, ip = %s\n", key, value.c_str());

    return value; 
}

PLAT_UINT ZSocket::getIDFromSockID(int sockId)
{
     //sockid->DID
    PLAT_UINT value = 0;
    map<int,s_linkInfo>::iterator it = m_mapSockId2Info.find(sockId);
    if (it != m_mapSockId2Info.end())
    {
        s_linkInfo st = it->second;
        value = st.did;
    }

    return value;
    //std::string ip = getIPFromSockID(sockId);
    //return getIDFromIP(ip);    
}

std::string ZSocket::getIPFromSockID(int sockid)
{
    string ret ="";
    map<long,int>::iterator it;
    for(it = m_mapLongIP2SockId.begin(); it != m_mapLongIP2SockId.end(); it++)
    {
        if (sockid == it->second )
        {
            ret = toString(it->first);
            break;
        }
    }
    return ret;   
}

int ZSocket::getSockIDFromID(int id)
{
    int ret = -1;
    //id->longip
    string value = getIPFromID(id);
    plog("Info: get IP=%s, did=%0x\n", value.c_str(), id);

    long long_address = inet_addr (value.c_str()) ;

    //longip->sockid
    map<long,int>::iterator it;
    it = m_mapLongIP2SockId.find(long_address);
    if ( it != m_mapLongIP2SockId.end())
    {
        ret = it->second;
    }

    if ( ret == -1)
    {
       plog("Failed: Get socketID from ID, Now IP2sockid Map:\n");
       for( it = m_mapLongIP2SockId.begin(); it != m_mapLongIP2SockId.end(); ++it)
       {
          plog("%s-%d  ", toString(it->first).c_str(), it->second);
       }
       plog("\n"); 
    }
    return ret;
}

void ZSocket::initIDIP()
{
   //std::vector<CIniFile::Record> CIniFile::GetSection("ip", "ip.config");
   //  std::map<int,long> m_mapID2LongIP;
   // std::map<int,std::string> m_mapID2IP;
}

bool ZSocket::isFromSelf(const char * buffer)
{
    char self[] = "self";

    char src[64] ="\0";
    memcpy(src, buffer, strlen(self));

    return string(src) != string(self) ? false : true;
}

//data input is DID
int ZSocket::sendConnect(int sockid,const char* data, int size, int flag)
{
    return 0;
//    char buf[64] = "\0";
//    static string strdst = "selfconnect";
//
//    if ( size + strdst.length() >= 64 ) 
//    {
//        plog("Warning: message content between self and agent size too large!!!");
//        exit(-1);
//    }
//    memcpy(buf, strdst.c_str(), strdst.length());
//    memcpy(buf + strdst.length(), data, size);
//
//    return write(sockid, buf, strlen(buf), flag);
    //return write(sockid, buf, strdst.length() + size, flag);
}
//data input is DID
int ZSocket::sendDisConnect(int sockid, const char* data, int size, int flag)
{
    return 0;
//    char buf[64] = "\0";
//    static string strdst = "selfdisconnect";
//    if ( size + strdst.length() >= 64 ) 
//    {
//        plog("Warning: message content between self and agent size too large!!!");
//        exit(-1);
//    }
//    memcpy(buf, strdst.c_str(), strdst.length());
//    memcpy(buf + strdst.length(), data, size);
//
//    return write(sockid, buf, strdst.length() + size, flag);
}

//data input is littlepack content
int ZSocket::sendTransfer(int sockid,const char* data, int size, int flag)
{
     return 0;

    //char buf[64+SIZE] = "\0";
    //static string strdst = "selftransfer";

    //memcpy(buf, strdst.c_str(), strdst.length());
    //memcpy(buf + strdst.length(), data, size);

    //plog("Want to transfer %d data at original style selftransfer\n",  strdst.length() + size);
    //for(PLAT_UINT32 j = 0;j < size; j++)
    //{
    //        plog("%02x  ", data[j]);
    //}
    //plog("\n");

    //return write(sockid, buf, strdst.length() + size, flag);
    
}

//buffer content is "selfconnect#ID.."
 bool ZSocket::isConnectCtrl(const char * buffer)
{
   static string strdst = "connect";
   char src[24];// = "connect";
   memset(src, 0, 24);
   memcpy(src, buffer + 4, strdst.length());

   string strsrc = string(src);
   return strsrc != strdst ? false : true;
}

bool ZSocket::isDisconnectCtrl(const char * buffer)
{
   static string strdst = "disconnect";
   char src[24];// = "connect";
   memset(src, 0, 24);
   memcpy(src, buffer + 4, strdst.length());

   string strsrc = string(src);
   return strsrc != strdst ? false : true;
}

bool ZSocket::isTransferCtrl(const char * buffer)
{
   static string strdst = "transfer";
   char src[24];// = "connect";
   memset(src, 0, 24);
   memcpy(src, buffer + 4, strdst.length());

   string strsrc = string(src);
   return strsrc != strdst ? false : true;
}

//int connect1025Server(int port,int seconds)
//{
//        //TSafeStruct_CfgData pa ;
//        //memset(&pa, 0, sizeof(TSafeStruct_CfgData));
//        //if (safeSocket_startSocket( &pa ) == FALSE)
//        //{
//        //plog ("safe socket failed to be started!\n");
//        //        return 0;
//        //}
//    /* create safe socket */
//        int safeSockID = safeSocket_socket (SS_PROTOCOL_UDP, SS_WITHCONNECTION);
//        if (safeSockID == SS_FAILED)
//        {
//                plog ("safeSockID failed to be created!\n");
//                return 0;
//        }
//
//        struct sockaddr_in  serverAddr;
//        serverAddr.sin_family = AF_INET;
//        serverAddr.sin_port = htons(port);
//        //serverAddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
//        serverAddr.sin_addr.s_addr = inet_addr ("192.168.20.58");
//
//        /* add safeSock to except set */
//        TSafeSocket_FdSet  exceptSet,readSet, writeSet;
//        SS_FD_ZERO (& exceptSet);
//    SS_FD_ZERO (& readSet);
//    SS_FD_ZERO (& writeSet);
//        SS_FD_SET (safeSockID, & exceptSet);
//    SS_FD_SET (safeSockID, & readSet);
//    SS_FD_SET (safeSockID, & writeSet);
//
//        /* connect to server */
//        if (safeSocket_connect (safeSockID, (const sockaddr*) & serverAddr, sizeof (struct sockaddr_in)) == SS_FAILED)
//        {
//           int errNumber = safeSocket_getErrorNum (safeSockID);
//           plog("connect failed, errNumber = %u\n", errNumber);
//
//           return 0;
//        }
//
//     struct timeval timeout;
//    timeout.tv_sec = seconds;//3s
//    timeout.tv_usec = 0;
//    
//    int  errNumber ;
//    if ( seconds > 0)
//        errNumber = safeSocket_select(0, &readSet,&writeSet, &exceptSet,&timeout);
//    else
//        errNumber = safeSocket_select (0, &readSet,&writeSet, &exceptSet, NULL);
//
//        ///* wait until connection has established */
//        //int errNumber = safeSocket_select (0, NULL, NULL, & exceptSet, NULL);
//        if (errNumber == 0)
//        {
//             int errNumber = safeSocket_getErrorNum (safeSockID);
//             plog ("select error: connection hasn't been established! errNumber =  %d\n", errNumber);
//        }
//
//        if( SS_FD_ISSET(safeSockID,&readSet))
//        {
//            plog("read set is set\n");
//        }
//        if( SS_FD_ISSET(safeSockID,&writeSet))
//        {
//            plog("writeSet set is set\n");
//        }
//        if ( SS_FD_ISSET(safeSockID,&exceptSet))
//        {
//             plog("except set is set\n");
//        }
//        if ( errNumber == 0)
//        {
//            plog("getchar to exit connect1025Server\n");
//            getchar();
//            return 0;
//        }
//    return errNumber;
//    return 0;
//}

//buffer content is "selfconnect#ID.."
//int ZSocket::procConnectCtrl(const char * buffer)
//{
    //bool badd = false;
    //int sockid = 0;
    //char msghead[] = "selfconnect";
    //unsigned int did = 0; 
    //sscanf(buffer + strlen(msghead),"%d", &did);
    //
    //sockid = connectTerminal(did,1086,163);  
    //return sockid;
//}

//int ZSocket::procDisconnectCtrl(const char * buffer)
//{
//    int bdel, bret;
//    char msghead[] = "selfdisconnect";
//    unsigned int did = 0;
//    sscanf(buffer + strlen(msghead),"%d", &did);
//
//    bret = disconnectTerminal(did);
//  
//    return bret;
//}

//buffer content is  littlepackage, buffer is little endian
int ZSocket::procTransferCtrl(unsigned char * buffer)
{
     //the data format is : "selftransfer"  + littlepack content
    unsigned int did = 0;
    int byteSent = -1;

    plog("Recevied data from self terminal ");
    s_packinfo pack;
    getPackinfo(pack,(unsigned char*)buffer);

    //pop form db and send out
    char popOutBuf[SIZE_L_MAX] = "\0";
    //if(1)
        //only just for pop it,
        //popOutBuf is not used for now
      //   m_pRedis->app_lpop(pack.did,(unsigned char*)popOutBuf);
    //else
    //{
    //    //another method
    //    memcpy(popOutBuf, buffer, pack.size);
    //    //write out the popOut2 without db support
    //}

    int sockidTo = getSockIDFromID(pack.did);
    //if (sockidTo > 0 && canWrite(sockidTo,3))
    if (sockidTo > 0 ) // for now do not judge
    {
        if (CUtility::needSwap())
        {
            CUtility::littlePackToBE(buffer);
        }

        //Only then write raw data to out, need skip 
        //unit head and messsage head and
        int skipCount = sizeof(T_MESSAGE_HEAD) + sizeof(T_UNIT_HEAD);
        byteSent = write (sockidTo,(const char*) buffer + skipCount,
            pack.size - skipCount, 0);
        if (byteSent == SS_FAILED)
        {
            int errNumber = safeSocket_getErrorNum (sockidTo);
            plog ("Failed: data to be sent, errNumber = %u\n", errNumber);
            return byteSent;
        }
        // We got some data
        plog("write to sockid %d, sid=%0x, did = %08x write out %d bytes \n",
                sockidTo,pack.sid, pack.did, byteSent);     
      
    }
    else
    {
        plog("Error: In procTransferCtrl, did = %08x, sockidTo = %d, do not write out!\n",
              pack.did, sockidTo);
    }
    return byteSent;
}

//buffer content is "selftransfer" + littlepackage
//int ZSocket::procTransferCtrl(const char * buffer)
//{
//     //the data format is : "selftransfer"  + littlepack content
//    char msghead[] = "selftransfer";
//    unsigned int did = 0;
//    int byteSent = -1;
//
//    char msgLittleBuf[SIZE] = "\0";
//    memcpy(msgLittleBuf, buffer + strlen(msghead),SIZE);
//    
//    char dd[22] = "\0";
//    memcpy(dd, buffer, strlen(msghead));
//    //plog("data: %s,", dd);
//
//    plog("Recevied data from self terminal ");
//    s_packinfo pack;
//    getPackinfo(pack,msgLittleBuf);
//
//    //pop form db and send out
//    char popOutBuf[SIZE] = "\0";
//    if(1)
//         m_pRedis->app_lpop(pack.did,(unsigned char*)popOutBuf);
//    else
//    {
//        //another method
//        memcpy(popOutBuf, buffer + sizeof(msghead), pack.size);
//        //write out the popOut2 without db support
//    }
//
//    int sockidTo = getSockIDFromID(pack.did);
//    if (sockidTo > 0 && canWrite(sockidTo,3))
//    {
//        byteSent = write (sockidTo, msgLittleBuf, pack.size, 0);
//        if (byteSent == SS_FAILED)
//        {
//            int errNumber = safeSocket_getErrorNum (sockidTo);
//            plog ("Failed: data to be sent, errNumber = %u\n", errNumber);
//            return byteSent;
//        }
//        // We got some data
//        plog("write to sockid %d, sid=%0x, write out %d bytes \n",sockidTo,pack.sid, byteSent);     
//      
//    }
//    else
//    {
//        plog("Error: In procTransferCtrl, did = %08x, sockidTo = %d, do not write out!\n",
//              pack.did, sockidTo);
//    }
//    return byteSent;
//}           

//init connect state unit package
void ZSocket::initConnectState(const char*  _ppack, const unsigned int did,
   const unsigned int sid, int type, int connectvalue)
{
    typedef struct connectStateData
    {
        PLAT_UBYTE result;
        PLAT_UBYTE resver1[3];
        PLAT_UINT sid;
        PLAT_UINT did;
        PLAT_UINT  resver2[2];
    }tConStateData;

    T_UNIT* ppack = (T_UNIT*) _ppack;
    tConStateData _connectStateData;
    memset(&_connectStateData, 0, sizeof(tConStateData));

    ppack->unitSize = sizeof(tConStateData);
    ppack->unitData = ( PLAT_BYTE* )(_ppack + sizeof(PLAT_BYTE) * sizeof(T_UNIT_HEAD));

    char cv = 0xaa;
    if (connectvalue == 1) //connect successful
        cv = 0x55;

    _connectStateData.result = cv;
    _connectStateData.sid = sid;
    _connectStateData.did = did;

    memcpy(ppack->unitData, (const char*)&_connectStateData, sizeof(tConStateData));

    switch (type)
    {
    case 0: //create connect
        {
            ppack->unitId = 0;
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //000110
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //cv
            ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff
            ppack->unitId = ppack->unitId;
        }
    break;
    case 1: //remove connect
    {
        ppack->unitId = 0;
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 30, 31,1); //01
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 29, 24,6); //000110
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 23, 16,cv); //cv
        ppack->unitId = CUtility::setBitsVal(ppack->unitId, 0, 15,0xff); //0xff

        ppack->unitId = ppack->unitId;
    }
    break;
    default:
    break;
    }
}

//the _buffer is little endian sequence
void ZSocket::getPackinfo(s_packinfo& packinfo, unsigned char* _buffer)
{
    unsigned char* buffer = (unsigned char*)_buffer;

    //print out little package info
    outputLittlepack((const unsigned char*)buffer);

    packinfo.datatype = CUtility::getLittlePackDataType(buffer);
    packinfo.sid = CUtility::getLittlePackSID(buffer);
    packinfo.did = CUtility::getLittlePackDID(buffer);
    packinfo.size = CUtility::getLittlePackSize(buffer);
        
    plog("Little pack info in socket: datatype=%0x (16), sid= 0x%0x, did=0x%0x, size = %d\n",
         packinfo.datatype, packinfo.sid,packinfo.did,packinfo.size);
}

void ZSocket::updateConnectResult(int listnum, int result)
{
    PLAT_UBYTE pack[1024] = "\0";

    int sockId = m_connectionList[listnum];

    PLAT_UINT dstID = getIDFromSockID(sockId);

    initPackage(pack,dstID, 0,result);//default 1, successful
    updateDbBuffer(pack);
    
    plog("link state connect succesful will be update to key linkstate in DB,"
        "its content is:\n");
    outputLittlepack(pack);

    unsigned char dbBuf[NETSIZE] = "\0";
    memcpy(dbBuf, m_dbBuf, NETSIZE);
    if (CUtility::needSwap())
    {
        CUtility::bigPackToBE(dbBuf);
    }

    setLinkstate(dbBuf);
    //m_pRedis->app_set("linkstate", dbBuf);
}

void ZSocket::setLinkstate(const unsigned char * dbBuf)
{
    static char key[100] = "\0";
    sprintf(key, "%08xlinkstate",m_selfSID); 
    if ( CUtility::isCCID(m_selfSID) )
    {
        sprintf(key, "%08xlinkstate",CUtility::getAtoIDFromCC(m_selfSID) ); 
        m_pRedis->app_set(key, dbBuf);
        sprintf(key, "%08xlinkstate",CUtility::getAtpIDFromCC(m_selfSID) ); 
        m_pRedis->app_set(key, dbBuf);
    }
    else
        m_pRedis->app_set(key, dbBuf);
}

// listnum: Current item in connectlist for for loops
void ZSocket::dealWithData(     int listnum     )
{
    //char self[] = "selftransfer";
    PLAT_UBYTE buffer[SIZE_L_MAX] = "\0";    //12 is "selftransfer" length
    char *cur_char;      /* Used in processing buffer */

    int sockId = m_connectionList[listnum];
    
    int      flag = 0;
    //long     t1 = GetTickCount();
    int      recvlen = read(sockId,buffer,SIZE_L_MAX,flag);
    //long     t2 = GetTickCount();
    //plog("read sockid = %d, len_result = %d data cost %d ms \n",
    //    sockId,recvlen, t2 - t1);

    if ( recvlen < 0) 
    {
        // Connection closed, close this end and free up entry in connectlist 
        plog("recv return %d < 0 , getLastErro = %d "
             "Connection lost: FD=%d;  Slot=%d, close the socket, ip = %s\n",
             recvlen, safeSocket_getErrorNum (sockId),
             sockId,listnum,getIPFromSockID(sockId).c_str());
        plog("Now do nothing, maybe should close the socket\n");
        //safeSocket_close(sockId);
        // m_connectionList[listnum] = 0;
    } 
    else
    {
        plog("Recv data lengh %d that must be communication message!"
            "Now only process this type message\n",recvlen );
        //get content
       
        plog("Message from other: true, just push it after add header\n");
        //if (CUtility::needSwap())
        //{
        //    CUtility::littlePackToLE(buffer);
        //}
       //from other terminal to me. push back
       //now it is communication msg with raw data
       //that has no message header and unit header
       // Add unit header and message header to received data.
        PLAT_UINT32 sid = getIDFromSockID(sockId);
        PLAT_UINT32 did = m_selfSID;

        //init message header and unit header
        typedef union _t_uid{
          struct {
                    PLAT_UINT16 dataid;
                    PLAT_UBYTE datainfo;
                    PLAT_UBYTE datasource_type;
                }S_uid_dec;
                PLAT_UINT32 uid;
        } U_uid;

        U_uid Uuid;
        memset(&Uuid, 0, sizeof(U_uid));
        //generate uid
        static PLAT_UBYTE index = 0x0;
        Uuid.S_uid_dec.datasource_type = 0x85;
        index ++;
        if (index > 0xff )
        {
            index = 0x1;
        }
        Uuid.S_uid_dec.datainfo = index;//[1-255]
        Uuid.S_uid_dec.dataid = 0x0000;

        T_UNIT_HEAD uheader;
        memset(&uheader, 0, sizeof(T_UNIT_HEAD));
        T_MESSAGE_HEAD mheader;
        memset(&mheader, 0, sizeof(T_MESSAGE_HEAD));
        
        //generate unit header
        uheader.unitId = Uuid.uid;
        uheader.unitSize = recvlen + sizeof(T_MESSAGE_HEAD);

        //generate message header
        mheader.sequenceNum = 0;
        mheader.timeStamp = 0;
        mheader.SID = sid;
        mheader.DID = did;
        mheader.msgType = 0;
        mheader.msgLen = recvlen;

        PLAT_UBYTE bufferAdded[SIZE_L_MAX] = "\0";
        memcpy(bufferAdded, &uheader, sizeof(T_UNIT_HEAD));
        memcpy(bufferAdded + sizeof(T_UNIT_HEAD), &mheader, sizeof(T_MESSAGE_HEAD));
        memcpy(bufferAdded + sizeof(T_UNIT_HEAD) + sizeof(T_MESSAGE_HEAD),
            &buffer,recvlen);
        
        s_packinfo pack;
        getPackinfo(pack,bufferAdded);

        //restore it for save into db
        if (CUtility::needSwap())
        {
            CUtility::littlePackToBE(bufferAdded);
        }

        //the data is from other terminals, push it to db
        if ( CUtility::isCCID(pack.did))
        {
            plog("Message from other to CC terminal\n");
              
            m_pRedis->app_rpush( CUtility::getAtpIDFromCC(pack.did ) ,
                                (const unsigned char*) bufferAdded);//atp
            m_pRedis->app_rpush( CUtility::getAtoIDFromCC(pack.did ),
                                (const unsigned char*) bufferAdded);//ato
        }
        else
            m_pRedis->app_rpush(pack.did, (const unsigned char*) bufferAdded);
       
  }//safeSocket_recv else end
}

bool ZSocket::needMonitor(int sockid)
{
   string ip = getIPFromSockID(sockid);
   long long_address = inet_addr ( ip.c_str());

   return m_mapLongIP2SockId.count(long_address) ? true : false;
}

void ZSocket::readSocks()
{
    if (SS_FD_ISSET(m_idListen,&m_excepSet))
    {
        plog("\nNew connection create with me\n");
         //check our "listening" socket
         handleNewConnection();
    }
    else
    {
        for (int i = 0; i < MAX_CNT_COUNT; i++) 
        {
            if (m_connectionList[i] >0 &&
                SS_FD_ISSET(m_connectionList[i],&m_excepSet))
            {
                //if ( canWrite(m_connectionList[i]))
                //if create connect occure
                plog("Get exception SET be set ip=%s socketID=%d\n",
                    getIPFromSockID(m_connectionList[i]).c_str(),m_connectionList[i] );
                if ( needMonitor(m_connectionList[i]) )
                {
                    plog("\nNeed Monitor, change link state in DB\n"
                        "Processs socket %d, ip =%s do sockect connect successful\n", 
                    m_connectionList[i],getIPFromSockID(m_connectionList[i]).c_str());
                    updateConnectResult(i, 1);
                }
                //if delete connection occure
                //do nothing
            }
        } 
   
        for (int i = 0; i < MAX_CNT_COUNT; i++) 
        {
            if (m_connectionList[i] >0 &&
                SS_FD_ISSET(m_connectionList[i],&m_readSet))
            {
                //plog("\nProcesss socket %d, ip =%s do dealWithData\n", 
                //    m_connectionList[i],getIPFromSockID(m_connectionList[i]).c_str());

                 long   t1 = GetTickCount();
                 dealWithData(i);
                 long   t2 = GetTickCount();
                 plog("dealWithData sockid = %d, ip = %s data cost %d ms \n",
                     m_connectionList[i],getIPFromSockID(m_connectionList[i]).c_str(), t2 - t1);
                
            }
        } 
    }
}

void ZSocket::procSelfDataInternal(const char* key)
{
    long  t1 = GetTickCount();
    long  t2 = GetTickCount();
    int len = m_pRedis->app_llen(key);
    if (len > 0)
    {
        t2 = GetTickCount();
        printf("xxx app_llen , time = %d - %ld ms. t2= %ld  t1=%ld key = %s\n",
                t2 - t1, t2 - t1,t2, t1,key);
    }

     for(int j =0;j <len;j++)
     {
         PLAT_UBYTE unitBuf[SIZE_L_MAX] = "\0";
         m_pRedis->app_lpop(key, unitBuf);
         if(CUtility::needSwap())
         {
             //big endian to little endian for little package
             CUtility::littlePackToLE(unitBuf);
         }

         plog("Sele key content is: %s in DB\n",key);
         //outputLittlepack(unitBuf);

         PLAT_UINT32 datatype =  CUtility::getLittlePackDataType(unitBuf);
         switch (datatype)
         {
            case 0x0a://msg out data
            case 0x05://msg in data
            {
               plog("Msg out / in data\n");
               plog("Self want to transfer data: true ----------------------------\n");
               int len = procTransferCtrl((unsigned char*)unitBuf);
               plog("procTransferCtrl data: length = %d\n", len);
            }
            break;
            case  0xB:
            {
                 plog("Msg link control data\n");
                 if ( CUtility::isConnectCmdLinkState(unitBuf) )
                 {
                    plog("Msg link control data - create connection.\n");
                    plog("Create connect %08x try :\n",CUtility::getLinkStateDID(unitBuf));
                    int retsockid  = connectTerminal( CUtility::getLinkStateDID(unitBuf), 1086,163); //163 ms
                    // string str = retsockid != -1 ? "true" : "false";
                    // plog("Create connect %08x try : "
                    // " connect return value = %d\n", CUtility::getLinkStateDID(unitBuf), retsockid);
                    
                 }
                 else
                 {
                    plog("Msg link control data - remvoe connection.\n");
                    plog("Delete connect  %08x try :\n",CUtility::getLinkStateDID(unitBuf));
                     int retsockid  = disconnectTerminal( CUtility::getLinkStateDID(unitBuf));
                    // string str = retsockid != -1 ? "true" : "false";
                    // plog("Delete connect  %08x try : disconnect result = %s\n", 
                    //         CUtility::getLinkStateDID(unitBuf), str.c_str());
                 }
            }
            break;
           default:
               {
                 plog("Warning: Should not read the type data in selfSID key in db. datatype = %d\n",
                                      datatype );
                 plog("The little pack uid is %08x", CUtility::getLittlePackUID(unitBuf));
               }
         }    
     }

     //debug
     if (len > 0)
     {
        t2 = GetTickCount();
        printf("xxxProc-self data keys len = %d in db , time = %d - %ld ms. t2= %ld  t1=%ld\n",
            len, t2 - t1, t2 - t1,t2, t1);
     }
     
}

void ZSocket::procSelfData()
{
     static  unsigned int selfID = m_selfSID;
     static bool b = false;
     static char selfkey[32] = "\0";
     
     if ( CUtility::isCCID(selfID))
     {    
           sprintf(selfkey,"%s%08x", "self", CUtility::getAtpIDFromCC(selfID));
           procSelfDataInternal(selfkey);
           if ( !b ) plog("selfkey is %s\n", selfkey);
           
           sprintf(selfkey,"%s%08x", "self", CUtility::getAtoIDFromCC(selfID));
           procSelfDataInternal(selfkey);
           if ( !b )  { plog("selfkey is %s\n", selfkey); b = true;}
     }
     else
     {
        sprintf(selfkey,"%s%08x", "self", selfID);

        long t1  = GetTickCount();
        procSelfDataInternal(selfkey);
        long   t2 = GetTickCount();
        if ( t2 - t1 != 0)
        {
            plog("procSelfDataInternal in procSelfData() %d - %ld ms (link or transfer) t2= %ld  t1=%ld\n", 
                t2 - t1,t2 - t1,t2,t1);
        }

        
         
         if ( !b )  { plog("selfkey is %s\n", selfkey); b = true;}
     }       

}

void ZSocket::procLoop(int timeoutSeconds)
{
    struct timeval timeout;
    if( timeoutSeconds >= 0)
    {
         //using default value, ignore parameter
         timeout.tv_sec = 0;
         timeout.tv_usec = 15000;//15ms
    }
    

    plog("Timeout interval in procLoop %d mircroseconds\n", timeout.tv_usec/1000);
    bool bneed = true;
    while (1) 
    {
        if ( bneed )
        {
            printf("**** start loop ****\n");
            plog("**** start loop ****\n");
            bneed = false;
        }
        long t1  = GetTickCount();
        procSelfData();
        long   t2 = GetTickCount();
        if ( t2 - t1 != 0)
        {
            plog("Process self data cost %d - %ld ms (link or transfer) t2= %ld  t1=%ld\n", 
                t2 - t1,t2 - t1,t2,t1);
            //printf("Process self data cost %d ms (link or transfer)\n", t2 - t1);
        }
        
        preSockSet();
        int readsocks = 0;

        if( timeoutSeconds >= 0)
        {
           readsocks = safeSocket_select (0, &m_readSet, NULL,  &m_excepSet,  &timeout);
        }
        else
        {
           readsocks = safeSocket_select (0, &m_readSet, NULL, &m_excepSet,  NULL);
        }

        if (readsocks > 0)
        {
             printf("Sockects has data to process\n");
             long   t1 = GetTickCount();
             readSocks();
             long   t2 = GetTickCount();
             plog("Process selected FD_SET data cost %d ms \n", t2 - t1);
             bneed = true;
        }
        else
        {
            if (readsocks < 0) plog("select error: data failed to be received!\n");
            else if (readsocks == 0)
            { 
                static int i = 0; 
                static int c = 10000000/timeout.tv_usec;
                if (i > c ){ plog(".");     fflush(stdout); i = -1;}
                i++;
             }
        }

    }//endof while
}
//
//void ZSocket::plog(const char* format, ...)
//{
//    static FILE *  fd = stdout;
//    bool bopen = false;
//    if (m_strfileLog.length() != 0)
//    {
//        if ( fd == stdout)
//        {
//            fd = fopen (m_strfileLog.c_str(), "a+"); 
//        }
//      
//      static int idx = 0;
//      fseek (fd, 0, SEEK_END);
//      int length = ftell (fd);
//      if (length >= 2000000) //max is 2M
//      {
//          fclose(fd);
//
//          idx ++;
//          char buffer [33] = "\0";
//          sprintf(buffer, "%d", idx);
//          //itoa (idx,buffer,10);
//          m_strfileLog = m_strfileLog + std::string(buffer) + std::string(".log");
//          fd = fopen (m_strfileLog.c_str(), "w+"); 
//      }
//
//      bopen = true;
//    }
//
//    va_list args;
//    va_start(args, format);
//    vfprintf(fd, format, args);
//    
//    fflush(fd);
//
//    if( bopen)
//        fclose(fd);
//    va_end(args);
//}

void ZSocket::plog(const char* format, ...)
{
    if ( !m_bEnableLog )
        return;

    FILE *  fd = stdout;
    bool bopen = false;
    if (m_strfileLog.length() != 0)
    {
      fd = fopen (m_strfileLog.c_str(), "a+"); 
      
      static int idx = 0;
      fseek (fd, 0, SEEK_END);
      int length = ftell (fd);
      if (length >= 2000000) //max is 2M
      {
          fclose(fd);

          idx ++;
          char buffer [33] = "\0";
          sprintf(buffer, "%d", idx);
          //itoa (idx,buffer,10);
          m_strfileLog = m_strfileLog + std::string(buffer) + std::string(".log");
          fd = fopen (m_strfileLog.c_str(), "w+"); 
      }

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

void ZSocket::setLog(const char* file) 
{
    if ( file != NULL && m_strfileLog.length() == 0)
    {
        m_strfileLog = std::string(file);
        FILE *  fd = fopen (file, "w+"); 
        fclose(fd);
    }
}

//ZSocketClient::ZSocketClient()
//{
//}
//
//ZSocketClient::~ZSocketClient()
//{
//}

//bool ZSocketClient::init()
//{
//    bool bret = false;
//    if (connectDb())
//    {
//        m_sockId = ZSocket::connectServer("127.0.0.1");
//        if ( m_sockId <= 0)
//        {
//            bret = false;
//            plog("Failed: connect server\n");
//        }
//        else
//        {
//            bret = true;
//            plog("m_sockId=%d\n", m_sockId );
//        }
//    }
//    return bret;
//}

//
//int ZSocketClient::connectServer(const char *ip,int port, int timeout)
//{
//    m_sockId = ZSocket::connectServer(ip,port,timeout);
//    plog("Client wants connect ip =%s, port=%d, timeout=%d, return sockid=%d\n",
//        ip,port, timeout,m_sockId);
//    return m_sockId;
//}

// transfer data by parameter DID,the destination is not the DID in littlepack
//int ZSocketClient::transferTerminal(const unsigned int did, const char* littlepack)
//{
//    if ( canWrite(m_sockId,3))
//    {
//        //plog("Msg type: transfer are sent\n");
//        //
//        //int flag = 0;
//        //int len = sendTransfer(m_sockId,littlepack, strlen(littlepack),flag);
//        //if( len <=0 )
//        //{
//        //    int errNumber = safeSocket_getErrorNum (m_sockId);
//        //    plog ("data failed to be sent, errNumber = %u\n", errNumber);
//        //    return -1;
//        //}
//        //else
//        //{
//        //    plog("Result: send transfer bytes = %d\n", len);
//        //    return len;
//        //}
//    }
//    return -1;
//}

//The littlepack is bigendian sequence
//int ZSocketClient::transferTerminal(const char* littlepack,int lenpack)
//{
//    plog("\nTerminal: transfer\n");
//    if ( canWrite(m_sockId,3))
//    {
//        plog("Msg type: transfer are sent\n");
//        int flag = 0;
//        int len = sendTransfer(m_sockId,littlepack, lenpack,flag);
//        if( len <=0 )
//        {
//            int errNumber = safeSocket_getErrorNum (m_sockId);
//            plog ("data failed to be sent, errNumber = %u\n", errNumber);
//            return -1;
//        }
//        else
//        {
//            plog("Result: send transfer bytes = %d\n", len);
//            return len;
//        }
//    }
//    else
//    {
//        plog("Failed: write sock\n");
//    }
//    return -1;
//}
//           
//int ZSocketClient::disconnectTermianl(int did)
//{
//    plog("\nTerminal: disconnect\n");
//    return connectTerminalInterl(did,false);
//}
//
//int ZSocketClient::connectTermianl(int did)
//{
//    plog("\nTerminal: connect\n");
//    return connectTerminalInterl(did,true);
//}
//
//// return value:
//// -1: network error
//// 0: control failed
//// 1: control succssful
//int ZSocketClient::connectTerminalInterl(int did, bool bconnect)
//{
//    int flag;
//    if ( canWrite(m_sockId,3))
//    {
//        char buf[64] = "\0";
//        sprintf(buf, "%d", did);
//
//        int len ;
//        if (bconnect)
//            len = sendConnect(m_sockId,buf, strlen(buf), 0);
//        else
//            len = sendDisConnect(m_sockId,buf, strlen(buf), 0);
//        if( len <=0 )
//        {
//            int errNumber = safeSocket_getErrorNum (m_sockId);
//            plog ("data failed to be sent, errNumber = %u\n", errNumber);
//            return -1;
//        }
//        else
//        {
//            plog("Result: send connect control bytes = %d, createConnect=%d\n", len, bconnect);
//        }
//    }
//    else
//    {
//        plog("Failed: write socket.\n");
//        return -1;
//    }
//   
//    //wait response, get connect result
//    if( canRead(m_sockId,16))
//    {
//        char buf[64] = "\0";
//        int len = read(m_sockId, buf, 64, 0);
//        if( len <=0 )
//        {
//            int errNumber = safeSocket_getErrorNum (m_sockId);
//            plog("GetResponse Failed: read error! errNumber = %d\n", errNumber);//connect = %s len = %d\n", buf,len);
//          
//            return -1;
//        }
//
//        string retval = string(buf);
//        int ret = retval == string("true") ? 1 : 0;
//        plog("GetResponse: content = %s len = %d, retval = %d\n", buf,len,ret);
//        return ret;
//    }
//    else
//    {
//        plog("Failed: read socket.\n");
//        return -1;
//    }
//}
