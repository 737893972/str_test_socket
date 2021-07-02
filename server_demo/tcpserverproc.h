/*
 * File name	: yx_tcpserverproc.h
 *
 * Created on	: 
 * Author		: 
 * Version		: 
 * Language		: C
 * Copyright	: Copyright 
 *
 */
#ifndef INCLUDE_TCPSERVERPROC_H_
#define INCLUDE_TCPSERVERPROC_H_

#define MAX_TCPSERVER_SOCK_CNT		10

int TCPServerSocketInit(U16 port);

INT32 TCPServerCacheInit(void);

void TCPServerCacheFree(void);


int TcpServStrHandle(void);

int TcpServerCommDeInit(void);


int TcpServRecStrTestProc(U8 cmd, U8 subcmd, U8* pDataBuf, U16 DataLen, U8* pOutBuf, U16* pOutLen);

INT32 TcpCheckMultiCliData(int servFd, int* sockFdBuf, int maxFdCnt, int timeoutMs, U8* outBuf, INT32 maxOutLen, U16* cliIdx);

int TcpServMultiCliProc(void);

#endif /*  */

