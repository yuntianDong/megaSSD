/*
 * RegisterMap.h
 *
 */

#ifndef _PROTOCAL_REGISTERMAP_H_
#define _PROTOCAL_REGISTERMAP_H_

#include <assert.h>

#include "protocol/RegisterTbl.h"
#include "buffer/Buffers.h"
#include "Logging.h"

#define	REG_MAP_BUF_ALIGN_SIZE		256
#define BITS_OF_BYTE				8

class RegisterMap
{
private:
	RegisterTbl*	mRegTbl;
	uint32_t		mRegMapWrPoint;
	uint32_t		mRegMapRdPoint;
	uint32_t		mRegMapBufSize;
	Buffers*		mRegMapBuf;

	bool			mbNeedFree;
protected:
	virtual void InitRegTbl(void){};
	virtual void InitRegVal(void){};

	bool SetRegVal(uint32_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t value)
	{
		bool rtn = mRegMapBuf->WriteRegVal(startByte,startBits,bitsLen,value);
		if(true == rtn)
		{
			mRegMapWrPoint	= startByte * BITS_OF_BYTE + startBits + bitsLen;
		}

		return rtn;
	};
	bool GetRegVal(uint32_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t& value)
	{
		bool rtn = mRegMapBuf->ReadRegVal(startByte,startBits,bitsLen,value);
		if(true == rtn)
		{
			mRegMapRdPoint	= startByte * BITS_OF_BYTE + startBits + bitsLen;
		}

		return rtn;
	};
	bool SetRegVal(const char* reg,uint64_t val,uint16_t offset=0,uint8_t length=0)
	{
		assert(NULL != mRegTbl);

		enOffType offType	= mRegTbl->GetRegOffType(reg);
		uint16_t byOffset	= mRegTbl->GetByteOffset(reg);
		uint16_t btOffset	= mRegTbl->GetBitOffset(reg);
		uint16_t nBitLen	= mRegTbl->GetLength(reg);

		if( (offset * BITS_OF_BYTE) >= nBitLen )
		{
			return false;
		}

		if(0 != length)
		{
			if(0 != btOffset)
			{
				return false;
			}

			nBitLen		= (nBitLen > length)?length:nBitLen;
		}

		if(REG_OFF_APP == offType)
		{
			if(0 != (mRegMapWrPoint % BITS_OF_BYTE) ||
					(mRegMapWrPoint + nBitLen) > mRegMapBufSize*BITS_OF_BYTE)
			{
				return false;
			}

			byOffset	= (uint16_t)( mRegMapWrPoint/ BITS_OF_BYTE) + byOffset;
		}
		else if(REG_OFF_B2F == offType)
		{
			byOffset	= mRegMapBufSize - byOffset - (uint16_t)(nBitLen + BITS_OF_BYTE -1) / BITS_OF_BYTE;
		}

		//LOGDEBUG("%s byteOff=%d,bitOff=%d,nBits=%d,val=0x%x",reg,(byOffset+offset),btOffset,nBitLen,val);

		return this->SetRegVal(byOffset+offset,btOffset,nBitLen,val);
	};
	bool GetRegVal(const char* reg,uint64_t& val,uint16_t offset=0,uint8_t length=0)
	{
		assert(NULL != mRegTbl);

		enOffType offType	= mRegTbl->GetRegOffType(reg);
		uint16_t byOffset	= mRegTbl->GetByteOffset(reg);
		uint16_t btOffset	= mRegTbl->GetBitOffset(reg);
		uint16_t nBitLen	= mRegTbl->GetLength(reg);

		if( (offset * BITS_OF_BYTE) >= nBitLen )
		{
			return false;
		}

		if(0 != length)
		{
			nBitLen		= (nBitLen > length)?length:nBitLen;
			btOffset	= 0;
		}

		if(REG_OFF_APP == offType)
		{
			if(0 != (mRegMapRdPoint % BITS_OF_BYTE) ||
					(mRegMapRdPoint + nBitLen) > mRegMapBufSize*BITS_OF_BYTE)
			{
				return false;
			}

			byOffset	= (uint16_t)( mRegMapRdPoint/ BITS_OF_BYTE) + byOffset;
		}
		else if(REG_OFF_B2F == offType)
		{
			byOffset	= mRegMapBufSize - byOffset - (uint16_t)(nBitLen + BITS_OF_BYTE -1) / BITS_OF_BYTE;
		}

		return this->GetRegVal(byOffset+offset,btOffset,nBitLen,val);
	};

	bool SetRegByBuf(const char* reg,Buffers* buf,uint16_t offset,uint16_t length)
	{
		if((offset + length) > buf->GetBufSize())
		{
			return false;
		}

		for(uint16_t idx=0;idx<length;idx++)
		{
			if(false == SetRegVal(reg,buf->GetByte(idx+offset),idx,BITS_OF_BYTE))
			{
				return false;
			}
		}

		return true;
	};
	bool GetRegByBuf(const char* reg,Buffers* buf,uint16_t offset,uint16_t length)
	{
		if((offset + length) > buf->GetBufSize())
		{
			return false;
		}

		for(uint16_t idx=0;idx<length;idx++)
		{
			uint64_t val = (uint64_t)-1;
			if(false == GetRegVal(reg,val,idx,BITS_OF_BYTE))
			{
				return false;
			}
			buf->SetByte(idx+offset,(uint8_t)val);
		}

		return true;
	};

	uint64_t GetRegVal(const char* reg,uint16_t offset=0)
	{
		uint64_t val = (uint64_t)-1;
		if( false == GetRegVal(reg,val,offset))
		{
			return (uint64_t)-1;
		}

		return val;
	}
public:
	RegisterMap(uint32_t regMapMaxSize,bool bLittleEndian)
		:mRegTbl(NULL),mRegMapWrPoint(0),mRegMapRdPoint(0),mRegMapBufSize(regMapMaxSize),mRegMapBuf(NULL),mbNeedFree(true)
	{
		LOGDEBUG("RegisterMap::RegisterMap(%d,%s)",regMapMaxSize,(bLittleEndian)?"True":"False");
		mRegTbl	= (RegisterTbl*)new RegisterTbl();
		mRegMapBuf	= (Buffers*)new Buffers(regMapMaxSize,REG_MAP_BUF_ALIGN_SIZE);
		mRegMapBuf->SetEndianMode((true == bLittleEndian)?BUFV_LITTLE_ENDIAN:BUFV_BIG_ENDIAN);

		InitRegTbl();
		InitRegVal();
	};
	RegisterMap(Buffers* regMapBuf)
		:mRegTbl(NULL),mRegMapWrPoint(0),mRegMapRdPoint(0),mRegMapBufSize(0),mRegMapBuf(regMapBuf),mbNeedFree(false)
	{
		LOGDEBUG("RegisterMap::RegisterMap(Buffers* regMapBuf)");
		mRegTbl	= (RegisterTbl*)new RegisterTbl();
		mRegMapBufSize = mRegMapWrPoint = mRegMapBuf->GetBufSize();

		InitRegTbl();
		InitRegVal();
	}
	virtual ~RegisterMap()
	{
		LOGDEBUG("RegisterMap::~RegisterMap()");
		if(NULL != mRegMapBuf && true == mbNeedFree)
		{
			delete mRegMapBuf;
			mRegMapBuf  = NULL;
		}

		if(NULL != mRegTbl)
		{
			delete mRegTbl;
			mRegTbl		= NULL;
		}
	};

	virtual void Debug(void)
	{
		printf("RegisterMap :\n");

		mRegTbl->Debug();
		printf("\tmRegMapWrPoint = %d\n",mRegMapWrPoint);
		printf("\tmRegMapRdPoint = %d\n",mRegMapRdPoint);
		printf("\tmRegMapBufSize = %d\n",mRegMapBufSize);
		printf("\tmRegMapBuf[0:%d] = \n",mRegMapBufSize);
		mRegMapBuf->Dump(0,mRegMapBufSize);
	}

	virtual void Dump(void)
	{
		printf("RegisterMap :\n");

		Buffers* buf = GetBuffer();

		buf->Dump(0,buf->GetBufSize());
	}

	Buffers* GetBuffer(void) {return mRegMapBuf;};
	RegisterTbl* GetRegTbl(void) {return mRegTbl;};

	bool SetWrPoint(uint32_t offset)
	{
		if( offset >= BITS_OF_BYTE * mRegMapBufSize )
		{
			return false;
		}

		mRegMapWrPoint = offset;
		return true;
	}
	bool SetRdPoint(uint32_t offset)
	{
		if( offset >= BITS_OF_BYTE * mRegMapBufSize )
		{
			return false;
		}

		mRegMapRdPoint = offset;
		return true;
	}
};

#endif /* _PROTOCAL_REGISTERMAP_H_ */
