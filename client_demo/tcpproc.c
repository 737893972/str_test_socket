/********************************************************************************
**
** 文件名:  tcpproc.c
** 版权所有:   (c) 2007-2020 
** 文件描述:   
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   |  修改记录
**===============================================================================
**| 2020/07/19 | Alan |  创建第一版本
*********************************************************************************/

#if 1


#include "glo_type.h"
#include "upgrade_prot.h"
#include "tcpproc.h"
#include "errcode.h"
#include "socketapi.h"
#include "devinfo.h"


#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


/*
********************************************************************************
*宏定义
********************************************************************************
*/

#define DYNAMIC_BUF		1
#define CONT_TIMEOUT	(5)
#define APP_USE_SOCKET_UDP		1	//在应用层直接使用socket
#define APP_USE_SOCKET_TCP		1	//在应用层直接使用socket

/*
********************************************************************************
* 定义模块变量
********************************************************************************
*/


//static PProcessCmdMsg sgUdpCmdFunc[] = {NULL, NULL};
static PProcessCmdMsg sgTcpCmdFunc[] = {StrTestCmdProc, NULL};




//static U8 stServAddr[MAX_SOCKET_ADDR_LEN] = {0};
//static U16 servAddrLen;

#if DYNAMIC_BUF > 0
//UDP
//static U8* sgUdpSendBuf = NULL;
//static U8* sgUdpRecvBuf = NULL;

//TCP
static U8* sgTcpSendBuf = NULL;
static U8* sgTcpRecBuf = NULL;
#else
static U8 sgUdpSendBuf[MAX_UDP_SEND_SIZE] = {};
static U8 sgTcpSendBuf[MAX_TCP_RESP_SIZE] = {};
static U8 sgTcpRecBuf[MAX_SLAVE_BUF_SIZE] = {};

#endif



#if APP_USE_SOCKET_TCP > 0

static int  sgTcpCliFd = -1;

BOOL GetTCPSocket(void)
{
	return (sgTcpCliFd > 0? TRUE : FALSE);
}

#endif


/*******************************************************************
** 函数名:     TCPClientCacheInit
** 函数描述:         TCP 发送缓冲初始化
** 参数:     无
** 返回:       无
********************************************************************/
INT32 TCPClientCacheInit(void)
{
#if DYNAMIC_BUF > 0 
    int ret = ERR_MALLOC;
    do
    {
        if (sgTcpRecBuf != NULL)
        {
        	;
        }
        else
        {
        	sgTcpRecBuf = (U8*)malloc(MAX_SLAVE_BUF_SIZE);
        	if (sgTcpRecBuf == NULL)
        	{
        		PrintErr("malloc tcp rec buf failed");
        		break;
        	}
        }

        if (sgTcpSendBuf != NULL)
        {
        	;
        }
        else
        {
        	sgTcpSendBuf = (U8*)malloc(MAX_TCP_RESP_SIZE);
        	if (sgTcpSendBuf == NULL)
        	{
        		PrintErr("malloc tcp send buf failed");
        		break;
        	}
        }
        ret = ERR_SUCCESS;
    }
    while (0);

    if (ret < ERR_SUCCESS)
    {
        if (sgTcpRecBuf)
        {
            Free(sgTcpRecBuf);
        }
        if (sgTcpSendBuf)
        {
            Free(sgTcpSendBuf);
        }
    }
	else
	{
		PrintInfo("tcp cache init succs");
	}
    return ret;
#else
    return ERR_SUCCESS;
#endif
}

/*******************************************************************
** 函数名:     TCPClientCacheFree
** 函数描述:         TCP 发送缓冲去初始化
** 参数:     无
** 返回:       无
********************************************************************/
BOOL TCPClientCacheFree(void)
{
#if DYNAMIC_BUF > 0 

    if (sgTcpRecBuf != NULL)
    {
    	Free(sgTcpRecBuf);
    	sgTcpRecBuf = NULL;
    }

    if (sgTcpSendBuf != NULL)
    {
    	Free(sgTcpSendBuf);
    	sgTcpSendBuf = NULL;
    }
#else
    ;
#endif
    return TRUE;
}

/*******************************************************************
** 函数名:     TCPTryConnectServer
** 函数描述:     连接TCP服务器并发送确认
** 参数:       [in] pdata：定时器特征值
** 返回:       无
********************************************************************/
int TCPTryConnectServer(const char* pIPStr, U16 port)
{
	return SocketTryConnect(pIPStr, port, CONT_TIMEOUT, &sgTcpCliFd);
}





/*******************************************************************
** 函数名:     
** 函数描述:   通道数据接收接口
** 参数:       [out] outBuf: 数据指针
**             [in] maxOutLen: 数据长度
** 返回:       成功返回数据长度，无数据返回错误码
********************************************************************/
int TcpRecvData(U8* outBuf, U32 maxOutLen)
{
	#if APP_USE_SOCKET_TCP > 0
    ASSERT_DESC(outBuf != NULL, "tcp rec buf null");
    
	if (sgTcpCliFd <= 0) 
	{
		return ERR_TCP_NO_CONT;
	}
	if (maxOutLen <= 0){
		return ERR_PARAM_INVALID;
	}
	return RecvTCPData(sgTcpCliFd, outBuf, maxOutLen);
	
	#endif
}


/*******************************************************************
** 函数名:     
** 函数描述:   Tcp通道数据发送接口
** 参数:       [in] inBuf: 数据指针
**             [in] inLen: 数据长度
** 返回:       成功返回true，失败返回false
********************************************************************/
int TcpSendData(U8 *inBuf, U16 inLen)
{
    int result = FALSE;

	#if APP_USE_SOCKET_TCP > 0
	ASSERT_DESC(inBuf != NULL, "tcp send buf null");
    
	if (sgTcpCliFd <= 0) 
	{
        return ERR_TCP_NO_CONT;
    }
    
	if (SendTCPData(sgTcpCliFd, inBuf, inLen) >= 0)
	{
        result = ERR_SUCCESS;
    }
	#endif	
    
    return result;
}



/*******************************************************************
** 函数名:     
** 函数描述:TCP通信通道去初始化
** 返回:       true or false
********************************************************************/
BOOL TcpCommReset(void)
{
    #if APP_USE_SOCKET_TCP > 0
	
	if (sgTcpCliFd)
	{
	    PrintWarn("<reset tcp cli>");
		close(sgTcpCliFd);
		sgTcpCliFd = -1;
	}

	#endif
	
    return TRUE;
}

/*******************************************************************
** 函数名:     TcpCommDeInit
** 函数描述:     去初始化
** 参数:       [in] 
** 返回:       true
********************************************************************/
int TcpCommDeInit(void)
{
	TCPClientCacheFree();
	TcpCommReset();
	return 0;
}

int TCPClientSendData(U8* inBuf, INT32 inLen)
{
    INT32 ret = -1;
    INT32 sendLen = 0;
    ASSERT_DESC(inBuf != NULL,  "inBuf null");

    ret = TcpDataPackWithProp(inBuf, inLen, sgTcpSendBuf, MAX_SERVER_BUF_SIZE);
    if (ret <= ERR_SUCCESS)
    {
        return ret;
    }
    sendLen = ret;
    ret = SendTCPData(sgTcpCliFd, sgTcpSendBuf, sendLen);
	if (ret < 0)
	{
		PrintErr("send tcp msg fail, errno=%d,%m", errno);
	}
    else
    {
        PrintDebug("send tcp msg ok");
        PRINT_HEX(sgTcpSendBuf, sendLen, "tcp send");
    }
    return ret;
}

/*******************************************************************
** 函数名:     GetUpgeUpdateInfo
** 函数描述:     UDP server means Tbox resp dev info msg
** 参数:       [in] inBuf [in] inLen [in] outBuf [in] pOutLen 
** 返回:     成功
********************************************************************/
INT32 ProcStrAbbreviationTest(U8* inBuf, U16 inLen, U8* outBuf, U16* pOutLen)
{
	int ret = 0;
	ASSERT_DESC(outBuf != NULL, "outbuf null");
	ASSERT_DESC(pOutLen != NULL, "pOutLen null");
	printf("\nget server resp msg str:[%s]\n", inBuf);

	*pOutLen = 0;
	return ret;
}


/*******************************************************************
** 函数名:     StrTestCmdProc
** 函数描述:     处理tcp server的命令
** 参数:       [in] cmd  [in] inBuf [in] inLen [in] outBuf [in] pOutLen
** 返回:     错误码
********************************************************************/
int StrTestCmdProc(U8 cmd, U8 subcmd, U8* pDataBuf, U16 DataLen, U8* pOutBuf, U16* pOutLen)
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
			ret = ProcStrAbbreviationTest(pDataBuf, DataLen, pOutBuf, pOutLen);
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


/*******************************************************************
** 函数名:     TmrTcpProc
** 函数描述:     链路探寻定时器 接收并确认是否回复TCP数据
** 参数:       [in]
** 返回:       负数：错误码 0：无数据 正数：收到数据
********************************************************************/
int TmrTcpProc(void)
{
	int ret = -1;
	sgTcpCmdFunc[sizeof(sgTcpCmdFunc)/sizeof(PProcessCmdMsg)-1] = NULL;//必须预留最后一个NULL指针位置
	
	do{
        
		ret = TcpRecvData(sgTcpRecBuf, MAX_SLAVE_BUF_SIZE);
		if (ret > ERR_SUCCESS)
		{
			ret = TcpRecvProc(sgTcpRecBuf, ret, sgTcpSendBuf, MAX_TCP_RESP_SIZE, sgTcpCmdFunc);
			if (ret > ERR_SUCCESS)
			{
				PrintDebug("tcp cli resp msg len %d", ret);
				PRINT_HEX(sgTcpSendBuf, ret, "client tcp resp");
                if (0 > TcpSendData(sgTcpSendBuf, ret)){
                    PrintErr("tcp send err exit");
                }
			}
		}		
	}while(0);

	return ret;
}

#endif

