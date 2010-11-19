#include "zsocket.h"

#include <stdio.h>
#include <stddef.h>

ZSocket::ZSocket() 
{
    memset((char *) &m_connectionList, 0, sizeof(m_connectionList));
	//init();
}

ZSocket::~ZSocket()
{
	unInit();
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
	return ret;
}

bool ZSocket::unInit()
{
	safeSocket_stopSocket ();
    return true;
}

int ZSocket::write(int id, char* data, int byteRead, int flag)
{
    return safeSocket_send (id, data, byteRead, flag);
}
 
//[in] Null-terminated character string representing a number expressed 
//in the Internet standard ".'' (dotted) notation.
int ZSocket::getWriteSocket(char *ip)
{
	int     errNumber;
    USHORT port = 1086;

	if (!init())
	{
		plog("Failed: safe socket start socket!");
		return -1;
	}

	/* create safe socket */
	int m_idWrite = safeSocket_socket (SS_PROTOCOL_UDP, SS_WITHCONNECTION);
	if (m_idWrite == SS_FAILED)
	{
		plog ("Failed: safeSockID created!\n");
		return -1;
	}

	/* bind safeSockID to listen connection request from client */
	SOCKADDR_IN   localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = port;
	localAddr.sin_addr.s_addr = inet_addr (ip);

 //   if (safeSocket_bind (m_idWrite, (const SOCKADDR *) & localAddr, sizeof (SOCKADDR)) == SS_FAILED)
	//{
	//	errNumber = safeSocket_getErrorNum (m_idWrite);
	//	plog ("bind error, errNumber: %u\n", errNumber);
	//	return false;
	//}

	/* connect to server */
	if (safeSocket_connect (m_idWrite, (const SOCKADDR *) & localAddr, sizeof (SOCKADDR)) 
        == SS_FAILED)
	{
	   errNumber = safeSocket_getErrorNum (m_idWrite);
	   plog ("connect failed, errNumber = %u\n", errNumber);
	   return -1;
	}

     /* add safeSock to except set */
	TSafeSocket_FdSet  exceptSet;
	SS_FD_ZERO (& exceptSet);
	SS_FD_SET (m_idWrite, & exceptSet);

    /* wait until connection has established */
	errNumber = safeSocket_select (0, NULL, NULL, & exceptSet, NULL);
	if (errNumber == 0)
	{
		plog ("select error: connection hasn't been established!\n");
		return -1;
	}

	if (SS_FD_ISSET(m_idWrite, &exceptSet))
		return m_idWrite;
	else 
		return -1;
}

bool ZSocket::preRead()
{
	int     errNumber;
	
    USHORT port = 1086;

	if (!init())
	{
		plog("Failed: safe socket start socket!");
		return false;
	}

	/* create safe socket */
	m_idListen = safeSocket_socket (SS_PROTOCOL_UDP, SS_WITHCONNECTION);
	if (m_idListen == SS_FAILED)
	{
		plog ("Failed: safeSockID created!\n");
		return false;
	}

	/* bind safeSockID to listen connection request from client */
	SOCKADDR_IN   localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = port;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);// inet_addr ("127.0.0.1");
    //localAddr.sin_addr.s_addr =  inet_addr ("127.0.0.1");

	if (safeSocket_bind (m_idListen, (const SOCKADDR *) & localAddr, sizeof (SOCKADDR)) == SS_FAILED)
	{
		errNumber = safeSocket_getErrorNum (m_idListen);
		plog ("bind error, errNumber: %u\n", errNumber);
		return false;
	}

	/* set safeSockID to listen state */
	if (safeSocket_listen (m_idListen, MAX_CNT_COUNT) == SS_FAILED) // the parameter value 3 means?
	{
		errNumber = safeSocket_getErrorNum (m_idListen);
		plog ("Failed: socket to be set to listen state, errNumber: %u\n", errNumber);
		return false;
	}
    return true;
}


void ZSocket::buildSelectList()
{
	SS_FD_ZERO(&m_readSet);
	SS_FD_SET(m_idListen,&m_readSet);
	
	for (int listnum = 0; listnum < MAX_CNT_COUNT; listnum++)
    {
		if (m_connectionList[listnum] != 0) 
        {
			SS_FD_SET(m_connectionList[listnum],&m_readSet);
		}
	}
}

void ZSocket::handleNewConnection() 
{
	/* We have a new connection coming in!  We'll
	try to find a spot for it in connectlist. */
    /* accept the subSafeSock in order to receive data */
	SOCKADDR_IN  clientAddr;
	size_t       clientAddrLen;

    /* Socket file descriptor for incoming connections */
	int connection = safeSocket_accept(m_idListen,(SOCKADDR *) & clientAddr, & clientAddrLen);
	if (connection < 0) 
    {
		plog("Error: accept listenid=%d\n",m_idListen);
		return;
	}
	
	for (int i = 0; (i < MAX_CNT_COUNT) && (connection != -1); i ++)
    {
		if (m_connectionList[i] == 0) 
        {
			plog("\nConnection accepted:   FD=%d; Slot=%d\n",connection,i);
			m_connectionList[i] = connection;
			connection = -1;
		}
    }

	if (connection != -1) 
    {
		/* No room left in the queue! */
		plog("\nNo room left for new client.\n");
        char data[102] = "Sorry, this server is too busy. Try again later!\r\n";
        safeSocket_send (connection, data, 1024 , SS_DATA_UNIMPORTANT);
		
        //close socket
		safeSocket_close (connection);
	}
}

// listnum: Current item in connectlist for for loops
void ZSocket::dealWithData(	int listnum	)
{
	char buffer[80];     /* Buffer for socket reads */
    memset(buffer, 0,80);
	char *cur_char;      /* Used in processing buffer */

    int sockId = m_connectionList[listnum];
    
    int      flag;
	if (read(sockId,buffer,79,&flag) < 0) 
    {
		// Connection closed, close this end and free up entry in connectlist 
		plog("\nConnection lost: FD=%d;  Slot=%d\n",sockId,listnum);
		safeSocket_close(sockId);
		m_connectionList[listnum] = 0;
	} 
    else
    {
         //response it
        char data[] =  "abcd";
        int byteSent = write (sockId, data, sizeof(data), SS_DATA_UNIMPORTANT);
		if (byteSent == SS_FAILED)
		{
			int errNumber = safeSocket_getErrorNum (sockId);
			plog ("Failed: data to be sent, errNumber = %u\n", errNumber);
			return;
		}
        else
        {
            // We got some data
		    plog("\nReceived: %s; slot=%d, write out %s",buffer, listnum,data);     
        }
	}
}

void ZSocket::readSocks() 
{
 	// check the sockets in connectlis
    for (int i = 0; i < MAX_CNT_COUNT; i++) 
    {
		if (SS_FD_ISSET(m_connectionList[i],&m_readSet))
        {
			dealWithData(i);
        }
	} 
}

void ZSocket::startReadLoop()
{
    struct timeval timeout;
    timeout.tv_sec = 10;
	timeout.tv_usec = 0;
    TSafeSocket_FdSet  exceptSet;
	while (1) 
	{
		/* Main server loop - forever */
		//buildSelectList();

        //preapare
        SS_FD_ZERO (&exceptSet);
        SS_FD_SET (m_idListen, &exceptSet);
        SS_FD_ZERO(&m_readSet);
	    for (int listnum = 0; listnum < MAX_CNT_COUNT; listnum++)
        {
		    if (m_connectionList[listnum] != 0) 
            {
			    SS_FD_SET(m_connectionList[listnum],&m_readSet);
		    }
	    }

		//int readsocks = safeSocket_select (0, &m_readSet, NULL, NULL,  &timeout);
        int readsocks = safeSocket_select (0, &m_readSet, NULL, &exceptSet,  NULL);

		if (readsocks < 0) 
		{
			plog("select error: data failed to be received!\n");
			//exit(EXIT_FAILURE);
		}
		else if (readsocks == 0) 
		{
			// Nothing ready to read, just show that we're alive
			printf(".");
			fflush(stdout);
		}
		else
		{
             //check our "listening" socket
	        if (SS_FD_ISSET(m_idListen,&exceptSet))
            {
		        handleNewConnection();
            }
            else
            {
                // check the sockets in connectlis
                for (int i = 0; i < MAX_CNT_COUNT; i++) 
                {
		            if (m_connectionList[i] >0 && SS_FD_ISSET(m_connectionList[i],&m_readSet))
                    {
			            dealWithData(i);
                    }
	            } 
            }//end of else
		}//end of else
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



