// PlatformAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "zsocket.h"

#include "zredis.h"

void invokeProc()
{
    //system("redis-server.exe &");
    return; 
}

int main (int argc, char **argv)
{
    invokeProc();

    ZSocket server;
    //server.setLog("agent_server.log");
    server.setLog();

    bool bdb = server.connectDb();
 
    if ( bdb && server.create() != -1
        && server.bind(1086) 
        && server.listen())
    {
        server.procLoop(10);
    }
   
    printf("Exist agent loop, now bdb = %d\n", bdb);
    //ZSocket client;
    //int socketid = client.getWriteSocket("127.0.0.1");

    //redis.disconnect();
	return 0;
}
