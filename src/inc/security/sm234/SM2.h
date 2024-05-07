/*******************************************************************************
 ******     Copyright (c) 2014--2018 OSR.Co.Ltd. All rights reserved.     ****** 
 *******************************************************************************/
#ifndef _OSR_SM2_H_
#define _OSR_SM2_H_

#include "Type.h"

//定义返回值错误码
enum OSR_SM2_RET_CODE
{
	OSR_SM2Success = 0, 
	OSR_SM2BufferNull,            // 空指针
	OSR_SM2InputLenInvalid,       // 输入长度非法，如长度为0等
	OSR_SM2PointHeadNot04,        // 公钥或曲线上点不以04开头
	OSR_SM2PubKeyError,           // 公钥错误
	OSR_SM2NotInCurve,            // 点不在曲线上
	OSR_SM2IntegerTooBig,         // 整数，如随机数K等过大
	OSR_SM2ZeroALL,               // 全0
	OSR_SM2DecryVerifyFailed,     // 解密校验失败
	OSR_SM2VerifyFailed,          // 签名验证失败
	OSR_SM2ExchangeRoleInvalid,   // 密钥交换用户角色无效
	OSR_SM2ZeroPoint,             // 0点，即无穷远点
	OSR_SM2InOutSameBuffer        // 输入输出同一个buffer
};

//SM2密钥交换用户角色
enum OSR_SM2_Exchange_Role
{
	OSR_SM2_Role_Receiver = 0,
	OSR_SM2_Role_Sender
};

extern "C" {

U8 OSR_SM2_Init();

U8 OSR_SM2_GetKey(U8 priKey[32], U8 pubKey[65]);

U8 OSR_SM2_GetPubKey_from_PriKey(U8 priKey[32], U8 pubKey[65]);

U8 OSR_SM2_sign_U32(U32 e[8], U32 k[8], U32 dA[8], U32 r[8], U32 s[8]);

U8 OSR_SM2_Sign(U8 E[32], U8 priKey[32], U8 signature[64]);

U8 OSR_SM2_Verify(U8 E[32], U8 pubKey[65], U8 signature[64]);

U8 OSR_SM2_encrypt_U32(U8 *M, U32 MByteLen, U32 k[8], U8 pubKey[65], U8 tag, U8 *C, U32 *CByteLen);

U8 OSR_SM2_Encrypt(U8 *M, U32 MByteLen, U8 pubKey[65], U8 tag, U8 *C, U32 *CByteLen);

U8 OSR_SM2_Decrypt(U8 *C, U32 CByteLen, U8 priKey[32], U8 tag, U8 *M, U32 *MByteLen);

U8 OSR_SM2_GetZ(U8 *ID, U16 byteLenofID, U8 pubKey[65], U8 Z[32]);

U8 OSR_SM2_GetE(U8 *M, U32 byteLen, U8 Z[32], U8 E[32]);

U8 OSR_SM2_ExchangeKey(U8 role, U8 *dA, U8 *PB, U8 *rA, U8 *RA, U8 *RB, U8 *ZA, U8 *ZB,	U32 kByteLen, U8 *KA, U8 *S1, U8 *SA);

U8 OSR_SM2_Close();

}

#endif
