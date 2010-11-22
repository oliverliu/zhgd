#include "zredis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ZRedis::ZRedis()
{
    m_pRedis = 0;
}

ZRedis::~ZRedis()
{
     app_unInit();
}

int ZRedis::app_unInit()
{
    if( m_pRedis != 0 )
    {
	    credis_close(m_pRedis);
        m_pRedis = 0;
    }
	return 0;
}

int ZRedis::app_init( const char *host)
{
	m_pRedis = credis_connect(host, 0, 10000);

	memset(m_charBuf,'\0',CHARSIZE);
	return m_pRedis != NULL ? 1 : 0;
}

int ZRedis::app_run()
{
	char *val;
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
	char *val;
	credis_get(m_pRedis, "SLEEP", &val);
	if(val != NULL)
		return atoi(val);	
	else 
		return 0;
}

int ZRedis::app_step(char *src)
{
	char *val;
	credis_getset(m_pRedis, src, "0", &val);
	if(val != NULL)
		return atoi(val);	
	else 
		return 0;
}

int ZRedis::app_rpush(char *key, const PLAT_UINT8* content)
{
    app2db(content,m_charBuf);
	credis_rpush(m_pRedis, key, m_charBuf);
	
	return 0;
}

int ZRedis::app_llen(const char *dst)
{
	return credis_llen(m_pRedis, dst);
}

int ZRedis::app_lpop(const char *key, PLAT_UINT8* outbuf)
{
	char *val;
	credis_lpop(m_pRedis,key,&val);
	if(val != NULL)
	{
		strncpy(m_charBuf,val,SIZE*2);
        db2app(m_charBuf,outbuf);
        return 1;
	}
	return 0;
}

int ZRedis::app_recv(char *key,PLAT_UINT8* outbuf)
{
    //if(key == NULL) 
    //{
    //    printf("please tellme where to recv!\n");
    //    return -1;
    //}

    //char *val;
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

int ZRedis::app_set(char *key, char *buf)
{
	credis_set(m_pRedis,key,buf);
	return 0;
}

int ZRedis::app_get(char *key, char *buf)
{
	char *val;
	
	int ret = 0;
    ret = credis_get(m_pRedis,key,&val);
	if(val != NULL)
	{
		strcpy(buf,val);
	}
	return ret;
}


void ZRedis::app2db(const PLAT_UINT8* appdata, char* dbbuf)
{
    int i;
	memset(dbbuf,'\0',CHARSIZE);
	for(i = 0;i < SIZE;i ++)
	{
		if(appdata[i] == 0)
		{
			dbbuf[i] = 1;
			dbbuf[i+SIZE] = 1;
		}
		else
		{
            dbbuf[i] = 2; //could not use 0, because string end of 0
			dbbuf[i+SIZE] = appdata[i];
		}
	}
    return;
}

//appdata size max is SIZE
void ZRedis::db2app(const char* dbbuf, PLAT_UINT8* appdata)
{
	PLAT_UINT8 u;

	for(int i = 0;i < SIZE;i ++)
	{
		u = (PLAT_UINT8) dbbuf[i];
		if(u == 1)
		{
			appdata[i] = 0;
		}
		else
		{
			appdata[i] = (PLAT_UINT8)dbbuf[i+SIZE];
		}
	}
}