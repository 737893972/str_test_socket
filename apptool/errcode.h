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




#define ERR_SUCCESS			0  //成功
#define ERR_FILE_OVERSIZE	-1  //升级包过大
#define ERR_NO_ENOUGH_SPACE	-2  //设备空间不足
#define ERR_UN_AUTHORIZE	-3  //无升级权限
#define ERR_DEV_BUSY		-4  //设备繁忙  请重试
#define ERR_TCP_CONT_FAIL	-5  //Tcp建立失败

#define ERR_NO_SUPPT_UPDATE	-6 //不支持升级
#define ERR_BUF_OVERSIZE	-7 //单包过大
#define ERR_OUTDATED_VER	-8 //此版本过低
#define ERR_CHECKSUM		-9 //包校验错误
#define ERR_INVALID_AUTHENTIC_CODE	-10 //升级授权码无效
#define ERR_PACKAGE_INDEX	-11//包序号不符合预期 主机可继续尝试
#define ERR_FILE_CHECKSUM	-12//整包校验错误
#define ERR_PARAM_INVALID	-13//参数错误
#define ERR_PKG_FMT			-14//格式错误
#define ERR_PKG_CMD			-15//命令错误
#define ERR_UDP_ERR			-16//UDP错误
#define ERR_TCP_ERR			-17//TCP错误
#define ERR_EXECUTE_FAIL	-18//执行失败
#define ERR_NONE_DATA		-19//无数据
#define ERR_FILE_ERR		-20//文件异常
#define ERR_LEN_ERR			-21//长度异常
#define ERR_MALLOC			-22//申请内存异常
#define ERR_UPGE_FILE_TYPE	-23//升级版本类型错误
#define ERR_SOCKET_ERR		-24//socket错误
#define ERR_SOCKET_TIMEOUT	-25//等待超时
#define ERR_TCP_NO_CONT		-26//TCP未连接




#define ERR_UNKONW_ERR		-128 //未知错误 若有其他提示 见databuf


#endif

#endif /*  */
