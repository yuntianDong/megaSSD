/*
 * file : PatternMngr.cpp
 */
#include <stdlib.h>
#include <string.h>

#include "pattern/PatternMngr.h"

PatternMngr::PatternMngr(stPatRecType type,Buffers* bitMap,bool bAddrOL):mPatRecType(type),mPatRec(nullptr)
{
	if(type == PATREC_BITMAP)
	{
		mPatRec = (PatternRecoder*)new PatRecBitmap(bitMap,bAddrOL);
	}
	else if(type == PATREC_CRC8)
	{
		mPatRec = (PatternRecoder*)new PatRecCRC8(bitMap,bAddrOL);
	}
	else
	{
		mPatRec = (PatternRecoder*)new PatRecBitmap(bitMap,bAddrOL);
	}
}

PatternMngr::~PatternMngr()
{
	del_obj(mPatRec);
}

bool PatternMngr::RecordFromWrBuf(Buffers* wrBuf,uint32_t bufOffset,uint64_t bitMapOffset,uint32_t length)
{
	if((bufOffset+length*wrBuf->GetSectorSize()) > wrBuf->GetBufSize())
	{
		LOGERROR("Invalid offset and length param");
		return false;
	}

	if(wrBuf == nullptr)
	{
		LOGERROR("Write Buffer is NULL, please check!");
		return false;
	}

	return mPatRec->Merge(*wrBuf,bufOffset,bitMapOffset,length);
}

bool PatternMngr::CompareWithRdBuf(Buffers* rdBuf,uint32_t bufOffset,uint64_t bitMapOffset,uint32_t length,std::string bufmode)
{
	if((bufOffset+length*rdBuf->GetSectorSize()) > rdBuf->GetBufSize())
	{
		LOGERROR("Invalid offset and length param");
		return false;
	}

	if(rdBuf == nullptr)
	{
		LOGERROR("Write Buffer is NULL, please check!");
		return false;
	}
	return mPatRec->Compare(*rdBuf,bufOffset,bitMapOffset,length,bufmode);
}

bool PatternMngr::FillPattern(Buffers* cmpBuf,uint32_t bufOffset,uint64_t bitMapOffset,uint32_t length)
{
	return mPatRec->Fill(*cmpBuf,bufOffset,bitMapOffset,length);
}

bool PatternMngr::DeletePattern(uint64_t bitMapOffset,uint32_t length)
{
	return mPatRec->DelPat(bitMapOffset,length);
}

void PatternMngr::DebugDump(uint32_t bitMapOffset,uint32_t length)
{
	mPatRec->DebugDump(bitMapOffset,length);
}

PatternRecoder* PatternMngr::GetPatRec(void)
{
	return mPatRec;
}