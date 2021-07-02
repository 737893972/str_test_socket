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



#if 1

#include "crc16.h"
#include "glo_type.h"
#include "errcode.h"
#include "upgrade_prot.h"
#include "devinfo.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>


static U16 sg_seq = 0;//
//static U16 sg_recSeq = 0;//for cli


static U8 slaveEncType = 0;

int SetUpgradeFmtSeqno(void)
{
	return ++sg_seq;
}

int GetUpgradeFmtSeqno(void)
{
	return sg_seq;
}





INT32 UpgradePkgParse(U8* inBuf, INT32 inLen, PUpgradePkgFmt pOutMsg, U8** ppDataBuf)
{
	U8 crcValue = 0;
	U8 crcRecValue = 0;
	PUpgradePkgFmt pInMsg = NULL;
	pInMsg = (PUpgradePkgFmt)inBuf;

	if (inBuf == NULL || pOutMsg == NULL || inLen == 0)
	{
		PrintErr("param err");
		return ERR_PARAM_INVALID;
	}

	if (pInMsg->FrameHead != PKG_FMT_HEAD || pInMsg->FrameTail != PKG_FMT_TAIL)
	{
		PrintErr("fmt err %x %x", pInMsg->FrameHead, pInMsg->FrameTail);
		return ERR_PKG_FMT;
	}

	if (pInMsg->BufLen > MAX_SLAVE_BUF_SIZE)
	{
		PrintErr("buf len err");
		return ERR_BUF_OVERSIZE;
	}

	crcValue = Crc16LSBCalcBuf((U8*)&pInMsg->Seqno, pInMsg->BufLen - 1);
	crcRecValue = inBuf[GetOffset(TUpgradePkgFmt, DataBuf) + pInMsg->DataLen];
	
	if (crcValue != crcRecValue)
	{
		PrintErr("checksum err crcValue %x rec crcValue %x", crcValue, crcRecValue);
		return ERR_CHECKSUM;
	}

	memcpy(pOutMsg, inBuf, GetOffset(TUpgradePkgFmt, DataBuf));
	if (pInMsg->DataLen)
	{
		*ppDataBuf = &pInMsg->DataBuf;
	}
		
	PrintDebug("rec pkg pass, app data len %d", pOutMsg->DataLen);
	
	return ERR_SUCCESS;
}

INT32 UpgradePkgPacket(const U8* inBuf, U16 inLen, PUpgradePkgFmt pstMsg)
{
	
	U32 crcLen = 0;
	U32 bufPkgLen = 0;
	U8 checkSum = 0;
	U16 crcIndex = 0;

	ASSERT_DESC(pstMsg != NULL, "pstMsg null");
	
	pstMsg->FrameHead = PKG_FMT_HEAD;
	pstMsg->FrameTail = PKG_FMT_TAIL;

	if (inLen > MAX_APP_BUF_SIZE)
	{
		PrintErr("err len");
		return ERR_BUF_OVERSIZE;
	}
	if (inBuf != NULL && inLen != 0)
	{
		memcpy(&pstMsg->DataBuf, inBuf, inLen);
		pstMsg->DataLen = inLen;
	}
	else
	{
		pstMsg->DataLen = 0;
	}

	if (slaveEncType != 0)//TODO
	{
		PrintErr("no support yet");
	}

	pstMsg->BufLen = sizeof(TUpgradePkgFmt) - sizeof(pstMsg->DataBuf) + pstMsg->DataLen - GetOffset(TUpgradePkgFmt, Seqno);
	crcLen = GetOffset(TUpgradePkgFmt, DataBuf) - GetOffset(TUpgradePkgFmt, Seqno) + pstMsg->DataLen;
	
	checkSum = Crc16LSBCalcBuf((U8*)&pstMsg->Seqno, crcLen);

	crcIndex = GetOffset(TUpgradePkgFmt, DataBuf) + pstMsg->DataLen;
	((U8*)pstMsg)[crcIndex] = checkSum;

	bufPkgLen = pstMsg->BufLen + GetOffset(TUpgradePkgFmt, Seqno);
	//PrintHexBuf((U8*)pstMsg, bufPkgLen);

	return bufPkgLen;
}


INT32 CheckRespMsgProto(U8* inBuf, INT32 inLen, U8* oriBuf)
{
	int ret = -1;
	U8* pDataBuf = NULL;
	TUpgradePkgFmt st_getResp;
	bzero(&st_getResp, sizeof(st_getResp));
	PUpgradePkgFmt pstBuf = NULL;
	pstBuf = (PUpgradePkgFmt)oriBuf;

	assert(inBuf != NULL);
	assert(oriBuf != NULL);
	
	PrintDebug("get resp");
	PrintHexBuf(inBuf, inLen);
	ret = UpgradePkgParse(inBuf, inLen, &st_getResp, &pDataBuf);
	if (ret == 0 )
	{
		if (st_getResp.Seqno != (U16)(GetUpgradeFmtSeqno()-1))
		{
			PrintErr("seq err rec %u should be %u",st_getResp.Seqno, (U16)(GetUpgradeFmtSeqno()-1));
			return ERR_PACKAGE_INDEX;
		}
		else if (st_getResp.SubCmd != pstBuf->SubCmd || st_getResp.Cmd != pstBuf->Cmd)
		{
			PrintErr("cmd err rec %d %d",st_getResp.SubCmd, pstBuf->SubCmd);
			return ERR_PKG_CMD;
		}
		else if(st_getResp.ErrCode < 0)
		{
			PrintErr("cmd exeu fail ret %d",st_getResp.ErrCode);
			return ERR_EXECUTE_FAIL;
		}
		PrintInfo("get resp success!!!seq %d subcmd %d errcode %d",st_getResp.Seqno , st_getResp.SubCmd, st_getResp.ErrCode);

		return ERR_SUCCESS;
		
	}
	else
	{
		return ERR_PKG_FMT;
	}
}

int TcpRecvProc(U8* recBuf, INT32 maxRecLen, U8* sendBuf, int maxSendLen, PProcessCmdMsg pProcesMsgFunc[])
{
	int ret = 0;
	INT32 pkgRet = -1;
	PUpgradePkgFmt pstResp = NULL;
	pstResp = (PUpgradePkgFmt)sendBuf;
    int i = 0;

	INT32 tcpSendLen = 0;
	U8* pDataBuf = NULL;
	TUpgradePkgFmt stRec;
	U16 respLen = 0;
	U8 respBuf[MAX_TCP_RESP_SIZE] = {0};
	
	ASSERT_DESC(recBuf != NULL, "recBuf null");
	
	bzero(&stRec, sizeof(stRec));
	bzero(respBuf, sizeof(respBuf));
	
	PrintDebug("rec tcp msg len %d", maxRecLen);
    PRINT_HEX(recBuf, maxRecLen, "tcp rec");
	ret = UpgradePkgParse(recBuf, maxRecLen, &stRec, &pDataBuf);
	if (ret != ERR_SUCCESS)
	{
		return ret;
	}

	#if 0//REQUEST_MASTER//only the master need to check response msg with orinal msg
	assert(sendBuf != NULL);
	if (stRec.Ack == 1)
	{
		PrintInfo("rec client msg subcmd %x", stRec.SubCmd);
	}
	else
	{
		ret = CheckRespMsgProto(recBuf, maxRecLen, sendBuf);
		if (ret != ERR_SUCCESS)
		{
			if (ERR_EXECUTE_FAIL == ret)
			{
				PrintErr("client resp errcode %d", stRec.ErrCode);
			}
			if (stRec.DataLen > 0 && pDataBuf != NULL)
			{
				PrintErr("client resp errmsg %s", pDataBuf);
			}
			return ret;
		}
	}
	

    /*
	if (stRec.Seqno <= sg_recSeq && sg_recSeq != 0 && ((U32)stRec.Seqno + sg_recSeq) != (U32)sg_recSeq)
	{
		PrintErr("unvalid seq %u cur seq %u",stRec.Seqno, sg_recSeq);
		return ERR_PACKAGE_INDEX;
	}
	sg_recSeq = stRec.Seqno;
	*/
	#endif
    
    while (pProcesMsgFunc[i] != NULL)
    {
        ret = (pProcesMsgFunc[i])(stRec.Cmd,stRec.SubCmd, pDataBuf, stRec.DataLen, respBuf, &respLen);//UpgradeInfoCmdProc
        if (ret != ERR_PKG_CMD)
        {
            break;
        }
        i++;
    }
	
	PrintDebug("pProcesMsgFunc exeu ret %d", ret);
	if (stRec.Ack == 0)
	{
		PrintDebug("no need to resp");
		return ERR_SUCCESS;
	}
	
	bzero(pstResp, MAX_TCP_RESP_SIZE);
    pstResp->Cmd = stRec.Cmd; 
	CREATE_PKG_HEAD_FMT(pstResp, stRec.Seqno, 0, ret, stRec.SubCmd);
	
	pkgRet = UpgradePkgPacket(respBuf, respLen, pstResp);
	if (pkgRet <=  0)
	{
		PrintErr("pkg err ret %d", pkgRet);
		return pkgRet;
	}
	else
	{
		tcpSendLen = pkgRet;
		PrintDebug("resp len %d",tcpSendLen);
	}
	
	return tcpSendLen;
}

int UdpServProc(U8* inBuf, INT32 inLen,U8* outBuf, INT32 maxOutLen, PProcessCmdMsg pProcesMsgFunc[])
{
	INT32 ret = -1;
	INT32 pkgRet = -1;
	TUpgradePkgFmt st_rec;
	PUpgradePkgFmt pstRespPkg = NULL;
	U16 respLen = 0;
	U8 respData[MAX_UDP_SEND_SIZE] = {0};
	U8* pDataBuf = NULL;
	INT32 respBufLen = 0;

	pstRespPkg = (PUpgradePkgFmt)outBuf;
	bzero(pstRespPkg, MAX_UDP_SEND_SIZE);
	bzero(respData, sizeof(respData));
	bzero(&st_rec, sizeof(st_rec));

		
	ret = UpgradePkgParse(inBuf, inLen, &st_rec, &pDataBuf);//pDataBuf point to inBuf->dataBuf area
	if (ret == 0)
	{
		if (st_rec.Cmd != CMD_MAJOR_UPDATE)
		{
			PrintErr("unsupport cmd");
			return ERR_PKG_CMD;
		}

        int i = 0;
        while (pProcesMsgFunc[i] != NULL)
        {
            ret = (pProcesMsgFunc[i])(st_rec.Cmd,st_rec.SubCmd, pDataBuf, st_rec.DataLen, respData, &respLen);
            if (ret != ERR_PKG_CMD)
            {
                break;
            }
            i++;
        }

		if (st_rec.Ack)
		{
		    pstRespPkg->Cmd = CMD_MAJOR_UPDATE;//TODO
			CREATE_PKG_HEAD_FMT(pstRespPkg, st_rec.Seqno, 0, ret, st_rec.SubCmd);
			pkgRet = UpgradePkgPacket(respData, respLen, pstRespPkg);
			if (pkgRet > 0)
			{
				respBufLen = pkgRet;
				PrintInfo("resp len %d", respBufLen);
			}
			else
			{
				PrintErr("pkg err ret %d", pkgRet);
				return pkgRet;
			}
		}	
		else
		{
			PrintInfo("no need to resp");
			return ERR_SUCCESS;
		}
		
	}
	else
	{
		PrintErr("parse err ret %d", ret);

		if (strstr((char*)inBuf, "echo test") != NULL)
		{
			PrintErr("get echo test str=%s", inBuf);
			snprintf((char*)pstRespPkg, maxOutLen, "hello world here is your msg: %s", inBuf);
			respBufLen = strlen((char*)pstRespPkg);
		}
	}

	return respBufLen;
}


INT32 TcpDataPackWithProp(U8* inBuf, INT32 inLen, U8* outBuf, INT32 maxOutLen)
{
	INT32 ret = -1;
	INT32 outLen = 0;
	PUpgradePkgFmt pst_pkg;
	ASSERT_DESC(outBuf != NULL, "outbuf null");
    //ASSERT_DESC(inBuf != NULL,  "inBuf null");
	
	bzero(outBuf, maxOutLen);
	pst_pkg = (PUpgradePkgFmt)outBuf;
	
	pst_pkg->Seqno = SetUpgradeFmtSeqno();
	pst_pkg->Ack = 1;

    pst_pkg->Cmd = CMD_MAJOR_STR_TEST;
	pst_pkg->SubCmd = SUBCMD_TEST_STR;
	pst_pkg->ErrCode = 0;

	ret = UpgradePkgPacket(inBuf, inLen, pst_pkg);
	if (ret <= 0)
	{
		return ret;
	}
	
	outLen = ret;
    return outLen;
}




#endif

