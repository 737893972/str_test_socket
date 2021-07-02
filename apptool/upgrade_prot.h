/**
 * FileName: upgrade_prot.h
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: 
 */

//---------------------------------------



#ifndef UPGRADE_PROT_H_
#define UPGRADE_PROT_H_

#include "glo_type.h"

#if 1

#define MAJOR_NET_IF		"bridge0"//"wlan0"//"enp0s3"//"bridge0"
#define PC_IF				"enp0s3"
#define UBUNTU_USB_IF		"enp0s6u2"


#define TCP_UPGE_PROT 19003
#define UDP_UPGE_PORT 19003

#define MAJOR_VER	0
#define MINOR_VER	2
#define PROT_VERSION  (MAJOR_VER.MINOR_VER)

#define MAX_FILE_REC_SIZE	(100*1024*1024)

#define MAX_FILE_NAME_SIZE	(100)

#define MAX_TCP_RESP_SIZE	(1024)
#define MAX_UDP_SEND_SIZE	(1024*10)
#define MAX_UDP_RECV_SIZE	(1024)

#define MIN_SLAVE_RECV_SIZE	(10*1024)

#define MAX_SLAVE_BUF_SIZE	(64*1024)		
#define MAX_SERVER_BUF_SIZE	 MAX_SLAVE_BUF_SIZE

#define MAX_APP_BUF_SIZE	(MAX_SLAVE_BUF_SIZE-sizeof(TUpgradePkgFmt))	
#define MAX_FILE_SPI_SIZE	(MAX_APP_BUF_SIZE - GetOffset(TUpgeTransPkgFmt, PkgBuf))  

#define MAX_SOCKET_ADDR_LEN		24//sizeof(struct sockeaddr_in)=16




#define CMD_MAJOR_UPDATE			0x01
	#define SUBCMD_DEV_INFO  		1
	#define SUBCMD_UPDATE_PROGRESS  2
	#define SUBCMD_SET_SERVER_INFO  3
	#define SUBCMD_CONNECT_RET		4

	#define SUBCMD_UPDATE_CONFIRM   10
	#define SUBCMD_UPDATE_PKG_INIT  11
	#define SUBCMD_UPDATE_PKG_BUF   12
	#define SUBCMD_UPDATE_FILE_CHECK  13
#define CMD_MAJOR_JSON_TRANS			0x02
    #define SUBCMD_DATA_REC         1
    #define SUBCMD_DATA_SEND          2
#define CMD_MAJOR_STR_TEST			0x03
    #define SUBCMD_TEST_STR         1

#define PKG_FMT_HEAD	0xaa
#define PKG_FMT_TAIL	0x55

#define CREATE_PKG_HEAD_FMT(pkg, seq, ack, ret, subcmd)  do{\
				pkg->Seqno = seq;\
				pkg->Ack = ack;\
				pkg->ErrCode = ret;\
				pkg->SubCmd = subcmd;\
			}while(0)

typedef enum _UPGE_STAT_E
{
	UPGE_IDLE = 0,
	UPGE_UPDATING,
	UPGE_SUSPEND,
	UPGE_SUCCESS,
}UPGE_STAT_E;


typedef struct _UpgradePkgStruct_
{
	U8 FrameHead;
	U8 FrameTail;
	U16 BufLen;
	U16 Seqno;
	U8 Ack;
	S8 ErrCode;
	U8 Cmd;
	U8 SubCmd;
	U16 DataLen;
	U8 DataBuf;//
	U8 CheckSum;
} GCC_PACKED TUpgradePkgFmt, *PUpgradePkgFmt;


typedef INT32 (*PRegMpuUserDefineCb)(char* inbuf, U16 inlen, char* echoStr, U16 maxEchoLen);

typedef int (*PProcessCmdMsg)(U8 cmd, U8 subcmd, U8* pDataBuf, U16 DataLen, U8* pOutBuf, U16* pOutLen);

int TcpRecvProc(U8* recBuf, INT32 maxRecLen, U8* sendBuf, int maxSendLen, PProcessCmdMsg pProcesMsgFunc[]);

int UdpServProc(U8* inBuf, INT32 inLen,U8* outBuf, INT32 maxOutLen, PProcessCmdMsg pProcesMsgFunc[]);


INT32 UpgradePkgParse(U8* inBuf, INT32 inLen, PUpgradePkgFmt pOutMsg, U8** ppDataBuf);

INT32 UpgradePkgPacket(const U8* inBuf, U16 inLen, PUpgradePkgFmt pstMsg);

int GetUpgradeFmtSeqno(void);

int SetUpgradeFmtSeqno(void);

INT32 CheckRespMsgProto(U8* inBuf, INT32 inLen, U8* oriBuf);

//int TcpSendPropData(int sock, U8* inBuf, INT32 inLen, U8* outBuf, INT32 maxOutLen);

#endif

#endif

