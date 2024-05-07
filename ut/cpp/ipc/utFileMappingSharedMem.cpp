/*
 * file: utFileMappingSharedMem.cpp
 */

#include<iostream>
using namespace std;
#include<gtest/gtest.h>

#include "ipc/FileMappingSharedMem.h"
#include "Logging.h"

#define	MAX_BUFSIZE			1*1024*1024
#define BUF_ALIGNMENT		128

class utFileMappingSharedMem : public testing::Test
{
public:
	FileMapSharedMem*		mpServer;
	FileMapSharedMem*		mpClient;
	Buffers*				mpRdBuf;
	Buffers*				mpWrBuf;

	virtual void SetUp()
	{
		mpServer	= new FileMapSharedMem(M_READWRITE);
		mpClient	= new FileMapSharedMem(M_READONLY);
		mpRdBuf		= new Buffers(MAX_BUFSIZE,BUF_ALIGNMENT);
		mpWrBuf		= new Buffers(MAX_BUFSIZE,BUF_ALIGNMENT);
	}

	virtual void TearDown()
	{
		if(NULL != mpServer)
		{
			delete mpServer;
			mpServer = NULL;
		}
		if(NULL != mpClient)
		{
			delete mpClient;
			mpClient = NULL;
		}

		if(NULL != mpRdBuf)
		{
			delete mpRdBuf;
			mpRdBuf = NULL;
		}
		if(NULL != mpWrBuf)
		{
			delete mpWrBuf;
			mpWrBuf = NULL;
		}
	}
};

TEST_F(utFileMappingSharedMem,CheckReadWrite)
{
	mpWrBuf->FillHexF(0,4096);
	mpRdBuf->FillZero(0,4096);
	EXPECT_EQ(mpServer->Write(mpWrBuf,0,4096),4096);
	EXPECT_TRUE(mpServer->Flush());

	EXPECT_EQ(mpClient->Read(mpRdBuf,0,4096),4096);

	uint32_t errPos,srcVal,dstVal;
	errPos = srcVal = dstVal;
	mpRdBuf->Compare(*mpWrBuf,0,0,4096,errPos,srcVal,dstVal);
	EXPECT_EQ(errPos,0xFFFFFFFF);
}
