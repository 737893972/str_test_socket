/*
 * File name	: devinfo.h
 *
 * Created on	: 
 * Author		: 
 * Version		: 
 * Language		: C
 * Copyright	: Copyright 
 *
 */


#ifndef INCLUDE_DEVINFO_H_
#define INCLUDE_DEVINFO_H_



#if 1

#include "glo_type.h"

#define OP_NAME_MAX_LEN			64		

typedef struct _param_property
{
	char word[OP_NAME_MAX_LEN];
}TParamProp, *PParamProp;







int GetNetMAC(const char* ifName, char *outBuf, INT32 maxOutLen);

int GetIPAddr(const char *ifName, char *ipAddr, U32 addrLen);

unsigned long GetFileSize(const char *path);

int GetPartitionFree(const char *dir, int unit, U32* pFreeSize);

int GetFileCkSum(const char* file, U16* cksum, U32 maxReadSize);

int GetFileContent(char* fileName, U16 index, U16 pkgSize, U8* outBuf, INT32 maxOutLen);

void Chmod(const char* srcFile, U32 mode);

void RegisterSignalProc(void);

BOOL CheckFilePathExist(const char* filePath);

BOOL CreateFolder(const char* path);

INT32 CopyFile(const char* oldFile, const char* newFile);

int GetStdinStr(char* outBuf, int maxOutLen, FILE *stream);

void PrintHexDescp(unsigned char *inbuf,int inLen, char* description);

void PrintHexBuf(unsigned char *buf,int len);

BOOL CheckUpdateProcAlive(U32 lastTick, U8 timeSec);

int PackWordsInitials(const char *inBuf, int maxBufLen, char* outBuf, int maxOutLen);

#endif /*  */

#endif
