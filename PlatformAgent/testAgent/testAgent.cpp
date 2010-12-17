// testAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "zsocket.h"

#include <stdlib.h>
#include <string>
using namespace std;

static char * logfile = "testAgent_log.txt";


int connect1025Server_1(int port,int seconds)
{
    if (safeSocket_startSocket(NULL) == -1)
	{
        printf ("safe socket failed to be started!\n");
		return 0;
	}
    /* create safe socket */
	int safeSockID = safeSocket_socket (SS_PROTOCOL_UDP, SS_WITHCONNECTION);
	if (safeSockID == SS_FAILED)
	{
		printf ("safeSockID failed to be created!\n");
		return 0;
	}

	SOCKADDR_IN  serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(1086);
	serverAddr.sin_addr.S_un.S_addr = inet_addr ("127.0.0.1");

	/* add safeSock to except set */
	TSafeSocket_FdSet  exceptSet,readSet, writeSet;
	SS_FD_ZERO (& exceptSet);
    SS_FD_ZERO (& readSet);
    SS_FD_ZERO (& writeSet);
	SS_FD_SET (safeSockID, & exceptSet);
    SS_FD_SET (safeSockID, & readSet);
    SS_FD_SET (safeSockID, & writeSet);

	/* connect to server */
	if (safeSocket_connect (safeSockID, (const SOCKADDR *) & serverAddr, sizeof (SOCKADDR)) == SS_FAILED)
	{
	   int errNumber = safeSocket_getErrorNum (safeSockID);
	   printf("connect failed, errNumber = %u\n", errNumber);

	   return 0;
	}

    struct timeval timeout;
    timeout.tv_sec = seconds;//3s
    timeout.tv_usec = 0;
    
    int  errNumber ;
    if ( seconds > 0)
        errNumber = safeSocket_select(0, NULL,NULL, &exceptSet,&timeout);
    else
        errNumber = safeSocket_select (0, NULL,NULL, &exceptSet, NULL);

	///* wait until connection has established */
	//int errNumber = safeSocket_select (0, NULL, NULL, & exceptSet, NULL);
	if (errNumber == 0)
	{
         int errNumber = safeSocket_getErrorNum (safeSockID);
		printf ("select error: connection hasn't been established!\n");
		return 0;
	}
    return errNumber;
}


int _tmain(int argc, _TCHAR* argv[])
{
       //if( client.connect1025Server_1(1087,3) < 0)
    //if( connect1025Server_1(1086,3) == 0)
    //{
    //    printf("FAiled: Socket client connect agent server\n");
    //}
    //
     
    ZSocketClient client;
    bool bret = false;
    if (client.connectDb() < 0)
    {
        printf("Failed: Connect db\n");
        return -1;
    }

    /*if( client.connectServer("127.0.0.1",1087,3) < 0)
    {
        printf("FAiled: Socket client connect agent server\n");
    } 
    */
    //debug
    if( client.connectServer("127.0.0.1",1086,3) < 0)
    { 
        printf("FAiled: Socket client connect agent server\n"); 
        getchar(); 
        return 0; 
    } 
    //if( client.connect1025Server_1(1087,3) < 0)
    //  //if( connect1025Server_1(1087,3) == 0) 
    //  {
    //       printf("FAiled: Socket client connect agent server\n");
    //  } 
    //
    int ret =  client.connectTermianl(0x20000002); 
    
    //ret =  client.disconnectTermianl(2);
    
    char *s = "Absdsddddddddddddfsdfasdfc";
    ret =  client.transferTerminal(s, strlen(s));
    s =  "A000bsdsdddddddd ddddfsdfasdfc";
    ret =  client.transferTerminal(s, strlen(s));

    printf("getchar to quit\n"); 
    getchar();

    return 0;




























 //   //ZSocket client;
 //  
 //   client.connectDb();
 //   int len = 0;
 //   char buf[24] = "\0";
 // 
 //   //int sockid1 = client.connectServer(""); 

 //   int sockid = client.connectServer("127.0.0.1"); 
 //   if (sockid <= 0)
 //   {
 //       printf("Failed: connect server\n");
 //   }
 //   else
 //   {
 //       printf("sockid=%d\n", sockid);
 //   }
 //   
 //   /*if ( client.canWrite(sockid,3))
 //   {
 //       sprintf(buf+2, "%0x", 0x20000002);

 //       char bufdd[64] = "\0";
 //       static string strdst = "selfconnect";
 //       memcpy(bufdd, strdst.c_str(), strdst.length());
 //       memcpy(bufdd + strdst.length(), buf, strlen(buf));
 //       
 //       int i = strdst.length() + strlen(buf);
 //       int j = strlen(bufdd);

 //       static string strdst1 = "selfconnect0x20000002";
 //       len = client.write(sockid, strdst1.c_str(),  strdst1.length(), 0);

 //        strdst1 = "selfconnect2werwrw2";
 //        len = client.write(sockid, strdst1.c_str(),  strdst.length()+3, 0);


 //      len = client.write(sockid, bufdd, strdst.length() + strlen(buf), 0);
 //      len = client.write(sockid, bufdd, strlen(bufdd), 0);
 //       len = client.write(sockid, bufdd, sizeof(bufdd), 0);

 //       len = client.write(sockid, "abc", 3, 0);
 //       len = client.write(sockid, "selfconnect20000002", 21, 0);

 //   }*/

 //   if ( client.canWrite(sockid,3))
 //   {
 //       //sprintf(buf+2, "%0x", 0x20000002);
 //       sprintf(buf, "%d", 0x2);

 //       int i = strlen(buf);
 //       //len = client.sendConnect(sockid,buf, strlen(buf),0);
 //       len = client.sendConnect(sockid,buf, sizeof(buf), 0);
 //       printf("Result: send connect retValue = %d\n", len);
 //   }

 //   if( client.canRead(sockid,116))
 //   {
 //       char buf[64] = "\0";
 //       int len = client.read(sockid, buf, 64, 0);
 //       if( len <=0 )
 //       {
 //           int errNumber = safeSocket_getErrorNum (sockid);
	//		printf ("data failed to be sent, errNumber = %u\n", errNumber);
 //       }

 //       printf("GetResponse: connect = %s len = %d\n", buf,len);
 //   }
 //   else
 //   {
 //       printf("GetResponse NOT: socket connect can not read\n");
 //   }
 //   
 //    if ( client.canWrite(sockid,13))
 //    {
 //       int discret = client.sendDisConnect(sockid,buf, sizeof(buf), 0);
 //       printf("Result: send disconnect retValue = %d\n", discret);
 //       if( client.canRead(sockid,10))
 //       {
 //           char buf[64] = "\0";
 //           int len = client.read(sockid, buf, 64, 0);
 //           printf("GetResponse: disconnect = %s, len = %d\n",buf,len);
 //       }
 //       else
 //       {
 //           printf("GetResponse NOT: socket disconnect can not read.\n");
 //       }
 //    }
 //  
 //    if ( client.canWrite(sockid))
 //    {
 //       printf("Msg type: transfer are sent\n");
 //       sprintf(buf, "%d%s", 0x20000002,"abc");
 //       client.sendTransfer(sockid,buf, sizeof(buf),0);
 //    }

 //   getchar();
 //   return 0;
 //   
 //   char  data[10];
	//int   byteSent;
	//int   byteRead;
 //   int     errNumber;

	//
 //   FILE *  pClientFile = fopen (logfile, "w");
 //   fclose(pClientFile);

 //   /* open the file read */
	//FILE *  pReadFile = fopen ("c:\\read.txt", "r"); 
	//if (pReadFile == NULL)
	//{
	//	printf ( "read.txt failed to be opened!\n");
	//	return -1;
	//}

 //   int socketid = client.getWriteSocket("127.0.0.1");

 //   pClientFile = fopen (logfile, "a+");

 //   bool bFinishRead = false;

 //   while (1)
 //   {
 //       TSafeSocket_FdSet m_recvSet, m_sendSet;
 //       SS_FD_ZERO(&m_recvSet);
 //       SS_FD_ZERO(&m_sendSet);
 //       SS_FD_SET(socketid,&m_recvSet);
 //       SS_FD_SET(socketid,&m_sendSet);

	//	int readsocks = safeSocket_select (0, &m_recvSet, &m_sendSet, NULL,  NULL);
 //       if (readsocks < 0) 
	//	{
	//		fprintf(pClientFile,"select error: data failed to be received!\n");
	//		//exit(EXIT_FAILURE);
	//	}
	//	else if (readsocks == 0) 
	//	{
	//		// Nothing ready to read, just show that we're alive
	//		printf(".");
	//		fflush(stdout);
	//	}
	//	else
	//	{
	//		if (SS_FD_ISSET(socketid,&m_recvSet))
 //           {
 //               char data[1024];
 //               memset(data, 0, 1024);
 //               int flag;
 //               client.read(socketid,data,1023, &flag);
 //               fprintf(pClientFile, "Receive in: %s\n", data);
 //           }
 //           if (SS_FD_ISSET(socketid,&m_sendSet))
 //           {
 //               if( !bFinishRead )
 //               {
 //                   memset(data,0,10);

	//	            byteRead = fread (data, 1, 4, pReadFile);
 //                   if (byteRead <= 0)
 //                   {
 //                       printf("Error: from file\n");
 //                       continue;
 //                   }
 //                   printf("%s\n",data);
 //                   byteSent = client.write(socketid, data, byteRead, SS_DATA_UNIMPORTANT);

	//	            /* indicate data has been sent */
	//	            fprintf (pClientFile, "write out: msg contains %u bytes - %s\n", byteSent, data);

	//	            if (byteSent == SS_FAILED)
	//	            {
	//		            errNumber = safeSocket_getErrorNum (socketid);
	//		            fprintf (pClientFile, "data failed to be sent, errNumber = %u\n", errNumber);
	//		            return -1;
	//	            }
 //               }
 //               else
 //               {
 //                   printf("has finished reading file!!!\n");
 //                   break;
 //               }
 //           }//(SS_FD_ISSET(socketid,&m_sendSet))

 //           if (byteSent < 4)
 //           {
 //               fflush(pClientFile);
 //               bFinishRead = true;
 //               fclose(pReadFile);
 //           }
	//	}
 //       
 //       fflush(pClientFile);
 //      
 //   }

 //   while(1)
 //   {
 //       Sleep(2000);
 //       fflush(pClientFile);
 //   }
 //  //
 //   fclose(pClientFile);
 //  //// fclose(pClientFile);

 //  // Sleep(1000*4);

	//return 0;
}

