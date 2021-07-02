/*
 * File name	: glo_type.h
 *
 * Created on	: 
 * Author		: 
 * Version		: 
 * Language		: C
 * Copyright	: Copyright 
 *
 */



#ifndef GLO_TYPE_H_
#define GLO_TYPE_H_

#include <stdio.h>
#include <stdlib.h>

#include "timetool.h"


#if 1

//#define NDEBUG //cancle assert

#define MAX_TCP_SERVER_ACCEPT_CNT   20



        

#ifdef NDEBUG
    #define ASSERT(EXPRESSION)                  do{ return; }while(0)
    #define ASSERT_DESC(EXPRESSION, descStr)    do{ return; }while(0)
    #define EXIT(echoStr)                       do{  exit(0);}while(0)
    
#else
    #define ASSERT(EXPRESSION)                       \
                do {                                                          \
                    if (!(EXPRESSION)) {                                      \
                        PrintErr("assert func %s line %d", __func__, __LINE__);\
                        exit(0);\
                    }                                                         \
                } while(0)

    #define ASSERT_DESC(EXPRESSION, descStr)     \
        do{                                 \
            if (!(EXPRESSION)) {            \
                PrintErr("assert func %s line %d desc:%s", __func__, __LINE__, descStr);    \
                exit(0);                    \
            }                               \
        }while(0)
        
    #define EXIT(echoStr)     do{ PrintWarn("%s", echoStr); exit(0);}while(0)

#endif



#define FTP_DEBUG_HEX	0//enable print hex data buf


#ifdef NDEBUG
    #define PRINT_HEX(buf, len, desc)  do{ ; }while(0)
#else
    #if FTP_DEBUG_HEX
    #define PRINT_HEX(buf, len, desc)  PrintHexDescp(buf, len, desc)
    #else
    #define PRINT_HEX(buf, len, desc)  do{ ; }while(0)
    #endif
#endif



    
#define    COLOR_NONE 					"\033[0m"
#define    BG_NONE_FONT_WHITE 			"\033[1;37m"
#define    BG_NONE_FONT_CYAN 			"\033[1;36m"
#define    BG_NONE_FONT_MAUVE 			"\033[1;35m"
#define    BG_NONE_FONT_YELLOW 			"\033[1;33m"
#define    BG_NONE_FONT_GREEN 			"\033[1;32m"
#define    BG_NONE_FONT_RED 			"\033[1;31m"
#define    BG_BLACK_FONT_WHITE 			"\033[1;40;37m"
#define    BG_WHITE_FONT_BLACK 			"\033[1;47;30m"
#define    BG_COLOR_END         		"\033[0m\r\n"

#ifdef NDEBUG

#define PrintErr(fmt, args...)      do{ ; }while(0)
#define PrintWarn(fmt, args...)     do{ ; }while(0)
#define PrintInfo(fmt, args...)     do{ ; }while(0)
#define PrintDebug(fmt, args...)    do{ ; }while(0)
#define Free(pAddr)    do{ free(pAddr); }while(0)


#else

#define PrintErr(fmt, args...) \
		do{\
			printf("[%s,%s,%s,%d]-" BG_NONE_FONT_RED fmt BG_COLOR_END, DebugLogDateTime(), __FILE__, __func__, __LINE__, ##args);\
		}while(0)
			
#define PrintWarn(fmt, args...) \
		do{\
			printf("[%s,%s,%s,%d]-" BG_NONE_FONT_YELLOW fmt BG_COLOR_END, DebugLogDateTime(), __FILE__, __func__, __LINE__, ##args);\
		}while(0)
				
#define PrintInfo(fmt, args...) \
		do{\
			printf("[%s,%s,%s,%d]-" BG_NONE_FONT_GREEN fmt BG_COLOR_END, DebugLogDateTime(), __FILE__, __func__, __LINE__, ##args);\
		}while(0)
				
#define PrintDebug(fmt, args...) \
		do{\
			if(0) printf("[%s,%s,%s,%d]-" BG_NONE_FONT_MAUVE fmt BG_COLOR_END, DebugLogDateTime(), __FILE__, __func__, __LINE__, ##args);\
		}while(0)

#define Free(pAddr) \
    do{\
        ASSERT_DESC(pAddr != NULL, "free invalid pointer");\
        PrintDebug("free addr %p", pAddr);\
        free(pAddr);\
    }while(0)
#endif

#ifndef NULL
	#ifdef __cplusplus
	#define NULL 0
	#else
	#define NULL ((void *)0)
	#endif
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef S8
typedef	signed char				S8;
#endif


#ifndef U8
typedef	unsigned char			U8;
#endif

#ifndef U16
typedef	unsigned short			U16;
#endif

#ifndef U32
typedef	unsigned int			U32;
#endif

#ifndef INT32
typedef	signed int				INT32;
#endif




#ifndef U64
typedef	unsigned long long		U64;
#endif

#ifndef BOOL
typedef	int						BOOL;
#endif

#ifndef WORD
typedef unsigned short			WORD;
#endif

#ifndef BYTE
typedef unsigned char			BYTE;
#endif

#ifndef DWORD
typedef unsigned long			DWORD;
#endif
#ifndef LONG
typedef long					LONG;
#endif



#define GetOffset(s,m) (size_t)&(((s *)0)->m)


#ifndef BYTE_FORMAT_DEFINE
	#define BYTE_FORMAT_DEFINE
	#define GCC_PACKED __attribute__((packed))
	#define GCC_ALIGN0 __attribute__((aligned(1)))
#endif

#define ERR_EXIT(m) \
    do { \
    	perror(m); \
    	exit(EXIT_FAILURE); \
    } while (0)
 
#endif

#endif /*  */
