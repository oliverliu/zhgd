// Definition of the ZSocket class
#ifndef Z_ZSocket_CLASS
#define Z_ZSocket_CLASS

/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
 * Create Data: 2010-11-15
 * Version: 0.1
 * Author: Liuning
 * Contact: liuning@zdwxgd.com
 * Description: for platform simulator network layer
 */
#include "safeSocket.h"
#include <string>

#define MAX_CNT_COUNT 1024
class ZSocket
{
public:
	ZSocket();
	virtual ~ZSocket();

    //client
    //return sockect id
    int getWriteSocket(char *ip);//until connect successful return, for client
	
    //server
    bool preRead();//stop at listen for server
	void startReadLoop();

    int write(int id, char* data, int byteRead, int flag);
    int read(int sockfd, void *buf, size_t len, int *pflags)
    {
        return safeSocket_recv(sockfd, buf, len, pflags);
    }

    //debug
    void setLog(char* file) 
    {
        m_strfileLog = std::string(file);
        FILE *  fd = fopen (file, "w"); 
        fclose(fd);
    }
 
private:
	int m_idListen;//for server
	TSafeSocket_FdSet  m_readSet;
    int m_connectionList[MAX_CNT_COUNT];//save acceptted fd

private:
	bool init();
	bool unInit();

    void buildSelectList();
	void readSocks();
	void handleNewConnection();
    void dealWithData(int listnum);
	void plog(const char* format, ...);

    std::string m_strfileLog;
};

#endif