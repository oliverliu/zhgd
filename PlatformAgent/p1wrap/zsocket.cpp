#include "zsocket.h"

#include <stdio.h>
#include <stddef.h>

#include "platformhead.h"
#include "cutility.h"
#include "zredis.h"

#include "IniFile.h"
#include <vector>
using namespace std;

static const char * s_dbip = "127.0.0.1";
ZSocket::ZSocket() 
{
    memset((char *) &m_connectionList, 0, sizeof(m_connectionList));
	//init();

    m_pRedis = NULL;
    m_bInit  = false;
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
	bool ret = true;
	/* start safe socket */
	if (safeSocket_startSocket() == FALSE)
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
    return safeSocket_send (id, data, byteRead, flag);
}

int ZSocket::read(int sockfd, void *buf, size_t len, int *pflags)
{
    return safeSocket_recv(sockfd, buf, len, pflags);
}
 
int ZSocket::connectServer(const char *ip,int port,int timeout)
{
    return getWriteSocket(ip,port,timeout);
}

int ZSocket::disconnectServer(char* ip)
{
    return -1;
}

int ZSocket::disconnectServer(int did)
{
    int sockid = getSockIDFromID(did);
    if (sockid < 0)
        return -1;
    else
        return safeSocket_close(sockid);
}

int ZSocket::getWriteSocket(const char *ip,int port,int timeout)
{
    int sockid = create();
    return connect(sockid, port, ip,timeout);	
}

int ZSocket::create()
{
    if (!init())
	{
		plog("Failed: safe socket start socket!");
		return -1;
	}

	/* create safe socket */
	m_idListen = safeSocket_socket (SS_PROTOCOL_UDP, SS_WITHCONNECTION);
	if (m_idListen == SS_FAILED)
	{
		plog ("Failed: safeSockID created!\n");
		return -1;
	}
    return m_idListen;
}

bool ZSocket::bind(int _port, const char *ip )
{
    USHORT port = _port;

    /* bind safeSockID to listen connection request from client */
    SOCKADDR_IN   localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = port;
    if ( ip != NULL)
    {
        //localAddr.sin_addr.s_addr =  inet_addr ("127.0.0.1");
        localAddr.sin_addr.s_addr = inet_addr (ip);
    }
    else
    {
         localAddr.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr ("127.0.0.1");
    }

    if (safeSocket_bind (m_idListen, (const SOCKADDR *) & localAddr, sizeof (SOCKADDR)) 
        == SS_FAILED)
    {
        int errNumber = safeSocket_getErrorNum (m_idListen);
        plog ("bind error, errNumber: %u\n", errNumber);
        return false;
    }
    return true;

}

bool ZSocket::listen()
{
    /* set safeSockID to listen state */
    if (safeSocket_listen (m_idListen, MAX_CNT_COUNT) == SS_FAILED) // the parameter value 3 means?
    {
	    int errNumber = safeSocket_getErrorNum (m_idListen);
	    plog ("Failed: socket to be set to listen state, errNumber: %u\n", errNumber);
	    return false;
    }
    return true;
}

int ZSocket::connect(int sockid, int port, const char *ip, int seconds)
{
    if( strlen(ip) <= 0)
    {
        printf("ip address is empty\n");
        return -1;
    }
	/* bind safeSockID to listen connection request from client */
	SOCKADDR_IN   localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = port;
    //localAddr.sin_addr.s_addr =  inet_addr ("127.0.0.1");
    localAddr.sin_addr.s_addr = inet_addr (ip);
   
	/* connect to server */
	if (safeSocket_connect (sockid, (const SOCKADDR *) & localAddr, sizeof (SOCKADDR)) 
        == SS_FAILED)
	{
	   int errNumber = safeSocket_getErrorNum (sockid);
	   plog ("connect failed, errNumber = %u\n", errNumber);
	   return -1;
	}

     /* add safeSock to except set */
	TSafeSocket_FdSet  exceptSet;
	SS_FD_ZERO (&exceptSet);
	SS_FD_SET (sockid, &exceptSet);

    struct timeval timeout;
    timeout.tv_sec = seconds;//3s
    timeout.tv_usec = 0;
    
    int  errNumber ;
    if ( seconds > 0)
        errNumber = safeSocket_select(0, NULL,NULL, &exceptSet,&timeout);
    else
        errNumber = safeSocket_select (0, NULL,NULL, &exceptSet, NULL);

	if (errNumber == 0)
	{
		plog ("Error: select connection hasn't been established! ip = %s,port=%d\n",ip,port);
		return -1;
	}

    int ret = -1;
	if (SS_FD_ISSET(sockid, &exceptSet))
		ret = sockid;
	
    return ret;
}
 void ZSocket::preSockSet()
 {
    SS_FD_ZERO (&m_excepSet);
    SS_FD_ZERO(&m_readSet);

    SS_FD_SET (m_idListen, &m_excepSet);
	for (int listnum = 0; listnum < MAX_CNT_COUNT; listnum++)
    {
		if (m_connectionList[listnum] != 0) 
        {
			SS_FD_SET(m_connectionList[listnum],&m_readSet);
		}
	}
 }

bool ZSocket::initServer(int port)
{
    if ( create() == -1 || !bind(port) || !listen() )
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

 string toString(long long_address)
 {
    string ret;
    struct in_addr addr;
    addr.s_addr = long_address;
    char *dot_ip = inet_ntoa(addr);
    ret = string(dot_ip);
    return ret;        
 } 

bool ZSocket::addConnection(long longip,int sockid )
{
    bool bret = false;
    for (int i = 0; i < MAX_CNT_COUNT; i ++)
    {
        if (m_connectionList[i] == 0) 
        {
            //printf("\nConnection accepted:   FD=%d; Slot=%d\n",sockid,i);
            plog("\nConnection accepted:   FD=%d; Slot=%d\n",sockid,i);
            m_connectionList[i] = sockid;
            m_mapLongIP2SockId.insert(pair<long,int>(longip, sockid) );
              
            plog("Connection: add to fd set, longip =%0x, ip=%s\n", longip, toString(longip).c_str());
            //printf("Connection: add to fd longip =%0x, ip=%s\n", longip, toString(longip).c_str());
            bret = true;
            break;
         }
    }
    return bret;
}

void ZSocket::handleNewConnection() 
{
	SOCKADDR_IN  clientAddr;
	size_t       clientAddrLen;

    /* Socket file descriptor for incoming connections */
	int connection = safeSocket_accept(m_idListen,(SOCKADDR *) &clientAddr, & clientAddrLen);
	if (connection < 0) 
    {
		plog("Error: accept listenid=%d\n",m_idListen);
		return;
	}
	
    bool badd =  addConnection(clientAddr.sin_addr.s_addr, connection);
	if (!badd) 
    {
		/* No room left in the queue! */
		plog("\nNo room left for new client.\n");
        char data[102] = "Sorry, this server is too busy. Try again later!\r\n";
        safeSocket_send (connection, data, 1024 , SS_DATA_UNIMPORTANT);
		
        //close socket
		safeSocket_close (connection);
	}
}

bool ZSocket::connectDb()
{
    m_pRedis = new ZRedis();
    return m_pRedis->connect(s_dbip);
}

#define ASSERT_SOCKID(id) {if(id <= 0) return false;}

bool ZSocket::canWrite(int sockId,int seconds )
{
    ASSERT_SOCKID(sockId);
    bool bret = false;
    TSafeSocket_FdSet set;
    SS_FD_ZERO (&set);
    SS_FD_SET (sockId, &set);

    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    
    int  socks ;
    if ( seconds > 0)
        socks = safeSocket_select (0, NULL, &set, NULL,  &timeout);
    else
        socks = safeSocket_select (0, NULL, &set, NULL,  NULL);
    if (socks > 0)
        bret = SS_FD_ISSET(sockId,& set);
    return bret;
}

bool ZSocket::canRead(int sockId,int seconds )
{
    ASSERT_SOCKID(sockId);
    bool bret = false;
    TSafeSocket_FdSet set;
    SS_FD_ZERO (&set);
    SS_FD_SET (sockId, &set);
    
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
   
    int  socks ;
    if ( seconds > 0)
        socks = safeSocket_select (0, &set, NULL, NULL,  &timeout);
    else
        socks = safeSocket_select (0, &set, NULL, NULL,  NULL);
    
    if (socks > 0)
        bret = SS_FD_ISSET(sockId,& set);
    return bret;
}


string ZSocket::getIPFromID(int id)
{
    char key[48];
    memset(key, 0 ,48);
    key[0]='0';
    key[1]='x';
    sprintf(key+2, "%0x", id);

    //id->longip
    string value = CIniFile::GetValue(key,"ip", "sid.config");

    return value;
}

int ZSocket::getSockIDFromID(int id)
{
    int ret = -1;
    //id->longip
    string value = getIPFromID(id);
    long long_address = inet_addr (value.c_str()) ;

    //longip->sockid
    map<long,int>::iterator it;
    it = m_mapLongIP2SockId.find(long_address);
    if ( it != m_mapLongIP2SockId.end())
    {
        ret = it->second;
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
    char buf[64] = "\0";
    static string strdst = "selfconnect";

    if ( size + strdst.length() >= 64 ) 
    {
        plog("Warning: message content between self and agent size too large!!!");
        exit(-1);
    }
    memcpy(buf, strdst.c_str(), strdst.length());
    memcpy(buf + strdst.length(), data, size);

    return write(sockid, buf, strlen(buf), flag);
    //return write(sockid, buf, strdst.length() + size, flag);
}
//data input is DID
int ZSocket::sendDisConnect(int sockid, const char* data, int size, int flag)
{
    char buf[64] = "\0";
    static string strdst = "selfdisconnect";
    if ( size + strdst.length() >= 64 ) 
    {
        plog("Warning: message content between self and agent size too large!!!");
        exit(-1);
    }
    memcpy(buf, strdst.c_str(), strdst.length());
    memcpy(buf + strdst.length(), data, size);

    return write(sockid, buf, strdst.length() + size, flag);
}

//data input is littlepack content
int ZSocket::sendTransfer(int sockid,const char* data, int size, int flag)
{
    char buf[64+SIZE] = "\0";
    static string strdst = "selftransfer";

    memcpy(buf, strdst.c_str(), strdst.length());
    memcpy(buf + strdst.length(), data, size);

    return write(sockid, buf, strdst.length() + size, flag);
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

//buffer content is "selfconnect#ID.."
int ZSocket::procConnectCtrl(const char * buffer)
{
    bool badd = false;
    int sockid = 0;
    char msghead[] = "selfconnect";
    unsigned int did = 0; 
    sscanf(buffer + strlen(msghead),"%d", &did);
   

    string ip = getIPFromID(did);
    if (ip.empty())
    {
        printf("Failed: Find ip from configure file for id=0x%0x.\n",did);
        return -1;
    }
    sockid = connectServer(ip.c_str(),1086,3);
    if( sockid != -1)
    {
        string ip = getIPFromID(did);
        long long_address = inet_addr ( ip.c_str()) ;
        badd = addConnection(long_address, sockid);
        if ( !badd )
        {
            plog("Warning: connect successful, but add monitor failed!!!! That is unnormal!\n");
        }
    }
    return sockid;
}

int ZSocket::procDisconnectCtrl(const char * buffer)
{
    int bdel, bret;
    char msghead[] = "selfdisconnect";
    unsigned int did = 0;
    sscanf(buffer + strlen(msghead),"%d", &did);

    bret = disconnectServer(did);
    if( bret != -1)
    {
        string value = getIPFromID(did);
        if (value.empty())
        {
            printf("Failed: Find ip from configure file for id=0x%0x.\n",did);
            return -1;
        }
        long long_address = inet_addr (value.c_str()) ;
        bdel = delConnection(long_address);
        if ( !bdel )
        {
            plog("Warning: disconnect successful, but del monitor failed!!!! That is unnormal!\n");
        }
    }
    else
    {
        plog("%0x has not connect agent, it need not disconnect\n", did);
    }
    return bret;
}

//buffer content is "selftransfer" + littlepackage
int ZSocket::procTransferCtrl(const char * buffer)
{
     //the data format is : "selftransfer"  + littlepack content
    char msghead[] = "selftransfer";
    unsigned int did = 0;
    int byteSent = -1;

    char msgLittleBuf[SIZE] = "\0";
    memcpy(msgLittleBuf, buffer + strlen(msghead),SIZE);
           
    s_packinfo pack;
    getPackinfo(pack,msgLittleBuf);

    //pop form db and send out
    char popOutBuf[SIZE] = "\0";
    if(1)
         m_pRedis->app_lpop(pack.did,(unsigned char*)popOutBuf);
    else
    {
        //another method
        memcpy(popOutBuf, buffer + sizeof(msghead), pack.size);
        //write out the popOut2 without db support
    }

    int sockidTo = getSockIDFromID(pack.did);
    if (sockidTo > 0 && canWrite(sockidTo,3))
    {
        byteSent = write (sockidTo, msgLittleBuf, pack.size, SS_DATA_UNIMPORTANT);
        if (byteSent == SS_FAILED)
        {
            int errNumber = safeSocket_getErrorNum (sockidTo);
            plog ("Failed: data to be sent, errNumber = %u\n", errNumber);
            return byteSent;
        }
        // We got some data
        plog("\nwrite to sockid %d, sid=%d",sockidTo,pack.sid);     
      
    }
    return byteSent;
}
           
void ZSocket::getPackinfo(s_packinfo& packinfo, char* _buffer)
{
    unsigned char* buffer = (unsigned char*)_buffer;

    if( CUtility::needSwap())
    {
        CUtility::littlePackToLE(buffer);
    }

    packinfo.datetype = CUtility::getLittlePackDateType(buffer);
    packinfo.sid = CUtility::getLittlePackSID(buffer);
    packinfo.did = CUtility::getLittlePackDID(buffer);
    packinfo.size = CUtility::getLittlePackSize(buffer);
        
    //restore it.
    if( CUtility::needSwap())
    {
        CUtility::littlePackToBE(buffer);
    }
}

// listnum: Current item in connectlist for for loops
void ZSocket::dealWithData(	int listnum	)
{
    char self[] = "selftransfer";
	char buffer[SIZE + 12 + 1];    //12 is "selftransfer" length
    memset(buffer, 0,SIZE+5);
	char *cur_char;      /* Used in processing buffer */

    int sockId = m_connectionList[listnum];
    
    int      flag;
	if (safeSocket_recv(sockId,buffer,SIZE+4,&flag) < 0) 
    {
		// Connection closed, close this end and free up entry in connectlist 
		plog("\nConnection lost: FD=%d;  Slot=%d\n",sockId,listnum);
		safeSocket_close(sockId);
		m_connectionList[listnum] = 0;
	} 
    else
    {
        //get content
       if (isFromSelf(buffer)) //transfer them to other terminal
       {
           plog("Transfer from self: true\n");
           if(isConnectCtrl(buffer))
           {
              plog("Transfer connect: true\n");
              int ret = procConnectCtrl(buffer);
              string str = ret != -1 ? "true" : "false";
              if (canWrite(sockId,3))
              {
                int len = write(sockId,str.c_str(), str.length(), SS_DATA_UNIMPORTANT);
                if ( len > 0)
                    plog("Response successful: connect result = %s\n", str.c_str());
                else
                    plog("Response Failed: write failed, the content = %s\n", str.c_str());
              }
              else
              {
                  plog("Failed: write sockid.\n");
              }
           }

           else if(isDisconnectCtrl(buffer))
           {
              plog("Transfer disconnect: true\n");
              int ret = procDisconnectCtrl(buffer);
              string str = ret != -1 ? "true" : "false";
              if (canWrite(sockId))
              {
                  int len = write(sockId,str.c_str(), str.length(),SS_DATA_UNIMPORTANT);
                  if ( len > 0)
                    plog("Response successful: disconnect result = %s\n", str.c_str());
                  else
                    plog("Response Failed: write failed, the content = %s\n", str.c_str());
              }
              else
              {
                  plog("Failed: write sockid.\n");
              }
           }

           else if(isTransferCtrl(buffer))
           {
               plog("Transfer data: true\n");
               int len = procTransferCtrl(buffer);
               plog("Transfer data: length = %d\n", len);
           }
       }
       else //from other terminal to me. push back
       {
           plog("Transfer from other: true\n");
           s_packinfo pack;
           getPackinfo(pack,buffer);

            //the data is from other terminals, push it to db
            // the did should be equal selfID
           m_pRedis->app_rpush(pack.did, (const unsigned char*) buffer);
       }
	}
}

void ZSocket::readSocks()
{
	if (SS_FD_ISSET(m_idListen,&m_excepSet))
    {
         //check our "listening" socket
		handleNewConnection();
    }
   // else
    {
        for (int i = 0; i < MAX_CNT_COUNT; i++) 
        {
		    if (m_connectionList[i] >0 &&
                SS_FD_ISSET(m_connectionList[i],&m_readSet))
            {
			    dealWithData(i);
            }
	    } 
    }
}

void ZSocket::procLoop(int timeoutSeconds)
{
    struct timeval timeout;
    if( timeoutSeconds >= 0)
    {
        timeout.tv_sec = timeoutSeconds;//10;
	    timeout.tv_usec = 0;
    }

    while (1) 
	{
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
            readSocks();
        }
        else
        {
            if (readsocks < 0) plog("select error: data failed to be received!\n");
            else if (readsocks == 0) { printf(".");	fflush(stdout);}
        }

	}//endof while
}

void ZSocket::plog(const char* format, ...)
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

void ZSocket::setLog(const char* file) 
{
    if ( file != NULL)
    {
        m_strfileLog = std::string(file);
        FILE *  fd = fopen (file, "w"); 
        fclose(fd);
    }
}


ZSocketClient::ZSocketClient()
{
}

ZSocketClient::~ZSocketClient()
{
}

bool ZSocketClient::init()
{
    bool bret = false;
    if (connectDb())
    {
        m_sockId = ZSocket::connectServer("127.0.0.1");
        if ( m_sockId <= 0)
        {
            bret = false;
            plog("Failed: connect server\n");
        }
        else
        {
            bret = true;
            plog("m_sockId=%d\n", m_sockId );
        }
    }
    return bret;
}

int ZSocketClient::connectServer(const char *ip,int port, int timeout)
{
    m_sockId = ZSocket::connectServer(ip,port,timeout);
    return m_sockId;
}

// transfer data by parameter did not the did in littlepack
int ZSocketClient::transferTerminal(const unsigned int did, const char* littlepack)
{
    if ( canWrite(m_sockId,3))
    {
        //printf("Msg type: transfer are sent\n");
        //
        //int flag = 0;
        //int len = sendTransfer(m_sockId,littlepack, strlen(littlepack),flag);
        //if( len <=0 )
        //{
        //    int errNumber = safeSocket_getErrorNum (m_sockId);
        //    printf ("data failed to be sent, errNumber = %u\n", errNumber);
        //    return -1;
        //}
        //else
        //{
        //    printf("Result: send transfer bytes = %d\n", len);
        //    return len;
        //}
    }
    return -1;
}

int ZSocketClient::transferTerminal(const char* littlepack)
{
    plog("Terminal: transfer\n");
    if ( canWrite(m_sockId,3))
    {
        plog("Msg type: transfer are sent\n");
        
        int flag = 0;
        int len = sendTransfer(m_sockId,littlepack, strlen(littlepack),flag);
        if( len <=0 )
        {
            int errNumber = safeSocket_getErrorNum (m_sockId);
            plog ("data failed to be sent, errNumber = %u\n", errNumber);
            return -1;
        }
        else
        {
            plog("Result: send transfer bytes = %d\n", len);
            return len;
        }
    }
    else
    {
        plog("Failed: write sock\n");
    }
    return -1;
}
           
int ZSocketClient::disconnectTermianl(int did)
{
    plog("Terminal: disconnect\n");
    return connectTerminalInterl(did,false);
}

int ZSocketClient::connectTermianl(int did)
{
    plog("Terminal: connect\n");
    return connectTerminalInterl(did,true);
}

// return value:
// -1: network error
// 0: control failed
// 1: control succssful
int ZSocketClient::connectTerminalInterl(int did, bool bconnect)
{
    int flag;
    if ( canWrite(m_sockId,3))
    {
        char buf[64] = "\0";
        sprintf(buf, "%d", did);

        int len ;
        if (bconnect)
            len = sendConnect(m_sockId,buf, strlen(buf), 0);
        else
            len = sendDisConnect(m_sockId,buf, strlen(buf), 0);
        if( len <=0 )
        {
            int errNumber = safeSocket_getErrorNum (m_sockId);
            plog ("data failed to be sent, errNumber = %u\n", errNumber);
            return -1;
        }
        else
        {
            plog("Result: send connect control bytes = %d, createConnect=%d\n", len, bconnect);
        }
    }
    else
    {
        plog("Failed: write socket.\n");
        return -1;
    }
   
    //wait response, get connect result
    if( canRead(m_sockId,16))
    {
        char buf[64] = "\0";
        int len = read(m_sockId, buf, 64, &flag);
        if( len <=0 )
        {
            int errNumber = safeSocket_getErrorNum (m_sockId);
            plog("GetResponse Failed: read error! errNumber = %d\n", errNumber);//connect = %s len = %d\n", buf,len);
          
            return -1;
        }

        string retval = string(buf);
        int ret = retval == string("true") ? 1 : 0;
        plog("GetResponse: content = %s len = %d, retval = %d\n", buf,len,ret);
        return ret;
    }
    else
    {
        plog("Failed: read socket.\n");
        return -1;
    }
}

