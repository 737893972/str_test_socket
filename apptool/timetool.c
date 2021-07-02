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
#include "timetool.h"
#include "errcode.h"

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
** 函数描述:   
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

int GetTimeTm(struct tm * pTm)
{
	time_t timeSec = time(NULL);
	memcpy(pTm, localtime(&timeSec), sizeof(struct tm));
	
	printf("%04d-%02d-%02d %02d:%02d:%02d\n",\
		1900+pTm->tm_year, 1 + pTm->tm_mon, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);  
	return 0;
}

char* GetTimeStr(char* pStr, INT32 maxStrLen)
{
	ASSERT(pStr != NULL);
	
	struct tm stTime; 
	GetTimeTm(&stTime);
	
	snprintf(pStr, maxStrLen, "%04d%02d%02d%02d%02d%02d",\
		1900+stTime.tm_year, 1 + stTime.tm_mon, stTime.tm_mday, stTime.tm_hour, stTime.tm_min, stTime.tm_sec);

	return pStr;
}

BOOL ConvertCompileDate(char *pSrcDate, char *pDestDate, int maxDestLen)
{
	if(NULL == pSrcDate || NULL == pDestDate)
	{
		return FALSE;
	}

	const char englishMonth[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char dateStr[40] = {0};
	char monthStr[4] = {0};
	int iYear = 0;
	int iMonth = 0;
	int iDay = 0;
	int i = 0;

	//获取编译日期
	sprintf(dateStr, "%s", pSrcDate); /*格式为Sep 18 2010*/
	sscanf(dateStr, "%s %d %d", monthStr, &iDay, &iYear);

	for (i = 0;i < 12;i++)
	{
		if (strncmp(monthStr, englishMonth[i], 3) == 0)
		{
			iMonth = i + 1;
			break;
		}
	}

	snprintf(pDestDate, maxDestLen, "%04d%02d%02d", iYear, iMonth, iDay);
	//printf("get date:%s\n", pDestDate);

	return TRUE;
}


char* GetCompileTimeStr(char *pOutDate, int maxOutLen)
{
	int hour = 0;
	int min = 0;
	int sec = 0;
	char compliedate[32] = {0};
	ConvertCompileDate(__DATE__, compliedate, sizeof(compliedate));
	
	sscanf(__TIME__, "%02d:%02d:%02d", &hour, &min, &sec);

	snprintf(pOutDate, maxOutLen,"%s-%02d%02d%02d", compliedate, hour, min, sec);
	return pOutDate;
}


