// PlatformAgent.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

//#include "zredis.h"
#include "zsocket.h"
#include "INIReader.h"


void invokeProc()
{
    //system("redis-server.exe &"); 
    return; 
}

int main (int argc, char **argv)
{
    int i =0 ;
    printf("Start Agent Run...\n");
    invokeProc();

    ZSocket server;    
    bool bdb = server.connectDb(); 
    
    if ( bdb && server.initServer(1086) )
    {
        printf("Start waiting client...\n");
        //printf("start create 20.58 socket timeout = 6s\n");
        //server.connectServer("192.168.20.58",1086,6);
        //getchar();
        server.procLoop(10); 
    }
    else
    {
        printf("Connect database or init server failed, getchar to quit\n");
        getchar();
    }
   
   //redis.disconnect(); 
	return 0;
}
