/*
 * file OSRSM234.cpp
 */

#include <stdlib.h>
#include <string.h>

#include "security/OSRSM234.h"

/*
 * OSR SM2
 */
U8 GetMaxVal(U8 val1,U8 val2)
{
	return (val1 > val2)?val1:val2;
}

void DumpArray(const char* msg,U8* datum,U32 len)
{
	printf("%s = ",msg);
	for(U32 idx=0;idx<len;idx++)
	{
		printf("%02x",datum[idx]);
	}
	printf("\n");
}

OSRSM2::OSRSM2(const char* usrID)
	:mpUsrID(NULL),mUsrIDLen(0),mbKeySet(false),mbEKeyCal(false),\
	 mbZKeyCal(false),mbSignCal(false),mLastErrCode(OSR_SM2Success),\
	 mMSeqMode(MSeq_C1C2C3)
{
	_Char2Ascii(usrID,mpUsrID,mUsrIDLen);

	memset(mPriKey,0,sizeof(U8)*LEN_PRIVATE_KEY);
	memset(mPubKey,0,sizeof(U8)*LEN_PUBLIC_KEY);
	memset(mEKey,0,sizeof(U8)*LEN_E_KEY);
	memset(mZKey,0,sizeof(U8)*LEN_Z_KEY);
	memset(mSignature,0,sizeof(U8)*LEN_SIGNATURE);

	::OSR_SM2_Init();
}

OSRSM2::OSRSM2(void)
{
	OSRSM2(DEF_USER_ID);
}

OSRSM2::~OSRSM2(void)
{
	::OSR_SM2_Close();

	if(NULL == mpUsrID)
	{
		free(mpUsrID);
		mpUsrID = NULL;
	}
}

void OSRSM2::_Char2Ascii(const char* str,U8* &vals,U16& valLen)
{
	int len	= strlen(str);

	if(0 >= len)
	{
		return;
	}

	vals = (U8*)malloc(sizeof(U8)*len);
	memset(vals,0,sizeof(U8)*len);

	for(int idx=0;idx<len;idx++)
	{
		vals[idx]	= (U8)(str[idx]);
	}

	valLen = (U16)len;
	return;
}

bool OSRSM2::_GetPKey(void)
{
	U8 rtn = ::OSR_SM2_GetKey(mPriKey,mPubKey);
	mLastErrCode	= rtn;
	mbKeySet 		= true;

	return rtn == OSR_SM2Success;
}

bool OSRSM2::_GetEKey(U8* Msg,U32 MsgLen)
{
	if( (false == mbKeySet && false == _GetPKey())
		|| (false == mbZKeyCal && false == _GetZKey()))
	{
		return false;
	}

	U8 rtn = ::OSR_SM2_GetE(Msg,MsgLen,mZKey,mEKey);
	mLastErrCode	= rtn;
	mbEKeyCal		= true;

	return rtn == OSR_SM2Success;
}

bool OSRSM2::_GetZKey(void)
{
	if(false == mbKeySet && false == _GetPKey())
	{
		return false;
	}

	U8 rtn = ::OSR_SM2_GetZ(mpUsrID,mUsrIDLen,mPubKey,mZKey);
	mLastErrCode	= rtn;
	mbZKeyCal 		= true;

	return rtn == OSR_SM2Success;
}

bool OSRSM2::_GetSign(void)
{
	if(false == mbEKeyCal ||
		(false == mbKeySet && false == _GetPKey()))
	{
		return false;
	}

	U8 rtn = ::OSR_SM2_Sign(mEKey,mPriKey,mSignature);
	mLastErrCode	= rtn;
	mbSignCal		= true;

	return rtn == OSR_SM2Success;
}

bool OSRSM2::_CheckSign(void)
{
	if(( false == mbSignCal && false == _GetSign() )
		|| (false == mbKeySet && false == _GetPKey())
		|| false == mbEKeyCal)
	{
		return false;
	}

	U8 rtn = ::OSR_SM2_Verify(mEKey,mPubKey,mSignature);
	mLastErrCode	= rtn;

	return rtn == OSR_SM2Success;
}

U32 OSRSM2::_Encrypt(U8* M,U32 MLen,U8* C)
{
	if( false == mbKeySet && false == _GetPKey() )
	{
		return false;
	}

	U32 CLen = 0;
	U8 rtn = ::OSR_SM2_Encrypt(M,MLen,mPubKey,(U8)mMSeqMode,C,&CLen);
	mLastErrCode = rtn;

	if(rtn == OSR_SM2Success)
	{
		return CLen;
	}

	return 0;
}

U32 OSRSM2::_Decrypt(U8* C,U32 CLen,U8* M)
{
	if( false == mbKeySet && false == _GetPKey() )
	{
		return false;
	}

	U32 MLen = 0;
	U8 rtn = ::OSR_SM2_Decrypt(C,CLen,mPriKey,(U8)mMSeqMode,M,&MLen);
	mLastErrCode = rtn;

	if(rtn == OSR_SM2Success)
	{
		return MLen;
	}

	return 0;
}

void OSRSM2::SetPKey(Buffers* key,uint16_t priKeyOff,uint16_t pubKeyOff)
{
	for(uint16_t idx=0;idx<GetMaxVal(LEN_PRIVATE_KEY,LEN_PUBLIC_KEY);idx++)
	{
		if(idx < LEN_PRIVATE_KEY)
		{
			mPriKey[idx] = key->GetByte(priKeyOff+idx);
		}
		if(idx < LEN_PUBLIC_KEY)
		{
			mPubKey[idx] = key->GetByte(pubKeyOff+idx);
		}
	}

	mbKeySet = true;
}

void OSRSM2::SetEKey(Buffers* eKey,uint16_t kOffset)
{
	for(uint16_t idx=0;idx<LEN_E_KEY;idx++)
	{
		mEKey[idx]	= eKey->GetByte(kOffset + idx);
	}

	mbEKeyCal= true;
}

void OSRSM2::SetZKey(Buffers* zKey,uint16_t kOffset)
{
	for(uint16_t idx=0;idx<LEN_Z_KEY;idx++)
	{
		mZKey[idx]	= zKey->GetByte(kOffset + idx);
	}

	mbZKeyCal= true;
}

void OSRSM2::SetSign(Buffers* sign,uint16_t sOffset)
{
	for(uint16_t idx=0;idx<LEN_SIGNATURE;idx++)
	{
		mSignature[idx]	= sign->GetByte(sOffset + idx);
	}

	mbSignCal= true;
}

void OSRSM2::SetUsrID(Buffers* usrID,uint16_t offset)
{
	for(uint16_t idx=0;idx<mUsrIDLen;idx++)
	{
		mpUsrID[idx]	= usrID->GetByte(offset + idx);
	}
}

void OSRSM2::GetPKey(Buffers* pKey,uint16_t priKeyOff,uint16_t pubKeyOff)
{
	for(uint16_t idx=0;idx<GetMaxVal(LEN_PRIVATE_KEY,LEN_PUBLIC_KEY);idx++)
	{
		if(idx < LEN_PRIVATE_KEY)
		{
			pKey->SetByte(priKeyOff+idx,mPriKey[idx]);
		}
		if(idx < LEN_PUBLIC_KEY)
		{
			pKey->SetByte(pubKeyOff+idx,mPubKey[idx]);
		}
	}
}

void OSRSM2::GetEKey(Buffers* eKey,uint16_t kOffset)
{
	for(uint16_t idx=0;idx<LEN_E_KEY;idx++)
	{
		eKey->SetByte(kOffset + idx,mEKey[idx]);
	}
}

void OSRSM2::GetZKey(Buffers* zKey,uint16_t kOffset)
{
	for(uint16_t idx=0;idx<LEN_Z_KEY;idx++)
	{
		zKey->SetByte(kOffset + idx,mZKey[idx]);
	}
}

void OSRSM2::GetSign(Buffers* sign,uint16_t sOffset)
{
	for(uint16_t idx=0;idx<LEN_SIGNATURE;idx++)
	{
		sign->SetByte(sOffset + idx,mSignature[idx]);
	}
}

void OSRSM2::GetUsrID(Buffers* usrID,uint16_t offset)
{
	for(uint16_t idx=0;idx<mUsrIDLen;idx++)
	{
		usrID->SetByte(offset + idx,mpUsrID[idx]);
	}
}

bool OSRSM2::Sign(Buffers* M,uint32_t mLen)
{
	if(NULL == M || 0 >= mLen)
	{
		return false;
	}

	if(false == _GetEKey((U8*)(M->GetBufferPoint()),(U32)mLen))
	{
		return false;
	}

	return _GetSign();
}

bool OSRSM2::Verify(void)
{
	return _CheckSign();
}

uint32_t OSRSM2::Encrypt(Buffers* M,uint32_t mLen,Buffers* C)
{
	return _Encrypt((U8*)(M->GetBufferPoint()),mLen,(U8*)(C->GetBufferPoint()));
}

uint32_t OSRSM2::Decrypt(Buffers* C,uint32_t cLen,Buffers* M)
{
	return _Decrypt((U8*)(C->GetBufferPoint()),cLen,(U8*)(M->GetBufferPoint()));
}

void OSRSM2::Dump(void)
{
	printf("\n*** OSRSM2 Debug Dump ***\n\n");

	::DumpArray("User ID",mpUsrID,mUsrIDLen);
	::DumpArray("Private Key",mPriKey,LEN_PRIVATE_KEY);
	::DumpArray("Public Key",mPubKey,LEN_PUBLIC_KEY);
	::DumpArray("E Key",mEKey,LEN_E_KEY);
	::DumpArray("Z Key",mZKey,LEN_Z_KEY);
	::DumpArray("Signature",mSignature,LEN_SIGNATURE);

	printf("mbKeySet = %s\n",(true==mbKeySet)?"True":"False");
	printf("mbEKeyCal = %s\n",(true==mbEKeyCal)?"True":"False");
	printf("mbZKeyCal = %s\n",(true==mbZKeyCal)?"True":"False");
	printf("mbSignCal = %s\n",(true==mbSignCal)?"True":"False");
}

/*
 * OSR SM3
 */
OSRSM3::OSRSM3(void)
{
	ReinitClass();
}

bool OSRSM3::GetDigest(Buffers* d,uint32_t offset)
{
	if(NULL == d || (offset + LEN_DIGEST) >= d->GetBufSize())
	{
		return false;
	}

	for(uint32_t idx=0;idx<LEN_DIGEST;idx++)
	{
		d->SetByte(offset+idx,mDigest[idx]);
	}

	return true;
}

void OSRSM3::ReinitClass(void)
{
	memset(&mOSRSM3Ctx,0,sizeof(OSR_SM3_Ctx));
	memset(mDigest,0,sizeof(U8)*LEN_DIGEST);
	mCurrStep	= SM3_STEP_INIT;
	mLastErrCode= 0;
}

bool OSRSM3::Step1_Init(void)
{
	ReinitClass();

	U8 rtn = ::OSR_SM3_Init(&mOSRSM3Ctx);
	mLastErrCode= rtn;
	mCurrStep	= SM3_STEP_INIT;

	return mLastErrCode == OSR_SM3Success;
}

bool OSRSM3::Step2_Process(Buffers* M,uint32_t mLen)
{
	if(NULL == M || 0 >= mLen)
	{
		return false;
	}

	if(SM3_STEP_INIT != mCurrStep && false == Step1_Init())
	{
		return false;
	}

	U8 rtn = ::OSR_SM3_Process(&mOSRSM3Ctx,(U8*)(M->GetBufferPoint()),mLen);
	mLastErrCode= rtn;
	mCurrStep	= SM3_STEP_PROC;

	return mLastErrCode == OSR_SM3Success;
}

bool OSRSM3::Step3_Done(void)
{
	if(SM3_STEP_PROC != mCurrStep)
	{
		return false;
	}

	U8 rtn = ::OSR_SM3_Done(&mOSRSM3Ctx,mDigest);
	mLastErrCode= rtn;
	mCurrStep	= SM3_STEP_DONE;

	return mLastErrCode == OSR_SM3Success;
}

bool OSRSM3::GetDigestHash(Buffers* M,uint32_t mLen)
{
	if(NULL == M || 0 >= mLen)
	{
		return false;
	}

	U8 rtn = ::OSR_SM3_Hash((U8*)(M->GetBufferPoint()),mLen,mDigest);
	mLastErrCode= rtn;
	mCurrStep	= SM3_STEP_DONE;

	return mLastErrCode == OSR_SM3Success;
}

void OSRSM3::Dump(void)
{
	printf("\n*** OSRSM3 Debug Dump ***\n\n");

	printf("OSR_SM3_Ctx\n");
	::DumpArray(".count = ",(U8*)(mOSRSM3Ctx.count),2*sizeof(U32));
	::DumpArray(".hash = ",(U8*)(mOSRSM3Ctx.hash),2*sizeof(U32));
	::DumpArray(".wbuf = ",(U8*)(mOSRSM3Ctx.wbuf),16*sizeof(U32));
	printf("mCurrStep = %d\n",mCurrStep);
	::DumpArray("mDigest = ",mDigest,LEN_DIGEST);
}

/*
 * OSR SM4
 */
OSRSM4::OSRSM4(void)
{
	memset(&mSM4Key,0,sizeof(U8)*LEN_SM4_KEY);
	memset(&mSM4IV,0,sizeof(U8)*LEN_SM4_IV);

	mSM4Mode	= SM4_M_ECB;
	mLastErrCode= OSR_SM4Success;

	bKeySet = bIVSet = bCTRSet = false;
}

bool OSRSM4::SetSM4Key(Buffers* K,uint32_t offset)
{
	if(NULL == K || (offset+LEN_SM4_KEY) >= K->GetBufSize())
	{
		return false;
	}

	for(uint32_t idx=0;idx<LEN_SM4_KEY;idx++)
	{
		mSM4Key[idx] = K->GetByte(offset+idx);
	}

	bKeySet = true;

	return true;
}

bool OSRSM4::GetSM4Key(Buffers* K,uint32_t offset)
{
	if(NULL == K || (offset+LEN_SM4_KEY) >= K->GetBufSize())
	{
		return false;
	}

	for(uint32_t idx=0;idx<LEN_SM4_KEY;idx++)
	{
		K->SetByte(offset+idx,mSM4Key[idx]);
	}

	return true;
}

bool OSRSM4::SetSM4IV(Buffers* IV,uint32_t offset)
{
	if(NULL == IV || (offset+LEN_SM4_IV) >= IV->GetBufSize())
	{
		return false;
	}

	for(uint32_t idx=0;idx<LEN_SM4_IV;idx++)
	{
		mSM4IV[idx] = IV->GetByte(offset+idx);
	}

	bIVSet = true;

	return true;
}

bool OSRSM4::GetSM4IV(Buffers* IV,uint32_t offset)
{
	if(NULL == IV || (offset+LEN_SM4_IV) >= IV->GetBufSize())
	{
		return false;
	}

	for(uint32_t idx=0;idx<LEN_SM4_IV;idx++)
	{
		IV->SetByte(offset+idx,mSM4IV[idx]);
	}

	return true;
}

bool OSRSM4::SetSM4CTR(Buffers* CTR,uint32_t offset)
{
	if(NULL == CTR || (offset+LEN_SM4_CTR) >= CTR->GetBufSize())
	{
		return false;
	}

	for(uint32_t idx=0;idx<LEN_SM4_CTR;idx++)
	{
		mSM4CTR[idx] = CTR->GetByte(offset+idx);
	}

	bCTRSet = true;

	return true;
}

bool OSRSM4::GetSM4CTR(Buffers* CTR,uint32_t offset)
{
	if(NULL == CTR || (offset+LEN_SM4_CTR) >= CTR->GetBufSize())
	{
		return false;
	}

	for(uint32_t idx=0;idx<LEN_SM4_CTR;idx++)
	{
		CTR->SetByte(offset+idx,mSM4CTR[idx]);
	}

	return true;
}

bool OSRSM4::Initial(bool bEncrypt)
{
	U8	rtn	= OSR_SM4Success;

	OSR_SM4_CRYPT En_De	= (true == bEncrypt)?OSR_SM4_ENCRYPT:OSR_SM4_DECRYPT;

	if(SM4_M_XTS_STEP == mSM4Mode || SM4_M_XTS_ONCE == mSM4Mode)
	{
		memset(&mXTSSM4CTX,0,sizeof(XTS_SM4_CTX));
		rtn = ::XTS_SM4_init(&mXTSSM4CTX,mSM4Key,mSM4IV,En_De);
	}
	else
	{
		rtn = ::OSR_SM4_Init(mSM4Key);
	}

	return rtn == OSR_SM4Success;
}

bool OSRSM4::CheckValidate(uint32_t mLen)
{
	if(false == bKeySet)
	{
		return false;
	}

	if(SM4_M_ECB != mSM4Mode && SM4_M_CTR != mSM4Mode && false == bIVSet)
	{
		return false;
	}

	if(SM4_M_CTR != mSM4Mode && false == bCTRSet)
	{
		return false;
	}

	if(0 == mLen)
	{
		return false;
	}

	if(SM4_M_CTR != mSM4Mode && (0 != mLen % SM4_INPUT_ALIGN))
	{
		return false;
	}

	if(SM4_M_CFB_128B == mSM4Mode && SM4_M_OFB_128B == mSM4Mode
			&& (0 != mLen % SM4_INPUT_ALIGN_128))
	{
		return false;
	}

	return true;
}

bool OSRSM4::Encrypt(Buffers* M,uint32_t mLen,Buffers* C)
{
	U8	rtn	= OSR_SM4Success;

	if(false == CheckValidate(mLen))
	{
		return false;
	}

	if(SM4_M_XTS_ONCE != mSM4Mode && false == Initial(true))
	{
		return false;
	}

	if(SM4_M_ECB == mSM4Mode)
	{
		rtn = ::OSR_SM4_ECB((U8*)(M->GetBufferPoint()),mLen,OSR_SM4_ENCRYPT,(U8*)(C->GetBufferPoint()));
	}
	else if(SM4_M_CBC == mSM4Mode)
	{
		rtn = ::OSR_SM4_CBC((U8*)(M->GetBufferPoint()),mLen,mSM4IV,OSR_SM4_ENCRYPT,(U8*)(C->GetBufferPoint()));
	}
	else if(SM4_M_CFB_128B == mSM4Mode)
	{
		rtn = ::OSR_SM4_CFB_128((U8*)(M->GetBufferPoint()),mLen,mSM4IV,OSR_SM4_ENCRYPT,(U8*)(C->GetBufferPoint()));
	}
	else if(SM4_M_OFB_128B == mSM4Mode)
	{
		rtn = ::OSR_SM4_OFB_128((U8*)(M->GetBufferPoint()),mLen,mSM4IV,(U8*)(C->GetBufferPoint()));
	}
	else if(SM4_M_CTR == mSM4Mode)
	{
		rtn = ::OSR_SM4_CTR((U8*)(M->GetBufferPoint()),mLen,mSM4CTR,(U8*)(C->GetBufferPoint()));
	}
	else if(SM4_M_XTS_STEP == mSM4Mode)
	{
		if(0 == mLen % SM4_INPUT_ALIGN)
		{
			rtn = ::XTS_SM4_crypto_block(&mXTSSM4CTX,(U8*)(M->GetBufferPoint()),mLen,(U8*)(C->GetBufferPoint()));
		}
		else
		{
			U32 inByteLen	= (int(mLen / SM4_INPUT_ALIGN) -1) * SM4_INPUT_ALIGN;
			U32 leftByteLen	= mLen - inByteLen;

			rtn = ::XTS_SM4_crypto_block(&mXTSSM4CTX,(U8*)(M->GetBufferPoint()),inByteLen,(U8*)(C->GetBufferPoint()));
			if(rtn == OSR_SM4Success)
			{
				rtn = ::XTS_SM4_crypto_remainder(&mXTSSM4CTX,(U8*)(M->GetBufferPoint()+inByteLen),leftByteLen,(U8*)(C->GetBufferPoint()+inByteLen));
			}
		}
	}
	else if(SM4_M_XTS_ONCE == mSM4Mode)
	{
		rtn = ::XTS_SM4_crypto(mSM4Key,mSM4IV,OSR_SM4_ENCRYPT,(U8*)(M->GetBufferPoint()),mLen,(U8*)(C->GetBufferPoint()));
	}

	if(SM4_M_XTS_ONCE != mSM4Mode)
	{
		::OSR_SM4_Close();
	}

	return rtn == OSR_SM4Success;
}

bool OSRSM4::Decrypt(Buffers* C,uint32_t cLen,Buffers* M)
{
	U8	rtn	= OSR_SM4Success;

	if(false == CheckValidate(cLen))
	{
		return false;
	}

	if(SM4_M_XTS_ONCE != mSM4Mode && false == Initial(false))
	{
		return false;
	}

	if(SM4_M_ECB == mSM4Mode)
	{
		rtn = ::OSR_SM4_ECB((U8*)(C->GetBufferPoint()),cLen,OSR_SM4_DECRYPT,(U8*)(M->GetBufferPoint()));
	}
	else if(SM4_M_CBC == mSM4Mode)
	{
		rtn = ::OSR_SM4_CBC((U8*)(C->GetBufferPoint()),cLen,mSM4IV,OSR_SM4_DECRYPT,(U8*)(M->GetBufferPoint()));
	}
	else if(SM4_M_CFB_128B == mSM4Mode)
	{
		rtn = ::OSR_SM4_CFB_128((U8*)(C->GetBufferPoint()),cLen,mSM4IV,OSR_SM4_DECRYPT,(U8*)(M->GetBufferPoint()));
	}
	else if(SM4_M_OFB_128B == mSM4Mode)
	{
		rtn = ::OSR_SM4_OFB_128((U8*)(C->GetBufferPoint()),cLen,mSM4IV,(U8*)(M->GetBufferPoint()));
	}
	else if(SM4_M_CTR == mSM4Mode)
	{
		rtn = ::OSR_SM4_CTR((U8*)(C->GetBufferPoint()),cLen,mSM4CTR,(U8*)(M->GetBufferPoint()));
	}
	else if(SM4_M_XTS_STEP == mSM4Mode)
	{
		if(0 == cLen % SM4_INPUT_ALIGN)
		{
			rtn = ::XTS_SM4_crypto_block(&mXTSSM4CTX,(U8*)(C->GetBufferPoint()),cLen,(U8*)(M->GetBufferPoint()));
		}
		else
		{
			U32 inByteLen	= (int(cLen / SM4_INPUT_ALIGN) -1) * SM4_INPUT_ALIGN;
			U32 leftByteLen	= cLen - inByteLen;

			rtn = ::XTS_SM4_crypto_block(&mXTSSM4CTX,(U8*)(C->GetBufferPoint()),inByteLen,(U8*)(M->GetBufferPoint()));
			if(rtn == OSR_SM4Success)
			{
				rtn = ::XTS_SM4_crypto_remainder(&mXTSSM4CTX,(U8*)(C->GetBufferPoint()+inByteLen),leftByteLen,(U8*)(M->GetBufferPoint()+inByteLen));
			}
		}
	}
	else if(SM4_M_XTS_ONCE == mSM4Mode)
	{
		rtn = ::XTS_SM4_crypto(mSM4Key,mSM4IV,OSR_SM4_DECRYPT,(U8*)(C->GetBufferPoint()),cLen,(U8*)(M->GetBufferPoint()));
	}

	if(SM4_M_XTS_ONCE != mSM4Mode)
	{
		::OSR_SM4_Close();
	}

	return rtn == OSR_SM4Success;
}

void OSRSM4::Dump(void)
{
	printf("\n*** OSRSM4 Debug Dump ***\n\n");

	printf("XTS_SM4_CTX\n");
	::DumpArray(".xts_sm4_rk_buf = ",(U8*)(mXTSSM4CTX.xts_sm4_rk_buf),32*sizeof(U32));
	::DumpArray(".xts_sm4_t = ",(U8*)(mXTSSM4CTX.xts_sm4_t),4*sizeof(U32));
	printf(".xts_sm4_en_de = %s\n",(mXTSSM4CTX.xts_sm4_en_de)?"Encrype":"Decrype");
	printf(".xts_sm4_first_block = %d\n",mXTSSM4CTX.xts_sm4_first_block);

	printf("mSM4Mode = %d\n",mSM4Mode);

	printf("bKeySet = %s\n",(bKeySet)?"True":"False");
	printf("bIVSet = %s\n",(bIVSet)?"True":"False");
	printf("bCTRSet = %s\n",(bCTRSet)?"True":"False");
	::DumpArray("mSM4Key = ",mSM4Key,LEN_SM4_KEY);
	::DumpArray("mSM4IV = ",mSM4IV,LEN_SM4_IV);
	::DumpArray("mSM4CTR = ",mSM4Key,LEN_SM4_CTR);
}
