// PlatformAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "zsocket.h"

int main (int argc, char **argv)
{
  
    ZSocket server;
    server.setLog("agent_server.log");
    
    if(server.preRead())
        server.startReadLoop();

    //ZSocket client;
    //int socketid = client.getWriteSocket("127.0.0.1");

	return 0;
}
