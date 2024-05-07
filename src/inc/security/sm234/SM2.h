/*******************************************************************************
 ******     Copyright (c) 2014--2018 OSR.Co.Ltd. All rights reserved.     ****** 
 *******************************************************************************/
#ifndef _OSR_SM2_H_
#define _OSR_SM2_H_

#include "Type.h"

//���巵��ֵ������
enum OSR_SM2_RET_CODE
{
	OSR_SM2Success = 0, 
	OSR_SM2BufferNull,            // ��ָ��
	OSR_SM2InputLenInvalid,       // ���볤�ȷǷ����糤��Ϊ0��
	OSR_SM2PointHeadNot04,        // ��Կ�������ϵ㲻��04��ͷ
	OSR_SM2PubKeyError,           // ��Կ����
	OSR_SM2NotInCurve,            // �㲻��������
	OSR_SM2IntegerTooBig,         // �������������K�ȹ���
	OSR_SM2ZeroALL,               // ȫ0
	OSR_SM2DecryVerifyFailed,     // ����У��ʧ��
	OSR_SM2VerifyFailed,          // ǩ����֤ʧ��
	OSR_SM2ExchangeRoleInvalid,   // ��Կ�����û���ɫ��Ч
	OSR_SM2ZeroPoint,             // 0�㣬������Զ��
	OSR_SM2InOutSameBuffer        // �������ͬһ��buffer
};

//SM2��Կ�����û���ɫ
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
