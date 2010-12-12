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
#include "safeSocket.h"

const int MAX_CNT_COUNT=1024;

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
    int getWriteSocket(const char *ip,int port = 1086, int timeout = -1);//until connect successful return, for client
    virtual int connectServer(const char *ip,int port = 1086, int timeout = -1);
   
    int sendConnect(int sockid, const char* data, int size, int flag);
    int sendDisConnect(int sockid, const char* data, int size, int flag);
    int sendTransfer(int sockid, const char* data, int size, int flag);

    int disconnectServer(char* ip);
    int disconnectServer(int did);
	
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

    bool canWrite(int sockId,int seconds = -1);
    bool canRead(int sockId, int seconds = -1);
    //debug
    void setLog(const char* file = NULL);
 
protected:
    void plog(const char* format, ...);
    void outputLittlepack(const unsigned char * buffer);

private:
    int  connect(int sockid, int _port, const char *ip,int timeout = -1);

	bool init();
	bool unInit();

 //   void buildSelectList();
    void preSockSet();
	void readSocks();
	void handleNewConnection();
    void dealWithData(int listnum);
	

    
    std::string getIPFromID(int id);
    std::string getIPFromSockID(int sockid);
    int  getSockIDFromID(int id);
     
    void initIDIP();
    bool isFromSelf(const char * buffer);// the msg send by host client,such as connect, disconnect or transfer
    void getPackinfo(struct _packinfo& packinfo, char* buffer);

     bool addConnection (long longip,int sockid);
     bool delConnection(long long_address);

    bool isTransferCtrl(const char * buffer);
    bool isConnectCtrl(const char * buffer);
    bool isDisconnectCtrl(const char * buffer);
    int procConnectCtrl(const char * buffer);
    int procDisconnectCtrl(const char * buffer);
    int procTransferCtrl(const char * buffer);
private:

    bool m_bInit;
	int m_idListen;//for server that is listen id, for client it is R/W socket id
	TSafeSocket_FdSet  m_readSet;
    TSafeSocket_FdSet  m_excepSet;
    int m_connectionList[MAX_CNT_COUNT];//save acceptted fd
    std::string m_strfileLog;

    std::map<long, int> m_mapLongIP2SockId;
    
    std::map<int,long> m_mapID2LongIP;
    std::map<int,std::string> m_mapID2IP;
    ZRedis* m_pRedis;

};

class ZSocketClient : public ZSocket
{
public:
    ZSocketClient();
    ~ZSocketClient();

    int connectServer(const char *ip,int port = 1086, int timeout = -1);
    bool init();
    int transferTerminal(const char* littlepack);//the did can be gotten from littlepack
    int transferTerminal(const unsigned int did, const char* littlepack);
    int disconnectTermianl(int did);
    int connectTermianl(int did);

private:
    int connectTerminalInterl(int did, bool bconnect);

private:
    ZSocket *m_sockClient;
    int      m_sockId;
};

#endif