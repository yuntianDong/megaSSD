/*******************************************************************************
 ******     Copyright (c) 2014--2018 OSR.Co.Ltd. All rights reserved.     ****** 
 *******************************************************************************/
#ifndef _OSR_SM4_H_
#define _OSR_SM4_H_

#include "Type.h"

/* data type 
typedef unsigned char U8;
typedef char S8;
typedef unsigned int U32;
*/


//定义返回值错误码
enum OSR_SM4_RET_CODE
{
	OSR_SM4Success = 0, 
	OSR_SM4BufferNull,            // 空指针
	OSR_SM4InputTooLong,          // 输入消息太长
	OSR_SM4InputLenInvalid,       // 输入长度非法，如长度为0，或者非分组长度倍数等
	OSR_SM4CryptInvalid,          // 非加解密操作，无效
	OSR_SM4InOutSameBuffer,       // 输入输出同一个buffer
	OSR_SM4GCMCheckFail           // GCM模式解密校验失败
};

//定义加解密操作
typedef enum 
{
	OSR_SM4_DECRYPT = 0,
	OSR_SM4_ENCRYPT
}OSR_SM4_CRYPT;


//XTS模式下context定义
typedef struct {
	U32 xts_sm4_rk_buf[32];
	U32 xts_sm4_t[4];
	OSR_SM4_CRYPT xts_sm4_en_de;
	U8 xts_sm4_first_block;
} XTS_SM4_CTX;



extern "C" {

U8 OSR_SM4_Init(U8 key[16]);

U8 OSR_SM4_ECB(U8 *in, U32 inByteLen, U8 En_De, U8 *out);

U8 OSR_SM4_CBC(U8 *in, U32 inByteLen, U8 iv[16], U8 En_De, U8 *out);

U8 OSR_SM4_CFB_128(U8 *in, U32 inByteLen, U8 IV[16], U8 En_De, U8 *out);

U8 OSR_SM4_OFB_128(U8 *in, U32 inByteLen, U8 IV[16], U8 *out);

U8 OSR_SM4_CTR(U8 *in, U32 inByteLen, U8 CTR[16], U8 *out);

U8 OSR_SM4_Close();



U8 XTS_SM4_init(XTS_SM4_CTX *ctx, U8 key[32], U8 i[16], OSR_SM4_CRYPT En_De);
U8 XTS_SM4_crypto_block(XTS_SM4_CTX *ctx, U8 *in, U32 inByteLen, U8 *out);
U8 XTS_SM4_crypto_remainder(XTS_SM4_CTX *ctx, U8 *in, U32 inByteLen, U8 *out);
U8 XTS_SM4_crypto(U8 key[32], U8 i[16], OSR_SM4_CRYPT En_De, U8 *in, U32 inByteLen, U8 *out);

}

#endif
