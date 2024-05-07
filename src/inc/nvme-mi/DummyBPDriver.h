/*
 * DummyBPDriver.h
 *
 *  Created on: 2020Äê12ÔÂ23ÈÕ
 *      Author: Lenovo
 */

#ifndef _NVME_MI_DUMMYBPDRIVER_H_
#define _NVME_MI_DUMMYBPDRIVER_H_

#include "NVMeMIBpDriver.h"
#include "Logging.h"

#define DUMMY_BPDRV_MAX_SIZE			5120

class DummyBPDriver : public NVMeMIBpDriver
{
private:
	uint8_t*	mTempBuf;
	uint16_t	mBufLen;
	uint16_t	mBufWrIdx;
	uint16_t	mBufRdIdx;

	void	FillFakeIdfyReturnDatum(uint8_t* buf,uint16_t maxBytes)
	{
		uint8_t	header[20]	= {0x84,0x91,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
		for(int idx=0;idx<20;idx++)
		{
			buf[idx]	= header[idx];
		}

		for(int idx=20;idx<maxBytes;idx++)
		{
			buf[idx]	= (uint8_t)idx;
		}
	}

	void	FillMCTPPackageDatum(uint16_t offset,uint8_t* datum,uint16_t maxBytes)
	{
		uint8_t header[8]	= {0x3a,0x0f,0x00,0x2a,0x01,0x0,0x0,0xc0};
		uint8_t* wrPointer	= mTempBuf + offset;

		for(int idx=0;idx<8;idx++)
		{
			wrPointer[idx]	= header[idx];
		}

		for(int idx=0;idx<maxBytes;idx++)
		{
			wrPointer[8+idx]= datum[idx];
		}

		wrPointer[2]	= maxBytes + 5;
	}

	void	HandleNVMeMIAdmIdfy(void)
	{
		LOGDEBUG("\t HandleNVMeMIAdmIdfy()");

		memset(mTempBuf,0,DUMMY_BPDRV_MAX_SIZE);

		uint8_t	datum[4120];	// 20 + 4096 + 4 = 4120
		memset(datum,0,4120);

		FillFakeIdfyReturnDatum(datum,4120);

		uint16_t	units	= 64;
		uint16_t	pieces	= (4120 + units - 1)/units;

		for(uint16_t idx=0;idx<pieces;idx++)
		{
			uint8_t dLen	= (idx < (pieces - 1))?units:(4120 - idx*units);

			FillMCTPPackageDatum(idx*73,datum+idx*units,dLen);
		}

		mBufWrIdx	= 4120 + (8+1)*pieces;
		mBufRdIdx	= 0;
	};
public:
	DummyBPDriver()
		:mTempBuf(NULL),mBufLen(0),mBufWrIdx(0),mBufRdIdx(0)
	{
		mTempBuf	= (uint8_t*)malloc(DUMMY_BPDRV_MAX_SIZE);
		memset(mTempBuf,0,DUMMY_BPDRV_MAX_SIZE);
		mBufLen		= DUMMY_BPDRV_MAX_SIZE;
	};

	virtual ~DummyBPDriver()
	{
		if(NULL != mTempBuf)
		{
			free(mTempBuf);
			mTempBuf = NULL;
		}
		mBufLen = 0;
	}

	void Debug(void)
	{
		printf("\n###############################\n");
		printf("\tmBufLen: %d\n",mBufLen);
		printf("\tBufWrIdx: %d\n",mBufWrIdx);
		printf("\tBufRdIdx: %d\n",mBufRdIdx);
		for(uint16_t idx=0;idx<128;idx++)
		{
			if(idx % 16 == 0)
			{
				printf("\n");
			}
			printf("%02x ",mTempBuf[idx]);
		}
		printf("\n###############################\n");
	}

	virtual uint16_t	GetUnitLength(void)	{return DUMMY_BPDRV_MAX_SIZE;};

	virtual bool ReadyForWrite(uint16_t xferBytes)
	{
		LOGDEBUG("DummyBPDriver::ReadyForWrite,xferBytes=%d\n",xferBytes);

		if(0 == xferBytes || (mBufWrIdx + xferBytes) > mBufLen)
		{
			return false;
		}

		mBufRdIdx	= 0;
		return true;
	};
	virtual bool Write(uint8_t* data,uint16_t len,uint16_t offset)
	{
		LOGDEBUG("DummyBPDriver::Write,len=%d,offset=%d\n",len,offset);

		if(0 == mBufLen || NULL == mTempBuf)
		{
			return false;
		}

		if((offset + len) > mBufLen)
		{
			return false;
		}

		memcpy(mTempBuf+mBufWrIdx,data,len);
		mBufWrIdx	+= len;

		return true;
	};
	virtual bool ReadyForRead(uint16_t& recvBytes)
	{
		LOGDEBUG("DummyBPDriver::ReadyForRead\n");

		if(0 == mBufLen || NULL == mTempBuf)
		{
			return false;
		}

		if(0x6 == mTempBuf[12])			// Identify
		{
			HandleNVMeMIAdmIdfy();
		}

		recvBytes	= mBufWrIdx;
		mBufWrIdx	= 0;

		return true;
	};
	virtual bool Read(uint8_t* data,uint16_t len,uint16_t offset)
	{
		LOGDEBUG("DummyBPDriver::Read,len=%d,offset=%d\n",len,offset);

		if(0 == mBufLen || NULL == mTempBuf )
		{
			return false;
		}

		if( (offset + len) > mBufLen )
		{
			return false;
		}

		memcpy(data,mTempBuf+mBufRdIdx,len);
		mBufRdIdx	+= len;

		return true;
	};
};

#endif /* _NVME_MI_DUMMYBPDRIVER_H_ */
