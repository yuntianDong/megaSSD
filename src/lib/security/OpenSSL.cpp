/*
 *  * OpenSSL.cpp
 *   */

#include <stdlib.h>
#include <string.h>

#include "security/OpenSSL.h"
#include "buffer/Buffers.h"
#include "Logging.h"

stEVPCipher	OpenSSLEVPCipher::gEVPCipherTbl[]	= \
{
	{"EVP_aes_128_ecb", EVP_aes_128_ecb(), 16},
	{"EVP_aes_192_ecb", EVP_aes_192_ecb(), 24},
	{"EVP_aes_256_ecb", EVP_aes_256_ecb(), 32},
	{"EVP_aes_128_cbc", EVP_aes_128_cbc(), 16},
	{"EVP_aes_192_cbc", EVP_aes_192_cbc(), 24},
	{"EVP_aes_256_cbc", EVP_aes_256_cbc(), 32},
	{"EVP_aes_128_ctr", EVP_aes_128_ctr(), 16},
	{"EVP_aes_192_ctr", EVP_aes_192_ctr(), 24},
	{"EVP_aes_256_ctr", EVP_aes_256_ctr(), 32},
	{"EVP_aes_128_ofb", EVP_aes_128_ofb(), 16},
	{"EVP_aes_192_ofb", EVP_aes_192_ofb(), 24},
	{"EVP_aes_256_ofb", EVP_aes_256_ofb(), 32},
	{"EVP_aes_128_cfb", EVP_aes_128_cfb(), 16},
	{"EVP_aes_192_cfb", EVP_aes_192_cfb(), 24},
	{"EVP_aes_256_cfb", EVP_aes_256_cfb(), 32},
	{"EVP_aes_128_xts", EVP_aes_128_xts(), 32},
	{"EVP_aes_256_xts", EVP_aes_256_xts(), 64},
	{"EVP_des_ede_ecb", EVP_des_ede_ecb(), 16},
	{"EVP_des_ede3_ecb", EVP_des_ede3_ecb(), 24},
	{"EVP_des_ede_cbc", EVP_des_ede_cbc(), 16},
	{"EVP_des_ede3_cbc", EVP_des_ede3_cbc(), 24},
	{"EVP_aes_128_ccm", EVP_aes_128_ccm(), 16},
	{"EVP_aes_192_ccm", EVP_aes_192_ccm(), 24},
	{"EVP_aes_256_ccm", EVP_aes_256_ccm(), 32},
	{"EVP_aes_128_gcm", EVP_aes_128_gcm(), 16},
	{"EVP_aes_192_gcm", EVP_aes_192_gcm(), 24},
	{"EVP_aes_256_gcm", EVP_aes_256_gcm(), 32}
};

#define	DEF_ALG_NAME	"EVP_aes_256_xts"

uint16_t GetMinVal(uint16_t val1,uint16_t val2)
{
	return (val1 < val2)?val1:val2;
}

OpenSSLEVPCipher::OpenSSLEVPCipher(void)
{
	memset(&mEVPCipher,0,sizeof(stEVPCipher));
	memset(mSSLKEYs,0,sizeof(uint8_t)*LEN_SSL_KEY_MAX);
	memset(mSSLIVs,0,sizeof(uint8_t)*LEN_SSL_IV_MAX);

	ChgAlgorithm(DEF_ALG_NAME);
}

OpenSSLEVPCipher::~OpenSSLEVPCipher(void)
{
}

bool OpenSSLEVPCipher::ChgAlgorithm(const char* algName)
{
	int algCount	= sizeof(gEVPCipherTbl)/sizeof(stEVPCipher);
	for(int idx=0;idx<algCount;idx++)
	{
		stEVPCipher*	pCipher	= &(gEVPCipherTbl[idx]);
		if( 0 == strncmp(algName,pCipher->algName,strlen(algName)) )
		{
			memcpy(&mEVPCipher,pCipher,sizeof(stEVPCipher));
			return true;
		}
	}

	return false;
}

void OpenSSLEVPCipher::SetKey(Buffers* key,uint16_t offset,uint16_t length)
{
	for(uint16_t idx=0;idx < GetMinVal(length,LEN_SSL_KEY_MAX);idx++)
	{
		mSSLKEYs[idx]	= key->GetByte(offset+idx);
	}
}

void OpenSSLEVPCipher::GetKey(Buffers* key,uint16_t offset,uint16_t length)
{
	for(uint16_t idx=0;idx < GetMinVal(length,LEN_SSL_KEY_MAX);idx++)
	{
		key->SetByte(offset+idx,mSSLKEYs[idx]);
	}
}

void OpenSSLEVPCipher::SetIV(Buffers* iv,uint16_t offset,uint16_t length)
{
	for(uint16_t idx=0;idx < GetMinVal(length,LEN_SSL_IV_MAX);idx++)
	{
		mSSLIVs[idx]	= iv->GetByte(offset+idx);
	}
}

void OpenSSLEVPCipher::GetIV(Buffers* iv,uint16_t offset,uint16_t length)
{
	for(uint16_t idx=0;idx < GetMinVal(length,LEN_SSL_IV_MAX);idx++)
	{
		iv->SetByte(offset+idx,mSSLIVs[idx]);
	}
}

bool OpenSSLEVPCipher::DoCrypt(uint8_t *inBuf, uint32_t inl, uint8_t *ouBuf, uint32_t *outl, int enc)
{
    if(NULL == inBuf || NULL == ouBuf)
    {
    	LOGERROR("Invalid Parameter!");
    	return false;
    }

	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_CipherInit_ex(ctx, mEVPCipher.evp_cipher, NULL, mSSLKEYs, mSSLIVs, enc);

	*outl	= 0;
	if (!EVP_CipherUpdate(ctx, ouBuf, (int*)outl, inBuf, inl))
	{
		LOGERROR("Fail to call EVP_CipherUpdate()...");
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	int templ = 0;
	if (!EVP_CipherFinal_ex(ctx, ouBuf + *outl, &templ))
	{
		LOGERROR("Fail to call EVP_CipherFinal_ex()...");
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	*outl	+= templ;
	EVP_CIPHER_CTX_free(ctx);

	return true;
}

uint32_t OpenSSLEVPCipher::Encrypt(Buffers* M,uint32_t mLen,Buffers* C)
{
	uint32_t outLen = 0;
	if( false == DoCrypt(C->GetBufferPoint(),mLen,M->GetBufferPoint(),&outLen,1) )
	{
		LOGERROR("Fail to call DoCrypt()");
		return 0;
	}

	return outLen;
}

uint32_t OpenSSLEVPCipher::Decrypt(Buffers* C,uint32_t cLen,Buffers* M)
{
	uint32_t outLen = 0;
	if( false == DoCrypt(M->GetBufferPoint(),cLen,C->GetBufferPoint(),&outLen,0) )
	{
		LOGERROR("Fail to call DoCrypt()");
		return 0;
	}

	return outLen;
}

