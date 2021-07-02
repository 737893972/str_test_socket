/**
 * FileName: server_demo.c
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: 
 */
 

#include "glo_type.h"
#include "tcpserverproc.h"
#include "upgrade_prot.h"
#include "errcode.h"
#include "timetool.h"
#include "devinfo.h"


#include "thread.h"
#include <unistd.h>
#include <signal.h> 


//########################################
#define DEMO_VERSION 		"Ver_0.0.1_"
#define DEMO_NAME 		    "server demo"
#define VERSION		DEMO_VERSION""__DATE__	


static pthread_t *sg_tcpServerThreadID = NULL;
static BOOL sgThreadExit = FALSE;
static U16     sgServPort = 19003;


void * TCPUpgradeProcess(void * arg)
{
    while(FALSE == sgThreadExit)
    {
        TcpServMultiCliProc();
    }
    return NULL;
}


/*
 * ProFuncInit
 */
void ProFuncInit(void)
{
	int ret = 0;
    ret = 0;
    PrintInfo("%s start---ver:%s", DEMO_NAME, VERSION);
    RegisterSignalProc();

    ret = TCPServerCacheInit();
    ASSERT_DESC(ret == ERR_SUCCESS, "cache malloc failed");
    if (ret < ERR_SUCCESS)
    {
        EXIT("client init failed,exit");
    }

    ret = TCPServerSocketInit(sgServPort);
    ASSERT_DESC(ret == ERR_SUCCESS, "tcp server established fail");
    if (ret < ERR_SUCCESS)
    {
        EXIT("client init failed,exit");
    }

	sg_tcpServerThreadID = CreateManageThread(NULL, TCPUpgradeProcess);
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
    if (sg_tcpServerThreadID != NULL)
    {
        StopManageLogicThread(sg_tcpServerThreadID);
    }
    TcpServerCommDeInit();
}

void UpdateServerIPPort(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (argc > 1)
        {
            if (argv[1][0] != '\0')
            {
                sgServPort = atoi(argv[1]);
            }
        }
        PrintWarn("update tcp server port %d", sgServPort);
    }
    else
    {
        printf("#############################");
        printf("\n\nusage: run server demo with server tcp Port value can modify default IP and Port\n");
        printf("      eg:./server_demo 25502\n\n");
        printf("#############################");
    }
}


int main(int argc, char **argv)//TODO ´«Èëip port
{
    UpdateServerIPPort(argc, argv);
	ProFuncInit();
	ProFuncRun();
	ProFuncEnd();

	return 0;
}


