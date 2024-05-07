#ifndef _HOST_BUFFER_H
#define _HOST_BUFFER_H

#include "buffer/Buffers.h"

#define HOST_BUFFER_ALIGNMENT			DEF_BUF_ALIGNMENT
#define SWAP_BUF_PIECE_UNIT				(64*1024*1024)	// 64MB -> 256GB
#define SWAP_BUF_FNAME_PRE				"SWAPBUF"
#define SWAP_BUF_NAME_LEN				10
#define SWAP_BUF_FNAME_LEN				40

typedef struct _stBufPatRec
{
	uint8_t		offset;
	uint8_t		length;
	uint8_t		patIdx;
}stBufPatRec;

class HostBuffer : public Buffers
{
public:
	HostBuffer(uint64_t size,const char* mode) : Buffers(size,HOST_BUFFER_ALIGNMENT,mode)
	{
	}

	HostBuffer(uint8_t * buffer,uint64_t size) : Buffers(buffer,size,HOST_BUFFER_ALIGNMENT)
	{
	}

	inline uint64_t GetHostBufSize(void) {return GetBufSize();};
	inline uint64_t GetHostBufAddr(void) {return GetBufferAddr();};
	inline uint16_t GetHostBufAlign(void) {return GetBufAlignment();};

	uint32_t Compare(Buffers& desBuff, uint64_t desOffset, uint64_t offset, \
			uint32_t length,uint32_t* pSrcVal=NULL,uint32_t* pDstVal=NULL)
	{
		uint32_t rtnErrPos	= 0;
		uint32_t rtnSrcVal,rtnDstVal;
		rtnSrcVal	= rtnDstVal = (uint32_t)-1;

		Buffers::Compare(desBuff,desOffset,offset,length,(uint32_t&)rtnErrPos,(uint32_t&)rtnSrcVal,(uint32_t&)rtnDstVal);
		if(NULL != pSrcVal)
			*pSrcVal = rtnSrcVal;
		if(NULL != pDstVal)
			*pDstVal = rtnDstVal;

		return rtnErrPos;
	};
};

class CommandBuf : public HostBuffer
{
public:
	CommandBuf(uint64_t size,const char* mode)
		:HostBuffer(size,mode)
	{
	}
};

class SectorBuf : public HostBuffer
{
private:
	uint32_t		muSectorSize;
public:
	SectorBuf(uint64_t bufSize,uint32_t sectorSize,const char* mode);

	uint32_t GetSectorSize(void){return muSectorSize;};
	void FillSector(uint64_t sOffset,uint16_t numSector,uint32_t pattern);
	void MarkAddrOverlay(uint64_t sOffset,uint16_t numSector,uint64_t logicAddr);

	bool ReadRegVal(uint64_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t& value)
	{
		UNUSED_PARAM(startByte);
		UNUSED_PARAM(startBits);
		UNUSED_PARAM(bitsLen);
		UNUSED_PARAM(value);
		LOGERROR("SectorBuf::ReadRegVal() Not Supported!");
		return false;
	}
	bool WriteRegVal(uint64_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t value)
	{
		UNUSED_PARAM(startByte);
		UNUSED_PARAM(startBits);
		UNUSED_PARAM(bitsLen);
		UNUSED_PARAM(value);
		LOGERROR("SectorBuf::WriteRegVal() Not Supported!");
		return false;
	}
};

class PatternBuf : public SectorBuf
{
public:
	PatternBuf(uint64_t bufSize,uint32_t sectorSize,const char* mode)
		:SectorBuf(bufSize,sectorSize,mode)
	{
	}

	/*
	 * Those operations should be prohibited for IO Buffer
	 */
	void SetByte(uint64_t offset,uint8_t val)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(val);
		LOGERROR("SectorBuf::SetByte() Not Supported!");
		return;
	}
	void SetWord(uint64_t offset,uint16_t val)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(val);
		LOGERROR("SectorBuf::SetWord() Not Supported!");
		return;
	}
	void SetDWord(uint64_t offset,uint32_t val)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(val);
		LOGERROR("SectorBuf::SetDWord() Not Supported!");
		return;
	}
	void SetQWord(uint64_t offset,uint64_t val)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(val);
		LOGERROR("SectorBuf::SetQWord() Not Supported!");
		return;
	}

	void FillWithBytePat(uint64_t offset,uint8_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("SectorBuf::FillWithBytePat() Not Supported!");
		return;
	}
	void FillWithWordPat(uint64_t offset,uint16_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("SectorBuf::FillWithWordPat() Not Supported!");
		return;
	}
	void FillWithDWordPat(uint64_t offset,uint32_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("SectorBuf::FillWithDWordPat() Not Supported!");
		return;
	}
	void FillWithQWordPat(uint64_t offset,uint64_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("SectorBuf::FillWithQWordPat() Not Supported!");
		return;
	}
};

class SwapBuf : public Buffers
{
private:
	uint8_t		mMaxBufFileIndex;
	char		mBufName[SWAP_BUF_NAME_LEN];
	uint8_t		mBufPieceIndex;
	uint64_t	mSwapBufSize;
protected:

	bool DumpBufToFile(uint8_t idx);
	bool LoadBufFromFile(uint8_t idx);
	bool NeedSwap(uint64_t offset);
	uint8_t		CalBufPieceIndex(uint64_t offset);
	uint32_t	CalBufPieceOffset(uint64_t offset);
	void Swap(uint64_t offset);
public:
	SwapBuf(const char * name,uint64_t bufSize,const char* mode);
	virtual ~SwapBuf();

	uint64_t 	GetBufSize(void) {return mSwapBufSize;};
	uint8_t 	GetByte(uint64_t offset);
	void 		SetByte(uint64_t offset,uint8_t val);

	/*
	 * Those operations should be prohibited for PatRec Buffer
	 */
	uint16_t	GetWord(uint64_t offset)
	{
		UNUSED_PARAM(offset);
		LOGERROR("PatRecBuf::GetWord() Not Supported!");
		return (uint16_t)-1;
	}
	uint32_t	GetDWord(uint64_t offset)
	{
		UNUSED_PARAM(offset);
		LOGERROR("PatRecBuf::GetDWord() Not Supported!");
		return (uint32_t)-1;
	}
	uint64_t	GetQWord(uint64_t offset)
	{
		UNUSED_PARAM(offset);
		LOGERROR("PatRecBuf::GetQWord() Not Supported!");
		return (uint64_t)-1;
	}

	void SetWord(uint64_t offset,uint16_t val)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(val);
		LOGERROR("PatRecBuf::SetWord() Not Supported!");
		return;
	}
	void SetDWord(uint64_t offset,uint32_t val)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(val);
		LOGERROR("PatRecBuf::SetDWord() Not Supported!");
		return;
	}
	void SetQWord(uint64_t offset,uint64_t val)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(val);
		LOGERROR("PatRecBuf::SetQWord() Not Supported!");
		return;
	}

	void FillWithBytePat(uint64_t offset,uint8_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("PatRecBuf::FillWithBytePat() Not Supported!");
		return;
	}
	void FillWithWordPat(uint64_t offset,uint16_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("PatRecBuf::FillWithWordPat() Not Supported!");
		return;
	}
	void FillWithDWordPat(uint64_t offset,uint32_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("PatRecBuf::FillWithDWordPat() Not Supported!");
		return;
	}
	void FillWithQWordPat(uint64_t offset,uint64_t pattern,uint32_t length)
	{
		UNUSED_PARAM(offset);
		UNUSED_PARAM(pattern);
		UNUSED_PARAM(length);
		LOGERROR("PatRecBuf::FillWithQWordPat() Not Supported!");
		return;
	}

	bool ReadRegVal(uint64_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t& value)
	{
		UNUSED_PARAM(startByte);
		UNUSED_PARAM(startBits);
		UNUSED_PARAM(bitsLen);
		UNUSED_PARAM(value);
		LOGERROR("PatRecBuf::ReadRegVal() Not Supported!");
		return false;
	}
	bool WriteRegVal(uint64_t startByte,uint8_t startBits,uint8_t bitsLen,uint64_t value)
	{
		UNUSED_PARAM(startByte);
		UNUSED_PARAM(startBits);
		UNUSED_PARAM(bitsLen);
		UNUSED_PARAM(value);
		LOGERROR("PatRecBuf::WriteRegVal() Not Supported!");
		return false;
	}
};

#endif
