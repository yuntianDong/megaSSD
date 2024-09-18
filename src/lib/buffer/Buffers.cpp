/*
 *  * file : Buffers.cpp
 *   */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer/Buffers.h"
#include "utility/CRC.h"

#define OFFSET_PARAM_CHECK(o,n,s,m)	if( ((o)+(n)) > (s) ){LOGERROR(m);return false;}

Buffers::Buffers(uint64_t size,uint16_t align,const char* mode)
	:mBuf(NULL),mBufSize(size),mBufAligned(align),mEndianMode(BUFV_BIG_ENDIAN),mbNeedRelease(true),mdriver(mode)
{
	this->AllocatBuf(size,align);
	this->InitialBuf(0);
}

Buffers::Buffers(uint8_t* buffer,uint64_t size,uint16_t align)
	:mBuf(buffer),mBufSize(size),mBufAligned(align),mEndianMode(BUFV_BIG_ENDIAN),mbNeedRelease(false)
{

}

Buffers::~Buffers(void)
{
	this->ReleaseBuf();
}

void Buffers::AllocatBuf(uint64_t size,uint16_t align)
{
	int err;

	if( 0 != (align&(align-1)) || 0 != (align % sizeof(void*)) )
	{
		LOGERROR("Invalid align Parameter, %d",align);
		this->mBuf	= nullptr;
		return;
	}

	if( 0 != (size % align))
	{
		size = ((uint64_t)(size / align) + 1) * align;
	}

	if(strcmp(this->mdriver,NVME_IN_USE) == 0)
	{
		err = posix_memalign((void **)&(this->mBuf),align,size);
		if(err)
		{
			LOGERROR("Fail to allocate memory");
			this->mBuf	= nullptr;
			return;
		}
	}
	else if(strcmp(this->mdriver,UIO_IN_USE) == 0)
	{
		this->mBuf  = (uint8_t*)spdk_zmalloc((size_t)size,(size_t)align,NULL,SPDK_ENV_LCORE_ID_ANY,SPDK_MALLOC_DMA);
		if(this->mBuf == nullptr)
		{
			LOGDEBUG("Fail to allocate memory");
		}
	}
	else
	{
		this->mBuf = nullptr;
	}
	this->mBufAligned	= align;
	this->mBufSize		= size;
}

uint64_t Buffers::ReadnBytes(uint64_t offset,uint8_t nBytes)
{
	uint64_t	val	= 0;

	for(uint8_t idx=0;idx<nBytes;idx++)
	{
		if(BUFV_BIG_ENDIAN == this->mEndianMode)
		{
			val |= (uint64_t)( *(this->mBuf + offset + nBytes-idx-1) ) << (8*idx);
		}
		else
		{
			val |= (uint64_t)( *(this->mBuf + offset + idx) ) << (8*idx);
		}
	}

	return val;
}

void Buffers::WritenBytes(uint64_t offset,uint8_t nBytes,uint64_t val)
{
	for(uint8_t idx=0;idx<nBytes;idx++)
	{
		uint8_t bitShift	= 0;

		if(BUFV_BIG_ENDIAN == this->mEndianMode)
		{
			bitShift	= ( nBytes - idx - 1 )*8;
		}
		else
		{
			bitShift	= idx*8;
		}

		*(this->mBuf + offset + idx) = (uint8_t)( ( val & ((uint64_t)0xFF << bitShift) ) >> bitShift );
	}
}

void Buffers::InitialBuf(uint8_t val)
{
	if(NULL != this->mBuf && 0 != this->mBufSize)
	{
		memset(this->mBuf,val,this->mBufSize);
	}
}

void Buffers::ReleaseBuf(void)
{
	
	if(true == mbNeedRelease && NULL != this->mBuf)
	{
		if(strcmp(this->mdriver,NVME_IN_USE) == 0)
		{
			free(this->mBuf);
			this->mBuf = NULL;
		}else{
			spdk_free((void*)this->mBuf);
			this->mBuf = NULL;
		}
		
	}
}

bool Buffers::GetBufVal(uint64_t offset,uint8_t& val)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_BYTE,GetBufSize(),"Invalid Offset Parameter!")
	val = *(this->mBuf + offset);
	return true;
}

bool Buffers::GetBufVal(uint64_t offset,uint16_t& val, enEndianMode eMode)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_WORD,GetBufSize(),"Invalid Offset Parameter!")

	if(BUFV_ENDIAN_UNK == eMode) eMode = this->mEndianMode;

	enEndianMode orgMode = this->mEndianMode;
	this->mEndianMode	= eMode;

	val = (uint16_t)ReadnBytes(offset,NBYTE_PER_WORD);

	this->mEndianMode	= orgMode;
	return true;
}

bool Buffers::GetBufVal(uint64_t offset,uint32_t& val, enEndianMode eMode)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_DWORD,GetBufSize(),"Invalid Offset Parameter!")

	if(BUFV_ENDIAN_UNK == eMode) eMode = this->mEndianMode;

	enEndianMode orgMode = this->mEndianMode;
	this->mEndianMode	= eMode;

	val = (uint32_t)ReadnBytes(offset,NBYTE_PER_DWORD);

	this->mEndianMode	= orgMode;
	return true;
}

bool Buffers::GetBufVal(uint64_t offset,uint64_t& val, enEndianMode eMode)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_QWORD,GetBufSize(),"Invalid Offset Parameter!")

	if(BUFV_ENDIAN_UNK == eMode) eMode = this->mEndianMode;

	enEndianMode orgMode = this->mEndianMode;
	this->mEndianMode	= eMode;

	val = (uint64_t)ReadnBytes(offset,NBYTE_PER_QWORD);

	this->mEndianMode	= orgMode;
	return true;
}

bool Buffers::SetBufVal(uint64_t offset,uint8_t val)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_BYTE,GetBufSize(),"Invalid Offset Parameter!")

	*(this->mBuf + offset) = val;
	return true;
}

bool Buffers::SetBufVal(uint64_t offset,uint16_t val,enEndianMode eMode)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_WORD,GetBufSize(),"Invalid Offset Parameter!")

	if(BUFV_ENDIAN_UNK == eMode) eMode = this->mEndianMode;

	enEndianMode orgMode = this->mEndianMode;
	this->mEndianMode	= eMode;

	WritenBytes(offset,NBYTE_PER_WORD,(uint64_t)val);

	this->mEndianMode	= orgMode;
	return true;
}

bool Buffers::SetBufVal(uint64_t offset,uint32_t val,enEndianMode eMode)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_DWORD,GetBufSize(),"Invalid Offset Parameter!")

	if(BUFV_ENDIAN_UNK == eMode) eMode = this->mEndianMode;

	enEndianMode orgMode = this->mEndianMode;
	this->mEndianMode	= eMode;

	WritenBytes(offset,NBYTE_PER_DWORD,(uint64_t)val);

	this->mEndianMode	= orgMode;
	return true;
}

bool Buffers::SetBufVal(uint64_t offset,uint64_t val,enEndianMode eMode)
{
	OFFSET_PARAM_CHECK(offset,NBYTE_PER_QWORD,GetBufSize(),"Invalid Offset Parameter!")

	if(BUFV_ENDIAN_UNK == eMode) eMode = this->mEndianMode;

	enEndianMode orgMode = this->mEndianMode;
	this->mEndianMode	= eMode;

	WritenBytes(offset,NBYTE_PER_QWORD,(uint64_t)val);

	this->mEndianMode	= orgMode;
	return true;
}

bool Buffers::FillBufVal(uint64_t offset,uint8_t pattern,uint32_t length)
{
	OFFSET_PARAM_CHECK(offset,length,GetBufSize(),"Invalid Offset+Length Parameter!")

	memset(this->mBuf + offset,pattern,length);
	return true;
}

bool Buffers::FillBufVal(uint64_t offset,uint16_t pattern,uint32_t length)
{
	OFFSET_PARAM_CHECK(offset,length,GetBufSize(),"Invalid Offset+Length Parameter!")

	for(uint32_t idx=0;idx<length;idx+=NBYTE_PER_WORD)
	{
		SetBufVal(offset+idx,pattern,GetEndianMode());
	}
	return true;
}

bool Buffers::FillBufVal(uint64_t offset,uint32_t pattern,uint32_t length)
{
	OFFSET_PARAM_CHECK(offset,length,GetBufSize(),"Invalid Offset+Length Parameter!")

	for(uint32_t idx=0;idx<length;idx+=NBYTE_PER_DWORD)
	{
		SetBufVal(offset+idx,pattern,GetEndianMode());
	}
	return true;
}

bool Buffers::FillBufVal(uint64_t offset,uint64_t pattern,uint32_t length)
{
	OFFSET_PARAM_CHECK(offset,length,GetBufSize(),"Invalid Offset+Length Parameter!")

	for(uint32_t idx=0;idx<length;idx+=NBYTE_PER_QWORD)
	{
		SetBufVal(offset+idx,pattern,GetEndianMode());
	}
	return true;
}

uint8_t Buffers::GetByte(uint64_t offset)
{
	uint8_t val = 0;
	GetBufVal(offset,val);

	return val;
}

uint16_t Buffers::GetWord(uint64_t offset)
{
	uint16_t val = 0;
	GetBufVal(offset,val);

	return val;
}

uint32_t Buffers::GetDWord(uint64_t offset)
{
	uint32_t val = 0;
	GetBufVal(offset,val);

	return val;
}

uint64_t Buffers::GetQWord(uint64_t offset)
{
	uint64_t val = 0;
	GetBufVal(offset,val);

	return val;
}

void Buffers::MarkAddrOverlay(uint64_t sOffset,uint16_t numSector,uint64_t logicAddr)
{
	UNUSED_PARAM(sOffset);
	UNUSED_PARAM(numSector);
	UNUSED_PARAM(logicAddr);

	LOGERROR("MarkAddrOverlay() is not implemented!");
	return;
}

void Buffers::FillSector(uint64_t sOffset,uint16_t numSector,uint32_t pattern)
{
	UNUSED_PARAM(sOffset);
	UNUSED_PARAM(numSector);
	UNUSED_PARAM(pattern);

	LOGERROR("FillSector() is not implemented!");
	return;
}

void Buffers::DumpBuffer(uint64_t offset,uint32_t length,PFPRINTFN fn)
{
	if(NULL == fn) fn=(PFPRINTFN)printf;

	fn("Buffer: Offset=%d,Length=%d\n",offset,length);

	for(int idx=0;idx<(int)length;)
	{
		uint64_t addr	= offset + idx;

		if((idx % 4096) == 0)
			fn("Block %d , Offset = 0x%x\n",int(idx/4096),addr);

		if((idx % 256) == 0)
		{
			fn("\n    \t ");
			for(int idx=0;idx<16;idx++)
			{
				fn("  %2d ",idx);
			}
		}

		if((idx % 16) == 0)
			fn("\n[0x%x]\t=",addr);

		fn(" 0x%02x",GetByte(addr));idx++;
	}

	fn("\n");
}

bool Buffers::ReadRegVal(uint64_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t& val)
{
	uint8_t bytesNum	= (startBits+bitsLen+(NBITS_PER_BYTE-1))/NBITS_PER_BYTE;
	if (bytesNum > NBYTE_PER_QWORD)
	{
		LOGERROR("Can only max support 8 bytes data, the input bits length:%d > 64",bytesNum);
		val		= (uint64_t)-1;
		return false;
	}

	uint64_t tmpVal	= ReadnBytes(startByte,bytesNum);
	if(0 != startBits || 0 != (bytesNum % NBYTE_PER_QWORD))
	{
		uint64_t bitsMask	= ( ((uint64_t)1 << bitsLen)-1 ) << startBits;
		tmpVal = (tmpVal & bitsMask) >> startBits;
	}

	val = tmpVal;
	return true;
}

bool Buffers::WriteRegVal(uint64_t startByte, uint8_t startBits, uint8_t bitsLen, uint64_t value)
{
	uint8_t bytesNum =  (startBits+bitsLen+(NBITS_PER_BYTE-1))/NBITS_PER_BYTE;

	if (bytesNum > NBYTE_PER_QWORD)
	{
		LOGERROR("Can only max support 8 bytes data, the input bits length:%d > 64",bytesNum);
		return false;
	}

	uint64_t readValue	= ReadnBytes(startByte,bytesNum);
	uint64_t bitMask	= (((uint64_t)1<<(bytesNum*8)) - 1) - ((((uint64_t)1<<bitsLen)-1) << startBits);
	uint64_t wrValue	= (readValue & bitMask) | (value << startBits);

	WritenBytes(startByte,bytesNum,wrValue);
	return true;
}

bool Buffers::CopyFromBuff(Buffers& fromBuff, uint64_t startByte, uint64_t fromByte, uint32_t length)
{
	OFFSET_PARAM_CHECK(fromByte,length,fromBuff.GetBufSize(),"Buffer Overflow Error!")
	OFFSET_PARAM_CHECK(startByte,length,GetBufSize(),"Buffer Overflow Error!")

	for(uint32_t idx=0; idx<length; idx++)
	{
		SetByte(startByte+idx, fromBuff.GetByte(fromByte+idx));
	}

	return true;
}

bool Buffers::CopyToBuff(Buffers& toBuff, uint64_t startByte, uint64_t toByte, uint32_t length)
{
	OFFSET_PARAM_CHECK(toByte,length,toBuff.GetBufSize(),"Buffer Overflow Error!")
	OFFSET_PARAM_CHECK(startByte,length,GetBufSize(),"Buffer Overflow Error!")

	for(uint32_t idx=0; idx<length; idx++)
	{
		toBuff.SetByte(toByte+idx, GetByte(startByte+idx));
	}

	return true;
}

uint32_t _GetDWValByOffset(Buffers& buf,uint64_t offset)
{
	return buf.GetDWord((offset >> 2) << 2);
}

bool Buffers::Compare(Buffers& dstBuff, uint64_t dstOffset, uint64_t offset, uint32_t length,\
		uint32_t &rtnErrPos, uint32_t &rtnSrcVal, uint32_t &rtnDesVal)
{
	uint8_t srcVal;
	uint8_t dstVal;

	rtnErrPos	= (uint32_t)-1;
	rtnSrcVal	= rtnDesVal	= (uint32_t)-1;

	OFFSET_PARAM_CHECK(dstOffset,length,dstBuff.GetBufSize(),"Buffer Overflow Error!")
	OFFSET_PARAM_CHECK(offset,length,GetBufSize(),"Buffer Overflow Error!")

	for(uint32_t idx=0;idx<length;idx++)
	{
		srcVal = GetByte(idx+offset);
		dstVal = dstBuff.GetByte(idx+dstOffset);

		if(srcVal != dstVal)
		{
			rtnErrPos = idx;
			rtnSrcVal = _GetDWValByOffset(*this,idx+offset);
			rtnDesVal = _GetDWValByOffset(dstBuff,idx+dstOffset);

			LOGERROR("Compare fail! error position:%u ,recive val: %u,expect val: %u",rtnErrPos,rtnDesVal,rtnSrcVal);
			LOGINFO("Dump pattern buffer");
			dstBuff.Dump(rtnErrPos,16);
			LOGINFO("Dump read buffer");
			this->Dump(rtnErrPos,16);
			return true;
		}
	}

	return true;
}

bool Buffers::ToFile(const char* fileName, uint64_t offset, uint32_t length)
{
	OFFSET_PARAM_CHECK(offset,length,GetBufSize(),"Buffer Overflow Error!")

	FILE *pfile = NULL;
	uint8_t *pBuff = GetBufferPoint();
	pfile = fopen(fileName, "wb+");
	fwrite(&pBuff[offset], sizeof(uint8_t), length, pfile);
	fflush(pfile);
	fclose(pfile);

	return true;
}

bool Buffers::FromFile(const char* fileName, uint64_t offset, uint32_t length)
{
	OFFSET_PARAM_CHECK(offset,length,GetBufSize(),"Buffer Overflow Error!")

	FILE *pfile = NULL;
	uint8_t *pBuff = GetBufferPoint();

	if ((pfile=fopen(fileName, "rb")) !=NULL)
	{
		size_t bytesRead = fread(&pBuff[offset], sizeof(uint8_t), length, pfile);
		if (bytesRead != length)
		{
			LOGERROR("Read file %s length error expt read %d bytes actual read %ld bytes",fileName,length,bytesRead);
			return false;
		}
		fclose(pfile);
	}
	else
	{
		LOGDEBUG("%s not exist, please check", fileName);
		return false;
	}

	return true;
}

uint8_t  Buffers::GenCRC8(uint64_t offset,uint32_t length)
{
	if(this->GetBufSize() < (offset+length))
	{
		LOGERROR("Invalid Parameter");
		return (uint8_t)-1;
	}

	return CalCRC8(GetBufferPoint()+offset,length);
}

uint16_t Buffers::GenCRC16(uint64_t offset,uint32_t length)
{
	if(this->GetBufSize() < (offset+length))
	{
		LOGERROR("Invalid Parameter");
		return (uint16_t)-1;
	}

	return CalCRC16(GetBufferPoint()+offset,length);
}

uint32_t Buffers::GenCRC32(uint64_t offset,uint32_t length)
{
	if(this->GetBufSize() < (offset+length))
	{
		LOGERROR("Invalid Parameter");
		return (uint32_t)-1;
	}

	return CalCRC32(GetBufferPoint()+offset,length);
}

