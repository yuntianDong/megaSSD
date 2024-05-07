/*******************************************************************************
 ******     Copyright (c) 2014--2018 OSR.Co.Ltd. All rights reserved.     ****** 
 *******************************************************************************/
#ifndef _OSR_SM3_H_
#define _OSR_SM3_H_

#include "Type.h"



//���巵��ֵ������
enum OSR_SM3_RET_CODE
{
    OSR_SM3Success = 0, 
    OSR_SM3BufferNull,            // ��ָ��
    OSR_SM3InputTooLong,          // ������Ϣ̫��
//    OSR_SM3InOutSameBuffer        // �������ͬһ��buffer
};



/* type to hold the SM3 context */
typedef struct
{
    U32 count[2];  //�洢�Ѿ�������Ϣ�ı��س���
    U32 hash[8];   //U32 *hash; ���ڴ���м估���ս��
    U32 wbuf[16];
}OSR_SM3_Ctx;

extern "C" {
U8 OSR_SM3_Init(OSR_SM3_Ctx * ctx);

U8 OSR_SM3_Process(OSR_SM3_Ctx * ctx, U8 * message, U32 byteLen);

U8 OSR_SM3_Done(OSR_SM3_Ctx * ctx, U8 digest[32]);

U8 OSR_SM3_Hash(U8 * message, U32 byteLen, U8 digest[32]);
}

#endif
