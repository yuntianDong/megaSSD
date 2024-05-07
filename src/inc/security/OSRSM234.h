#ifndef _X_OSR_SM234_H
#define _X_OSR_SM234_H

#include "sm234/SM2.h"
#include "sm234/SM3.h"
#include "sm234/SM4.h"
#include "buffer/Buffers.h"

/*
 * OSR SM2
 * */
#define LEN_PRIVATE_KEY				32
#define LEN_PUBLIC_KEY				65
#define LEN_SIGNATURE				64
#define LEN_E_KEY					32
#define LEN_Z_KEY					32

#define MAX_LEN_USR_ID				8192
#define DEF_USER_ID					"1234567812345678"
#define DEF_LEN_USER_ID				16

typedef enum _enMSeqMode
{
	MSeq_C1C2C3	=0,
	MSeq_C1C3C2	=1,
}enMSeqMode;

class OSRSM2
{
private:
	U8* mpUsrID;
	U16 mUsrIDLen;

	bool mbKeySet;
	bool mbEKeyCal;
	bool mbZKeyCal;
	bool mbSignCal;

	U8 mLastErrCode;
	enMSeqMode	mMSeqMode;

	U8 mPriKey[LEN_PRIVATE_KEY];
	U8 mPubKey[LEN_PUBLIC_KEY];
	U8 mEKey[LEN_E_KEY];
	U8 mZKey[LEN_Z_KEY];
	U8 mSignature[LEN_SIGNATURE];

	void _Char2Ascii(const char* str,U8* &vals,U16& valLen);
	bool _GetPKey(void);
	bool _GetEKey(U8* Msg,U32 MsgLen);
	bool _GetZKey(void);
	bool _GetSign(void);
	bool _CheckSign(void);

	U32 _Encrypt(U8* M,U32 MLen,U8* C);
	U32 _Decrypt(U8* C,U32 CLen,U8* M);
public:
	OSRSM2(void);
	OSRSM2(const char* usrID);
	virtual ~OSRSM2(void);

	U8 GetLastError(void){return mLastErrCode;};

	void SetMSeqMode(enMSeqMode seqMode) {mMSeqMode = seqMode;};
	enMSeqMode GetMSeqMode(void) {return mMSeqMode;};

	void Dump(void);

	void SetPKey(Buffers* key,uint16_t priKeyOff,uint16_t pubKeyOff);
	void SetEKey(Buffers* eKey,uint16_t kOffset=0);
	void SetZKey(Buffers* zKey,uint16_t kOffset=0);
	void SetSign(Buffers* sign,uint16_t sOffset=0);
	void SetUsrID(Buffers* usrID,uint16_t offset=0);
	void GetPKey(Buffers* pKey,uint16_t priKeyOff,uint16_t pubKeyOff);
	void GetEKey(Buffers* eKey,uint16_t kOffset=0);
	void GetZKey(Buffers* zKey,uint16_t kOffset=0);
	void GetSign(Buffers* sign,uint16_t sOffset=0);
	void GetUsrID(Buffers* usrID,uint16_t offset=0);

	bool Sign(Buffers* M,uint32_t mLen);
	bool Verify(void);

	uint32_t Encrypt(Buffers* M,uint32_t mLen,Buffers* C);
	uint32_t Decrypt(Buffers* C,uint32_t cLen,Buffers* M);
};

/*
 * OSR SM3
 */
#define LEN_DIGEST			32

typedef enum _enSM3Step
{
	SM3_STEP_INIT,
	SM3_STEP_PROC,
	SM3_STEP_DONE
}enSM3Step;

class OSRSM3
{
private:
	OSR_SM3_Ctx		mOSRSM3Ctx;
	U8				mDigest[LEN_DIGEST];
	enSM3Step		mCurrStep;
	U8 				mLastErrCode;

	void ReinitClass(void);
public:
	OSRSM3(void);

	void Dump(void);

	U8 GetLastError(void){return mLastErrCode;};
	bool GetDigest(Buffers* d,uint32_t offset);

	bool Step1_Init(void);
	bool Step2_Process(Buffers* M,uint32_t mLen);
	bool Step3_Done(void);

	bool GetDigestHash(Buffers* M,uint32_t mLen);
};

/*
 * OSR SM4
 */

typedef enum _enSM4Mode
{
	SM4_M_ECB,
	SM4_M_CBC,
	SM4_M_CFB_128B,
	SM4_M_OFB_128B,
	SM4_M_CTR,
	SM4_M_XTS_STEP,
	SM4_M_XTS_ONCE,
	SM4_M_FENCE,
}enSM4Mode;

#define LEN_SM4_KEY			16
#define LEN_SM4_IV			16
#define LEN_SM4_CTR			16

#define SM4_INPUT_ALIGN		16
#define SM4_INPUT_ALIGN_128	128

class OSRSM4
{
private:
	XTS_SM4_CTX	mXTSSM4CTX;
	U8			mSM4Key[LEN_SM4_KEY];
	U8			mSM4IV[LEN_SM4_IV];
	U8			mSM4CTR[LEN_SM4_CTR];
	enSM4Mode	mSM4Mode;

	U8 			mLastErrCode;
	bool		bKeySet;
	bool		bIVSet;
	bool		bCTRSet;

	bool		Initial(bool bEncrypt);
	bool		CheckValidate(uint32_t mLen);
public:
	OSRSM4(void);

	U8 GetLastError(void){return mLastErrCode;};
	void SetSM4Mode(enSM4Mode mode) {
		if(mode < SM4_M_FENCE){
			mSM4Mode = mode;
		}
	};
	enSM4Mode GetSM4Mode(void) {return mSM4Mode;};

	void Dump(void);

	bool SetSM4Key(Buffers* K,uint32_t offset);
	bool GetSM4Key(Buffers* K,uint32_t offset);
	bool SetSM4IV(Buffers* IV,uint32_t offset);
	bool GetSM4IV(Buffers* IV,uint32_t offset);
	bool SetSM4CTR(Buffers* CTR,uint32_t offset);
	bool GetSM4CTR(Buffers* CTR,uint32_t offset);

	bool Encrypt(Buffers* M,uint32_t mLen,Buffers* C);
	bool Decrypt(Buffers* C,uint32_t cLen,Buffers* M);
};

#endif
