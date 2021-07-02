/*
 * File name	: errcode.h
 *
 * Created on	: 
 * Author		: 
 * Version		: 
 * Language		: C
 * Copyright	: Copyright 
 *
 */

 
#ifndef ERRCODE_H_
#define ERRCODE_H_



#if 1




#define ERR_SUCCESS			0  //�ɹ�
#define ERR_FILE_OVERSIZE	-1  //����������
#define ERR_NO_ENOUGH_SPACE	-2  //�豸�ռ䲻��
#define ERR_UN_AUTHORIZE	-3  //������Ȩ��
#define ERR_DEV_BUSY		-4  //�豸��æ  ������
#define ERR_TCP_CONT_FAIL	-5  //Tcp����ʧ��

#define ERR_NO_SUPPT_UPDATE	-6 //��֧������
#define ERR_BUF_OVERSIZE	-7 //��������
#define ERR_OUTDATED_VER	-8 //�˰汾����
#define ERR_CHECKSUM		-9 //��У�����
#define ERR_INVALID_AUTHENTIC_CODE	-10 //������Ȩ����Ч
#define ERR_PACKAGE_INDEX	-11//����Ų�����Ԥ�� �����ɼ�������
#define ERR_FILE_CHECKSUM	-12//����У�����
#define ERR_PARAM_INVALID	-13//��������
#define ERR_PKG_FMT			-14//��ʽ����
#define ERR_PKG_CMD			-15//�������
#define ERR_UDP_ERR			-16//UDP����
#define ERR_TCP_ERR			-17//TCP����
#define ERR_EXECUTE_FAIL	-18//ִ��ʧ��
#define ERR_NONE_DATA		-19//������
#define ERR_FILE_ERR		-20//�ļ��쳣
#define ERR_LEN_ERR			-21//�����쳣
#define ERR_MALLOC			-22//�����ڴ��쳣
#define ERR_UPGE_FILE_TYPE	-23//�����汾���ʹ���
#define ERR_SOCKET_ERR		-24//socket����
#define ERR_SOCKET_TIMEOUT	-25//�ȴ���ʱ
#define ERR_TCP_NO_CONT		-26//TCPδ����




#define ERR_UNKONW_ERR		-128 //δ֪���� ����������ʾ ��databuf


#endif

#endif /*  */
