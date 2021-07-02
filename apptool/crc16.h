/**
 * FileName: crc16.h
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: crc的英文全称为cyclic redundancy check，中文名称为循环冗余
 * 				校验（码）。它是一类重要的线性分组码，编码和解码方法简单，检错
 * 				能力强，在通信领域广泛地用于实现差错控制。crc 16 使用crc 16 bit
 * 				校验表
 */

//---------------------------------------



#ifndef CRC16_H_
#define CRC16_H_

#include "glo_type.h"


/**
 * 函数名	: Crc16CalcBuf
 * 功能说明	: 获得数据的crc 16 的校验和
 * 参数说明	:
 *		data
 *			[in] 数据区
 *		dataLen
 *			[in] 数据区的大小

 * 返回值	:
 *		计算得出 的crc 16 校验和
 */

U8 Crc16LSBCalcBuf(U8 *data, U16 dataLen);

U16 Crc16CalcBuf(U8 *data, U16 dataLen);

U16 Crc16CalcBufRepeate(U8 *data, U16 dataLen, U16* cksumTemp);


#endif
