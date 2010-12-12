#ifndef  _Z_REIDS_H
#define _Z_REIDS_H

/* (C) COPYRIGHT 2010 BY ZHEDAWANGXIN CORPORATION ALL RIGHTS RESERVED
 * Create Data: 2010-11-19
 * Version: 0.1
 * Author: Liuning
 * Contact: liuning@zdwxgd.com
 * Description: for platform simulator network layer
 */

#include "credisv2.h"
#include "platformhead.h"

//using namespace credisv2;

class ZRedis
{
public:
    ZRedis();
    ~ZRedis();

   /* init data buffer and credis */
    //if connect successful return 1, else return 0
    int connect(const char * host);

    /* close credis */
    int disconnect();
  
    int app_rpush(char *key, const PLAT_UINT8 * content);
    int app_llen(const char *key);
    int app_lpop(const char *key, PLAT_UINT8* outbuf);
    
    int app_rpush(int keyid, const PLAT_UINT8* content);
    int app_llen(int keyid); 
    int app_lpop(int keyid, PLAT_UINT8* outbuf);

    int app_run();
    
    int app_recv(char *key,PLAT_UINT8* outbuf);
   
    int app_display();               //控制是否平台打印
    int app_displaybig();            //控制是否打印大包

    int app_sleep();
    int app_step(char *);

    int app_set(const char *,const PLAT_UINT8 *);
    int app_get(const char *,PLAT_UINT8* );

private:
    char* app_getid();
    void app2db(const PLAT_UINT8* appdata, char* dbbuf);
    void db2app(const char* dbbuf, PLAT_UINT8* appdata);

private:
    REDIS m_pRedis;
    char m_charBuf[CHARSIZE];
   
   
};

#endif //_Z_REIDS_H
