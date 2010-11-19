// testAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "zsocket.h"

#include <stdlib.h>

static char * logfile = "testAgent_log.txt";
int _tmain(int argc, _TCHAR* argv[])
{
    char  data[10];
	int   byteSent;
	int   byteRead;
    int     errNumber;

	FILE *  pClientFile = fopen (logfile, "w");
    fclose(pClientFile);

    /* open the file read */
	FILE *  pReadFile = fopen ("c:\\read.txt", "r"); 
	if (pReadFile == NULL)
	{
		fprintf (pClientFile, "read.txt failed to be opened!\n");
		return -1;
	}

    ZSocket client;
    int socketid = client.getWriteSocket("127.0.0.1");

    
    pClientFile = fopen (logfile, "a+");

    bool bFinishRead = false;

    while (1)
    {
        TSafeSocket_FdSet m_recvSet, m_sendSet;
        SS_FD_ZERO(&m_recvSet);
        SS_FD_ZERO(&m_sendSet);
        SS_FD_SET(socketid,&m_recvSet);
        SS_FD_SET(socketid,&m_sendSet);

        static bool bfirst = false;
		if (!bfirst)
		{
			bfirst = true;
			memset(data,0,10);

			byteRead = fread (data, 1, 4, pReadFile);
            if (byteRead <= 0)
            {
                printf("Error: from file\n");
                continue;
            }
            printf("%s\n",data);
			byteSent = client.write(socketid, data, byteRead, SS_DATA_UNIMPORTANT);
		
			if (byteSent == SS_FAILED)
			{
				errNumber = safeSocket_getErrorNum (socketid);
				fprintf (pClientFile, "data failed to be sent, errNumber = %u\n", errNumber);
				return -1;
			}
              /* indicate data has been sent */
			fprintf (pClientFile, "write out1: msg contains %u bytes - %s\n", byteSent, data);
		}

		int readsocks = safeSocket_select (0, &m_recvSet, &m_sendSet, NULL,  NULL);
        if (readsocks < 0) 
		{
			fprintf(pClientFile,"select error: data failed to be received!\n");
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
			if (SS_FD_ISSET(socketid,&m_recvSet))
            {
                char data[1024];
                memset(data, 0, 1024);
                int flag;
                client.read(socketid,data,1023, &flag);
                fprintf(pClientFile, "Receive in: %s\n", data);
            }
            if (SS_FD_ISSET(socketid,&m_sendSet))
            {
                if( !bFinishRead )
                {
                    memset(data,0,10);

		            byteRead = fread (data, 1, 4, pReadFile);
                    if (byteRead <= 0)
                    {
                        printf("Error: from file\n");
                        continue;
                    }
                    printf("%s\n",data);
                    byteSent = client.write(socketid, data, byteRead, SS_DATA_UNIMPORTANT);

		            /* indicate data has been sent */
		            fprintf (pClientFile, "write out: msg contains %u bytes - %s\n", byteSent, data);

		            if (byteSent == SS_FAILED)
		            {
			            errNumber = safeSocket_getErrorNum (socketid);
			            fprintf (pClientFile, "data failed to be sent, errNumber = %u\n", errNumber);
			            return -1;
		            }
                }
                else
                {
                    printf("has finished reading file!!!\n");
                    break;
                }
            }//(SS_FD_ISSET(socketid,&m_sendSet))

            if (byteSent < 4)
            {
                fflush(pClientFile);
                bFinishRead = true;
                fclose(pReadFile);
            }
		}
        
        fflush(pClientFile);
       
    }

    while(1)
    {
        Sleep(2000);
        fflush(pClientFile);
    }
   //
    fclose(pClientFile);
   //// fclose(pClientFile);

   // Sleep(1000*4);

	return 0;
}

