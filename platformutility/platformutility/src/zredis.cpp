#include "zredis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//using namespace credisv2;

//#define _CRT_SECURE_NO_WARNINGS 

ZRedis::ZRedis()
{
    m_pRedis = 0;
}

ZRedis::~ZRedis()
{
    //just only prevention program
    disconnect();
}

int ZRedis::disconnect()
{
    if( m_pRedis != 0 )
    {
        credis_close(m_pRedis);
        m_pRedis = 0;
    }
        return 0;
}

int ZRedis::connect( const char *host)
{
    m_pRedis = credis_connect(host, 0, 10000);
    if(m_pRedis == NULL)
    {
        printf("Connect DB failed, db ip = %s\n", host);
    }
    memset(m_charBuf,'\0',CHARSIZE);
    return m_pRedis != NULL ? true : false;
}

int ZRedis::app_run()
{
    char *val = NULL;
    credis_get(m_pRedis, "RUN", &val);
    if(val != NULL)
            return atoi(val);       
    else 
            return 0;
}

int ZRedis::app_display()
{
    char *val = NULL;
    int i = 0;
    credis_get(m_pRedis, "DISPLAY", &val);
    if(val != NULL)
            i = atoi(val);  
        
    return i;
}

int ZRedis::app_displaybig()
{
    char *val = NULL;
    int i = 0;
    credis_get(m_pRedis, "DISPLAYBIG", &val);
    if(val != NULL)
            i = atoi(val);  
        
    return i;
}

int ZRedis::app_sleep()
{
    char *val = NULL;
    credis_get(m_pRedis, "SLEEP", &val);
    if(val != NULL)
       return atoi(val);       
    else 
       return 0;
}

int ZRedis::app_step(char *src)
{
    char *val = NULL;
    credis_getset(m_pRedis, src, "0", &val);
    if(val != NULL)
       return atoi(val);       
    else 
       return 0;
}

int ZRedis::app_rpush(int keyid, const PLAT_UINT8* content)
{
    char key[IDSIZE];
    memset(key, 0, IDSIZE);
    sprintf(key, "%08x", keyid);

    return app_rpush(key,content);
}

int ZRedis::app_llen(int keyid) 
{
    char key[IDSIZE];
    sprintf(key, "%08x", keyid);
    return app_llen(key);
}

int ZRedis::app_lpop(int keyid, PLAT_UINT8* outbuf)
{
    char key[IDSIZE];
    memset(key, 0, IDSIZE);
    sprintf(key, "%08x", keyid);
    return app_lpop(key,outbuf);
}

int ZRedis::app_rpush(char *key, const PLAT_UINT8* content)
{
    app2db(content,m_charBuf);
    return credis_rpush(m_pRedis, key, m_charBuf);
}

int ZRedis::app_llen(const char *dst)
{
   return credis_llen(m_pRedis, dst);
}

int ZRedis::app_lpop(const char *key, PLAT_UINT8* outbuf)
{
    char *val  = NULL;
    credis_lpop(m_pRedis,key,&val);
    if(val != NULL)
    {
        strncpy(m_charBuf,val,SIZE_L_MAX*2);
        db2app(m_charBuf,outbuf);
        return 1;
    }
    return 0;
}

int ZRedis::app_del(const char *key)
{
    return credis_del(m_pRedis, key);
}

int ZRedis::app_recv(char *key,PLAT_UINT8* outbuf)
{
    //if(key == NULL) 
    //{
    //    printf("please tellme where to recv!\n");
    //    return -1;
    //}

    //char *val = NULL;
    //PLAT_UINT8 u;
    //int i;

    //credis_get(m_pRedis, key, &val);

    //if(val != NULL)
    //{
    //    strncpy(m_charBuf,val,SIZE*2);
    //    db2app(m_charBuf,outbuf);
    //    return 1;
    //}
    //else
    //{
    //    printf("recv %s wrong\n",wid);
    //    return -1;
    //}
    return 0;
}

int ZRedis::app_set(const char *key, const PLAT_UINT8* content)
{
    app2db(content, m_charBuf);
    credis_set(m_pRedis, key, m_charBuf);
    
    return 0;
}

//return value: -1: failed
int ZRedis::app_get(const char *key, PLAT_UINT8* outbuf)
{
    char *val = NULL;
        
    int ret = -1;
    ret = credis_get(m_pRedis,key,&val);
    if(val != NULL)
    {
        strcpy(m_charBuf,val);
        db2app(m_charBuf,outbuf);
    }
    return ret;
}


void ZRedis::app2db(const PLAT_UINT8* appdata, char* dbbuf)
{
    int i;
    memset(dbbuf,'\0',CHARSIZE);
    for(i = 0;i < SIZE_L_MAX;i ++)
    {
            if(appdata[i] == 0)
            {
                    dbbuf[i] = 1;
                    dbbuf[i+SIZE_L_MAX] = 1;
            }
            else
            {
                    dbbuf[i] = 2; //could not use 0, because string end of 0
                    dbbuf[i+SIZE_L_MAX] = appdata[i];
            }
    }
    return;
}

//appdata size max is SIZE
void ZRedis::db2app(const char* dbbuf, PLAT_UINT8* appdata)
{
    PLAT_UINT8 u;

    for(int i = 0;i < SIZE_L_MAX;i ++)
    {
            u = (PLAT_UINT8) dbbuf[i];
            if(u == 1)
            {
                    appdata[i] = 0;
            }
            else
            {
                    appdata[i] = (PLAT_UINT8)dbbuf[i+SIZE_L_MAX];
            }
    }
}
