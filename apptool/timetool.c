/**
 * FileName: timetool.c
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: 
 */

//---------------------------------------
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define OUTPUT_TIME_FORMAT	"%04d-%02d-%02d %02d:%02d:%02d.%03ld"//

char *DebugLogDateTime(void)
{
	static char timeBuf[64] = {0};
	struct timeval tv;
    struct timezone tz;   
    struct tm *t;
     
    gettimeofday(&tv, &tz);
	
	t = localtime(&tv.tv_sec);

	sprintf(timeBuf, OUTPUT_TIME_FORMAT, t->tm_year+1900, t->tm_mon+1, \
			t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,tv.tv_usec/1000);

	return timeBuf;
}



/*
 * 
 */
void MsSleep(int ms)
{

	struct timeval delay;
	delay.tv_sec = ms / 1000;
	delay.tv_usec = (ms % 1000) * 1000;
	select(0, NULL, NULL, NULL, &delay);
}

/*
 * 
 */
unsigned int GetTickCount(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/*******************************************************************
** 函数名:     GetSysSecond
** 函数描述:   获取系统时间的秒数
** 参数:       [in] 无
**            
** 返回:         秒数
********************************************************************/
unsigned int GetSysSecCnt(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec;
}


