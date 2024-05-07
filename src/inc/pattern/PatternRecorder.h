#ifndef _PATTERN_RECORDER_H
#define _PATTERN_RECORDER_H

#include <stdint.h>

#include "buffer/HostBuffer.h"

#define BUF_ADDROL_BYTES		8

class PatternRecoder
{
private:
	bool mbAddrOverlay;
public:
	PatternRecoder(bool bAddrOL)
		:mbAddrOverlay(bAddrOL)
	{
	}
	~PatternRecoder(){};
	bool IsAddrOverlay(void){return mbAddrOverlay;};

	virtual void DebugDump(uint32_t offset,uint32_t length)=0;
	virtual bool AddPat(uint64_t offset,uint32_t length,uint32_t pattern)=0;
	virtual bool DelPat(uint64_t offset,uint32_t length)=0;
	virtual bool GetPat(uint64_t offset,uint32_t& rtnPattern)=0;
	virtual bool IsMapped(uint64_t offset,uint32_t length)=0;
	virtual bool IsSamePat(uint64_t offset,uint32_t length)=0;

	virtual bool Merge(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length)=0;
	virtual bool Compare(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length,std::string bufmode)=0;
	virtual bool Fill(Buffers& buf,uint32_t bufOffset,uint64_t patOffset,uint16_t length)=0;
};

class PatRecBitmap : public PatternRecoder
{
private:
	Buffers*	mpBitmapBuf;
	uint32_t	muBufSize;
	uint32_t*	mpPatIdxMap;
	uint8_t		mcMinPatIdx;
	uint8_t		mcMaxPatIdx;

protected:
	void	CalBitmapPos(uint64_t offset,uint32_t& pos,bool& bLeft);
	void	UptBitmapIdx(uint32_t pos,uint8_t lIndex=(uint8_t)-1,uint8_t rIndex=(uint8_t)-1);
	void	GetBitmapIdx(uint32_t pos,uint8_t& lIndex,uint8_t& rIndex);

public:
	PatRecBitmap(Buffers* bitMapBuf,bool bAddrOL);
	~PatRecBitmap(){};

	void DebugDump(uint32_t offset,uint32_t length)
	{
		if(mpBitmapBuf != nullptr)
		{
			mpBitmapBuf->Dump(offset,length);
		}
		else{
			LOGERROR("bitmapBuf is null!");
		}
	};

	bool AddPat(uint64_t offset,uint32_t length,uint32_t patIdx);
	bool DelPat(uint64_t offset,uint32_t length);
	bool GetPat(uint64_t offset,uint32_t& rtnPatIdx);

	bool IsMapped(uint64_t offset,uint32_t length);
	bool IsSamePat(uint64_t offset,uint32_t length);

	bool Merge(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length);
	bool Compare(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length,std::string bufmode);
	bool Fill(Buffers& buf,uint32_t bufOffset,uint64_t patOffset,uint16_t length);
};

class PatRecCRC8 : public PatternRecoder
{
private:
	Buffers*	mpBitmapBuf;
	uint32_t	muBufSize;

	void UptCRCVal(uint32_t pos,uint8_t crc);
	uint8_t GetCRCVal(uint32_t pos);
public:
	PatRecCRC8(Buffers* bitMapBuf,bool bAddrOL);
	~PatRecCRC8(){};

	void DebugDump(uint32_t offset,uint32_t length){mpBitmapBuf->Dump(offset,length);};

	bool AddPat(uint64_t offset,uint32_t length,uint32_t crcVal);
	bool DelPat(uint64_t offset,uint32_t length);
	bool GetPat(uint64_t offset,uint32_t& crcVal);

	bool IsMapped(uint64_t offset,uint32_t length);
	bool IsSamePat(uint64_t offset,uint32_t length);

	bool Merge(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length);
	bool Compare(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length,std::string bufmode);
	bool Fill(Buffers& buf,uint32_t bufOffset,uint64_t patOffset,uint16_t length)
	{LOGERROR("PatRecCRC8 not support!");return false;};
};

#endif
