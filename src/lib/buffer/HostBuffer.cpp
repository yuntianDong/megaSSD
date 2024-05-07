/*
 * file : UFSBuffer.cpp
 */

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "buffer/HostBuffer.h"

#define SWAP_PATH				"./SWAP"

SectorBuf::SectorBuf(uint64_t bufSize,uint32_t sectorSize,const char* mode)
	:HostBuffer(bufSize,mode),muSectorSize(sectorSize)
{
	muSectorSize = (muSectorSize >> 2) << 2;			// DWORD aligned
	bufSize	= int(bufSize/muSectorSize) * muSectorSize;	// Sector Size aligned
}

void SectorBuf::FillSector(uint64_t sOffset,uint16_t numSector,uint32_t pattern)
{
	FillBufVal(sOffset*muSectorSize,pattern,numSector*muSectorSize);
}

void SectorBuf::MarkAddrOverlay(uint64_t sOffset,uint16_t numSector,uint64_t logicAddr)
{
	for(uint16_t idx=0;idx<numSector;idx++)
	{
		HostBuffer::SetQWord((sOffset+idx)*muSectorSize,logicAddr+idx);
	}
}

SwapBuf::SwapBuf(const char * name,uint64_t bufSize,const char* mode)
	:Buffers(SWAP_BUF_PIECE_UNIT,DEF_BUF_ALIGNMENT,mode),mSwapBufSize(bufSize)
{
	strncpy(mBufName,name,SWAP_BUF_NAME_LEN);
	mMaxBufFileIndex	= int((bufSize + SWAP_BUF_PIECE_UNIT -1)/SWAP_BUF_PIECE_UNIT);
	mBufPieceIndex		= 0;

	if( 0 != access(SWAP_PATH,0) )
	{
		if(-1 == mkdir(SWAP_PATH,0x777))
		{
			LOGERROR("Fail to Create Folder!!!");
		}
	}
}

SwapBuf::~SwapBuf()
{
	for(uint8_t idx=0;idx<mMaxBufFileIndex;idx++)
	{
		char filename[SWAP_BUF_FNAME_LEN];
		snprintf(filename,SWAP_BUF_FNAME_LEN,"%s/%s_%s_%02x",SWAP_PATH,mBufName,SWAP_BUF_FNAME_PRE,idx);
		remove(filename);
	}
}

bool SwapBuf::DumpBufToFile(uint8_t idx)
{
	char filename[SWAP_BUF_FNAME_LEN];
	snprintf(filename,SWAP_BUF_FNAME_LEN,"%s/%s_%s_%02x",SWAP_PATH,mBufName,SWAP_BUF_FNAME_PRE,idx);
	return this->ToFile(filename,0,SWAP_BUF_PIECE_UNIT);
}

bool SwapBuf::LoadBufFromFile(uint8_t idx)
{
	char filename[SWAP_BUF_FNAME_LEN];
	snprintf(filename,SWAP_BUF_FNAME_LEN,"%s/%s_%s_%02x",SWAP_PATH,mBufName,SWAP_BUF_FNAME_PRE,idx);
	return this->FromFile(filename,0,(uint32_t)SWAP_BUF_PIECE_UNIT);
}

uint8_t SwapBuf::CalBufPieceIndex(uint64_t offset)
{
	return uint8_t(offset/(uint32_t)SWAP_BUF_PIECE_UNIT);
}

uint32_t SwapBuf::CalBufPieceOffset(uint64_t offset)
{
	return offset % (uint32_t)SWAP_BUF_PIECE_UNIT;
}

bool SwapBuf::NeedSwap(uint64_t offset)
{
	return (offset >= ((mBufPieceIndex+1)*(uint32_t)SWAP_BUF_PIECE_UNIT)) ||
			(offset < (mBufPieceIndex*(uint32_t)SWAP_BUF_PIECE_UNIT));
}

void SwapBuf::Swap(uint64_t offset)
{
	if(true == NeedSwap(offset))
	{
		uint8_t idx = CalBufPieceIndex(offset);
		if(false == DumpBufToFile(mBufPieceIndex))
		{
			LOGERROR("Fail to Swap Buffer!!!Dump Buf to File Failed!!!");
		}
		if(false == LoadBufFromFile(idx))
		{
			Buffers::FillZero(0,SWAP_BUF_PIECE_UNIT);
		}
		mBufPieceIndex = idx;
	}
}

uint8_t	SwapBuf::GetByte(uint64_t offset)
{
	Swap(offset);

	return Buffers::GetByte(CalBufPieceOffset(offset));
}

void	SwapBuf::SetByte(uint64_t offset,uint8_t val)
{
	Swap(offset);

	return Buffers::SetByte(CalBufPieceOffset(offset),val);
}
