/*
 * file : PatternRecorder.cpp
 */

#include "pattern/PatternRecorder.h"
#include "pattern/PatternBitmap.h"
#include "pattern/PatternCRC8.h"

PatRecBitmap::PatRecBitmap(Buffers* bitMapBuf,bool bAddrOL)
	:PatternRecoder(bAddrOL),mpBitmapBuf(bitMapBuf)
{
	muBufSize	= mpBitmapBuf->GetBufSize();
	mcMinPatIdx	= PAT_IDX_MIN;
	mcMaxPatIdx	= PAT_IDX_MAX;
}

void PatRecBitmap::CalBitmapPos(uint64_t offset,uint32_t& pos,bool& bLeft)
{
	pos		= int(offset / (NBITS_PER_BYTE/PAT_IDX_BITS));
	bLeft	= (offset % (NBITS_PER_BYTE/PAT_IDX_BITS)) == 0;
}

void PatRecBitmap::UptBitmapIdx(uint32_t pos,uint8_t lIndex,uint8_t rIndex)
{
	uint8_t tmpVal = (uint8_t)(lIndex<<PAT_IDX_BITS) | rIndex;
	mpBitmapBuf->SetByte(pos,tmpVal);
}

void PatRecBitmap::GetBitmapIdx(uint32_t pos,uint8_t& lIndex,uint8_t& rIndex)
{
	uint8_t	tmpVal	= mpBitmapBuf->GetByte(pos);
	lIndex	= (tmpVal >> PAT_IDX_BITS) & 0x0F;
	rIndex	= tmpVal & 0x0F;
}

bool PatRecBitmap::AddPat(uint64_t offset,uint32_t length,uint32_t patIdx)
{
	if(0 != (patIdx >> 8) && not PatternBitmap::IsValidIndex(patIdx))
	{
		LOGERROR("Invalid Pattern Index!");
		return false;
	}

	if(muBufSize*2 < offset+length)
	{
		return false;
	}

	for(uint32_t idx=0;idx<length;)
	{
		uint32_t	bufPos	= 0;
		bool		bLeft	= false;
		uint8_t		tmpIdx1,tmpIdx2;

		CalBitmapPos(offset+idx,bufPos,bLeft);
		GetBitmapIdx(bufPos,tmpIdx1,tmpIdx2);
		if(0==idx && !bLeft)
		{
			UptBitmapIdx(bufPos,tmpIdx1,patIdx);
			idx++;
		}
		else if(bLeft && idx==(length-1))
		{
			UptBitmapIdx(bufPos,patIdx,tmpIdx2);
			idx++;
		}
		else
		{
			UptBitmapIdx(bufPos,patIdx,patIdx);
			idx+=2;
		}
	}

	return true;
}

bool PatRecBitmap::DelPat(uint64_t offset,uint32_t length)
{
	if(0 == length)
	{
		length	= muBufSize*2 - offset;
	}

	if(muBufSize*2 < offset+length)
	{
		LOGERROR("Invalid Offset+Length Parameter!");
		return false;
	}

	AddPat(offset,length,(uint8_t)0);

	return true;
}

bool PatRecBitmap::GetPat(uint64_t offset,uint32_t& rtnPatIdx)
{
	if(muBufSize*2 <= offset)
	{
		LOGERROR("Invalid Offset Parameter!");
		return false;
	}

	uint32_t	bufPos	= 0;
	bool		bLeft	= false;
	uint8_t		tmpIdx,rtnIndex;
	CalBitmapPos(offset,bufPos,bLeft);

	if(true == bLeft)
	{
		GetBitmapIdx(bufPos,rtnIndex,tmpIdx);
	}
	else
	{
		GetBitmapIdx(bufPos,tmpIdx,rtnIndex);
	}

	if(false == PatternBitmap::IsValidIndex(rtnIndex))
	{
		return false;
	}

	rtnPatIdx	= (uint32_t)rtnIndex;
	return true;
}

bool PatRecBitmap::IsMapped(uint64_t offset,uint32_t length)
{
	if(muBufSize*2 < offset+length)
	{
		LOGERROR("Invalid Offset+Length Parameter!");
		return false;
	}

	for(uint32_t idx=0;idx<length;)
	{
		uint32_t	bufPos	= 0;
		bool		bLeft	= false;
		uint8_t		lIndex,rIndex;

		CalBitmapPos(offset+idx,bufPos,bLeft);
		GetBitmapIdx(bufPos,lIndex,rIndex);

		if(0 == idx && false == bLeft)
		{
			if(PAT_IDX_UNMAP == rIndex)
			{
				return false;
			}
			idx++;
		}
		else if(idx == (length-1) && true == bLeft)
		{
			if(PAT_IDX_UNMAP == lIndex)
			{
				return false;
			}
			idx++;
		}
		else
		{
			if(PAT_IDX_UNMAP == lIndex || PAT_IDX_UNMAP == rIndex)
			{
				return false;
			}
			idx+=2;
		}
	}

	return true;
}

bool PatRecBitmap::IsSamePat(uint64_t offset,uint32_t length)
{
	if(muBufSize*2 < offset+length)
	{
		LOGERROR("Invalid Offset+Length Parameter!");
		return false;
	}

	uint8_t	tmpIdx	= (uint8_t)-1;

	for(uint32_t idx=0;idx<length;)
	{
		uint32_t	bufPos	= 0;
		bool		bLeft	= false;
		uint8_t		lIndex,rIndex;

		CalBitmapPos(offset+idx,bufPos,bLeft);
		GetBitmapIdx(bufPos,lIndex,rIndex);

		if((uint8_t)-1 == tmpIdx)
		{
			tmpIdx	= rIndex;
		}

		if(0 == idx && false == bLeft)
		{
			if(tmpIdx != rIndex)
			{
				return false;
			}
			idx++;
		}
		else if(idx == (length-1) && true == bLeft)
		{
			if(tmpIdx != lIndex)
			{
				return false;
			}
			idx++;
		}
		else
		{
			if(tmpIdx != lIndex || tmpIdx != rIndex)
			{
				return false;
			}
			idx+=2;
		}
	}

	return true;
}

bool PatRecBitmap::Merge(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length)
{
	if(false == buf.IsSectorBuf() ||
			(bufOffset + length*buf.GetSectorSize()) > buf.GetBufSize() ||
			0 != bufOffset % buf.GetSectorSize() ||
			((uint64_t)muBufSize)*2 < (offset+length) )
	{
		LOGERROR("Invalid Parameter!");
		LOGDEBUG("bitmapBufSize:%lu,bufSize:%u,bufOffset:%u,bufSectorSize:%u,length:%u,offset:%lu",((uint64_t)muBufSize)*2,buf.GetBufSize(),bufOffset,buf.GetSectorSize(),length,offset);
		return false;
	}

	uint32_t tmpPat = 0;
	for(uint16_t idx=0;idx<length;idx++)
	{
		uint32_t rdPos	= bufOffset + (idx * buf.GetSectorSize()) + (true==IsAddrOverlay() ? BUF_ADDROL_BYTES : 0);
		uint32_t patVal	= buf.GetDWord(rdPos);
		uint8_t patIdx	= PatternBitmap::GetIndexByPattern(patVal);
		if(tmpPat != patVal)
		{
			tmpPat = patVal;
		}
		if(false == PatternBitmap::IsValidIndex(patIdx))
		{
			LOGERROR("Invalid PatIdx:%d",patIdx);
			return false;
		}

		if(false == AddPat(offset+idx,1,patIdx))
		{
			LOGERROR("Fail to AddPat(%lu,1,%u)",offset+idx,patIdx);
			return false;
		}
	}
	// LOGDEBUG("Dump bitmap buf");
	// mpBitmapBuf->Dump(offset/2,length/2);
	return true;
}

bool PatRecBitmap::Compare(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length,std::string bufmode)
{
	if(false == buf.IsSectorBuf() ||
			(bufOffset + length*buf.GetSectorSize()) > buf.GetBufSize() ||
			0 != bufOffset % buf.GetSectorSize() ||
			muBufSize*2 < (offset+length) )
	{
		LOGERROR("Invalid Parameter!");
		return false;
	}

	uint16_t sectorSize	= buf.GetSectorSize();

	SectorBuf cmpBuf(buf.GetBufSize()-bufOffset,sectorSize,bufmode.c_str());

	// uint32_t tmpPat = -1;
	for(uint16_t idx=0;idx<length;idx++)
	{
		uint32_t patIdx = PAT_INVALID_IDX;
		if(false == GetPat(offset+idx,patIdx))
		{
			LOGERROR("Fail to GetPat(%lu)",offset+idx);
			return false;
		}

		// if(tmpPat != patIdx)
		// {
		// 	LOGDEBUG("Fill Sector idx:%u,pat idx:%u",idx,patIdx);
		// 	tmpPat = patIdx;
		// }

		if(PAT_IDX_UNMAP == patIdx)
		{
			cmpBuf.FillSector(idx,1,0x0);
		}
		else
		{
			uint32_t patVal = PatternBitmap::GetPatternByIndex((uint8_t)patIdx);
			cmpBuf.FillSector(idx,1,patVal);

			if(true == IsAddrOverlay())
			{
				cmpBuf.MarkAddrOverlay(idx,1,offset+idx);
			}
		}
	}

	uint32_t errPos,srcVal,dstVal;
	errPos = srcVal = dstVal = BUF_CMP_NOERROR;
	if(false == buf.Compare(cmpBuf,0,bufOffset,length*sectorSize,errPos,srcVal,dstVal))
	{
		LOGERROR("Fail to call Buffers.Compare()");
		return false;
	}

	return errPos == BUF_CMP_NOERROR;
}

bool PatRecBitmap::Fill(Buffers& buf,uint32_t bufOffset,uint64_t patOffset,uint16_t length)
{
	if(false == buf.IsSectorBuf())
	{
		LOGERROR("It requires a Sector Buffer!");
		return false;
	}

	for(uint16_t idx=0;idx<length;idx++)
	{
		uint32_t patIdx	= 0;
		if(false == GetPat(patOffset+idx,patIdx))
		{
			LOGERROR("Fail to Get Pattern from Offset=%lx",patOffset+idx);
			return false;
		}

		uint32_t patVal = PatternBitmap::GetPatternByIndex((uint8_t)patIdx);
		buf.FillSector(bufOffset+idx,1,patVal);
		
		if(true == IsAddrOverlay() && true == IsMapped(patOffset+idx,1))
		{
			buf.MarkAddrOverlay(bufOffset+idx,1,patOffset+idx);
		}
		
	}

	return true;
}

PatRecCRC8::PatRecCRC8(Buffers* bitMapBuf,bool bAddrOL)
	:PatternRecoder(bAddrOL),mpBitmapBuf(bitMapBuf)
{
	muBufSize	= mpBitmapBuf->GetBufSize();
}

void PatRecCRC8::UptCRCVal(uint32_t pos,uint8_t crc)
{
	mpBitmapBuf->SetByte(pos,crc);
}

uint8_t PatRecCRC8::GetCRCVal(uint32_t pos)
{
	return mpBitmapBuf->GetByte(pos);
}

bool PatRecCRC8::AddPat(uint64_t offset,uint32_t length,uint32_t crcVal)
{
	if(0 != (crcVal >> 8) ||
			muBufSize < (offset+length) ||
			PAT_V_UNMAP == crcVal)
	{
		LOGERROR("Parameter Error");
		return false;
	}

	for(uint32_t idx=0;idx<length;idx++)
	{
		UptCRCVal(offset+idx,(uint8_t)crcVal);
	}

	return true;
}

bool PatRecCRC8::DelPat(uint64_t offset,uint32_t length)
{
	if(0 == length)
	{
		length = muBufSize - offset;
	}

	if(muBufSize < (offset+length))
	{
		LOGERROR("Invalid Offset+Length Parameter!");
		return false;
	}

	for(uint32_t idx=0;idx<length;idx++)
	{
		UptCRCVal(offset+idx,PAT_V_UNMAP);
	}

	return true;
}

bool PatRecCRC8::GetPat(uint64_t offset,uint32_t& crcVal)
{
	if(muBufSize <= offset)
	{
		LOGERROR("Invalid Offset Parameter!");
		return false;
	}

	crcVal = (uint32_t)GetCRCVal(offset);
	return true;
}

bool PatRecCRC8::IsMapped(uint64_t offset,uint32_t length)
{
	if(muBufSize < (offset+length))
	{
		LOGERROR("Invalid Offset+Length Parameter!");
		return false;
	}

	for(uint32_t idx=0;idx<length;idx++)
	{
		uint8_t val = GetCRCVal(offset+idx);
		if(PAT_V_UNMAP == val)
		{
			return false;
		}
	}

	return true;
}

bool PatRecCRC8::IsSamePat(uint64_t offset,uint32_t length)
{
	if(muBufSize < (offset+length))
	{
		LOGERROR("Invalid Offset+Length Parameter!");
		return false;
	}

	uint8_t expCRC	= GetCRCVal(offset);

	for(uint32_t idx=1;idx<length;idx++)
	{
		uint8_t val = GetCRCVal(offset+idx);
		if(expCRC != val)
		{
			return false;
		}
	}

	return true;
}

bool PatRecCRC8::Merge(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length)
{
	if(	false == buf.IsSectorBuf() ||
		0 != bufOffset % buf.GetSectorSize() ||
		(bufOffset + length*buf.GetSectorSize()) > buf.GetBufSize() ||
		muBufSize < (offset+length) )
	{
		LOGERROR("Invalid Parameter!");
		return false;
	}

	uint32_t sectorSize = buf.GetSectorSize();

	for(uint16_t idx=0;idx<length;idx++)
	{
		uint8_t crc = buf.GenCRC8(bufOffset+idx*sectorSize,sectorSize);
		if(false == AddPat(offset+idx,1,crc))
		{
			LOGERROR("Fail to call AddPat(%lu,1)",offset+idx);
			return false;
		}
	}

	return true;
}

bool PatRecCRC8::Compare(Buffers& buf,uint32_t bufOffset,uint64_t offset,uint16_t length,std::string bufmode)
{
	if(	false == buf.IsSectorBuf() ||
		0 != bufOffset % buf.GetSectorSize() ||
		(bufOffset + length*buf.GetSectorSize()) > buf.GetBufSize() ||
		muBufSize < (offset+length) )
	{
		LOGERROR("Invalid Parameter!");
		return false;
	}

	uint32_t sectorSize = buf.GetSectorSize();

	for(uint16_t idx=0;idx<length;idx++)
	{
		uint8_t actCRC = buf.GenCRC8(bufOffset+idx*sectorSize,sectorSize);
		uint32_t expCRC = 0;
		if(false == GetPat(offset+idx,expCRC))
		{
			LOGERROR("Fail to call AddPat(%lu,1)",offset+idx);
			return false;
		}

		if(expCRC != (uint32_t)actCRC)
		{
			return false;
		}
	}

	return true;
}
