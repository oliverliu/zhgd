// Definition of the ZSocket class
#ifndef Z_ZSOCKET_CLASS
#define Z_ZSOCKET_CLASS

/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
 * Create Data: 2010-11-15
 * Version: 0.1
 * Author: Liuning
 * Contact: liuning@zdwxgd.com
 * Description: for platform simulator network layer
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 */
#include <string>
#include <map>
#include "platform.h"
#include "safeSocket.h"

const int MAX_CNT_COUNT=1024;
#define SIZE 2048

typedef struct _packinfo
{
    unsigned int datatype;
    unsigned int sid;
    unsigned int did ;
    unsigned int size;
}s_packinfo;

class ZRedis;
class ZSocket
{
public:
	ZSocket();
	virtual ~ZSocket();

    //client
    //return sockect id, connect ip
    //int getWriteSocket(const char *ip,int port = 1086, int timeout = -1);
    //Block; until connect successful return, for client
    virtual int connectServer(const char *ip,int port = 1086, int timeout = -1);
    //Unblock
    virtual int connectTerminal(unsigned int did,int port = 1086, int timeout = -1);
   
    virtual int disconnectServer(char* ip);
    virtual int disconnectTerminal(int did);

    int sendConnect(int sockid, const char* data, int size, int flag);
    int sendDisConnect(int sockid, const char* data, int size, int flag);
    int sendTransfer(int sockid, const char* data, int size, int flag);
	
    //server 
    //Successful: 1; failed: 0
    int  connectDb(); 
    int  create();
    bool bind(int port,const char* ip = NULL);
    bool listen();
  
    bool initServer(int port); //just only for invoke conveniencely.
    void  procLoop(int timeoutSeconds = -1);//default is wait forever

    int write(int id, const char* data, int byteRead, int flag);
    int read(int sockfd, void *buf, size_t len, int pflags);

    bool canWrite(int sockId,int microseconds = -1);
    bool canRead(int sockId, int microseconds = -1);
    //debug
    void setLog(const char* file = NULL);
 
protected:
    void plog(const char* format, ...);
    void outputLittlepack(const unsigned char * buffer);

    //init connect state unit package
    //type: 0: create connect
    //        1: remove connect
    //connectvalue: 1: successful
    //          other: failed
    void initPackage(unsigned  char *  _ppack, unsigned int did, int type, int connectvalue);
    void updateDbBuffer(unsigned  char *  ppack);

    void procSelfData();
private:
    int  connect(int sockid, int _port, const char *ip,int timeout = -1);

    bool init();
    bool unInit();

 //   void buildSelectList();
    void preSockSet();
    void readSocks();
    void handleNewConnection();
    void dealWithData(int listnum);
    void updateConnectResult(int listnum,int result);
	
    std::string getIPFromID(int id);
    unsigned int getIDFromIP(std::string ip);
    std::string getIPFromSockID(int sockid);
	
    unsigned int getIDFromSockID(int sockid);
    int  getSockIDFromID(int id);
     
    void initIDIP();
    // the msg send by host client,such as connect, disconnect or transfer
    bool isFromSelf(const char * buffer);
    // The parameter buffer is little endian sequence
    void getPackinfo(struct _packinfo& packinfo, unsigned char* buffer);

     bool addConnection (long longip,int sockid);
     bool delConnection(long long_address);	 

    bool isTransferCtrl(const char * buffer);
    bool isConnectCtrl(const char * buffer);
    bool isDisconnectCtrl(const char * buffer);
    int procConnectCtrl(const char * buffer);
    int procDisconnectCtrl(const char * buffer);
    int procTransferCtrl(unsigned char * buffer);
    void procSelfDataInternal(const char* key);

    void initConnectState(const char *  _ppack, const unsigned int  did, 
        const unsigned int  sid,int type, int connectvalue);
    bool needMonitor(int sockid);
private:

    bool m_bInit;
    int m_idListen;//for server that is listen id, for client it is R/W socket id
    TSafeSocket_FdSet  m_readSet;
    TSafeSocket_FdSet  m_excepSet;
	
    int m_connectionList[MAX_CNT_COUNT];//save acceptted fd
   // int m_sockReadMonitorList[MAX_CNT_COUNT];
   // int m_sockExceptMonitorList[MAX_CNT_COUNT];
	
    std::string m_strfileLog;

    std::map<long, int> m_mapLongIP2SockId;
    
    std::map<int,long> m_mapID2LongIP;
    std::map<int,std::string> m_mapID2IP;
    ZRedis* m_pRedis;

     //save data in db, the data is little endian, that used for app_get/app_set
    //When wirte out linkstate, it will convert to big endian through temporary variable
    unsigned char m_dbBuf[SIZE];

    int m_maxSetNums;
};

/*
class ZSocketClient : public ZSocket
{
public:
    ZSocketClient();
    ~ZSocketClient();

    int connectServer(const char *ip,int port = 1086, int timeout = -1);
    //bool init();

    int transferTerminal(const char* littlepack,int lenpack);//the did can be gotten from littlepack
    int transferTerminal(const unsigned int did, const char* littlepack);
    
    // return value:
    // -1: network error
    // 0: control failed
    // 1: control succssful
    int disconnectTermianl(int did);
    int connectTermianl(int did);

private:
    int connectTerminalInterl(int did, bool bconnect);

private:
    //ZSocket *m_sockClient;
    int      m_sockId;//socket between self and agent 
};
*/

#endif
