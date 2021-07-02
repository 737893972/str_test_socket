/**
 * FileName: socketapi.c
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: 
 */

//---------------------------------------



#if 1

#include "glo_type.h"
//#include "yx_upgrade_process.h"
#include "errcode.h"
#include "devinfo.h"

#include "upgrade_prot.h"
#include "socketapi.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>  
#include <fcntl.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h>

BOOL CheckStrIPValid(const char* pIPStr)
{
    if (INADDR_NONE == inet_addr(pIPStr))
    {
        return TRUE;
    }
    return FALSE;
}


/*******************************************************************
** 函数名:     
** 函数描述:TCP通道数据连接服务器
** 参数:       [in] port 端口号
**        [in] pDestAddr 目标地址
**        [in] timeout 阻塞超时时间
** 返回:       成功返回 0，失败返回 错误码
********************************************************************/
int SocketTryConnect(const char *pDestAddr, unsigned short port, U8 timeout, int* pSockFd)
{
	int ret;
	long setParam = 0;
	struct sockaddr_in serverSockaddr;

	int timeoutRet;
	fd_set fdr, fdw;
    struct timeval stTime;
	FD_ZERO(&fdr);
	FD_ZERO(&fdw);
	int err = 0;
    int  tcpCliFd = -1;
	socklen_t  errlen = 0;
    
    errlen = sizeof(err);
	//bzero(sgContFailRet, sizeof(sgContFailRet));

	if (NULL == pDestAddr || port == 0 || pSockFd == NULL)
	{
		return ERR_PARAM_INVALID;
	}

	if (*pSockFd > 0)
	{
		close(*pSockFd);
		*pSockFd = -1;
	}

	tcpCliFd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > tcpCliFd)
	{
		PrintErr("Can't create socket:%m");
		return ERR_TCP_ERR;
	}

	bzero(&serverSockaddr,sizeof(serverSockaddr));
	serverSockaddr.sin_family = AF_INET;
	serverSockaddr.sin_port = htons(port);
	serverSockaddr.sin_addr.s_addr = inet_addr(pDestAddr);
	
	//if(inet_pton(AF_INET, pDestAddr, (void*)&serverSockaddr.sin_addr) <= 0)
	{
	//	PrintErr("trans ip addr err");
	//	return ERR_PARAM_INVALID;
	}

	if (timeout > 0)
	{
		setParam = fcntl(tcpCliFd, F_GETFL);
		if(fcntl(tcpCliFd, F_SETFL, setParam | O_NONBLOCK) < 0)
		{
			close(tcpCliFd);
			tcpCliFd = -1;
			PrintErr("fcntl set err:%m");
			return ERR_TCP_ERR;
		}
	}
	
		
	ret = connect (tcpCliFd, (struct sockaddr *)&serverSockaddr,  sizeof(serverSockaddr));
	do
	{
		if (ret == 0)
		{
			PrintInfo("Connect %s success: sockfd: %d", pDestAddr, tcpCliFd);
			break;
		}
		else if(ret < 0 && timeout > 0  && errno == EINPROGRESS)
		{
			ret = ERR_TCP_CONT_FAIL;
			PrintInfo("sock %d Connecting %s:%d wait a moment(%d sec):%m", \
                tcpCliFd, pDestAddr, port, timeout);
			
			FD_SET(tcpCliFd, &fdr);
			FD_SET(tcpCliFd, &fdw);
			stTime.tv_sec = timeout - 1;
			stTime.tv_usec = 100*1000;//ms
			timeoutRet = select(tcpCliFd + 1, &fdr, &fdw, NULL, &stTime);
			
			if (timeoutRet == 0)
			{
				PrintErr("connect timeout:%m");
				ret = ERR_TCP_ERR;
				break;
			}
			else if (timeoutRet == 1 && FD_ISSET(tcpCliFd, &fdw))
			{
				PrintWarn("Connect %s %d success", pDestAddr, port);
				ret = ERR_SUCCESS;
				break;
			}
			else if (timeoutRet == 2 )
			{
				if (getsockopt(tcpCliFd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) 
				{
                     PrintErr("getsockopt(SO_ERROR): %m");
					 break;
                }
				if (err) 
				{
					PrintErr("connect server timeout: %m");
					break;
				}
			}
		}
		else
		{
			PrintErr("Connect err: sockfd: %d ret value :%d err:%m", tcpCliFd, ret);
			break;
		}
	}
	while (0);
	
	if (ret < 0)
	{
		//snprintf(sgContFailRet, sizeof(sgContFailRet), "%s", strerror(errno));
		close(tcpCliFd);
		PrintInfo("close tcp socket");
		tcpCliFd = -1;
	}
    else
    {
        *pSockFd = tcpCliFd;
    }   


	return ret;
}


int RecvTCPData(int socket, BYTE *buf, INT32 maxOutLen)
{
	int len = 0;
	int curLen = 0;
	int packageLen = 0;
	int waitCnt = 0;

	int revHeadLen  = sizeof(TUpgradePkgFmt) - 2;

	len = recv(socket, buf, revHeadLen, 0);
	if (len == 0)
	{
		return ERR_TCP_ERR;
	}
    else if(len == -1)
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            //PrintDebug("recv tcp none data");
            return ERR_NONE_DATA;
        }
        else
        {
            return ERR_TCP_ERR;
        }
    }
	else if(buf[0] != PKG_FMT_HEAD || buf[1] != PKG_FMT_TAIL)
	{
		PrintErr("recv tcp buf head err %x %x", buf[0], buf[1]);
		PrintHexBuf(buf, revHeadLen);
		return ERR_PKG_FMT;
	}
	
	curLen = len;
	if(len == revHeadLen)
	{
		packageLen = ((PUpgradePkgFmt)buf)->BufLen + 4;
		PrintDebug("parse tcp buf len %d", packageLen);

		/*if(packageLen > maxOutLen)
		{
			PrintErr("rec len cache buf %u", maxOutLen);
			return ERR_BUF_OVERSIZE;
		} if get a large pkg,need keep recv whole pkg then return err
		*/
		
		while(curLen < packageLen)
		{
			len = read(socket, buf + curLen, packageLen - curLen);
			if(len > 0)
			{
				curLen += len;
				waitCnt = 0;
			}
			else if(len == -1 && (errno == EINTR || errno == EAGAIN))
			{
				MsSleep(10);
				if (waitCnt++ > SOCKET_READ_WAIT_TIME)
				{
					PrintErr("read timeout break curlen %d",len);
					break;
				}
			}
            else
			{
                return ERR_TCP_ERR;
            }
		}

		if(curLen > maxOutLen)
		{
			PrintErr("rec pkg oversize");
			return ERR_BUF_OVERSIZE;
		}

		return curLen;
	}
	else
	{
		PrintErr("Recv Socket head length error(%d-%d)", len, revHeadLen);
		return ERR_PKG_FMT;
	}
}


int SendTCPData(int socketFd, void *buf, int sendLen)
{
	int sendTime = 0;
	int curLen = 0;
	int len = 0;
	int ret = 0;

	curLen = send(socketFd, buf, sendLen, 0);
	if (-1 == curLen)
	{
		PrintWarn("handle:%d errno:%d %m", socketFd, errno);
		if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
		{
			sendTime++;
			curLen = 0;
		}
		else
		{
			return ERR_TCP_ERR;
		}
	}
    else if(curLen == 0)
    {
        return ERR_TCP_ERR;
    }

	while (curLen < sendLen)
	{
		len = send (socketFd, buf + curLen, sendLen - curLen, 0);
		if(len > 0)
		{
			curLen += len;
		}
		else if (0 == len)
		{
			PrintWarn("out of buffer:%m");
			MsSleep(10);
		}
		else
		{
			if(errno == EINTR)
			{
				PrintInfo("handle: %d %m", socketFd);
				MsSleep(10);
			}
			else if ((EFAULT == errno) || (EAGAIN == errno))
			{
				if ((0 == curLen) && (sendTime++ > SOCKET_SEND_ERR_TIME))
				{
					PrintWarn("out of buffer, cancel send......");
					return ERR_SOCKET_TIMEOUT;
				}

				PrintWarn("%m, errno:%d curLen:%d", errno, curLen);
				MsSleep(10);

				continue;
			}
			else if ((ECONNRESET == errno) || (EPIPE == errno))
			{
				PrintInfo("handle:%d errno:%d %m", socketFd, errno);
				return ERR_TCP_ERR;
			}
			else
			{
				PrintInfo("handle:%d errno:%d %m", socketFd, errno);
			}
		}

		if (curLen == sendLen)
		{
			//curLen = 0;
		}
	}
	PrintDebug("socketFd:%d len:%d, really send:%d", socketFd, sendLen, curLen);
	if(curLen == sendLen)
	{
		ret = ERR_SUCCESS;
	}
	else
	{
		ret = ERR_TCP_ERR;
	}

	return ret;
}



int TcpServSockConnect(U16 port, int* sockFd)
{
    int tcpSockFd = -1;
	struct sockaddr_in sin;
    long setParam = 0;
    ASSERT_DESC(sockFd != NULL, "param sockFd can be NULL");
    ASSERT_DESC(port != 0, "port invalid");

    if (*sockFd > 0)
    {
        PrintWarn("close server fd first");
        close(*sockFd);
    }
	if ((tcpSockFd = socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		return ERR_TCP_CONT_FAIL;
	}

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port);

	int val = 1;
	setsockopt(tcpSockFd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val) );
    
    setParam = fcntl(tcpSockFd, F_GETFL);
    fcntl(tcpSockFd, F_SETFL, setParam | O_NONBLOCK);

	if (bind(tcpSockFd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		PrintErr("TCP bind ERROR");
		return ERR_TCP_ERR;
	}

	if (listen(tcpSockFd, 1) == -1)
	{
		close(tcpSockFd);
		ERR_EXIT("TCP listen port ERROR");
	}

	PrintInfo("TCP listen port:%d successful", port);
	*sockFd = tcpSockFd;
    
	return ERR_SUCCESS;
}

//return rec len or errcode
INT32 TcpCheckDataTimeout(int sock, int timeoutMs, U8* outBuf, INT32 maxOutLen)
{
	fd_set catch_fd_set;
	int new_cli_fd = -1;
	int maxfd = 0;
	int ret  = 0;

    if (sock < 0)
    {
        return ERR_TCP_NO_CONT;
    }
    new_cli_fd = sock;
	FD_ZERO(&catch_fd_set);
	FD_SET(new_cli_fd, &catch_fd_set);
	maxfd = new_cli_fd;

	struct timeval tv;
	tv.tv_sec = timeoutMs/1000;
	tv.tv_usec = (timeoutMs%1000) * 1000;
	
	ret = select (maxfd + 1, &catch_fd_set, NULL, NULL, &tv);
	if (ret < 0)
	{
		//PrintErr("select err");
		return ERR_EXECUTE_FAIL;
	}

	if (FD_ISSET(new_cli_fd, &catch_fd_set))
	{
		//PrintDebug("socket %d have data", new_cli_fd);
		ret = RecvTCPData(new_cli_fd, outBuf, maxOutLen);
		if (ERR_TCP_ERR == ret)
		{
			PrintWarn("get sockfd %d err %m.close current socket", new_cli_fd);
			close(new_cli_fd);
			FD_CLR(new_cli_fd, &catch_fd_set);
			new_cli_fd = 0;
		}
		else if(ret > 0)
		{	
			PrintInfo("get sockfd %d data len %d", new_cli_fd, ret);
		}
        else
        {
            ;
        }      
	}
	return ret;
}

INT32 TcpCheckMultiCliData(int servFd, int* sockFdBuf, int maxFdCnt, int timeoutMs, U8* outBuf, INT32 maxOutLen, U16* cliIdx)
{
    fd_set catch_fd_set;
    int i = 0;
    int maxfd = 0;
    int index =0;
    int ret  = 0;
    int outLen = 0;
    int sockFdCount = 0;
    BYTE recvBuf[MAX_TCP_RESP_SIZE] = {0};
    struct timeval tv;
    
    ASSERT_DESC(servFd > 0, "invalid servFd");
    ASSERT_DESC(sockFdBuf != NULL, "sockFdBuf null");
    ASSERT_DESC(outBuf != NULL, "outBuf null");
    ASSERT_DESC(maxFdCnt != 0, "maxFdCnt 0");
    ASSERT_DESC(cliIdx != NULL, "client fd buf idx pointer null");
    
    tv.tv_sec = 0;
    tv.tv_usec = 500 * 1000;
    
    FD_ZERO(&catch_fd_set);
    FD_SET(servFd, &catch_fd_set);
    maxfd = servFd;
   
    for (index = 0; index < maxFdCnt; index++)
    {
        if (sockFdBuf[index] > 0)
        {
            FD_SET(sockFdBuf[index], &catch_fd_set);
            if (maxfd < sockFdBuf[index])
            {
                maxfd = sockFdBuf[index];
            }
            sockFdCount++;
        }
    }

    if (sockFdCount <= 0)
    {
        return ERR_NONE_DATA;
    }

    ret = select (maxfd + 1, &catch_fd_set, NULL, NULL, &tv);
    if (ret < 0)
    {
        PrintDebug("tcp timeout ");
        return ERR_NONE_DATA;
    }

    for (i = 0; i < maxFdCnt; ++i)
    {
        if (sockFdBuf[i] > 0 && FD_ISSET(sockFdBuf[i], &catch_fd_set))
        {
            //ret = 0;
            PrintDebug("socket %d idx %d have data", sockFdBuf[i], i);
            ret = RecvTCPData(sockFdBuf[i], recvBuf, sizeof(recvBuf));
            if (ERR_TCP_ERR == ret)
            {
                PrintWarn("get sockfd %d err %m.close current socket", sockFdBuf[i]);
                FD_CLR(sockFdBuf[i], &catch_fd_set);
                close(sockFdBuf[i]);
                sockFdBuf[i] = -1;
                sockFdCount--;
            }
            else if(ret > 0)
            {           
                #if 1
                outLen = (ret > maxOutLen?  maxOutLen : ret);
                PrintInfo("socket %d idx %d rec data len %d ok ret %d", sockFdBuf[i], i, outLen, ret);
                memcpy(outBuf, recvBuf, outLen);
                *cliIdx = i;
                PRINT_HEX(outBuf, outLen, "serv rec");
                break;
                #else
            	ret = TcpRecvProc(recvBuf, ret, outBuf, maxOutLen, sgTcpServCmdFunc);
            	if (ret > 0)
            	{
            	    respLen = ret;
                    ret = SendTCPData(sockFdBuf[i], outBuf, respLen);
                    if (ret == ERR_TCP_ERR)
                	{
                	    PrintErr("resp cli fd err, close");
                        FD_CLR(sockFdBuf[i], &catch_fd_set);
                	    close(sockFdBuf[i]);
                        sockFdBuf[i] = -1;
                    }
                    else if(ret == ERR_SUCCESS)
                    {
                        PrintInfo("tcp server resp cli len %d succs", respLen);
                    }
            	}
                #endif
            }
        }
    }
    if (i >= maxFdCnt)
    {
        return ERR_NONE_DATA;
    }
    return ret;
}

int TcpAcceptCliConnection(int servFd, int* sockFdBuf, int maxFdCnt)
{
	fd_set catch_fd_set;
	int new_cli_fd = 0;
	int i  = 0;
	int socklen = 0;
	struct sockaddr_in cli_sockaddr;
	long setParam = 0;
    
	ASSERT_DESC(servFd > 0, "invalid servFd");
    ASSERT_DESC(maxFdCnt != 0, "maxFdCnt can not be 0");
    ASSERT_DESC(sockFdBuf != NULL, "sockFdBuf NULL");
    
	FD_ZERO(&catch_fd_set);
	FD_SET(servFd, &catch_fd_set);
	socklen = sizeof(cli_sockaddr);
    
	new_cli_fd = accept (servFd, (struct sockaddr *)&(cli_sockaddr),(socklen_t *)&socklen);
	if (0 > new_cli_fd)
	{
        if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) && new_cli_fd == -1)
        {
            return ERR_NONE_DATA;
        }
        else
        {
            PrintWarn("accept client failed %m %d", new_cli_fd);
		    return ERR_TCP_ERR;
        }
	}
	
	setParam = fcntl(new_cli_fd, F_GETFL);
	fcntl(new_cli_fd, F_SETFL, setParam | O_NONBLOCK);
	
    
    for (i = 0; i < maxFdCnt; ++i)
    {
        if (sockFdBuf[i] < 0)
        {
            PrintInfo("get new client connect socket %d idx %d", new_cli_fd, i);
            sockFdBuf[i] = new_cli_fd;
            break;
        }
    }
    if (i >= maxFdCnt)
    {
        PrintErr("new client connection reach the max, can not save cli fd,close");
        close(new_cli_fd);
        return ERR_NONE_DATA;
    }
    
	return ERR_SUCCESS;
}


/*******************************************************************
** 函数名:     InitUDPServerSocket
** 函数描述:   
** 参数:       [in] port 端口号
** 返回:       成功返回 0，失败返回 错误码
********************************************************************/
INT32 InitUDPServerSocket(U16 port, int* pUdpServFd)
{
	long setParam = 0;
    int  udpServFd = -1;

    ASSERT_DESC(pUdpServFd != NULL, "udp fd null");
    
	if ((*pUdpServFd) > 0)
	{
		close(*pUdpServFd);
	}
	
    if ((udpServFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
		PrintErr("socket error");
		return ERR_SOCKET_ERR;
	}

	//struct ifreq ifr;
	//memset(&ifr, 0x00, sizeof(ifr));
	//strncpy(ifr.ifr_name, MAJOR_NET_IF, strlen(MAJOR_NET_IF));
	//setsockopt(udpServFd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));

	
	setParam = fcntl(udpServFd, F_GETFL);
	if(fcntl(udpServFd, F_SETFL, setParam | O_NONBLOCK) < 0)
	{
		close(udpServFd);
		udpServFd = -1;
		PrintErr("fcntl set err:%m");
		return ERR_UDP_ERR;
	}

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
	
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);not use
	
    if (bind(udpServFd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
		PrintErr("bind error");
		return ERR_UDP_ERR;
	}
	
	PrintInfo("UDP listen port %d",port);
    *pUdpServFd = udpServFd;
        
    return ERR_SUCCESS;
    
}


#endif

