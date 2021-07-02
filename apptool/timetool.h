/**
 * FileName: timetool.h
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: 
 */

//---------------------------------------

#ifndef INCLUDE_TIMETOOL_H_
#define INCLUDE_TIMETOOL_H_

#include "glo_type.h"

char *DebugLogDateTime(void);

void MsSleep(int ms);

unsigned int GetTickCount(void);

unsigned int GetSysSecCnt(void);

char* GetTimeStr(char* pStr, INT32 maxStrLen);

BOOL ConvertCompileDate(char *pSrcDate, char *pDestDate, int maxDestLen);

char* GetCompileTimeStr(char *pOutDate, int maxOutLen);

#endif

