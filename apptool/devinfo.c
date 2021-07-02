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


#if 1

#include "devinfo.h"
#include "errcode.h"
#include "crc16.h"


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <time.h> 
#include <assert.h> 
#include <ctype.h>




#define MAX_MAC_SIZE			MAX_LEN_24
#define ETH_ALEN	6




/*
 * 
 * 
 */
void PrintHexBuf(unsigned char *buf,int len)
{
	#if FTP_DEBUG_HEX > 0
	int i;
	{
		printf("  hex trace,len =%d\n    ",len);
		for(i = 0;i<len;i++)
		{
			if(i%16 ==0 && i !=0)
				printf("\n    ");
			printf("%02x,",buf[i] & 0xff);
		}
		printf("\n");
	}
	#endif
}

/*
 * 鑾峰彇MAC鍦板潃
 */
int GetNetMAC(const char* ifName, char *outBuf, INT32 maxOutLen)
{
	if (ifName == NULL || outBuf == NULL)
	{
		return ERR_PARAM_INVALID;
	}
	
	unsigned char macaddr[ETH_ALEN];

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq req;
	int err;

	strcpy(req.ifr_name, ifName);
	err=ioctl(s,SIOCGIFHWADDR, &req);
	close(s);
	
	if(err != -1)
	{
		memcpy(macaddr, req.ifr_hwaddr.sa_data, ETH_ALEN);
		snprintf(outBuf, maxOutLen, "%.2X%.2X%.2X%.2X%.2X%.2X",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
		return ERR_SUCCESS;
	}
	else
	{
		return ERR_EXECUTE_FAIL;
	}
}

int GetIPAddr(const char *ifName, char *ipAddr, U32 addrLen)
{
    int ret = -1;
    struct ifreq ifr;
    struct sockaddr_in *sin;
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name, ifName, sizeof(ifr.ifr_name));
	//PrintInfo("ifname:%s\n", ifr.ifr_name);

    if (0 > (ret = ioctl(sock, SIOCGIFADDR, &ifr)))
    {
        PrintErr("ioctl get SIOCGIFADDR failed!!");
        return ERR_EXECUTE_FAIL;
    }
    close(sock);

    sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    if (NULL == inet_ntoa(sin->sin_addr))
    {
        return ERR_EXECUTE_FAIL;
    }
    strncpy(ipAddr, inet_ntoa(sin->sin_addr), addrLen);

    return ERR_SUCCESS;
}

unsigned long GetFileSize(const char *path)  
{  
    unsigned long filesize = 0;   
    struct stat statbuff;  
	
    if(stat(path, &statbuff) < 0)
	{  
		PrintInfo("err get file size,%s",path);
        return 0;  
    }
	else
	{  
        filesize = statbuff.st_size;  
		PrintInfo("get file:%s size %lu", path, filesize);
    }  
    return filesize;  
}  



//unit: 0-MBytes, 1-KBytes, default MBytes
int GetPartitionFree(const char *dir, int unit, U32* pFreeSize)
{
    struct statfs diskInfo;  
	//U32 freeSize = 0;
	int ret = -1;

	if (pFreeSize == NULL || dir == NULL)
	{
		return ERR_PARAM_INVALID;
	}
	
    ret = statfs(dir, &diskInfo);  
	if ( ret != 0 )
	{
		PrintErr("getPartitionUse statfs:%s error", dir);
		return ERR_EXECUTE_FAIL;
	}
	
    U64 allBlocks = diskInfo.f_bsize;  
    //U64 tmpTotalSize = allBlocks * diskInfo.f_blocks;  
    U64 tmpFreeDisk = diskInfo.f_bfree*allBlocks;  
    //totleSize = tmpTotalSize>>20;  
	//freeSize  = tmpFreeDisk >> 20;
	
	if(unit == 0)
	{	
		//totleSize	= tmpTotalSize>>20;  
		*pFreeSize  = tmpFreeDisk;// >> 20;  
	}
	else if( unit == 1 )
	{
		//totleSize	= tmpTotalSize>>10;  
		*pFreeSize  = tmpFreeDisk >> 10; 
	}

	PrintInfo("%s have %u freespace", dir, *pFreeSize);
	return 0;
}


int GetFileCkSum(const char* file, U16* cksum, U32 maxReadSize)
{
	FILE * pFile = NULL;
	U8* cacheBuf = NULL;
	U16 crcValue = 0;
	int cnt = 0;
	
	if((pFile = fopen(file, "r")) == NULL) //
	{
		PrintErr("Open File %s Fail!", file);
		return ERR_FILE_ERR;
	}

	if ( (cacheBuf = malloc(maxReadSize)) == NULL )
	{
	    PrintErr("err malloc len %u!", maxReadSize);
		return ERR_MALLOC;
	}

	fseek(pFile, 0, SEEK_SET);

	while(!feof(pFile)) 
	{
		cnt = fread(cacheBuf, 1, maxReadSize, pFile);
		Crc16CalcBufRepeate(cacheBuf, cnt, &crcValue);
	}

	Free(cacheBuf);
	*cksum = crcValue;

	return ERR_SUCCESS;
	
}


int GetFileContent(char* fileName, U16 index, U16 pkgSize, U8* outBuf, INT32 maxOutLen)
{
	static FILE * pFile = NULL;
	int realbyte = 0;
	long offset = 0;

	assert(fileName != NULL);
	
	if (pFile == NULL)
	{
		if((pFile = fopen(fileName, "rb")) == NULL) 
		{
			PrintErr(" Open dev File Fail!");
			return ERR_FILE_ERR;
		}
		PrintInfo(" Open dev File ok!");
	}
	

	if (index <= 0)
	{
		PrintErr("err index 0!!!!!!!!!");
		index = 1;
	}
	offset = (index-1)*pkgSize;
	
	fseek(pFile, offset, SEEK_SET);

	if(!feof(pFile)) 
	{
		realbyte = fread ( outBuf, 1, pkgSize, pFile );
	}
	else
	{
		PrintErr("close(pFile)");
		fclose(pFile);
		pFile = NULL;
	}

	
	PrintInfo(" get index %d realbyte %d", index, realbyte);
	if (realbyte < pkgSize && pFile)
	{
		PrintErr("fclose(pFile)");
		fclose(pFile);
		pFile = NULL;
	}
	return realbyte;
}


int GetFileContentTest(char* fileName, U8* outBuf, INT32 maxOutLen)
{
	FILE * pFile = NULL;
	int realbyte = 0;
	long offset = 0;
	U16 index = 0;
	U16 pkgSize = 10000;

	assert(fileName != NULL);
	
	if (pFile == NULL)
	{
		if((pFile = fopen(fileName, "rb")) == NULL) 
		{
			PrintErr(" Open dev File Fail!");
			return ERR_FILE_ERR;
		}
		PrintErr(" Open dev File ok!");
	}
	
	
	index = 1;
	fseek(pFile, 0, SEEK_SET);

	while(!feof(pFile)) 
	{
		offset = (index-1)*pkgSize;
		fseek(pFile, offset, SEEK_SET);
		realbyte = fread ( outBuf, 1, pkgSize, pFile );
		PrintInfo(" get index %d realbyte %d", index, realbyte);
		index++;
	}

	
	
	PrintErr("close(pFile)");
	fclose(pFile);
	pFile = NULL;

	return realbyte;
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
	assert(pStr != NULL);
	
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

void Chmod(const char* srcFile, U32 mode)
{
	#include <sys/types.h>  
	#include <sys/stat.h>
	chmod(srcFile, mode);
}



void DumpTraceDebug(void)  
{  
	#include <execinfo.h>     /* for backtrace() */  
	#define BACKTRACE_SIZE   16  
	
    int j, nptrs;  
    void *buffer[BACKTRACE_SIZE];  
    char **strings;  
      
    nptrs = backtrace(buffer, BACKTRACE_SIZE);  
      
    PrintWarn("backtrace() returned %d addresses", nptrs);  
  
    strings = backtrace_symbols(buffer, nptrs);  
    if (strings == NULL) {  
        perror("backtrace_symbols");  
        exit(EXIT_FAILURE);  
    }  
  
    for (j = 0; j < nptrs; j++)  
        printf("  [%02d] %s\n", j, strings[j]);  
  
    Free(strings);  
}
#include <signal.h>  

static void SigHandleCb(int signo)  
{  
	
	PrintInfo("rec signal %d", signo);

	if (signo == SIGINT)
	{
		PrintWarn("rec exit %d signal", SIGINT);
        //TODO deinit
		exit(0);
	}
	
	#if 0 

	GetProcMaps();

    PrintDebug("\n=========>>>catch signal %d <<<=========\n", signo);  
      
    PrintErr("Dump stack start...\n");  
    DumpTraceDebug();  
    PrintErr("Dump stack end...\n");  
  
    signal(signo, SIG_DFL); /* 恢复信号默认处理 */  
    raise(signo);           /* 重新发送信号 */  
	#endif    
	
	return; 		
}

void RegisterSignalProc(void)
{
	signal(SIGALRM, SigHandleCb);
	signal(SIGINT, SigHandleCb); 
	signal(SIGPIPE, SIG_IGN);
}
  
BOOL CheckFilePathExist(const char* filePath)
{
	if (access(filePath, F_OK) == 0)
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CreateFolder(const char* path)
{
	if (access(path, F_OK) == 0)
	{
		return FALSE;
	}
	if(mkdir(path, 0755) == -1)
	{   
		return FALSE;
	}
	return TRUE;
}

INT32 CopyFile(const char* oldFile, const char* newFile)
{
	#define MAX_MOVE_SIZE	(30*1024)
	FILE * pFile = NULL;
	FILE * pDestFile = NULL;
	U8* cacheBuf = NULL;
	int rBytes = 0;
	int wBytes = 0;
	int ret = 0;
	
	if (oldFile == NULL || newFile == NULL || oldFile[0] == 0 || newFile[0] == 0 ){
		return ERR_PARAM_INVALID;
	}
	if (0 == strcmp(oldFile, newFile))
	{
		return ERR_FILE_ERR;
	}
	/*if (access(newFile, F_OK) == 0){
		PrintErr("dest %s existed\n", newFile);
		return ERR_FILE_ERR;
	}*/
    do
    {
        if((pFile = fopen(oldFile, "r")) == NULL){
    		PrintErr("open src %s err", oldFile);
    		ret = ERR_FILE_ERR;
            break;
    	}
    	if((pDestFile = fopen(newFile, "w")) == NULL){
    		PrintErr("open dest %s err", newFile);
    		ret = ERR_FILE_ERR;
            break;
    	}

    	if ( (cacheBuf = malloc(MAX_MOVE_SIZE)) == NULL )
    	{
    		ret = ERR_MALLOC;
            break;
    	}
    	fseek(pFile, 0, SEEK_SET);
    	fseek(pDestFile, 0, SEEK_SET);

    	while(!feof(pFile)) 
    	{
    		rBytes = fread(cacheBuf, 1, MAX_MOVE_SIZE, pFile);
    		wBytes = fwrite (cacheBuf, 1, rBytes, pDestFile);
    		if (wBytes < rBytes)
    		{
    			PrintErr("write len %d less than read len %d", wBytes, rBytes);
    			ret = ERR_FILE_ERR;
    			break;
    		}
    	}
    }
    while (0);

    if (cacheBuf){
        Free(cacheBuf);
    }
	if (pFile){
        fclose(pFile);
    }
    if (pDestFile){
        fclose(pDestFile);
    }	
	return ret;
}

U8* GetJsonTestData(int* outLen)
{
	static U8 json_test_ft[] = {};
    *outLen = 0;//sizeof(json_test_ft);
    return json_test_ft;

}

int GetAPList(const char* fileName, U8 listIP[][20], int maxListCnt)
{
    FILE * pFile = NULL;
    int cnt = 0;
    char infobuf[200]={0};
    
    if((pFile = fopen(fileName, "r")) == NULL) 
    {
        PrintErr("Open File %s Fail!", fileName);
        return ERR_FILE_ERR;
    }

    fseek(pFile, 0, SEEK_SET);
    while(fgets(infobuf, sizeof(infobuf), pFile) != NULL)  
    {  
        //printf("%s\n",infobuf);
        if(strstr(infobuf,"IP address") != NULL)
        {
            continue;
        }
        sscanf(infobuf, "%s %*s %*s %*s %*s %*s", &listIP[cnt][0]);
        PrintDebug("get ip %s", listIP[cnt]);
        cnt++;
        if (cnt >= maxListCnt)
        {
            break;
        }
    } 
    fclose(pFile);
    
    return cnt;
}


void PrintHexDescp(unsigned char *inbuf,int inLen, char* description)
{
	int i;
	printf("  %s hex trace,len =%d\n    ",description, inLen);
	for(i = 0; i < inLen; i++)
	{
		if(i%16 ==0 && i !=0)
			printf("\n    ");
		printf("%02x ",inbuf[i] & 0xff);
	}
	printf("\n");
}

int GetStdinStr(char* outBuf, int maxOutLen, FILE *stream)
{
    int recLen = 0;
    ASSERT_DESC(outBuf != NULL, "stream out buf null");
    ASSERT_DESC(stream != NULL, "stream addr NULL");
    
    printf("\nplease input your words(length less than %d):\n", maxOutLen);
	if (NULL == fgets(outBuf, maxOutLen, stream))
	{
		return ERR_NONE_DATA;
	}
    
    recLen = strlen(outBuf) - 1;
    outBuf[recLen] = '\0';
    PrintInfo("echo str len %d:%s", recLen, outBuf);
    
    return recLen;
}



int GetWordsFromStr(const char *inBuf, int maxBufLen, const char* pchDilem, PParamProp pWordBuf, int maxWordCnt) 
{
	char *pTempBuf = NULL; ;
	char *pchStrTmpIn = NULL;
	char *pWord = NULL;
    int count = 0;

    ASSERT_DESC(inBuf != NULL, "inbuf null");
    ASSERT_DESC(pchDilem != NULL, "pchDilem null");
    ASSERT_DESC(pWordBuf != NULL, "pWordBuf null");
    
    if ((pTempBuf = (char*)malloc(maxBufLen + 1)) == NULL){
        //ASSERT(pTempBuf != NULL);
        return ERR_MALLOC;
    }
	snprintf(pTempBuf, maxBufLen + 1, "%s",inBuf);
	pWord = pTempBuf;
    
	while(NULL != ( pWord = strtok_r( pWord, pchDilem, &pchStrTmpIn) ))
	{
	    /*if( strlen(pWord) > OP_NAME_MAX_LEN)
		{
			PrintErr("word len too long", pWord[0]);
			continue;
		}*/
        
		snprintf(pWordBuf[count].word, OP_NAME_MAX_LEN, "%s", pWord);
		PrintInfo("get word [%s]",pWordBuf[count].word);
	    count ++;
        if(count >= maxWordCnt)
		{
			PrintWarn("words cnt reach the max num %d", maxWordCnt);
			continue;
		}
		pWord = NULL;
	}
    
    Free(pTempBuf);
	return count;
}


int PackWordsInitials(const char *inBuf, int maxBufLen, char* outBuf, int maxOutLen)
{
    int packCnt = 0;
    int charCnt = 0;
    int i = 0;
    TParamProp stParams[100];
    ASSERT_DESC(outBuf != NULL, "outbuf null");
    
    memset(&stParams[0], 0, sizeof(stParams));
    
    packCnt = GetWordsFromStr(inBuf, maxBufLen, " ", &stParams[0], sizeof(stParams)/sizeof(TParamProp));
    if (packCnt <= ERR_SUCCESS)
    {
        return 0;
    }
    
    for (i = 0; i < packCnt; ++i)
    {
        if (isalpha(stParams[i].word[0]))
        {
            outBuf[charCnt] = toupper(stParams[i].word[0]);
            ++charCnt;
            if (charCnt >= (maxOutLen - 1))
            {
                PrintWarn("words Initials num reach max %d,break", maxOutLen);
                break;
            }
        }
        else
        {
            continue;
        }
    }
    outBuf[++charCnt] = '\0';
    
    return charCnt;
}


BOOL CheckUpdateProcAlive(U32 lastTick, U8 timeSec)
{
	if (lastTick > 0 && GetSysSecCnt() - lastTick > timeSec)
	{
		//PrintErr("timeout wait server data");
		return TRUE;
	}
    return FALSE;
}

#endif

