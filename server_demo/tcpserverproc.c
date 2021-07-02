/*
 * File name	: yx_tcpproc.c
 *
 * Created on	: 
 * Author		: 
 * Version		: 
 * Language		: C
 * Copyright	: Copyright 
 *
 */



#include "glo_type.h"
#include "upgrade_prot.h"
//#include "upgrade_process.h"
#include "errcode.h"
#include "devinfo.h"
#include "socketapi.h"
#include "crc16.h"
#include "timetool.h"

#include "tcpserverproc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h> 
#include <fcntl.h> 



static int gTcpSocket = -1;
static int sgCliCommFd[MAX_TCP_SERVER_ACCEPT_CNT] = {-1};

static U8* sgTcpSendBuf = NULL;

//the main process  of user customed functions
static PProcessCmdMsg sgTcpServCmdFunc[] = {TcpServRecStrTestProc, NULL};


INT32 TCPServerCacheInit(void)
{
	if (sgTcpSendBuf != NULL)
	{
		return ERR_SUCCESS;
	}
	sgTcpSendBuf = malloc(MAX_SERVER_BUF_SIZE);
	if (sgTcpSendBuf == NULL)
	{
	    return ERR_MALLOC;
	}

	return ERR_SUCCESS;
}

void TCPServerCacheFree(void)
{
	if (sgTcpSendBuf != NULL)
	{
		Free(sgTcpSendBuf);
		sgTcpSendBuf = NULL;
	}
	
}

/*******************************************************************
** 函数名:     
** 函数描述:TCP通信通道去初始化
** 返回:       true or false
********************************************************************/
BOOL TcpServerCommReset(void)
{
    #if APP_USE_SOCKET_TCP > 0
	int i = 0;
    int cliCnt = 0;
	if (gTcpSocket)
	{
	    PrintDebug("<reset tcp server>");
		close(gTcpSocket);
		gTcpSocket = -1;
	}

    for (i = 0; i < MAX_TCP_SERVER_ACCEPT_CNT; ++i)
    {
        if (sgCliCommFd[i] > 0)
        {
            close(sgCliCommFd[i]);
            sgCliCommFd[i] = -1;
            cliCnt++;
        }
    }
    PrintInfo("close tcp cli connection cnt %d", cliCnt);

	#endif
	
    return TRUE;
}

int TCPServerSocketInit(U16 port)
{
    sgTcpServCmdFunc[sizeof(sgTcpServCmdFunc)/sizeof(PProcessCmdMsg)-1] = NULL;
    
    memset(sgCliCommFd, -1, sizeof(sgCliCommFd));
    return TcpServSockConnect(port, &gTcpSocket);
}


/*******************************************************************
** 函数名:     TcpCommDeInit
** 函数描述:     去初始化
** 参数:       [in] 
** 返回:       true
********************************************************************/
int TcpServerCommDeInit(void)
{
	TCPServerCacheFree();
	TcpServerCommReset();
	return 0;
}




//user function 
INT32 ServerProcClientResp(U8* inBuf, U16 inLen, U8* outBuf, U16* pOutLen)
{
	int ret = 0;
    U16 charCnt = 0;
	ASSERT_DESC(outBuf != NULL, "outbuf null");
	ASSERT_DESC(pOutLen != NULL, "pOutLen null");
    if (inLen <= 0)
    {
        PrintWarn("rec empty str buf return");
        return ERR_SUCCESS;
    }
    charCnt = PackWordsInitials((const char*)inBuf, inLen, (char*)outBuf, MAX_SERVER_BUF_SIZE);
    *pOutLen = charCnt;
	return ret;
}

int TcpServRecStrTestProc(U8 cmd, U8 subcmd, U8* pDataBuf, U16 DataLen, U8* pOutBuf, U16* pOutLen)
{
	int ret  = ERR_SUCCESS;

    if (cmd != CMD_MAJOR_STR_TEST)
    {
        return ERR_PKG_CMD;
    }
    
	switch (subcmd)
	{
		case SUBCMD_TEST_STR:
		{
			ret = ServerProcClientResp(pDataBuf, DataLen, pOutBuf, pOutLen);
		}break;
		default:
		{
			ret = ERR_PKG_CMD;
			PrintErr("unsupport cmd %u", cmd);
			break;
		}
	}
	return ret;
}



//not use
int TcpServStrHandle(void)
{
	int ret  = 0;
    int respLen = 0;
	BYTE recvBuf[MAX_TCP_RESP_SIZE] = {0};

    ret = TcpAcceptCliConnection(gTcpSocket, &sgCliCommFd[0],sizeof(sgCliCommFd));
    ASSERT_DESC(ret != ERR_TCP_ERR, " serv fd accept err");

	ret = TcpCheckDataTimeout(sgCliCommFd[0], 1000, recvBuf, sizeof(recvBuf));
	if (ret <= 0)
	{
		//PrintInfo("CheckTcpData Timeout");
		return 0;
	}
	ret = TcpRecvProc(recvBuf, ret, sgTcpSendBuf, MAX_SERVER_BUF_SIZE, sgTcpServCmdFunc);
	if (ret > 0)
	{
	    respLen = ret;
        ret = SendTCPData(sgCliCommFd[0], sgTcpSendBuf, respLen);
		
        if (ret == ERR_TCP_ERR)
    	{
    	    PrintErr("resp cli fd err, close");
    	    close(sgCliCommFd[0]);
            sgCliCommFd[0] = -1;
        }
        else if(ret == ERR_SUCCESS)
        {
            PrintInfo("tcp server resp cli len %d succs", respLen);
        }
	}
    return ret;
}



int TcpServMultiCliProc(void)
{
    int ret  = 0;
    U16 cliIdx = 0;
    int respLen = 0;
	BYTE recvBuf[MAX_TCP_RESP_SIZE] = {0};
    
    ret = TcpAcceptCliConnection(gTcpSocket, &sgCliCommFd[0],MAX_TCP_SERVER_ACCEPT_CNT);
    ASSERT_DESC(ret != ERR_TCP_ERR, " serv fd accept err");
    
    ret = TcpCheckMultiCliData(gTcpSocket, &sgCliCommFd[0],MAX_TCP_SERVER_ACCEPT_CNT, \
            200, recvBuf, sizeof(recvBuf), &cliIdx);
    if (ret <= 0)
    {
        return ERR_NONE_DATA;
    }
    
    ret = TcpRecvProc(recvBuf, ret, sgTcpSendBuf, MAX_SERVER_BUF_SIZE, sgTcpServCmdFunc);
	if (ret > 0)
	{
	    ASSERT_DESC(cliIdx < MAX_TCP_SERVER_ACCEPT_CNT, "invalid client idx");
	    respLen = ret;
        ret = SendTCPData(sgCliCommFd[cliIdx], sgTcpSendBuf, respLen);
        if (ret == ERR_TCP_ERR)
    	{
    	    PrintErr("resp cli fd err, close fd idx %d", cliIdx);
    	    close(sgCliCommFd[cliIdx]);
            sgCliCommFd[cliIdx] = -1;
        }
        else if(ret == ERR_SUCCESS)
        {
            PrintInfo("tcp server resp cli len %d succs", respLen);
        }
	}
    return ret;
}


