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
** ������:     
** ��������:TCPͨ���������ӷ�����
** ����:       [in] port �˿ں�
**        [in] pDestAddr Ŀ���ַ
**        [in] timeout ������ʱʱ��
** ����:       �ɹ����� 0��ʧ�ܷ��� ������
********************************************************************/
int SocketTryConnect(const char *pDestAddr, unsigned short port, U8 timeout, int* pSockFd);

/*******************************************************************
** ������:     InitUDPServerSocket
** ��������:   
** ����:       [in] port �˿ں�
** ����:       �ɹ����� 0��ʧ�ܷ��� ������
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

