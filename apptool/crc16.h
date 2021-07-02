/**
 * FileName: crc16.h
 *
 * Copyright (C) 
 *
 * Created: 
 * Author: 
 *
 * Description: crc��Ӣ��ȫ��Ϊcyclic redundancy check����������Ϊѭ������
 * 				У�飨�룩������һ����Ҫ�����Է����룬����ͽ��뷽���򵥣����
 * 				����ǿ����ͨ������㷺������ʵ�ֲ����ơ�crc 16 ʹ��crc 16 bit
 * 				У���
 */

//---------------------------------------



#ifndef CRC16_H_
#define CRC16_H_

#include "glo_type.h"


/**
 * ������	: Crc16CalcBuf
 * ����˵��	: ������ݵ�crc 16 ��У���
 * ����˵��	:
 *		data
 *			[in] ������
 *		dataLen
 *			[in] �������Ĵ�С

 * ����ֵ	:
 *		����ó� ��crc 16 У���
 */

U8 Crc16LSBCalcBuf(U8 *data, U16 dataLen);

U16 Crc16CalcBuf(U8 *data, U16 dataLen);

U16 Crc16CalcBufRepeate(U8 *data, U16 dataLen, U16* cksumTemp);


#endif
