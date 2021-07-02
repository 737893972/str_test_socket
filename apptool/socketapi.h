/**
 * FileName: yx_upgrade_process.h
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: 
 */

//---------------------------------------


#ifndef SOCKETAPI_H_
#define SOCKETAPI_H_





#if 1




#define SOCKET_SEND_ERR_TIME	3
#define SOCKET_READ_WAIT_TIME	3

/*******************************************************************
** 函数名:     
** 函数描述:TCP通道数据连接服务器
** 参数:       [in] port 端口号
**        [in] pDestAddr 目标地址
**        [in] timeout 阻塞超时时间
** 返回:       成功返回 0，失败返回 错误码
********************************************************************/
int SocketTryConnect(const char *pDestAddr, unsigned short port, U8 timeout, int* pSockFd);

/*******************************************************************
** 函数名:     InitUDPServerSocket
** 函数描述:   
** 参数:       [in] port 端口号
** 返回:       成功返回 0，失败返回 错误码
********************************************************************/
INT32 InitUDPServerSocket(U16 port, int* pUdpServFd);

int RecvTCPData(int socket, unsigned char *buf, int maxOutLen);

int SendTCPData(int socketFd, void *buf, int sendLen);

INT32 TcpCheckDataTimeout(int sock, int timeoutMs, U8* outBuf, INT32 maxOutLen);

int TcpAcceptCliConnection(int servFd, int* sockFdBuf, int maxFdCnt);

int TcpServSockConnect(U16 port, int* sockFd);

BOOL CheckStrIPValid(const char* pIPStr);

#endif

#endif

