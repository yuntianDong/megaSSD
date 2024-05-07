#ifndef _X_OPENSSL_H
#define _X_OPENSSL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/evp.h>
#include "buffer/Buffers.h"

#define	EVT_ALG_NAME_MAXLEN		32
#define LEN_SSL_KEY_MAX			256
#define LEN_SSL_IV_MAX			16

typedef struct _stEVPCipher
{
	char			  algName[EVT_ALG_NAME_MAXLEN];
	const EVP_CIPHER *evp_cipher;
	int 			  key_size;
}stEVPCipher;

class OpenSSLEVPCipher
{
private:
	stEVPCipher	mEVPCipher;
	uint8_t		mSSLKEYs[LEN_SSL_KEY_MAX];
	uint8_t		mSSLIVs[LEN_SSL_IV_MAX];
protected:
	static stEVPCipher	gEVPCipherTbl[];

	bool DoCrypt(uint8_t* inBuf, uint32_t inl, uint8_t* ouBuf, uint32_t *outl, int enc);
public:
	OpenSSLEVPCipher(void);
	virtual ~OpenSSLEVPCipher(void);

	bool ChgAlgorithm(const char* algName);

	void SetKey(Buffers* key,uint16_t offset,uint16_t length);
	void GetKey(Buffers* key,uint16_t offset,uint16_t length);
	void SetIV(Buffers* iv,uint16_t offset,uint16_t length);
	void GetIV(Buffers* iv,uint16_t offset,uint16_t length);

	uint32_t Encrypt(Buffers* M,uint32_t mLen,Buffers* C);
	uint32_t Decrypt(Buffers* C,uint32_t cLen,Buffers* M);
};

#endif
