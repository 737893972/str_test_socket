/**
 * FileName: client_demo.c
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: 
 */
 

#include "tcpproc.h"
#include "glo_type.h"
#include "thread.h"
#include "errcode.h"
#include "devinfo.h"
#include "timetool.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>  
#include <string.h>



//#################### ####################
#define DEMO_VERSION 		"Ver_0.0.1_"
#define DEMO_NAME 		    "client demo"

#define VERSION		DEMO_VERSION""__DATE__	
#define MAX_WAIT_RESP_SEC   (5)

static pthread_t *sg_tcpProcThreadID = NULL;
static BOOL sgThreadExit = FALSE;
static U32 sgSendTick = 0;
const char* consExitStr = ".exit";
static char    sgServIPBuf[16] = "127.0.0.1";
static U16     sgServPort = 19003;

void * TcpClientLoopProc(void * arg)
{
    char strBuf[1000] = {0};
    int sStrLen = 0;
    int ret = ERR_SUCCESS;
    
    while(FALSE == sgThreadExit)
    {
        if (sgSendTick == 0)
        {
            if ((sStrLen = GetStdinStr(strBuf, sizeof(strBuf), stdin)) > 0)
            {
                if (sStrLen == strlen(consExitStr) && strncmp(consExitStr, strBuf, strlen(consExitStr)) == 0)
                {
                    sgThreadExit = TRUE;
                    PrintWarn("rec exit cmd, now exit");
                    break;
                    //EXIT("rec exit cmd, now exit");
                }
                ret = TCPClientSendData((U8*)strBuf, sStrLen);
                if (ret == ERR_TCP_ERR || ret == ERR_TCP_NO_CONT)
                {
                    ASSERT_DESC(ret != ERR_TCP_ERR, "tcp err");
                    ASSERT_DESC(ret != ERR_TCP_NO_CONT, "tcp disconnected");
                }
                else if(ret == ERR_SUCCESS)
                {
                    sgSendTick = GetSysSecCnt();
                    PrintInfo("send data to serv successful,wait response in %dsec",MAX_WAIT_RESP_SEC);
                }
                else
                {
                    printf("send words to server err, try again");
                    sgSendTick = 0;
                }
            }
        }

        if (sgSendTick > 0)
        {
            ret = TmrTcpProc();
            if (ret == ERR_SUCCESS)
            {
                sgSendTick = 0;
                PrintDebug("get server resp successful, now you can try again, please input words:");
            }
            else
            {
                ASSERT_DESC(ret != ERR_TCP_ERR, "tcp rec err exit");
                ASSERT_DESC(ret != ERR_TCP_NO_CONT, "tcp disconnected exit");
                ASSERT_DESC(TRUE != CheckUpdateProcAlive(sgSendTick, MAX_WAIT_RESP_SEC), "tcp rec timeout exit");
        	}
        }
        
        MsSleep(100);
    }
    return NULL;
}
/*
 * 
 */
void ProFuncInit(void)
{
    int ret = 0;

    PrintInfo("%s start---ver:%s", DEMO_NAME, VERSION);
    RegisterSignalProc();

    ret = TCPClientCacheInit();
    ASSERT_DESC(ret == ERR_SUCCESS, "cache malloc failed");
    if (ret < ERR_SUCCESS)
    {
        EXIT("client init failed,exit");
    }
    ret = TCPTryConnectServer(sgServIPBuf, sgServPort);
    ASSERT_DESC(ret == ERR_SUCCESS, "tcp connected failed");
    if (ret < ERR_SUCCESS)
    {
        EXIT("client init failed,exit");
    }
    sg_tcpProcThreadID = CreateManageThread(NULL, TcpClientLoopProc);

    return;
}

/*
 * ProFuncRun
 */
void ProFuncRun(void)
{
    while(FALSE == sgThreadExit)
    {
        MsSleep(1000);
    }
    PrintWarn("thread %s end",DEMO_NAME);
}


/*
 *  ProFuncEnd
 */
void ProFuncEnd(void)
{
    if (sg_tcpProcThreadID != NULL)
    {
        StopManageLogicThread(sg_tcpProcThreadID);
    }
    TcpCommDeInit();
}

void UpdateServerIPPort(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (argc > 1)
        {
            if (argv[1][0] != '\0')
            {
                snprintf(sgServIPBuf, sizeof(sgServIPBuf), "%s", argv[1]);
            }
        }
        if (argc > 2)
        {
            if (argv[2][0] != '\0')
            {
                sgServPort = atoi(argv[2]);
            }
        }
        PrintWarn("update ip %s port %d", sgServIPBuf, sgServPort);
    }
    else
    {
        printf("#############################\n");
        printf("\n\nusage: run client demo with server IP and tcp Port value can modify default IP and Port\n");
        printf("      eg:./client_demo 192.168.1.1 25502\n\n");
        printf("#############################\n");
    }
        
}

int main(int argc, char **argv)
{
    UpdateServerIPPort(argc, argv);
    ProFuncInit();
    ProFuncRun();
    ProFuncEnd();
    return 0;
}


