/*
 * File name	: yx_tcpproc.h
 *
 * Created on	: 
 * Author		: 
 * Version		: 
 * Language		: C
 * Copyright	: Copyright 
 *
 */
#ifndef INCLUDE_TCPPROC_H_
#define INCLUDE_TCPPROC_H_

#include "glo_type.h"

/*******************************************************************
** ������:     TmrTcpProc
** ��������:     ��·̽Ѱ��ʱ�� ���ղ��ظ�TCP����
** ����:       [in]
** ����:       ������������ 0�������� �������յ�����
********************************************************************/
int TmrTcpProc(void);

/*******************************************************************
** ������:     
** ��������:TCPͨ��ͨ��ȥ��ʼ��
** ����:       true or false
********************************************************************/
BOOL TcpCommReset(void);

/*******************************************************************
** ������:     
** ��������:   Tcpͨ�����ݷ��ͽӿ�
** ����:       [in] inBuf: ����ָ��
**             [in] inLen: ���ݳ���
** ����:       �ɹ�����true��ʧ�ܷ���false
********************************************************************/
int TcpSendData(U8 *inBuf, U16 inLen);

/*******************************************************************
** ������:     
** ��������:   ͨ�����ݽ��սӿ�
** ����:       [out] outBuf: ����ָ��
**             [in] maxOutLen: ���ݳ���
** ����:       �ɹ��������ݳ��ȣ������ݷ��ش�����
********************************************************************/
int TcpRecvData(U8* outBuf, U32 maxOutLen);

/*******************************************************************
** ������:     TcpCommDeInit
** ��������:     ȥ��ʼ��
** ����:       [in] 
** ����:       true
********************************************************************/
int TcpCommDeInit(void);

/*******************************************************************
** ������:     TCPTryConnectServer
** ��������:     ����TCP������������ȷ��
** ����:       [in] pdata����ʱ������ֵ
** ����:       ��
********************************************************************/
int TCPTryConnectServer(const char* pIPStr, U16 port);

/*******************************************************************
** ������:     TCPClientCacheInit
** ��������:         TCP ���ͻ����ʼ��
** ����:     ��
** ����:       ��
********************************************************************/
INT32 TCPClientCacheInit(void);

/*******************************************************************
** ������:     StrTestCmdProc
** ��������:     ����tcp server������
** ����:       [in] cmd  [in] inBuf [in] inLen [in] outBuf [in] pOutLen
** ����:     ������
********************************************************************/
int StrTestCmdProc(U8 cmd, U8 subcmd, U8* pDataBuf, U16 DataLen, U8* pOutBuf, U16* pOutLen);

int TCPClientSendData(U8* inBuf, INT32 inLen);

INT32 TcpDataPackWithProp(U8* inBuf, INT32 inLen, U8* outBuf, INT32 maxOutLen);

#endif /*  */
